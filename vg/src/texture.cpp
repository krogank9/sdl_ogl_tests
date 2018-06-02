#include "texture.h"

#include "context.h"

Texture::Texture(Context* ctx, int width, int height, bool is_dummy)
	: ctx(ctx), width(width), height(height), is_dummy(is_dummy), is_multisampled(!is_dummy)
{
	// dummy texture for rendering to screen
	if (is_dummy)
		framebuffer_id = 0;
	else
	{
		init(width, height, 0);
		initMultiSampled(width, height);
	}
}

Texture::Texture(Context* ctx, Color solidColor)
	: ctx(ctx), width(1), height(1), is_dummy(false), is_multisampled(false)
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
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
	//			 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	// this does not work, even though wiki says they are equivalent. I'm using GLES3.
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixels);

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
	// texture
	glGenTextures(1, &multisampled_texture_id);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampled_texture_id);

	glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, ctx->internal_msaa_level, GL_RGBA8, width, height, false);
	//glTexImage2D(GL_TEXTURE_2D_MULTISAMPLE, 0/*mipmap level*/, GL_RGBA, width, height,
			//	 0/*border*/, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixels);

	// frame buffer for rendering to this texture
	glGenFramebuffers(1, &multisampled_framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, multisampled_framebuffer_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampled_texture_id, 0);

	// make sure uninitialized texStorage is cleared.. screen is green if I don't do this
	glClear(GL_COLOR_BUFFER_BIT);
}

Texture::~Texture()
{
	if (!is_dummy)
	{
		glDeleteTextures(1, &texture_id);
		glDeleteFramebuffers(1, &framebuffer_id);

		if (is_multisampled)
		{
			glDeleteTextures(1, &multisampled_texture_id);
			glDeleteFramebuffers(1, &multisampled_framebuffer_id);
		}
	}
}

void Texture::bindToMask() const
{
	if (is_dummy)
		return;

	if (is_multisampled)
	{
		// blit multisampled fbo to regular fbo
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_id);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampled_framebuffer_id);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Texture::bindToTexture() const
{
	if (is_dummy)
		return;

	if (is_multisampled)
	{
		// blit multisampled fbo to regular fbo
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_id);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampled_framebuffer_id);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Texture::bindToRenderTarget()
{
	glBindFramebuffer(GL_FRAMEBUFFER, is_multisampled? multisampled_framebuffer_id:framebuffer_id);
	glViewport(0,0, width,height);

	if (render_frame_num != ctx->getRenderFrame())
	{
		render_frame_num = ctx->getRenderFrame();
		glClear(GL_COLOR_BUFFER_BIT);
	}
}
