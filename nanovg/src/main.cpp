#include <stdio.h>
#include <stdlib.h>
#include <SDL_opengles2.h>
#include "SDL.h"
#define GLFW_INCLUDE_ES2
#define NANOVG_GLES2_IMPLEMENTATION
#include "nanovg.h"
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"

int main(int argc, char **argv) { //SDL entry point, at least on android
  SDL_Window *window = NULL;
  SDL_GLContext context;
  SDL_Event event;
  NVGcontext* vg = NULL;

	int flags = SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER | SDL_INIT_HAPTIC);//only SDL_INIT_EVERYTHING may suffice
	if (SDL_Init(flags) < 0)//if it doesn't work, try and log a helpful error
	{
		//__android_log_print(ANDROID_LOG_ERROR, "SDL-my-main", "error: sdl2 init failed: %s", SDL_GetError());
		return 1;
	}
	//better safe than sorry vv
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
	window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 800,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);//only SDL_WINDOW_OPENGL may suffice
	if (!window)//if it doesn't work, lower the bar
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
		window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 800,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);//same as above
		if (!window)//if this doesn't work, it's probably becouse of an error. so we log it
		{
			//__android_log_print(ANDROID_LOG_ERROR, "SDL-my-main", "error2: sdl2 init failed: %s", SDL_GetError());
			return 1;
		}
	}
	context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	vg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);//initialize nanovg. if it fails, log it. there is no get_error though.
	if (vg == NULL) {
		//__android_log_print(ANDROID_LOG_ERROR, "SDL-my-main", "error: nanovg init failed");
		return 1;
	}

	int winWidth, winHeight;
	int fbWidth, fbHeight;
	float pxRatio;

	SDL_GetWindowSize(window, &winWidth, &winHeight);
	fbWidth=winWidth; fbHeight=winHeight;

	pxRatio = (float)fbWidth / (float)winWidth;

	int quit=0;
	int x = 5;
	bool wrong = false;
	while (!quit)//main loop
	{
		if( !wrong )
			x++;
		else
			x--;

		if( x > 300 )
			wrong = true;
		if( x < 0 )
			wrong = false;

		SDL_PollEvent(&event);//are there any interesting events for us?

		switch(event.type)
		{
		case SDL_QUIT:
		  quit=1;
		  break;
		}

		glClear( GL_COLOR_BUFFER_BIT );

		nvgBeginFrame(vg, winWidth, winHeight, pxRatio);//important, guess it's not like framebuffer

		nvgBeginPath(vg);//this is your masterpiece
		nvgRect(vg, 300+x,100, 120,30);//same
		nvgRect(vg, 300-x,300, 120,30);//same
		nvgFillColor(vg, nvgRGBA(255,0,0,255));//same
		nvgStrokeColor(vg, nvgRGBA(150,0,150,255));//same
		nvgStrokeWidth(vg, 8);
		nvgClosePath(vg);//same
		nvgStroke(vg);//same
		nvgFill(vg);//same

		nvgEndFrame(vg);//if we started a frame, it makes sense to end it
		SDL_GL_SwapWindow(window);//we have double bufferring by default, so make sure our changes get on the screen
	}

	return 0;
}
