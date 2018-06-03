#include "context.h"
#include "color.h"

Context::Context(int viewport_width, int viewport_height, int msaa_level)
	: vg_shader("vg_test.vert", "vg_test.frag"),
	  msaa_level(msaa_level),
	  unit_quad(TriCache::makeQuad(this, 1, 1)),
	  screen_quad(TriCache::makeQuad(this, viewport_width, viewport_height)),
	  viewport_width(viewport_width), viewport_height(viewport_height),
	  white_texture(this, Color::WHITE()),
	  clear_texture(this, Color::TRANSPARENT()),
	  grey_texture(this, Color(100, 100, 100, 255)),
	  purple_texture(this, Color::PURPLE())
{
	vg_shader.use();
	vg_shader.setInt("texture", 0);
	vg_shader.setInt("mask_texture", 1);
}

Context::~Context()
{
	delete unit_quad;
	delete screen_quad;

	std::map<std::string, Texture*>::iterator it = render_textures.begin();
	for (; it != render_textures.end(); it++)
	{
		std::pair<const std::string, Texture*>& p = *it;
		delete p.second;
	}
}
