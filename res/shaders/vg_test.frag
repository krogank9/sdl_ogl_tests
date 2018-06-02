#version 310 es

precision mediump float;
precision mediump sampler2DMS;

out vec4 FragColor;

in vec2 screen_pos;
in vec2 tex_coord;

uniform sampler2D texture;
uniform sampler2D mask_texture;

uniform vec2 texture_size;
uniform vec2 mask_texture_size;

uniform float color_multiplier;

void main(void) {
    FragColor = texture2D(texture, tex_coord) * texture2D(mask_texture, screen_pos/mask_texture_size) * color_multiplier;
    //FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
