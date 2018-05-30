#include "tri_cache.h"
#include "context.h"
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

void TriCache::render(vec2f pos, vec2f scale, float rotation,
					  const Texture& texture, const Texture& target, const Texture& mask,
					  float blendMultiplier) const
{	
	ctx->getVgShader().use();

	texture.bindToTexture();
	mask.bindToMask();
	target.bindToRenderTarget();

	// if blendMultiplier is negative, subtract from target texture
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	if (blendMultiplier < 0.0f)
	{
		blendMultiplier = -blendMultiplier;
		glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
	}
	else
	{
		glDisable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
	}
	ctx->getVgShader().setFloat("color_multiplier", blendMultiplier);

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

	glm::mat4 transform(1.0f);

	// scale, rotate, translate
	// then scale to -1.0 1.0f NDC (target w&h)
	transform = glm::scale(transform, glm::vec3(2.f/target.width, -2.f/target.height, 1.0f)); //ndc
	transform = glm::translate(transform, glm::vec3(target.width/-2.f, target.height/-2.f, 0.0f)); //ndc

	transform = glm::translate(transform, glm::vec3(pos.x, pos.y, 0.0f));
	transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(scale.x, scale.y, 1.0f));

	glUniformMatrix4fv(ctx->getVgShader().getUniformLocation("transform"), 1, GL_FALSE, glm::value_ptr(transform));

	// draw elements
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_indices_buffer);
	glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);

	// disable attrib arrays
	glDisableVertexAttribArray(ctx->getVgShader().getAttribLocation("a_position"));
	if (has_texture_coords)
		glDisableVertexAttribArray(ctx->getVgShader().getAttribLocation("a_tex_coord"));
}
