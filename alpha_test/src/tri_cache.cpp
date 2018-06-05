#include "tri_cache.h"

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "context.h"

TriCache::TriCache(Context* ctx,
				   GLfloat const* verts, const GLsizei verts_size,
				   GLuint const* indices, const GLsizei indices_size,
				   GLfloat const* texture_coords)
	: ctx(ctx),
	  verts_size(verts_size),
	  indices_size(indices_size),
	  has_texture_coords(texture_coords != 0)
{
	glGenBuffers(1, &gl_vert_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, gl_vert_buffer);
	glBufferData(GL_ARRAY_BUFFER, verts_size*sizeof(GLfloat), verts, GL_STATIC_DRAW);

	if (has_texture_coords)
	{
		glGenBuffers(1, &gl_tex_coords_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, gl_tex_coords_buffer);
		glBufferData(GL_ARRAY_BUFFER, verts_size*sizeof(GLfloat), texture_coords, GL_STATIC_DRAW);
	}

	glGenBuffers(1, &gl_indices_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_indices_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size*sizeof(GLuint), indices, GL_STATIC_DRAW);
}

TriCache::~TriCache()
{
}

TriCache* TriCache::makeQuad(Context* ctx, vec2f size)
{
	// for unit sq tri cache
	GLfloat quad_verts[] = {
		-size.x/2.f, -size.y/2.f, // left top
		size.x/2.f, -size.y/2.f, // right top
		size.x/2.f, size.y/2.f, // right bot
		-size.x/2.f, size.y/2.f, // left bot
	};

	GLfloat quad_tex_coords[] = {
		0.f, 0.f, // left top
		size.x, 0.f, // right top
		size.x, size.y, // right bot
		0.f, size.y, // left bot
	};

	const GLuint quad_indices[] = {
		0, 1, 2, // first triangle
		0, 2, 3 // second triangle
	};

	return new TriCache(ctx, quad_verts, 8, quad_indices, 6, quad_tex_coords);
}

// blendMultiplier so later we can add RenderDescr class:
// <Texture, intensity> pairs used for render & mask.
// multiple render targets and masks. RenderDescr can cache multiple masks mixed
void TriCache::render(vec2f pos, vec2f scale, float rotation,
					  Texture& texture, Texture& mask, RenderNameList targets)
{
	ctx->getVgShader().use();

	texture.bindToTexture();
	mask.bindToMask();
	ctx->getVgShader().setVec2("texture_size", texture.width, texture.height);
	ctx->getVgShader().setVec2("mask_texture_size", mask.width, mask.height);

	// bind verts
	glBindBuffer(GL_ARRAY_BUFFER, gl_vert_buffer);
	glVertexAttribPointer(
	   ctx->getVgShader().getAttribLocation("a_position"), // attribute to set
	   2,			// size
	   GL_FLOAT,	// type
	   GL_FALSE,	// normalized?
	   0,			// stride
	   (void*)0		// array buffer offset
	);
	glEnableVertexAttribArray(ctx->getVgShader().getAttribLocation("a_position"));
	// bind texture coords
	if (has_texture_coords)
	{
		glBindBuffer(GL_ARRAY_BUFFER, gl_tex_coords_buffer);
		glVertexAttribPointer(
		   ctx->getVgShader().getAttribLocation("a_tex_coord"), // attribute to set
		   2,			// size
		   GL_FLOAT,	// type
		   GL_FALSE,	// normalized?
		   0,			// stride
		   (void*)0		// array buffer offset
		);
		glEnableVertexAttribArray(ctx->getVgShader().getAttribLocation("a_tex_coord"));
	}

	// set "transform" uniform mat4
	glm::mat4 world_transform(1.0f);
	glm::mat4 ndc_transform(1.0f);

	// scale, rotate, translate
	// translate so center of screen is 0,0, then scale to -1.0 1.0f NDC (target w&h)
	ndc_transform = glm::scale(ndc_transform, glm::vec3(2.f/ctx->getViewportWidth(), 2.f/ctx->getViewportHeight(), 1.0f)); //ndc
	ndc_transform = glm::translate(ndc_transform, glm::vec3(ctx->getViewportWidth()/-2.f, ctx->getViewportHeight()/-2.f, 0.0f)); //ndc

	world_transform = glm::translate(world_transform, glm::vec3(pos.x, pos.y, 0.0f));
	world_transform = glm::rotate(world_transform, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	world_transform = glm::scale(world_transform, glm::vec3(scale.x, scale.y, 1.0f));

	glUniformMatrix4fv(ctx->getVgShader().getUniformLocation("ndc_transform"), 1, GL_FALSE, glm::value_ptr(ndc_transform));
	glUniformMatrix4fv(ctx->getVgShader().getUniformLocation("world_transform"), 1, GL_FALSE, glm::value_ptr(world_transform));

	// draw elements
	for (unsigned int i=0; i<targets.size(); ++i)
	{
		ctx->getRenderTexture(targets.getName(i)).bindToRenderTarget();
		Color renderColorMask = targets.getColorMask(i);

		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);


		if (renderColorMask.hasNegative())
		{
			renderColorMask.makePositive();
			glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
		}
		else
		{
			glBlendEquation(GL_FUNC_ADD);
		}

		// make sure not to premultiply twice,
		ctx->getVgShader().setBool("texture_premultiplied", texture.isPremultiplied());
		ctx->getVgShader().setBool("mask_premultiplied", mask.isPremultiplied());

		ctx->getVgShader().setVec4("render_color_mask", renderColorMask.r, renderColorMask.g, renderColorMask.b, renderColorMask.a);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_indices_buffer);
		glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);
	}

	// disable attrib arrays
	glDisableVertexAttribArray(ctx->getVgShader().getAttribLocation("a_position"));
	if (has_texture_coords)
		glDisableVertexAttribArray(ctx->getVgShader().getAttribLocation("a_tex_coord"));
}
