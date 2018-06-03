#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

#define RES_PATH_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <GLES3/gl3.h>
#include "SDL.h"
#include "shader.h"

SDL_Window *window;

Shader program;
GLuint vertexbuffer;

void initGL()
{
	program = Shader("gl1.vert", "gl1.frag");
	// An array of 3 vectors which represents 3 vertices
	static const GLfloat g_vertex_buffer_data[] = {
		//-1.0f, 1.0f // left top
		//1.0f, 1.0f, // right top
		//1.0f,  -1.0f // right bot
		// positions			// colors
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 0.0f, // bot right
		-0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f, // bot left
		0.0f,  0.5f, 0.0f,		0.0f, 0.0f, 1.0f  // top middle
	};

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);
}

void render()
{
	static bool inited = false;
	if( !inited )
	{
		inited = true;
		initGL();
	}
	program.use();

	/////////// loop
	glDepthMask(false);

	float timeValue = SDL_GetTicks()/1000.f;
	float botValue = sin(timeValue+0)/2.0f + 0.5;
	float midValue = sin(timeValue+1.5f)/2.0f + 0.5;
	float topValue = sin(timeValue+5.f)/2.0f + 0.5;
	//glUniform4f(program.getUniformLocation("ourColor"), greenValue/2, greenValue, greenValue/1.1, 1.0f);
	glUniform3f(program.getUniformLocation("offset_pos"), 0.5f, 0.0f, 0.0f);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	long int csize = sizeof(float)*3;
	glBufferSubData(GL_ARRAY_BUFFER, csize, sizeof(float), &botValue);
	glBufferSubData(GL_ARRAY_BUFFER, csize*3 + sizeof(float), sizeof(float), &midValue);
	glBufferSubData(GL_ARRAY_BUFFER, csize*5 + sizeof(float)*2, sizeof(float), &topValue);
	// position
	glVertexAttribPointer(
	   program.getAttribLocation("a_position"),                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   6*sizeof(float),                  // stride
	   (void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(program.getAttribLocation("a_position"));
	// color
	glVertexAttribPointer(
				program.getAttribLocation("a_color"),                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				6*sizeof(float), // vec3f (position) + vec3f (color), 6 floats
				(void*)(3*sizeof(float))	 // color is after first vec3f position
				);
	glEnableVertexAttribArray(program.getAttribLocation("a_color"));
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(program.getAttribLocation("a_position"));
	glDisableVertexAttribArray(program.getAttribLocation("a_color"));
}

void main_loop_iteration()
{
	SDL_Event event;

	SDL_PollEvent(&event);//are there any interesting events for us?

	switch(event.type)
	{
	case SDL_QUIT:
		exit(0);
	}

	glClear( GL_COLOR_BUFFER_BIT );

	render();

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

	int winWidth, winHeight;

	SDL_GetWindowSize(window, &winWidth, &winHeight);

#ifdef EMSCRIPTEN
	emscripten_set_main_loop((em_callback_func)main_loop_iteration, 0, 1);
#else
	while (true)
		main_loop_iteration();
#endif

	return 0;
}
