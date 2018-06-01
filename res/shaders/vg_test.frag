#version 310 es

precision mediump float;
precision mediump sampler2DMS;

out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2DMS texture;
uniform sampler2DMS mask_texture;

uniform vec2 texture_size;
uniform vec2 mask_texture_size;

uniform float color_multiplier;

vec4 textureMS(in sampler2DMS tex, in vec2 texCoord, in vec2 textureSize, in int sampleIx)
{
	ivec2 texelCoords = ivec2(textureSize * clamp(texCoord, 0.0, 1.0));
	return texelFetch(tex, texelCoords, sampleIx);
}

void main(void) {
	FragColor = textureMS(texture, tex_coord, texture_size, 0)
	* textureMS(mask_texture, tex_coord, mask_texture_size, 0) * color_multiplier;
	//gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
