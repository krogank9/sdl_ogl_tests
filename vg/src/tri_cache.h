#ifndef TRI_CACHE_H
#define TRI_CACHE_H

#include "texture.h"
#include "vec2.h"
#include "vec4.h"
#include "SDL_opengles2.h"

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
				const Texture& texture, const Texture& target, const Texture& mask,
				float blendMultiplier=1.0f) const;

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
