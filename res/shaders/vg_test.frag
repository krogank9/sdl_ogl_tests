precision mediump float;

varying vec2 tex_coord;

uniform sampler2D render_texture;
uniform sampler2D mask_texture;

uniform float color_multiplier;

void main(void) {
    gl_FragColor = texture2D(render_texture, tex_coord) * texture2D(mask_texture, tex_coord) * color_multiplier;
    //gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
