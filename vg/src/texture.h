#ifndef TEXTURE_H
#define TEXTURE_H

#include "SDL_opengles2.h"
#include "color.h"

class Texture
{
public:
	Texture(int width, int height, bool is_dummy=false);
	Texture(Color solidColor);

	~Texture();

	void bindToMask() const;
	void bindToTexture() const;
	void bindToRenderTarget() const;

	const int width;
	const int height;
	const bool is_dummy;

private:
	void init(int width, int height, uint32_t* pixels);

	GLuint framebuffer_ID;
	GLuint texture_ID;
};

#endif // TEXTURE_H
