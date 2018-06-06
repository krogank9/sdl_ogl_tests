#ifndef VG_CONTEXT_H
#define VG_CONTEXT_H

#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <map>

#include "utils.h"
#include "texture.h"
#include "tri_cache.h"

class Context
{
private:
	// only need one shader per application
	static Shader* cur_shader;
	static Shader* vg_shader;
	static int num_ctx; // ref count for global shader destructing

	int viewport_width;
	int viewport_height;

	uint64_t render_frame_num;

	TriCache* unit_quad;
	TriCache* screen_quad;

	// todo clear if haven't been accessed
	std::map<std::string, Texture*> render_textures;
	std::set<std::string> render_textures_accessed;

public:
	Context(int viewport_width=0, int viewport_height=0, int internal_msaa_level=8, bool render_to_screen=true);
	~Context();

	Shader& getVgShader()
	{
		return *vg_shader;
	}
	void useVgShader()
	{
		if (cur_shader != vg_shader)
			cur_shader = vg_shader;
		vg_shader->use();
	}

	uint64_t getRenderFrame() { return render_frame_num; }
	void clear()
	{
		// remove render textures which are not in use
		std::map<std::string, Texture*>::iterator it = render_textures.begin();
		for (; it != render_textures.end();)
		{
			std::pair<const std::string, Texture*>& p = *it;
			bool texture_was_accessed = p.first == "" || std::find(
						render_textures_accessed.begin(),
						render_textures_accessed.end(),
						p.first) != render_textures_accessed.end();
			if (texture_was_accessed)
			{
				p.second->clear();
				++it;
			}
			else
				it = render_textures.erase(it);
		}

		++render_frame_num;

		render_textures_accessed.clear();
	}
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
			texture = new Texture(this, viewport_width, viewport_height, name=="" && render_to_screen);
		}
		else if (texture->width != viewport_width || texture->height != viewport_height)
		{
			delete texture;
			texture = new Texture(this, viewport_width, viewport_height, name=="" && render_to_screen);
		}

		render_textures_accessed.insert(name);
		render_textures[name] = texture;
		return *texture;
	}

	void blitToFramebuffer(GLuint id)
	{
		getRenderTexture("").blitToFramebuffer(id);
	}

	TriCache& getScreenQuad() { return *screen_quad; }
	TriCache& getUnitQuad() { return *unit_quad; }

	const int internal_msaa_level;
	const bool render_to_screen;

	// some debug & ease of use primitives
	Texture black_texture;
	Texture white_texture;
	Texture clear_texture;
	Texture grey_texture;
	Texture purple_texture;
	Texture crimson_texture;
	Texture red_texture;
	Texture green_texture;
	Texture blue_texture;
};

#endif // VG_CONTEXT_H
