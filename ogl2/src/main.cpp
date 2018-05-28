#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <SDL_opengles2.h>
#include "SDL.h"
#include "shader.h"
#include "img_load.h"

int winWidth, winHeight;
SDL_Window* window;

Shader program;
GLuint vertexbuffer;
GLuint indicesbuffer;
GLuint texture1;
GLuint texture2;

// An array of 3 vectors which represents 3 vertices
static const GLfloat g_vertex_buffer_data[] = {
	//-1.0f, 1.0f // left top
	//1.0f, 1.0f, // right top
	//1.0f,  -1.0f // right bot
	// positions			// colors			// texture coords
	-0.5f, 0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	0.0f, 0.0f, // left top
	0.5f, 0.9f, 0.0f,		1.0f, 0.0f, 0.0f,	2.0f, 0.0f, // right top
	0.5f,  -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	2.0f, 2.0f, // right bot
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 2.0f, // left bot
};

static const GLuint g_vertex_indices[] = {
	0, 1, 2, // first triangle
	0, 2, 3 // second triangle
};

void initGL()
{
	program = Shader("tex.vert", "tex.frag");

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &indicesbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_vertex_indices), g_vertex_indices, GL_DYNAMIC_DRAW);

	// make textures
	int width, height;
	unsigned char* texData = loadImg("tux.bmp", &width, &height);

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	free(texData);

	texData = loadImg("awesomeface.bmp", &width, &height);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

	program.use();

	program.setInt("texture1", 0);
	program.setInt("texture2", 1);

	free(texData);
}

void render(int width, int height)
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
	glUniform3f(program.getUniformLocation("offset_pos"), botValue-0.5f, 0.0f, 0.0f);
	glUniform1f(program.getUniformLocation("fade"), botValue);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	long int csize = sizeof(float)*3;
	glBufferSubData(GL_ARRAY_BUFFER, csize, sizeof(float), &botValue);
	glBufferSubData(GL_ARRAY_BUFFER, csize+(8+1)*sizeof(float) + sizeof(float), sizeof(float), &midValue);
	glBufferSubData(GL_ARRAY_BUFFER, csize+(8*2+1)*sizeof(float), sizeof(float), &topValue);
	// position
	glVertexAttribPointer(
	   program.getAttribLocation("a_position"), // attribute to set
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   8*sizeof(float),    // stride: vec3f (position) + vec3f (color), + vec2f (tex) 8 floats
	   (void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(program.getAttribLocation("a_position"));
	// color
	glVertexAttribPointer(
				program.getAttribLocation("a_color"), // attribute to set
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				8*sizeof(float),	// stride
				(void*)(3*sizeof(float))	 // color is after first vec3f position
				);
	glEnableVertexAttribArray(program.getAttribLocation("a_color"));
	// texture
	glVertexAttribPointer(
				program.getAttribLocation("a_tex_coord"), // attribute to set
				2,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				8*sizeof(float),	// stride
				(void*)(6*sizeof(float))	 // tex coord is after vec3f position & vec3f color
				);
	glEnableVertexAttribArray(program.getAttribLocation("a_tex_coord"));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	//glUniform1i(program.getUniformLocation("our_texture"), GL_TEXTURE1);

	// Draw the triangle !
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesbuffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle

	glDisableVertexAttribArray(program.getAttribLocation("a_position"));
	glDisableVertexAttribArray(program.getAttribLocation("a_color"));
	glDisableVertexAttribArray(program.getAttribLocation("a_tex_coord"));
}

void main_loop_iteration()
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch(event.type)
	{
	case SDL_QUIT:
		exit(0);
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
