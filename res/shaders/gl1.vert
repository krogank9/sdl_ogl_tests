attribute vec3 a_position;
attribute vec3 a_color;

uniform vec3 offset_pos;
varying vec3 ourColor;

void main(void) {
        gl_Position = vec4(a_position + offset_pos, 1.0);
	ourColor = a_color;
}
