#version 300 es

in vec3 a_position;
in vec3 a_color;
in vec2 a_tex_coord;

out vec3 our_color;
out vec2 tex_coord;

uniform mat4 transform;

void main(void) {
        gl_Position = transform * vec4(a_position, 1.0);
	our_color = a_color;
	tex_coord = a_tex_coord;
}
