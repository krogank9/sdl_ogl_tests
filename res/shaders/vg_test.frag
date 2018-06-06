precision mediump float;

const float IGNORE_CHANNEL = 555.0;

//out vec4 FragColor;

varying vec2 screen_pos;
varying vec2 tex_coord;

uniform sampler2D texture;
uniform sampler2D mask1_texture;
uniform sampler2D mask2_texture;
uniform sampler2D mask3_texture;
uniform sampler2D mask4_texture;

uniform vec2 texture_size;
uniform vec2 mask1_texture_size;
uniform vec2 mask2_texture_size;
uniform vec2 mask3_texture_size;
uniform vec2 mask4_texture_size;

uniform vec4 render_tint;
uniform vec4 mask1_tint;
uniform vec4 mask2_tint;
uniform vec4 mask3_tint;
uniform vec4 mask4_tint;

uniform bool texture_premultiplied;
uniform bool mask1_premultiplied;
uniform bool mask2_premultiplied;
uniform bool mask3_premultiplied;
uniform bool mask4_premultiplied;

uniform int mask_count;

vec4 premultiply(vec4 color) {
	return vec4(color.rgb * color.a, color.a);
}

vec4 undo_premultiply(vec4 color) {
	if (color.a == 0.0)
		return vec4(0.0, 0.0, 0.0, 0.0);
	return vec4(color.rgb / color.a, color.a);
}

vec4 get_mask(sampler2D texture, vec2 pos, vec4 tint, bool premultiplied) {
	vec4 color = texture2D(texture, pos);

	if (premultiplied)
		color = undo_premultiply(color);

	if (tint.r < 0.0)
	{
		color.r = (1.0 - color.r);
		tint.r = -tint.r;
	}
	if (tint.g < 0.0)
	{
		color.g = (1.0 - color.g);
		tint.g = -tint.g;
	}
	if (tint.b < 0.0)
	{
		color.b = (1.0 - color.b);
		tint.b = -tint.b;
	}
	if (tint.a < 0.0)
	{
		color.a = (1.0 - color.a);
		tint.a = -tint.a;
	}

	return vec4(tint.r == IGNORE_CHANNEL? 1.0 : color.r*tint.r,
	            tint.g == IGNORE_CHANNEL? 1.0 : color.g*tint.g,
	            tint.b == IGNORE_CHANNEL? 1.0 : color.b*tint.b,
	            tint.a == IGNORE_CHANNEL? 1.0 : color.a*tint.a);
}

void main(void) {
	vec4 texture_color = texture2D(texture, tex_coord/texture_size);
	if (texture_premultiplied)
		texture_color = undo_premultiply(texture_color);

	texture_color *= render_tint;

	// need to get negative mask working here
	if (mask_count >= 1)
		texture_color *= get_mask(mask1_texture, screen_pos/mask1_texture_size, mask1_tint, mask1_premultiplied);
	if (mask_count >= 2)
		texture_color *= get_mask(mask2_texture, screen_pos/mask2_texture_size, mask2_tint, mask2_premultiplied);
	if (mask_count >= 3)
		texture_color *= get_mask(mask3_texture, screen_pos/mask3_texture_size, mask3_tint, mask3_premultiplied);
	if (mask_count >= 4)
		texture_color *= get_mask(mask4_texture, screen_pos/mask4_texture_size, mask4_tint, mask4_premultiplied);

	gl_FragColor = premultiply(texture_color);
}
