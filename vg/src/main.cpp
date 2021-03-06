#ifdef EMSCRIPTEN
	#include <emscripten/emscripten.h>
	#include <emscripten/html5.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#ifndef EMSCRIPTEN
	#include <GLES3/gl32.h>
#else
	#include <GLES3/gl3.h>
#endif

#include "utils.h"
#include "context.h"
#include "tri_cache.h"
#include "line_cache.h"
#include "texture.h"
#include "render_name_list.h"

int winWidth, winHeight;
SDL_Window* window;

#include <vector>
#include <iostream>
std::vector<int> all_calcs;
Uint32 last_calc(0);
Uint32 last_print_time(0);

void calc_FPS()
{
	Uint32 now = SDL_GetTicks();
	Uint32 elapsed = now - last_calc;
	last_calc = now;

	all_calcs.push_back(elapsed);
	if (all_calcs.size()>100)
		all_calcs.erase(all_calcs.begin());

	if (now-last_print_time > 1000 && all_calcs.size() > 0)
	{
		Uint32 avg_frame_time = 0;
		for (unsigned int i=0; i<all_calcs.size(); i++)
			avg_frame_time += all_calcs[i];
		float FPS = 1000.f/(avg_frame_time / all_calcs.size());

		last_print_time = now;
		std::cout << "FPS: " << FPS << std::endl;
	}
}

#ifndef EMSCRIPTEN
void MessageCallback( GLenum ,//source,
				 GLenum type,
				 GLuint ,//id,
				 GLenum severity,
				 GLsizei ,//length,
				 const GLchar* message,
				 const void* )//userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		   ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
			type, severity, message );
}
#endif

#define WIN_NAME "vg"

void initGL()
{
#ifndef EMSCRIPTEN
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
#endif
}

void render(Context* ctx)
{
	static bool inited = false;
	if( !inited )
	{
		inited = true;
		initGL();
	}

	/////////// loop
	ctx->clear();

	float sin_slow = sin(SDL_GetTicks()/3000.f);

	float spin_rads = (sin(SDL_GetTicks()/12000.f) * M_PI*2) - M_PI;
	float spin_rads_fast = (sin(SDL_GetTicks()/6000.f) * M_PI*2) - M_PI;
	float spin_rads_slow = (sin(SDL_GetTicks()/24000.f) * M_PI*2) - M_PI;

	//sin_slow = 0.5f;
	//spin_rads = 0.5f;
	//spin_rads_slow = 0.3f;

	//ctx->getUnitQuad().render(ctx->getViewportSize()/2.f, vec2f(100.f, 100.f), 0.3f, ctx->empty_texture, RenderNameList(), "");
	//ctx->getScreenQuad().render(ctx->getViewportSize()/2.f, vec2f(1.f, 1.f), 0.f, ctx->getRenderTexture("abc"), RenderNameList(), "");

	std::vector<vec2f> verts;
	verts.push_back(vec2f(100,100));
	verts.push_back(vec2f(200,200));
	verts.push_back(vec2f(400,200));
	//verts.push_back(vec2f(800,300));
	LineCache lc(ctx, verts);
	lc.getLine(10, LineCache::NONE).render(vec2f(), vec2f(1,1), 0.f, ctx->white_texture, RenderNameList(), "");
}

void main_loop_iteration(void* v_ctx)
{
	Context* ctx = (Context*)v_ctx;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_QUIT:
			std::cout << "quit" << std::endl;
			exit(0);
			break;
		case SDL_WINDOWEVENT:
			switch(event.window.event)
			{
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				SDL_GetWindowSize(window, &winWidth, &winHeight);
				std::cout << "resized" << std::endl;
				ctx->updateViewportSize(winWidth, winHeight);
				break;
			case SDL_WINDOWEVENT_CLOSE:
				std::cout << "closed" << std::endl;
			}
			break;
		}
	}

	render(ctx);
	calc_FPS();

	SDL_GL_SwapWindow(window);//we have double bufferring by default, so make sure our changes get on the screen
}

int main()
{
	int flags = SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER | SDL_INIT_HAPTIC);//only SDL_INIT_EVERYTHING may suffice
	if (SDL_Init(flags) < 0)//if it doesn't work, try and log a helpful error
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error: sdl2 init failed: %s", SDL_GetError());
		return 1;
	}
	//SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
	window = SDL_CreateWindow(WIN_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 800,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);//only SDL_WINDOW_OPENGL may suffice
	if (!window)//if it doesn't work, lower the bar
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
		window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 800,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);//same as above
		if (!window)//if this doesn't work, it's probably becouse of an error. so we log it
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error2: sdl2 init failed: %s", SDL_GetError());
			return 1;
		}
	}

	SDL_GetWindowSize(window, &winWidth, &winHeight);

#ifdef EMSCRIPTEN
	EmscriptenWebGLContextAttributes attrs;
	attrs.antialias = true;
	attrs.majorVersion = 2;
	attrs.minorVersion = 0;
	attrs.alpha = false;
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webgl_context = emscripten_webgl_create_context(0, &attrs);
	emscripten_webgl_make_context_current(webgl_context);

	Context ctx(winWidth, winHeight, 4);
	glDepthMask(false);

	emscripten_set_main_loop_arg((em_arg_callback_func)main_loop_iteration, &ctx, 0, 1);
#else
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	Context ctx(winWidth, winHeight);
	glDepthMask(false);
	while (true)
		main_loop_iteration(&ctx);
#endif

	return 0;
}
