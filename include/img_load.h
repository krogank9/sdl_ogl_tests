#ifndef IMG_LOAD_H
#define IMG_LOAD_H

#include <string>
#include "res_path.h"
#include "SDL.h"
#include "stdint.h"

unsigned char* loadImg(std::string name, int* width, int *height, std::string resFolder="images")
{
	std::string filePath = getResourcePath(resFolder)+name;

	SDL_Log("loading %s", filePath.c_str());

	SDL_Surface* img = SDL_LoadBMP(filePath.c_str());
	long length = (img->w * img->h) * 4;

	(*width) = img->w;
	(*height) = img->h;

	uint32_t* upx = (uint32_t*)img->pixels;

	unsigned char* data = (unsigned char*)malloc(length);
	long pos = 0;
	while (pos < length)
	{
		SDL_GetRGBA( *upx, img->format, data+pos+0, data+pos+1, data+pos+2, data+pos+3 );
		pos += 4;
		upx++;
	}

	SDL_FreeSurface(img);

	return data;
}

#endif // IMG_LOAD_H
