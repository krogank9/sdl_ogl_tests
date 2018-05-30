#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

#define RES_PATH_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <SDL_opengles2.h>
#include "SDL.h"
#include "img_load.h"
#include "context.h"
#include "tri_cache.h"
#include "texture.h"

int winWidth, winHeight;
SDL_Window* window;

Context* ctx;
TriCache* testCache;
Texture* dummyTexture = new Texture(0,0,true);
Texture* dummyCopy = new Texture(0,0);
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

	delete dummyTexture;
	dummyTexture = new Texture(winWidth, winHeight, true);

	delete dummyCopy;
	dummyCopy = new Texture(dummyTexture->width, dummyTexture->height);

	redTexture = new Texture(Color(255, 0, 0, 255));
	blueTexture = new Texture(Color(0,0,255,255));
	greenTexture = new Texture(Color::GREEN());

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

	//testCache->render(vec2f(100.f, 100.f), vec2f(1.f,1.f), glm::radians(0.f), *greenTexture, *dummyTexture, ctx->white_texture);
	//ctx->unit_tri_cache.render(vec2f(1024/2.f, 800/2.f), vec2f(100.f,100.f), glm::radians(15.f), *greenTexture, *dummyTexture, ctx->white_texture);

	//ctx->unit_tri_cache.render(vec2f(0.f, 0.f), vec2f((float)dummyTexture->width,(float)dummyTexture->height), glm::radians(0.f), *greenTexture, *dummyTexture, ctx->white_texture);
	ctx->unit_tri_cache.render(vec2f(dummyCopy->width/2.f, dummyCopy->height/2.f), vec2f(dummyCopy->width/2.f,dummyCopy->height/2.f), glm::radians(45.f), *greenTexture, *dummyCopy, ctx->white_texture);
	//ctx->unit_tri_cache.render(vec2f(dummyCopy->width/2.f, dummyCopy->height/2.f), vec2f(dummyCopy->width/1.5f,dummyCopy->height/1.5f), glm::radians(0.f), *redTexture, *dummyCopy, ctx->white_texture);
	ctx->unit_tri_cache.render(vec2f(dummyTexture->width/2.f, dummyTexture->height/2.f), vec2f(dummyTexture->width/2.f,dummyTexture->height), glm::radians(0.f), *dummyCopy, *dummyTexture, ctx->white_texture);
	//ctx->unit_tri_cache.render(vec2f(dummyTexture->width/2.f, dummyTexture->height/2.f), vec2f(dummyCopy->width/1.5f,dummyCopy->height/1.5f), glm::radians(0.f), *redTexture, *dummyCopy, ctx->white_texture);
	//ctx->unit_tri_cache.render(vec2f(0.f, 0.f), vec2f((float)dummyTexture->width,(float)dummyTexture->height), glm::radians(0.f), *dummyCopy, *dummyTexture, ctx->white_texture);

	//testCache->render(vec2f(), vec2f(1,1), glm::radians(5.f), *redTexture, *dummyTexture, ctx->white_texture);
	//testCache->render(vec2f(), vec2f(0.5,0.5), glm::radians(5.f), ctx->white_texture, *dummyTexture, ctx->white_texture, -1.0f);
	//testCache->render(vec2f(), vec2f(), 0, *redTexture, *dummyTexture, ctx->white_texture);
}

void main_loop_iteration()
{
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
				glViewport(0,0, winWidth, winHeight);
				std::cout << "resized" << std::endl;
				delete dummyTexture;
				dummyTexture = new Texture(winWidth, winHeight, true);
				delete dummyCopy;
				dummyCopy = new Texture(winWidth, winHeight);
				break;
			case SDL_WINDOWEVENT_CLOSE:
				std::cout << "closed" << std::endl;
			}
			break;
		}
	}

	glClear( GL_COLOR_BUFFER_BIT );

	render(winWidth, winHeight);

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
