attribute vec2 a_position;

uniform mat4 transform;

void main(void) {
    gl_Position = transform * vec4(a_position, 0.0, 1.0);
}
