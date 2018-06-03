#ifndef VG_CONTEXT_H
#define VG_CONTEXT_H

#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <map>

#include "utils.h"
/*
class Context
{
private:
	// only need one shader per application
	static Shader* vg_shader;
	static int num_ctx;

	int viewport_width;
	int viewport_height;

	uint64_t render_frame_num;

public:
	Context(int viewport_width=0, int viewport_height=0, int msaa_level=8);
	~Context();

	Shader& getVgShader() { return *vg_shader; }
	uint64_t getRenderFrame() { return render_frame_num; }
	void clear()
	{
	}
	void updateViewportSize(int width, int height)
	{
		viewport_width = width;
		viewport_height = height;
		//delete screen_quad;
		//screen_quad = TriCache::makeQuad(this, viewport_width, viewport_height);
	}
	int getViewportWidth() { return viewport_width; }
	int getViewportHeight() { return viewport_height; }

	const int msaa_level;
};
*/
#endif // VG_CONTEXT_H
