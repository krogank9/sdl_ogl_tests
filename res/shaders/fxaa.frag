precision mediump float;

varying vec2 tex_coord;

uniform sampler2D render_texture;
uniform vec2 render_texture_size;

//optimized version for mobile, where dependent
//texture reads can be a bottleneck
vec4 fxaa(sampler2D tex, vec2 fragCoord, vec2 resolution,
          vec2 v_rgbNW, vec2 v_rgbNE,
          vec2 v_rgbSW, vec2 v_rgbSE,
          vec2 v_rgbM)
{
    float FXAA_REDUCE_MIN = (1.0/ 128.0);
    float FXAA_REDUCE_MUL = (1.0 / 8.0);
    float FXAA_SPAN_MAX = 8.0;

    vec4 color;
    vec2 inverseVP = vec2(1.0 / resolution.x, 1.0 / resolution.y);
    vec3 rgbNW = texture2D(tex, v_rgbNW).xyz;
    vec3 rgbNE = texture2D(tex, v_rgbNE).xyz;
    vec3 rgbSW = texture2D(tex, v_rgbSW).xyz;
    vec3 rgbSE = texture2D(tex, v_rgbSE).xyz;
    vec4 texColor = texture2D(tex, v_rgbM);
    vec3 rgbM  = texColor.xyz;
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) *
                          (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);

    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
              max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
              dir * rcpDirMin)) * inverseVP;

    vec3 rgbA = 0.5 * (
        texture2D(tex, fragCoord * inverseVP + dir * (1.0 / 3.0 - 0.5)).xyz +
        texture2D(tex, fragCoord * inverseVP + dir * (2.0 / 3.0 - 0.5)).xyz);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture2D(tex, fragCoord * inverseVP + dir * -0.5).xyz +
        texture2D(tex, fragCoord * inverseVP + dir * 0.5).xyz);

    float lumaB = dot(rgbB, luma);
    if ((lumaB < lumaMin) || (lumaB > lumaMax))
	color = vec4(rgbA, texColor.a);
    else
	color = vec4(rgbB, texColor.a);
    return color;
}

void main()
{
    float one_pixel_x = 1.0/render_texture_size.x;
    float one_pixel_y = 1.0/render_texture_size.y;

    vec2 NW = tex_coord.xy + vec2(-one_pixel_x, -one_pixel_y);
    vec2 NE = tex_coord.xy + vec2(one_pixel_x, -one_pixel_y);
    vec2 SW = tex_coord.xy + vec2(-one_pixel_x, one_pixel_y);
    vec2 SE = tex_coord.xy + vec2(one_pixel_x, one_pixel_y);
    vec2 M = tex_coord.xy;
    gl_FragColor = fxaa(render_texture, tex_coord.xy*render_texture_size, render_texture_size, NW, NE, SW, SE, M);
    //gl_FragColor = texture2D(render_texture, tex_coord.xy);
    return;

    float R_fxaaSpanMax = 8.0;
    float R_fxaaReduceMin = 1.0/128.0;
    float R_fxaaReduceMul = 1.0/8.0;

    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaTL = dot(luma, texture2D(render_texture, NW).xyz);
    float lumaTR = dot(luma, texture2D(render_texture, NE).xyz);
    float lumaBL = dot(luma, texture2D(render_texture, SW).xyz);
    float lumaBR = dot(luma, texture2D(render_texture, SE).xyz);
    float lumaM  = dot(luma, texture2D(render_texture, M).xyz);

    vec2 dir;
    dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
    dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));

    float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (R_fxaaReduceMul * 0.25), R_fxaaReduceMin);
    float inverseDirAdjustment = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(vec2(R_fxaaSpanMax, R_fxaaSpanMax),
            max(vec2(-R_fxaaSpanMax, -R_fxaaSpanMax), dir * inverseDirAdjustment)) * vec2(one_pixel_x, one_pixel_y);

    vec3 result1 = (1.0/2.0) * (
            texture2D(render_texture, tex_coord.xy + (dir * vec2(1.0/3.0 - 0.5))).xyz +
            texture2D(render_texture, tex_coord.xy + (dir * vec2(2.0/3.0 - 0.5))).xyz);

    vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (
            texture2D(render_texture, tex_coord.xy + (dir * vec2(0.0/3.0 - 0.5))).xyz +
            texture2D(render_texture, tex_coord.xy + (dir * vec2(3.0/3.0 - 0.5))).xyz);

    float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
    float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
    float lumaResult2 = dot(luma, result2);

    if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
       gl_FragColor = vec4(result1, 1.0);
    else
      gl_FragColor = vec4(result2, 1.0);
}
