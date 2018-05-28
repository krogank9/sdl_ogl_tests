attribute vec3 a_position;
attribute vec3 a_color;
attribute vec2 a_tex_coord;

varying vec3 our_color;
varying vec2 tex_coord;

uniform mat4 transform;

void main(void) {
        gl_Position = transform * vec4(a_position, 1.0);
	our_color = a_color;
	tex_coord = a_tex_coord;
}
