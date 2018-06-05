attribute vec2 a_position;
attribute vec2 a_tex_coord;

varying vec2 tex_coord;
varying vec2 screen_pos;

uniform mat4 world_transform; // pixel coordinate scale/rotate/translate
uniform mat4 ndc_transform; // normalize to NDC

void main(void) {
    vec4 screen_pos4 = world_transform * vec4(a_position, 0.0, 1.0);
    gl_Position = ndc_transform * screen_pos4;
    screen_pos = screen_pos4.xy;
    tex_coord = a_tex_coord;
}
