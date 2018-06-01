attribute vec2 a_position;
attribute vec2 a_tex_coord;

varying vec2 tex_coord;

uniform mat4 transform;

void main(void) {
        gl_Position = transform * vec4(a_position, 0.0, 1.0);
	tex_coord = a_tex_coord;
}
