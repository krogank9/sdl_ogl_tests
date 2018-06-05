precision mediump float;

//out vec4 FragColor;

varying vec2 screen_pos;
varying vec2 tex_coord;

uniform sampler2D texture;
uniform sampler2D mask_texture;

uniform vec2 texture_size;
uniform vec2 mask_texture_size;

uniform vec4 render_color_mask;

uniform bool mask_premultiplied;
uniform bool texture_premultiplied;

vec4 premultiply(vec4 color) {
    return vec4(color.rgb * color.a, color.a);
}

vec4 undo_premultiply(vec4 color) {
    if (color.a == 0.0)
	return vec4(0.0, 0.0, 0.0, 0.0);
    return vec4(color.rgb / color.a, color.a);
}

void main(void) {
    vec4 mask_color = texture2D(mask_texture, screen_pos/mask_texture_size);
    if (mask_premultiplied)
	mask_color = undo_premultiply(mask_color);

    vec4 texture_color = texture2D(texture, tex_coord/texture_size);
    if (texture_premultiplied)
	texture_color = undo_premultiply(texture_color);

    texture_color *= mask_color * render_color_mask;

    gl_FragColor = premultiply(mask_color * texture_color);
    //gl_FragColor = mask_color * texture_color;
}
