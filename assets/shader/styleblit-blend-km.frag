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

#define u_size 4096

vec3 mixBox(vec4 c)
{
    float c00 = c[0]*c[0];
    float c11 = c[1]*c[1];
    float c22 = c[2]*c[2];
    float c33 = c[3]*c[3];
    float c01 = c[0]*c[1];
    float c02 = c[0]*c[2];

    vec3 rgb = vec3(0.0);

    rgb += c[0]*c00 *       vec3(1.0*+0.07717053,1.0*+0.02826978,1.0*+0.24832992);
    rgb += c[1]*c11 *       vec3(1.0*+0.95912302,1.0*+0.80256528,1.0*+0.03561839);
    rgb += c[2]*c22 *       vec3(1.0*+0.74683774,1.0*+0.04868586,1.0*+0.00000000);
    rgb += c[3]*c33 *       vec3(1.0*+0.99518138,1.0*+0.99978149,1.0*+0.99704802);
    rgb += c00*c[1] *       vec3(3.0*+0.01606382,3.0*+0.27787927,3.0*+0.10838459);
    rgb += c01*c[1] *       vec3(3.0*-0.22715650,3.0*+0.48702601,3.0*+0.35660312);
    rgb += c00*c[2] *       vec3(3.0*+0.09019473,3.0*-0.05108290,3.0*+0.66245019);
    rgb += c02*c[2] *       vec3(3.0*+0.26826063,3.0*+0.22364570,3.0*+0.06141500);
    rgb += c00*c[3] *       vec3(3.0*-0.11677001,3.0*+0.45951942,3.0*+1.22955000);
    rgb += c[0]*c33 *       vec3(3.0*+0.35042682,3.0*+0.65938413,3.0*+0.94329691);
    rgb += c11*c[2] *       vec3(3.0*+1.07202375,3.0*+0.27090076,3.0*+0.34461513);
    rgb += c[1]*c22 *       vec3(3.0*+0.92964458,3.0*+0.13855183,3.0*-0.01495765);
    rgb += c11*c[3] *       vec3(3.0*+1.00720859,3.0*+0.85124701,3.0*+0.10922038);
    rgb += c[1]*c33 *       vec3(3.0*+0.98374897,3.0*+0.93733704,3.0*+0.39192814);
    rgb += c22*c[3] *       vec3(3.0*+0.94225681,3.0*+0.26644346,3.0*+0.60571754);
    rgb += c[2]*c33 *       vec3(3.0*+0.99897033,3.0*+0.40864351,3.0*+0.60217887);
    rgb += c01*c[2] *       vec3(6.0*+0.31232351,6.0*+0.34171197,6.0*-0.04972666);
    rgb += c01*c[3] *       vec3(6.0*+0.42768261,6.0*+1.17238033,6.0*+0.10429229);
    rgb += c02*c[3] *       vec3(6.0*+0.68054914,6.0*-0.23401393,6.0*+0.35832587);
    rgb += c[1]*c[2]*c[3] * vec3(6.0*+1.00013113,6.0*+0.42592007,6.0*+0.31789917);
    return rgb;
}

mat3 mixbox_srgb32f_to_latent(vec3 rgb)
{
    vec3 xyz = clamp(rgb, 0.0f,1.0f)*255.0f;

    ivec3 ixyz = ivec3(xyz);
    vec3 txyz = xyz - vec3(ixyz);

    int ox = 1;
    int oy = 256;
    int oz = 256*256;

    int pos = (ixyz.x*ox + ixyz.y*oy + ixyz.z*oz);

    vec4 c = vec4(0.0);

    c += (1.0f-txyz.x)*(1.0f-txyz.y)*(1.0f-txyz.z) * texture(c_table, (vec2((pos+0)           % u_size, u_size - int((pos+0)           / u_size)) + vec2(0.5, -0.5)) / vec2(u_size));
    c += (     txyz.x)*(1.0f-txyz.y)*(1.0f-txyz.z) * texture(c_table, (vec2((pos+ox)          % u_size, u_size - int((pos+ox)          / u_size)) + vec2(0.5, -0.5)) / vec2(u_size));
    c += (1.0f-txyz.x)*(     txyz.y)*(1.0f-txyz.z) * texture(c_table, (vec2((pos+oy)          % u_size, u_size - int((pos+oy)          / u_size)) + vec2(0.5, -0.5)) / vec2(u_size));
    c += (     txyz.x)*(     txyz.y)*(1.0f-txyz.z) * texture(c_table, (vec2((pos+ox +oy)      % u_size, u_size - int((pos+ox +oy)      / u_size)) + vec2(0.5, -0.5)) / vec2(u_size));
    c += (1.0f-txyz.x)*(1.0f-txyz.y)*(     txyz.z) * texture(c_table, (vec2((pos+oz)          % u_size, u_size - int((pos+oz)          / u_size)) + vec2(0.5, -0.5)) / vec2(u_size));
    c += (     txyz.x)*(1.0f-txyz.y)*(     txyz.z) * texture(c_table, (vec2((pos+ox +oz)      % u_size, u_size - int((pos+ox +oz)      / u_size)) + vec2(0.5, -0.5)) / vec2(u_size));
    c += (1.0f-txyz.x)*(     txyz.y)*(     txyz.z) * texture(c_table, (vec2((pos+oy +oz)      % u_size, u_size - int((pos+oy +oz)      / u_size)) + vec2(0.5, -0.5)) / vec2(u_size));
    c += (     txyz.x)*(     txyz.y)*(     txyz.z) * texture(c_table, (vec2((pos+ox +oy +oz)  % u_size, u_size - int((pos+ox +oy +oz)  / u_size)) + vec2(0.5, -0.5)) / vec2(u_size));

    c[3] = 1.0f-(c[0]+c[1]+c[2]);

    vec3 mixrgb = mixBox(c);

    return mat3( c[0],                 c[3], (rgb.b - mixrgb[2]),
    c[1],  (rgb.r - mixrgb[0]),                   0,
    c[2],  (rgb.g - mixrgb[1]),                   0);
}

vec3 mixbox_latent_to_srgb32f(mat3 latent)
{
    vec4 coefficients = vec4(latent[0][0], latent[1][0], latent[2][0], latent[0][1]);
    vec3 rgb = mixBox(coefficients);
    vec3 out_rgb = vec3(
    clamp(rgb[0]+latent[1][1],0.0f,1.0f),
    clamp(rgb[1]+latent[2][1],0.0f,1.0f),
    clamp(rgb[2]+latent[0][2],0.0f,1.0f));

    return out_rgb;
}

vec3 mixbox_lerp_srgb32f(vec3 rgbColor1, vec3 rgbColor2, float t)
{
    mat3 latent_a = mixbox_srgb32f_to_latent(rgbColor1);
    mat3 latent_b = mixbox_srgb32f_to_latent(rgbColor2);

    mat3 latent_c = t * latent_a + (1.0 - t) * latent_b;

    return mixbox_latent_to_srgb32f(latent_c);
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
                sumColor += mixbox_lerp_srgb32f(texture(u_sourceStyle, uv ).xyz, texture(u_sourceStyle2, uv).xyz, u_blend);
//                sumColor = mixbox_lerp_srgb32f(vec3(1.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), u_blend);
                sumWeight += 1.0;
            }
        }
    }

    outputColor.xyz = (sumWeight>0.0) ? sumColor/sumWeight : texture(u_sourceStyle,vec2(0.0,0.0)).xyz;
    outputColor.a = 1.0;
}