#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include <GLES3/gl32.h>

#include "utils.h"
#include "context.h"
#include "tri_cache.h"
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

#define WIN_NAME "alpha_test"

void initGL()
{
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
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

	ctx->getScreenQuad().render(vec2f(ctx->getViewportWidth()/2.f, ctx->getViewportHeight()/2.f), vec2f(1,1), 0.f, ctx->green_texture, ctx->white_texture, "");

	ctx->getUnitQuad().render(vec2f(ctx->getViewportWidth()/2.f, ctx->getViewportHeight()/2.f), vec2f(ctx->getViewportWidth()/2.f,ctx->getViewportHeight()/2.f), 0.f, ctx->grey_texture, ctx->white_texture, RenderNameList("fg1", Color(1.f, 1.f, 1.f, 1.0f)));
	ctx->getUnitQuad().render(vec2f(ctx->getViewportWidth()/2.f, ctx->getViewportHeight()/2.f), vec2f(ctx->getViewportWidth()/2.f,ctx->getViewportHeight()/2.f), 0.5f, ctx->purple_texture, ctx->white_texture, RenderNameList("fg2", Color(1.f, 1.f, 1.f, 1.0f)));

	//ctx->getScreenQuad().render(vec2f(ctx->getViewportWidth()/2.f, ctx->getViewportHeight()/2.f), vec2f(1,1), 0.f, ctx->getRenderTexture("green_bg"), ctx->white_texture, "");
	//ctx->getScreenQuad().render(vec2f(ctx->getViewportWidth()/2.f, ctx->getViewportHeight()/2.f), vec2f(1,1), 0.f, ctx->getRenderTexture("fg1"), ctx->white_texture, "");

	ctx->getScreenQuad().render(vec2f(ctx->getViewportWidth()/2.f, ctx->getViewportHeight()/2.f), vec2f(1,1), 0.f, ctx->getRenderTexture("fg2"), ctx->white_texture, "fg1");
	ctx->getScreenQuad().render(vec2f(ctx->getViewportWidth()/2.f, ctx->getViewportHeight()/2.f), vec2f(1,1), 0.f, ctx->getRenderTexture("fg1"), ctx->white_texture, "");

	//ctx->getScreenQuad().render(vec2f(ctx->getViewportWidth()/2.f, ctx->getViewportHeight()/2.f), vec2f(1,1), 0.f, ctx->getRenderTexture("fgg"), ctx->white_texture, "");

	ctx->getRenderTexture("").blitToFramebuffer(0);
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
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
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

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	Context ctx(winWidth, winHeight);
	glDepthMask(false);
	while (true)
		main_loop_iteration(&ctx);

	return 0;
}
