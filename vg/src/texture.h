#ifndef TEXTURE_H
#define TEXTURE_H

#include <GLES3/gl31.h>
#include <GLES3/gl32.h>

#include "color.h"

#define MAKE_PO2(x) (x<2?1: x<4?2: x<8?4:\
					x<16?8: x<32?16: x<64?32:\
					x<128?64: x<256?128: x<512?256:\
					x<1024?512: x<2048?1024: x<4096?2048:\
					x<8192?4096: 8192)

class Context;

class Texture
{
public:
	Texture(Context* ctx, int width, int height, bool is_dummy=false);
	Texture(Context* ctx, Color solidColor);

	~Texture();

	void bindToMask() const;
	void bindToTexture() const;
	void bindToRenderTarget();

	const int width;
	const int height;
	const bool is_dummy;

	const bool is_multisampled;

private:
	void init(int width, int height, uint32_t* pixels);
	void initMultiSampled(int width, int height);

	GLuint framebuffer_id;
	GLuint texture_id;
	GLuint multisampled_framebuffer_id;
	GLuint multisampled_texture_id;

	uint64_t render_frame_num;

	float scale_uv; // allow for supersampling

	Context* ctx;
};

#endif // TEXTURE_H
