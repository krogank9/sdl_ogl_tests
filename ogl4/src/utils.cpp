#include "utils.h"

unsigned char* Utils::loadImg(std::string name, int* width, int *height, std::string resFolder)
{
	std::string filePath = Utils::getResourcePath(resFolder)+name;

	SDL_Log("loading %s", filePath.c_str());

	SDL_Surface* img = SDL_LoadBMP(filePath.c_str());
	long length = (img->w * img->h) * img->format->BytesPerPixel;

	(*width) = img->w;
	(*height) = img->h;

	uint32_t* upx = (uint32_t*)img->pixels;

	unsigned char* data = (unsigned char*)malloc(length);
	long pos = 0;
	while (pos < length)
	{
		SDL_GetRGBA( *upx, img->format, data+pos+0, data+pos+1, data+pos+2, data+pos+3 );
		pos += img->format->BytesPerPixel;
		upx++;
	}

	SDL_FreeSurface(img);

	return data;
}

/*
 * Get the resource path for resources located in res/subDir
 * It's assumed the project directory is structured like:
 * bin/
 *  the executable
 * res/
 *  Lesson1/
 *  Lesson2/
 *
 * Paths returned will be Project_Root/res/subDir
 */
std::string Utils::getResourcePath(const std::string &subDir){
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif
	static std::string baseRes;
	if (baseRes.empty()){
		char *basePath = SDL_GetBasePath();
		if (basePath){
			baseRes = basePath;
			SDL_free(basePath);
		}
		else {
			std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
			return "";
		}
		//We replace the last bin/ with res/ to get the the resource path
		size_t pos = baseRes.rfind("bin");
		baseRes = baseRes.substr(0, pos) + "res" + PATH_SEP;
	}
	return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
}
