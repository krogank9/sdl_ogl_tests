#version 310 es

in vec2 a_position;
in vec2 a_tex_coord;

out vec2 tex_coord;

uniform mat4 transform;

void main(void) {
	gl_Position = transform * vec4(a_position, 0.0, 1.0);
	tex_coord = a_tex_coord;
}
