#version 300 es
precision highp float;

#define BLEND_RADIUS 1

uniform sampler2D u_NNF;           // buffer 2
uniform sampler2D u_sourceStyle;   // source Style
//uniform sampler2D u_sourceStyle2;   // source Style
uniform sampler2D u_targetMask;    // buffer 1
//uniform sampler2D c_table;
uniform vec2 u_targetSize;
uniform vec2 u_sourceSize;
uniform float u_blend;

out vec4 outputColor;

#define u_size 4096

void main()
{
    vec2 xy = gl_FragCoord.xy;

    vec3 sumColor = vec3(0.0);
    float sumWeight = 0.0;

    float t = sin(u_blend);

    if (texture(u_targetMask,(xy)/u_targetSize).a>0.0)
    {
        for(int oy=-BLEND_RADIUS;oy<=+BLEND_RADIUS;oy++)
        for(int ox=-BLEND_RADIUS;ox<=+BLEND_RADIUS;ox++)
        {
            if (texture(u_targetMask,(xy+vec2(ox,oy))/u_targetSize).a>0.0)
            {
                vec2 uv = texture(u_NNF,(xy+vec2(ox,oy))/u_targetSize).xy;
                sumColor += texture(u_sourceStyle, uv ).xyz;
                sumWeight += 1.0;
            }
        }
    }

    outputColor.xyz = (sumWeight>0.0) ? sumColor/sumWeight : texture(u_sourceStyle,vec2(0.0,0.0)).xyz;
    outputColor.a = 1.0;
}