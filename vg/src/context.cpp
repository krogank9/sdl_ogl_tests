#include "context.h"
#include "color.h"

Shader* Context::vg_shader;
int Context::num_ctx;

Context::Context(int viewport_width, int viewport_height, int msaa_level)
	: msaa_level(msaa_level),
	  viewport_width(viewport_width), viewport_height(viewport_height)
	  //white_texture(this, Color::WHITE()),
	  //clear_texture(this, Color::TRANSPARENT()),
	  //grey_texture(this, Color(100, 100, 100, 255)),
	  //black_texture(this, Color::BLACK()),
	  //purple_texture(this, Color::PURPLE())
{

	if (num_ctx == 0)
	{
		vg_shader = new Shader("vg_test.vert", "vg_test.frag");
		vg_shader->use();
		vg_shader->setInt("texture", 0);
		vg_shader->setInt("mask_texture", 1);
	}
	num_ctx++;

	//unit_quad = TriCache::makeQuad(this, 1, 1);
	//screen_quad = TriCache::makeQuad(this, viewport_width, viewport_height);
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

	num_ctx--;
	if (num_ctx == 0)
		delete vg_shader;
}
