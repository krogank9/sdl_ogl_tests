precision mediump float;

//varying vec2 tex_coord;

//uniform sampler2D render_texture;
//uniform sampler2D mask_texture;

//uniform float blendMultiplier;

void main(void) {
    //gl_FragColor = texture2D(render_texture, tex_coord) * texture2D(mask_texture, tex_coord) * blendMultiplier;
    gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
