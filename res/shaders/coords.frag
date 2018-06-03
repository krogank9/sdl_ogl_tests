#version 300 es

precision mediump float;

out vec4 FragColor;

in vec4 vertex_color;
in vec3 our_color;
in vec2 tex_coord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float fade;

void main(void) {
    FragColor = vec4(our_color, 1.0);
    vec2 rev_tex_coord = vec2(-tex_coord.x, tex_coord.y);
    //FragColor = mix(texture(texture1, tex_coord), texture(texture2, rev_tex_coord), fade) * vec4(our_color, 1.0);
}
