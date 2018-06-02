#version 310 es

in vec2 a_position;
in vec2 a_tex_coord;

out vec2 tex_coord;
out vec2 screen_pos;

uniform mat4 screen_transform;
uniform mat4 full_transform;

void main(void) {
    vec4 screen_pos4 = screen_transform * vec4(a_position, 0.0, 1.0);
    gl_Position = full_transform * vec4(a_position, 0.0, 1.0);
    screen_pos = screen_pos4.xy;
    tex_coord = a_tex_coord;
}
