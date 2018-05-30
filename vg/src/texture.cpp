#include "texture.h"

Texture::Texture(int width, int height, bool is_dummy)
	: width(width), height(height), is_dummy(is_dummy)
{
	// dummy texture for rendering to screen
	if (is_dummy)
		framebuffer_ID = 0;
	else
		init(width, height, 0);
}

Texture::Texture(Color solidColor) : width(1), height(1), is_dummy(false)
{
	uint32_t colorByte = solidColor.getRGBA();
	init(1, 1, &colorByte);
}

void Texture::init(int width, int height, uint32_t* pixels)
{
	// texture
	glGenTextures(1, &texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexImage2D(GL_TEXTURE_2D, 0/*mipmap level*/, GL_RGBA, width, height,
				 0/*border*/, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// frame buffer for rendering to this texture
	glGenFramebuffers(1, &framebuffer_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_ID, 0);
}

Texture::~Texture()
{
	if (framebuffer_ID != 0)
	{
		glDeleteTextures(1, &texture_ID);
		glDeleteFramebuffers(1, &framebuffer_ID);
	}
}

void Texture::bindToMask() const
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
}

void Texture::bindToTexture() const
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
}

void Texture::bindToRenderTarget() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_ID);
}
