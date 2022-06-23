
constexpr static char fragSource2try[] = R"glsl(#version 300 es
precision highp float;


out vec4 outputColor;

uniform sampler2D u_jitterTable;

vec2 RandomJitterTable(vec2 uv)
{
    return texture(u_jitterTable,uv).xy;
}

vec2 SeedPoint(vec2 p, float spacing)
{
    vec2 coordinate = floor(p/h);
    vec2 jitter = RandomJitterTable(b);
    return floor(spacing * (coordinate + jitter));
}

void main()
{
    vec2 p = gl_FragCoord.xy;
    outputColor = vec4(SeedPoint(p, gl_FragCoord.x), 0.0, 1.0);
})glsl";

constexpr static char fragSource2old[] = R"glsl(#version 300 es
precision highp float;

out vec4 outputColor;

#define LEVELS 6

uniform mat4 u_view;
uniform sampler2D u_target; // the previous render pass view normal buffer
uniform sampler2D u_normals; // the normals of the source style examplar
uniform sampler2D u_jitterTable; // the noise texture
uniform sampler2D u_normalToSourceLUT; // idk
// uniform sampler2D u_matcap;
// uniform sampler2D u_matcap2;
uniform vec2 u_targetSize;
uniform vec2 u_sourceSize;
uniform float u_threshold;
uniform float u_blend;




// pxc: pixel on normal texture
// returns respective uv cooordinate on style texture
vec2 lookupCoord(vec2 pxc)
{
    vec2 pattern = texture(u_target, pxc / u_targetSize).rg;
    // return pattern;
    return texture(u_normalToSourceLUT, pattern.rg).rg;
}

// sourceCoords: uv coordinate of source normal texture
// targetCoords: uv coordinate of target normal texture
// returns error of guide normal to target
float calcGuideError(vec2 sourceCoords, vec2 targetCoords)
{
    vec4 diff = abs(texture(u_target, targetCoords) - texture(u_normals, sourceCoords));
    return diff.x + diff.y + diff.z;
}

// returns implicit seed in a pixel vicinity
// jitter effectivelly shaping the seed neigborhood
vec2 findSeed(vec2 pxc, float spacing)
{
    vec2 coords = floor(pxc / spacing);
    vec2 jitter = texture(u_jitterTable, coords / vec2(256.0, 256.0)).rg;
    jitter *= u_blend;
    return (coords + jitter) * spacing;
}

// returns nearest implicit seed in a pixel vicinity
vec2 findNearestSeed(vec2 pxc, float spacing)
{
    float bestD = 9999999999.0;
    vec2 bestPxc = vec2(0.0, 0.0);

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            vec2 p = findSeed((pxc + vec2(i, j) * spacing), spacing);
            float d = distance(p, pxc);
            if (bestD > d)
            {
                bestPxc = p;
                bestD = d;
            }
        }
    }

    return bestPxc;
}

// performs style texture lookup for the given pixel and seed
// calculates transfer error and compaares with the best result so far
void findBestCoords(vec2 pxc, vec2 origSeedPxc, float l, float thresh,
inout vec2 bestCoords, inout float bestError, inout float bestL,
inout vec2 bestSeed)
{
    vec2 seedPxc = lookupCoord(origSeedPxc) * u_sourceSize;

    vec2 sourcePxc = seedPxc + (pxc - origSeedPxc);
    vec2 sourceCoords = sourcePxc / u_sourceSize;

    if ((distance(pxc, origSeedPxc) < l / 2.0) &&
    (sourcePxc.x > 0.0 && sourcePxc.y > 0.0) &&
    (sourcePxc.x < u_sourceSize.x && sourcePxc.y < u_sourceSize.y))
    {
        float error = calcGuideError(sourceCoords, pxc.xy / u_sourceSize);

        if (error < thresh && error < bestError)
        {
            bestL = l;
            bestError = error;

            bestCoords = sourceCoords;
            bestSeed = seedPxc / u_sourceSize; // todo maybe
        }
    }
}

// main entry for finding the best pixel to transfer from source style texture to target
vec2 styleBlit(vec2 p)
{

    float bestError = 9999999999.0;
    float bestL = -1.0;
    vec2 pxc = gl_FragCoord.xy; // maybe this is wrong
    // pxc *= vec2(800.0, 800.0);

    vec2 bestCoords = lookupCoord(pxc);
    vec2 bestSeed = vec2(0.0, 0.0);
    vec2 nearSeed;

    // descending the levels for finding the largest chunk of pixels to transfer
    for (int i = LEVELS; i > 0; i--)
    {
        float l = pow(2.0, float(i));

        if (l > bestL)
        {
            nearSeed = findNearestSeed(pxc, l);
            findBestCoords(pxc, nearSeed, l, u_threshold, bestCoords, bestError, bestL, bestSeed);
        }
    }

    // deconstructs the coordinate for passing
    return bestCoords.xy;
}

void main()
{
    vec2 p = gl_FragCoord.xy;
    outputColor = vec4(styleBlit(p), 0.0, 1.0);
})glsl";




