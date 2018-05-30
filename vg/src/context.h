#ifndef VG_CONTEXT_H
#define VG_CONTEXT_H

#include "utils.h"
#include "texture.h"
#include "tri_cache.h"

class Context
{
private:
	Shader vgShader;

	int viewport_width;
	int viewport_height;
public:
	Context();

	Shader& getVgShader() { return vgShader; }

	const Texture white_texture;
	const Texture clear_texture;
	const TriCache unit_tri_cache;
};

#endif // VG_CONTEXT_H
