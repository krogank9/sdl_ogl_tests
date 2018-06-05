#ifndef TEXTURE_H
#define TEXTURE_H

#include <GLES3/gl3.h>

#include "color.h"
#include "render_name_list.h"

class Context;

class Texture
{
public:
	Texture(Context* ctx, int width, int height);
	Texture(Context* ctx, Color solidColor);

	~Texture();

	void bindToMask();
	void bindToTexture();
	void bindToRenderTarget(bool premultiplying=true);

	void blitToFramebuffer(GLuint id);
	GLuint getFramebuffer();
	void clear();

	bool isPremultiplied() { return is_premultiplied; }

	const int width;
	const int height;

	bool is_multisampled;

private:
	void init(int width, int height, uint32_t* pixels);
	void initMultiSampled(int width, int height);

	GLuint framebuffer_id;
	GLuint texture_id;
	GLuint multisampled_framebuffer_id;
	GLuint multisampled_renderbuffer_id;

	float scale_uv; // allow for supersampling

	bool is_cleared;
	bool is_msaa_blit;
	bool is_premultiplied;

	Context* ctx;
};

#endif // TEXTURE_H
