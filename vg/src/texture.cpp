#include "texture.h"

#include "context.h"

Texture::Texture(Context* ctx, int width, int height)
	: ctx(ctx), width(width), height(height), is_multisampled(ctx->msaa_level > 0), is_cleared(false)
{
	init(width, height, 0);
	if (ctx->msaa_level > 0)
		initMultiSampled(width, height);
}

Texture::Texture(Context* ctx, Color solidColor)
	: ctx(ctx), width(1), height(1), is_multisampled(false), is_cleared(false)
{
	uint32_t colorByte = solidColor.getRGBA();
	init(1, 1, &colorByte);
}

void Texture::init(int width, int height, uint32_t* pixels)
{
	// texture
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	// this works:
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
				 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glGenerateMipmap(GL_TEXTURE_2D);

	// frame buffer for rendering to this texture
	glGenFramebuffers(1, &framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);
}

void Texture::initMultiSampled(int width, int height)
{
	int max_samples = 0;
	glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
	// texture
	//glGenTextures(1, &multisampled_texture_id);
	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampled_texture_id);

	glGenRenderbuffers(1, &multisampled_renderbuffer_id);
	glBindRenderbuffer(GL_RENDERBUFFER, multisampled_renderbuffer_id);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, min(ctx->msaa_level, max_samples), GL_RGBA8, width, height);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);

	//glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, ctx->internal_msaa_level, GL_RGBA8, width, height, false);
	//glTexImage2D(GL_TEXTURE_2D_MULTISAMPLE, 0/*mipmap level*/, GL_RGBA, width, height,
			//	 0/*border*/, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixels);

	// frame buffer for rendering to this texture
	glGenFramebuffers(1, &multisampled_framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, multisampled_framebuffer_id);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, multisampled_renderbuffer_id);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampled_texture_id, 0);

	// make sure uninitialized texStorage is cleared.. screen is green if I don't do this
	glClear(GL_COLOR_BUFFER_BIT);
}

Texture::~Texture()
{
	glDeleteTextures(1, &texture_id);
	glDeleteFramebuffers(1, &framebuffer_id);

	if (is_multisampled)
	{
		glDeleteRenderbuffers(1, &multisampled_renderbuffer_id);
		//glDeleteTextures(1, &multisampled_texture_id);
		glDeleteFramebuffers(1, &multisampled_framebuffer_id);
	}
}

void Texture::blitToFramebuffer(GLuint id)
{
	if (id != framebuffer_id)
	{
		blitToFramebuffer(framebuffer_id);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
	}
	else if (is_multisampled)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampled_framebuffer_id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
	}
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Texture::clear()
{
	if (is_cleared)
		return;

	//GLint save_bound_framebuffer = 0;
	//glGetIntegerv(GL_FRAMEBUFFER_BINDING, &save_bound_framebuffer);

	glViewport(0,0, width,height);
	// bind multisample & regular fb and glClear()
	if (is_multisampled)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, multisampled_framebuffer_id);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
	glClear(GL_COLOR_BUFFER_BIT);

	//glBindFramebuffer(GL_FRAMEBUFFER, save_bound_framebuffer);

	is_cleared = true;
}

void Texture::bindToMask()
{
	if (is_multisampled)
		blitToFramebuffer(framebuffer_id);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Texture::bindToTexture()
{
	if (is_multisampled)
		blitToFramebuffer(framebuffer_id);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Texture::bindToRenderTarget()
{
	glBindFramebuffer(GL_FRAMEBUFFER, is_multisampled? multisampled_framebuffer_id:framebuffer_id);
	glViewport(0,0, width,height);

	is_cleared = false; // about to be rendered to, not guaranteed to be cleared
}
