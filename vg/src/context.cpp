#include "context.h"
#include "color.h"

// for unit sq tri cache
static const GLfloat unit_tri_verts[] = {
	-0.5f, -0.5f, // left top
	0.5f, -0.5f, // right top
	0.5f, 0.5f, // right bot
	-0.5f, 0.5f, // left bot
};

static const GLfloat unit_tri_tex_coords[] = {
	0.f, 0.f, // left top
	1.f, 0.f, // right top
	1.f, 1.f, // right bot
	0.f, 1.f, // left bot
};

static const GLuint unit_tri_indices[] = {
	0, 1, 2, // first triangle
	0, 2, 3 // second triangle
};

Context::Context()
	: vgShader("vg_test.vert", "vg_test.frag"),
	  white_texture(Color::WHITE()),
	  clear_texture(Color::TRANSPARENT()),
	  unit_tri_cache(this, unit_tri_verts, 8, unit_tri_indices, 6, unit_tri_tex_coords)
{
	vgShader.use();
	vgShader.setInt("render_texture", 0);
	vgShader.setInt("mask_texture", 1);
}
