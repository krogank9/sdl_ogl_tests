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
					  Texture& texture, RenderNameList masks, RenderNameList targets)
{
	ctx->useVgShader();
	texture.bindToTexture();
	ctx->getVgShader().setVec2("texture_size", texture.width, texture.height);
	// make sure not to premultiply twice
	ctx->getVgShader().setBool("texture_premultiplied", texture.isPremultiplied());

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

	// scale, rotate, translate
	glm::mat4 world_transform(1.0f);
	world_transform = glm::translate(world_transform, glm::vec3(pos.x, pos.y, 0.0f));
	world_transform = glm::rotate(world_transform, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	world_transform = glm::scale(world_transform, glm::vec3(scale.x, scale.y, 1.0f));
	glUniformMatrix4fv(ctx->getVgShader().getUniformLocation("world_transform"), 1, GL_FALSE, glm::value_ptr(world_transform));

	// bind masks
	ctx->getVgShader().setInt("mask_count", masks.size());
	for (unsigned int i=0; i<masks.size() && i<MAX_MASKS; ++i)
	{
		Texture& mask = ctx->getRenderTexture(masks.getName(i));
		Color mask_tint = masks.getColorMask(i);
		mask.bindToMask(i+1);
		std::string mask_prefix = "mask" + std::to_string(i+1);

		ctx->getVgShader().setBool(mask_prefix+"_premultiplied", mask.isPremultiplied());
		ctx->getVgShader().setVec4(mask_prefix+"_tint", mask_tint.r, mask_tint.g, mask_tint.b, mask_tint.a);
		ctx->getVgShader().setVec2(mask_prefix+"_texture_size", mask.width, mask.height);
	}

	// draw elements
	for (unsigned int i=0; i<targets.size(); ++i)
	{
		// translate so center of screen is 0,0, then scale to -1.0 1.0f NDC (target w&h)
		bool flip_render = targets.getName(i) == "" && ctx->is_final;
		glm::mat4 ndc_transform(1.0f);
		ndc_transform = glm::scale(ndc_transform, glm::vec3(2.f/ctx->getViewportWidth(), (flip_render?-2.f:2.f)/ctx->getViewportHeight(), 1.0f)); //ndc
		ndc_transform = glm::translate(ndc_transform, glm::vec3(ctx->getViewportWidth()/-2.f, ctx->getViewportHeight()/-2.f, 0.0f)); //ndc
		glUniformMatrix4fv(ctx->getVgShader().getUniformLocation("ndc_transform"), 1, GL_FALSE, glm::value_ptr(ndc_transform));

		Texture& target = ctx->getRenderTexture(targets.getName(i));
		target.bindToRenderTarget();

		Color render_tint = targets.getColorMask(i);

		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (render_tint.hasNegative())
		{
			render_tint.makePositive();
			// just here for completeness, so you can subtract from masks
			glBlendFunc(GL_ONE, GL_ONE);
			glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
		}
		else
		{
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
		}

		ctx->getVgShader().setVec4("render_tint", render_tint.r, render_tint.g, render_tint.b, render_tint.a);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_indices_buffer);
		glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);
	}

	// disable attrib arrays
	glDisableVertexAttribArray(ctx->getVgShader().getAttribLocation("a_position"));
	if (has_texture_coords)
		glDisableVertexAttribArray(ctx->getVgShader().getAttribLocation("a_tex_coord"));
}
