precision mediump float;

//out vec4 FragColor;

varying vec2 screen_pos;
varying vec2 tex_coord;

uniform sampler2D texture;
uniform sampler2D mask_texture;

uniform vec2 texture_size;
uniform vec2 mask_texture_size;

uniform vec4 render_color_mask;

void main(void) {
    vec4 mask_color = texture2D(mask_texture, screen_pos/mask_texture_size);
    vec4 texture_color = texture2D(texture, tex_coord/texture_size) * render_color_mask;
    gl_FragColor = mask_color * texture_color;
}
