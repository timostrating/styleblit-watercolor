#version 300 es
precision highp float;

out vec4 outputColor;

uniform sampler2D u_target;         // Buffer 1
uniform sampler2D u_source;         // The texture where the normals are on a sphere
uniform sampler2D u_jitterTable;    // random noise
// uniform sampler2D u_matcap;
// uniform sampler2D u_matcap2;
uniform vec2 u_targetSize;
uniform vec2 u_sourceSize;
uniform float u_threshold;

float my_sum(vec3 xyz) { return xyz.x + xyz.y + xyz.z; }

float my_frac(float x) { return x-floor(x); }

vec4 pack(vec2 xy)
{
    float x = xy.x/255.0;
    float y = xy.y/255.0;
    return vec4(my_frac(x),floor(x)/255.0, my_frac(y),floor(y)/255.0);
}

bool inside(vec2 uv,vec2 size)
{
    return (all(greaterThanEqual(uv,vec2(0,0))) && all(lessThan(uv,size)));
}

vec2 RandomJitterTable(vec2 uv)
{
    return texture(u_jitterTable,(uv+vec2(0.5,0.5))/vec2(256,256)).xy;
}

vec2 SeedPoint(vec2 p,float h)
{
    vec2 b = floor(p/h);
    vec2 j = RandomJitterTable(b);
    return floor(h*(b+j));
}

vec2 NearestSeed(vec2 p,float h)
{
    vec2 s_nearest = vec2(0,0);
    float d_nearest = 10000.0;

    for(int x=-1;x<=+1;x++) {
        for(int y=-1;y<=+1;y++) {
            vec2 s = SeedPoint(p+h*vec2(x,y),h);
            float d = length(s-p);
            if (d<d_nearest)
            {
                s_nearest = s;
                d_nearest = d;
            }
        }
    }

    return s_nearest;
}

vec3 GS(vec2 uv) { return texture(u_source,(uv+vec2(0.5,0.5))/u_sourceSize).rgb; }
vec3 GT(vec2 uv) { return texture(u_target,(uv+vec2(0.5,0.5))/u_targetSize).rgb; }
// vec3 CS(vec2 uv) { return texture(u_matcap,(uv+vec2(0.5,0.5))/u_sourceSize).rgb; }
// vec3 CS2(vec2 uv){ return texture(u_matcap2,(uv+vec2(0.5,0.5))/u_sourceSize).rgb; }

vec2 ArgMinLookup(vec3 targetNormal)
{
    return vec2(targetNormal.x,targetNormal.y);
}

void main()
{
    vec2 p = gl_FragCoord.xy;
    vec2 o = ArgMinLookup(GT(p));

    for(int level=6;level>0;level--)
    {
        vec2 q = NearestSeed(p,pow(2.0,float(level)));
        vec2 u = ArgMinLookup(GT(q)) * u_sourceSize;

        float e = my_sum(abs(GT(p)-GS(u+(p-q))))*255.0;

        if (e<u_threshold)
        {
            o = (u+(p-q)) / u_sourceSize;
            if (inside(o, u_sourceSize)) { break; }
        }
    }

    //         outputColor = vec4(texture(u_target, gl_FragCoord.xy / vec2(800.0)).xyz, 1.0);

//    outputColor = pack(o);
    outputColor = vec4(o, 0.0, 1.0);
}