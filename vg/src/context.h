#ifndef VG_CONTEXT_H
#define VG_CONTEXT_H

#include <vector>
#include <string>
#include <map>

#include "utils.h"
#include "texture.h"
#include "tri_cache.h"

class Context
{
private:
	Shader vg_shader;

	int viewport_width;
	int viewport_height;

	uint64_t render_frame_num;

	TriCache* unit_quad;
	TriCache* screen_quad;

	std::map<std::string, Texture*> render_textures;

public:
	Context(int viewport_width=0, int viewport_height=0, int msaa_level=8);
	~Context();

	Shader& getVgShader() { return vg_shader; }
	uint64_t getRenderFrame() { return render_frame_num; }
	void clear() { ++render_frame_num; }
	void updateViewportSize(int width, int height)
	{
		viewport_width = width;
		viewport_height = height;
		delete screen_quad;
		screen_quad = TriCache::makeQuad(this, viewport_width, viewport_height);
	}
	int getViewportWidth() { return viewport_width; }
	int getViewportHeight() { return viewport_height; }

	// generate viewport size textures
	Texture& getRenderTexture(std::string name)
	{
		Texture* texture = render_textures[name];

		if (texture == 0)
		{
			texture = new Texture(this, viewport_width, viewport_height);
		}
		else if (texture->width != viewport_width || texture->height != viewport_height)
		{
			delete texture;
			texture = new Texture(this, viewport_width, viewport_height);
		}

		render_textures[name] = texture;
		return *texture;
	}

	void blitToFramebuffer(GLuint id)
	{
		getRenderTexture("").blitToFramebuffer(id);
	}

	TriCache& getScreenQuad() { return *screen_quad; }
	TriCache& getUnitQuad() { return *unit_quad; }

	const int msaa_level;

	// some debug & ease of use primitives
	Texture white_texture;
	Texture clear_texture;
	Texture grey_texture;
	Texture purple_texture;
};

#endif // VG_CONTEXT_H
