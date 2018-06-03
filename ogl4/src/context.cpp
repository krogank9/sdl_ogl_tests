#include "context.h"
/*
Shader* Context::vg_shader;
int Context::num_ctx;

Context::Context(int viewport_width, int viewport_height, int msaa_level)
	: msaa_level(msaa_level),
	  viewport_width(viewport_width), viewport_height(viewport_height)
{

	if (num_ctx == 0)
	{
		vg_shader = new Shader("coords.vert", "coords.frag");
		vg_shader->use();
		vg_shader->setInt("texture", 0);
		vg_shader->setInt("mask_texture", 1);
	}
	num_ctx++;
}

Context::~Context()
{
	num_ctx--;
	if (num_ctx == 0)
		delete vg_shader;
}
*/
