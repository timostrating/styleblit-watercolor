#version 300 es
precision highp float;

#define BLEND_RADIUS 1

uniform sampler2D u_NNF;           // buffer 2
uniform sampler2D u_sourceStyle;   // source Style
uniform sampler2D u_sourceStyle2;   // source Style
uniform sampler2D u_targetMask;    // buffer 1
uniform sampler2D c_table;
uniform vec2 u_targetSize;
uniform vec2 u_sourceSize;
uniform float u_blend;

out vec4 outputColor;

// source https://newbedev.com/from-rgb-to-hsv-in-opengl-glsl
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    vec2 xy = gl_FragCoord.xy;

    vec3 sumColor = vec3(0.0);
    float sumWeight = 0.0;

    if (texture(u_targetMask,(xy)/u_targetSize).a>0.0)
    {
        for(int oy=-BLEND_RADIUS;oy<=+BLEND_RADIUS;oy++)
        for(int ox=-BLEND_RADIUS;ox<=+BLEND_RADIUS;ox++)
        {
            if (texture(u_targetMask,(xy+vec2(ox,oy))/u_targetSize).a>0.0)
            {
                vec2 uv = texture(u_NNF,(xy+vec2(ox,oy))/u_targetSize).xy;
                sumColor += hsv2rgb(u_blend * rgb2hsv(texture(u_sourceStyle, uv).xyz) + (1.0f - u_blend) * rgb2hsv(texture(u_sourceStyle2, uv).xyz));
                //                sumColor = u_blend * vec3(1.0, 1.0, 0.0) + (1.0f - u_blend) * vec3(0.0, 0.0, 1.0);
                sumWeight += 1.0;
            }
        }
    }

    outputColor.xyz = (sumWeight>0.0) ? sumColor/sumWeight : texture(u_sourceStyle,vec2(0.0,0.0)).xyz;
    outputColor.a = 1.0;
}