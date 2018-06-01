#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

#define RES_PATH_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <GLES3/gl31.h>

#include "img_load.h"
#include "context.h"
#include "tri_cache.h"
#include "texture.h"

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
		for (int i=0; i<all_calcs.size(); i++)
			avg_frame_time += all_calcs[i];
		float FPS = 1000.f/(avg_frame_time / all_calcs.size());

		last_print_time = now;
		std::cout << "FPS: " << FPS << std::endl;
	}
}

class TexturePtr
{
public:
	TexturePtr(const TexturePtr& copy) { counter = copy.counter; (*counter)++; ptr = copy.ptr; }
	TexturePtr(Context* ctx, int width, int height) { ptr = new Texture(ctx, width, height); counter = new int(1); }
	~TexturePtr() { if (*counter == 1) { delete ptr; delete counter; } else (*counter)--; }

	Texture& deref() { return *ptr; }

	Texture* ptr;
	int* counter;
};

#define NUM_COPIES 3
#define WIN_NAME "fx"
#define SSAA_LEVEL 1

Context* ctx;
TriCache* testCache;
Texture* dummyTexture;
Texture* dummyCopy;
//std::vector<TexturePtr> dummyList;
Texture* redTexture;
Texture* blueTexture;
Texture* greenTexture;

GLuint vertexbuffer;
GLuint indicesbuffer;
GLuint texture1;
GLuint texture2;

// An array of 3 vectors which represents 3 vertices
static GLfloat g_vertex_buffer_data[] = {
	//-1.0f, 1.0f // left top
	//1.0f, 1.0f, // right top
	//1.0f,  -1.0f // right bot
	// positions			// colors			// texture coords
	-250.0f, -250.0f, // left top
	250.0f, -250.0f, // right top
	250.0f,  250.0f, // right bot
	-250.0f, 250.0f, // left bot
};

static GLuint g_vertex_indices[] = {
	0, 1, 2, // first triangle
	0, 2, 3 // second triangle
};

void initGL()
{
	ctx = new Context();

	//delete dummyTexture;
	dummyTexture = new Texture(ctx, winWidth, winHeight, true);

	delete dummyCopy;
	dummyCopy = new Texture(ctx, winWidth, winHeight);
//	dummyCopy.clear();
//	for (int level = SSAA_LEVEL; level > 0; level /= 2)
//		dummyCopy.push_back(TexturePtr(ctx, winWidth*level, winHeight*level));

	redTexture = new Texture(ctx, Color(255, 0, 0, 255));
	blueTexture = new Texture(ctx, Color(0,0,255,255));
	greenTexture = new Texture(ctx, Color::GREEN());

	testCache = new TriCache(ctx, g_vertex_buffer_data, 8, g_vertex_indices, 6);
}

void render(int width, int height)
{
	static bool inited = false;
	if( !inited )
	{
		inited = true;
		initGL();
	}

	/////////// loop
	glDepthMask(false);
	ctx->clear();

/*
	//ctx->unit_tri_cache.render(vec2f(dummyCopy->width/2.f, dummyCopy->height/2.f), vec2f(dummyCopy->width,dummyCopy->height), 0.f, ctx->grey_texture, *dummyCopy, ctx->white_texture);
	bool every_other_sec = (SDL_GetTicks()%5000) < 2500;
	ctx->unit_tri_cache.render(vec2f(dummyCopy[0].ptr->width/2.f, dummyCopy[0].ptr->height/2.f), vec2f(dummyCopy[0].ptr->width,dummyCopy[0].ptr->height), 0.f, every_other_sec?ctx->white_texture:ctx->clear_texture, dummyCopy[0].deref(), ctx->white_texture);
	ctx->unit_tri_cache.render(vec2f(dummyCopy[0].ptr->width/2.f, dummyCopy[0].ptr->height/2.f), vec2f(dummyCopy[0].ptr->width,dummyCopy[0].ptr->height), 0.5f, ctx->grey_texture, dummyCopy[0].deref(), ctx->white_texture);
	for (int i=1; i<dummyCopy.size(); i++)
	{
		ctx->unit_tri_cache.render(vec2f(dummyCopy[i].ptr->width/2.f, dummyCopy[i].ptr->height/2.f), vec2f(dummyCopy[i].ptr->width,dummyCopy[i].ptr->height), glm::radians(0.f), dummyCopy[i-1].deref(), dummyCopy[i].deref(), ctx->white_texture);
	}
	//ctx->unit_tri_cache.render(vec2f(dummyTexture->width/2.f, dummyTexture->height/2.f), vec2f(dummyTexture->width,dummyTexture->height), glm::radians(0.f), dummyCopy.back().deref(), *dummyTexture, ctx->white_texture);
	ctx->unit_tri_cache.renderFinal(vec2f(dummyTexture->width/2.f, dummyTexture->height/2.f), vec2f(dummyTexture->width, dummyTexture->height), dummyCopy.back().deref(), *dummyTexture);
*/
	ctx->unit_tri_cache.render(vec2f(dummyCopy->width/2.f, dummyCopy->height/2.f), vec2f(dummyCopy->width,dummyCopy->height), 0.5f, ctx->grey_texture, *dummyCopy, ctx->white_texture);
	ctx->unit_tri_cache.render(vec2f(dummyTexture->width/2.f, dummyTexture->height/2.f), vec2f(dummyTexture->width,dummyTexture->height), 0.f, *dummyCopy, *dummyTexture, ctx->white_texture);
}

void main_loop_iteration()
{
	render(winWidth, winHeight);
	calc_FPS();

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
				delete dummyTexture;
				dummyTexture = new Texture(ctx, winWidth, winHeight, true);
				delete dummyCopy;
				dummyCopy = new Texture(ctx, winWidth, winHeight);

//				dummyCopy.clear();
//				for (int level = SSAA_LEVEL; level > 0; level /= 2)
//					dummyCopy.push_back(TexturePtr(ctx, winWidth*level, winHeight*level));

				break;
			case SDL_WINDOWEVENT_CLOSE:
				std::cout << "closed" << std::endl;
			}
			break;
		}
	}

	SDL_GL_SwapWindow(window);//we have double bufferring by default, so make sure our changes get on the screen
}

int main()
{
	SDL_GLContext context;

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
	context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	int fbWidth, fbHeight;
	float pxRatio;

	SDL_GetWindowSize(window, &winWidth, &winHeight);
	fbWidth=winWidth; fbHeight=winHeight;

	pxRatio = (float)fbWidth / (float)winWidth;

#ifdef EMSCRIPTEN
	emscripten_set_main_loop((em_callback_func)main_loop_iteration, 0, 1);
#else
	while (true)
		main_loop_iteration();
#endif

	return 0;
}
