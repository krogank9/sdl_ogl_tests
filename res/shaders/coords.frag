precision mediump float;

varying vec4 vertex_color;
varying vec3 our_color;
varying vec2 tex_coord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float fade;

void main(void) {
    //gl_FragColor = vec4(our_color, 1.0);
    vec2 rev_tex_coord = vec2(-tex_coord.x, tex_coord.y);
    gl_FragColor = mix(texture2D(texture1, tex_coord), texture2D(texture2, rev_tex_coord), fade) * vec4(our_color, 1.0);
}
