#include <stdio.h>
#include <stdlib.h>
#include <GLES3/gl3.h>
#include "SDL.h"
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int winWidth, winHeight;
SDL_Window* window;

Shader program;
GLuint vertexbuffer;
GLuint indicesbuffer;

// An array of 3 vectors which represents 3 vertices
static const GLfloat g_vertex_buffer_data[] = {
	// positions
	-0.5f, 0.5f, // left top
	0.5f, 0.5f, // right top
	0.5f,  -0.5f, // right bot
	-0.5f, -0.5f, // left bot
};

static const GLuint g_vertex_indices[] = {
	0, 1, 2, // first triangle
	0, 2, 3 // second triangle
};

void renderQuad(float angle=0.0f, float scaleX=1.0f, float scaleY=1.0f)
{
	glm::mat4 transform(1.0f);
	// grey
	//transform = glm::translate(transform, glm::vec3(0, 0, 0));
	transform = glm::rotate(transform, glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(scaleX, scaleY, 1.f));
	glUniformMatrix4fv(program.getUniformLocation("transform"), 1, GL_FALSE, glm::value_ptr(transform));
	program.setVec4("our_color", 100/255.f, 100/255.f, 100/255.f, 1.f);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesbuffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void initGL()
{
	program = Shader("coords.vert", "coords.frag");

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &indicesbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_vertex_indices), g_vertex_indices, GL_DYNAMIC_DRAW);

	program.use();
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

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// position
	glVertexAttribPointer(program.getAttribLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(program.getAttribLocation("a_position"));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glm::mat4 transform(1.0f);
	// grey
	//transform = glm::translate(transform, glm::vec3(0, 0, 0));
	transform = glm::rotate(transform, glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));
	//transform = glm::scale(transform, glm::vec3(1.f, 1.f, 1.f));
	glUniformMatrix4fv(program.getUniformLocation("transform"), 1, GL_FALSE, glm::value_ptr(transform));
	program.setVec4("our_color", 100/255.f, 100/255.f, 100/255.f, 1.f);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesbuffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// purple
	transform = glm::mat4(1.0f);
	//transform = glm::translate(transform, glm::vec3(0, 0, 0));
	transform = glm::rotate(transform, glm::radians(25.f), glm::vec3(0.0f, 0.0f, 1.0f));
	//transform = glm::scale(transform, glm::vec3(1.f, 1.f, 1.f));
	glUniformMatrix4fv(program.getUniformLocation("transform"), 1, GL_FALSE, glm::value_ptr(transform));
	program.setVec4("our_color", 128/255.f, 0.f, 128/255.f, 0.5f);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// green
	transform = glm::mat4(1.0f);
	//transform = glm::translate(transform, glm::vec3(0, 0, 0));
	transform = glm::rotate(transform, glm::radians(45.f), glm::vec3(0.0f, 0.0f, 1.0f));
	//transform = glm::scale(transform, glm::vec3(1.f, 1.f, 1.f));
	glUniformMatrix4fv(program.getUniformLocation("transform"), 1, GL_FALSE, glm::value_ptr(transform));
	program.setVec4("our_color", 0.f, 1.f, 0.f, 0.5f);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(program.getAttribLocation("a_position"));
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
	window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);//only SDL_WINDOW_OPENGL may suffice

	context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	SDL_GetWindowSize(window, &winWidth, &winHeight);

	while (true)
		main_loop_iteration();

	return 0;
}
