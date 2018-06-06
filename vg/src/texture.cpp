#include "texture.h"

#include "context.h"

Texture::Texture(Context* ctx, int width, int height, bool is_dummy)
	: ctx(ctx), width(width), height(height),
	  is_multisampled(ctx->internal_msaa_level > 0 && !is_dummy),
	  is_cleared(true), is_msaa_blit(true),
	  is_premultiplied(false), is_dummy(is_dummy)
{
	if (is_dummy)
	{
		framebuffer_id = 0;
		// didn't create ourselves, not guaranteed to be cleared
		is_cleared = false;
		return;
	}

	init(width, height, 0);
	if (is_multisampled)
		initMultiSampled(width, height);
}

Texture::Texture(Context* ctx, Color solidColor)
	: ctx(ctx), width(1), height(1),
	  is_multisampled(false), is_cleared(true),
	  is_msaa_blit(true), is_premultiplied(false),
	  is_dummy(false)
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

	glGenRenderbuffers(1, &multisampled_renderbuffer_id);
	glBindRenderbuffer(GL_RENDERBUFFER, multisampled_renderbuffer_id);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, min(ctx->internal_msaa_level, max_samples), GL_RGBA8, width, height);

	// frame buffer for rendering to this texture
	glGenFramebuffers(1, &multisampled_framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, multisampled_framebuffer_id);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, multisampled_renderbuffer_id);

	// make sure uninitialized renderbufferStorage is cleared..
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
}

Texture::~Texture()
{
	if (is_dummy)
		return;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDeleteTextures(1, &texture_id);
	glDeleteFramebuffers(1, &framebuffer_id);

	if (is_multisampled)
	{
		glDeleteRenderbuffers(1, &multisampled_renderbuffer_id);
		//glDeleteTextures(1, &multisampled_texture_id);
		glDeleteFramebuffers(1, &multisampled_framebuffer_id);
	}
}

GLuint Texture::getFramebuffer()
{
	if (is_dummy)
		return 0;

	if (is_multisampled && !is_msaa_blit)
		blitToFramebuffer(framebuffer_id);
	return framebuffer_id;
}

void Texture::blitToFramebuffer(GLuint id)
{
	if (is_dummy)
		return;

	// always need to resolve MSAA fbo before blitting to anything else
	if (is_multisampled && !is_msaa_blit && !is_cleared)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampled_framebuffer_id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_id);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	if (id != framebuffer_id)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
}

void Texture::clear()
{
	if (is_cleared)
		return;

	glViewport(0,0, width,height);
	glClearColor(0,0,0,0);

	// bind multisample & regular fb and glClear()
	if (is_multisampled)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, multisampled_framebuffer_id);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
	glClear(GL_COLOR_BUFFER_BIT);

	is_cleared = true;
	is_msaa_blit = true;
	is_premultiplied = false;
}

void Texture::bindToMask(int id)
{
	if (is_dummy)
		return;

	if (is_multisampled)
		blitToFramebuffer(framebuffer_id);

	if (id == 1)
		glActiveTexture(GL_TEXTURE1);
	else if (id == 2)
		glActiveTexture(GL_TEXTURE2);
	else if (id == 3)
		glActiveTexture(GL_TEXTURE3);
	else// if (id == 4)
		glActiveTexture(GL_TEXTURE4);

	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Texture::bindToTexture()
{
	if (is_dummy)
	{
		ctx->getVgShader().setBool("has_texture", false);
		return;
	}
	ctx->getVgShader().setBool("has_texture", true);

	if (is_multisampled)
		blitToFramebuffer(framebuffer_id);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Texture::bindToRenderTarget(bool premultiplying)
{
	is_premultiplied = premultiplying || is_premultiplied;

	glBindFramebuffer(GL_FRAMEBUFFER, is_multisampled? multisampled_framebuffer_id:framebuffer_id);
	glViewport(0,0, width,height);

	is_cleared = false; // about to be rendered to, not guaranteed to be cleared
	is_msaa_blit = false;
}
