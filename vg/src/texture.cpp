#include "texture.h"

#include "context.h"

Texture::Texture(Context* ctx, int width, int height, bool is_dummy)
	: ctx(ctx), width(width), height(height), is_dummy(is_dummy)
{
	// dummy texture for rendering to screen
	if (is_dummy)
		framebuffer_id = 0;
	else
		init(width, height, 0);
}

Texture::Texture(Context* ctx, Color solidColor)
	: ctx(ctx), width(1), height(1), is_dummy(false)
{
	uint32_t colorByte = solidColor.getRGBA();
	init(1, 1, &colorByte);
}

void Texture::init(int width, int height, uint32_t* pixels)
{
	// texture
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_id);
	glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA, width, height, false);
	//glTexImage2D(GL_TEXTURE_2D_MULTISAMPLE, 0/*mipmap level*/, GL_RGBA, width, height,
			//	 0/*border*/, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixels);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glGenerateMipmap(GL_TEXTURE_2D);

	// frame buffer for rendering to this texture
	glGenFramebuffers(1, &framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_id, 0);
}

Texture::~Texture()
{
	if (framebuffer_id != 0)
	{
		glDeleteTextures(1, &texture_id);
		glDeleteFramebuffers(1, &framebuffer_id);
	}
}

void Texture::bindToMask() const
{
	if (!is_dummy)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_id);
	}
}

void Texture::bindToTexture() const
{
	if (!is_dummy)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_id);
	}
}

void Texture::bindToRenderTarget()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
	glViewport(0,0, width,height);
	if (render_frame_num != ctx->getRenderFrame())
	{
		render_frame_num = ctx->getRenderFrame();
		glClear(GL_COLOR_BUFFER_BIT);
	}
}
