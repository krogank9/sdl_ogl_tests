#ifndef TRI_CACHE_H
#define TRI_CACHE_H

#include <iostream>

#include <GLES3/gl3.h>

#include "vec2.h"
#include "vec4.h"
#include "texture.h"

using namespace std;

class Context;

class TriCache
{
public:
	TriCache(Context* ctx,
			 GLfloat const* verts, const GLsizei verts_size,
			 GLuint const* indices, const GLsizei indices_size,
			 GLfloat const* texture_coords=0);
	~TriCache();

	void render(vec2f pos, vec2f scale, float rotation,
				Texture& texture, Texture& target, Texture& mask,
				float blendMultiplier=1.0f);

	static TriCache* makeQuad(Context* ctx, vec2f size);
	static TriCache* makeQuad(Context* ctx, int width, int height)
	{
		return makeQuad(ctx, vec2f(width, height));
	}

private:
	Context* ctx;

	const GLsizei verts_size;
	const GLsizei indices_size;
	bool has_texture_coords;

	GLuint gl_vert_buffer;
	GLuint gl_indices_buffer;
	GLuint gl_tex_coords_buffer;
};

#endif // TRI_CACHE_H
