#ifndef VG_CONTEXT_H
#define VG_CONTEXT_H

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

public:
	Context();

	Shader& getVgShader() { return vg_shader; }
	uint64_t getRenderFrame() { return render_frame_num; }
	void clear() { ++render_frame_num; }

	const Texture white_texture;
	const Texture grey_texture;
	const Texture clear_texture;
	const TriCache unit_tri_cache;
};

#endif // VG_CONTEXT_H
