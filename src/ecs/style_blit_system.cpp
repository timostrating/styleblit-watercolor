#include "../../external/entt/src/entt/entity/registry.hpp"
#include "../graphics/shader_program.h"
#include "../graphics/frame_buffer.h"
#include "ecs_system.h"
#include "components.cpp"

#include "../util/external/stb_image.h"
#include "../util/external/stb_image_resize.h"
#include "../graphics/camera.h"
#include "../util/input/keyboard.h"
#include "../util/debug/profiler.h"
#include "../util/external/unit_test.hpp"


static GLuint createTexture2D(GLint format,int width,int height,const void* data,GLint filter,GLint wrap)
{
    GLuint texture;
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,format,GL_UNSIGNED_BYTE,data);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wrap);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrap);
    return texture;
}

static GLuint loadTexture(const std::string& fileName,GLint format,const int resolution,GLint filter, GLint warp)
{
    const int numChannels = (format==GL_RGBA) ? 4 : 3;
    int width;
    int height;

    stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(fileName.c_str(),&width,&height,NULL,numChannels);
    unsigned char* resizedImage = new unsigned char[resolution*resolution*numChannels];
    stbir_resize_uint8(image,width,height,0,resizedImage,resolution,resolution,0,numChannels);

    GLuint texture = createTexture2D(format,resolution,resolution,resizedImage,filter,warp);

    delete[] resizedImage;
    stbi_image_free(image);

    return texture;
}
static float timetracker = 0.0f;


class StyleBlitSystem : public ECS_System
{
    constexpr static char vertSource4[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;
        layout (location = 1) in vec3 a_normal;

        uniform mat4 MVP;
        uniform mat4 normalMatrix;

        out vec3 v_color;
        out vec3 v_pos;

        void main() {
            v_pos = a_pos;
            vec4 n = normalize(normalMatrix*vec4(a_normal,0.0));
            v_color = n.xyz*0.5+vec3(0.5);
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";

    // From StyleBlit example
    constexpr static char fragSource4[] = R"glsl(#version 300 es
        precision highp float;

        #define BLEND_RADIUS 1

        in vec3 v_pos;
        out vec4 outputColor;

        uniform sampler2D u_sourceStyle;
        uniform sampler2D u_targetMask;
        uniform vec2 u_targetSize;
        uniform vec2 u_sourceSize;

        vec3 random3r( vec3 p ) {
            return fract(sin(
                vec3(dot(p,vec3(127.1,227.1,311.7)),
                    dot(p,vec3(817.2,217.2,146.2)),
                    dot(p,vec3(269.5,269.5,183.3)))
                )*43758.5453);
        }

        void main()
        {
            vec3 pos = v_pos;
            pos += vec3(1.);
            pos *= 2.;

            vec3 i_st = floor(pos);
            vec3 f_st = fract(pos);

            float m_dist = 1.;  // minimum distance
            vec3 m_point = vec3(0.0);

            for (int y= -1; y <= 1; y++) {
                for (int x= -1; x <= 1; x++) {
                    for (int z= -1; z <= 1; z++) {
                        vec3 neighbor = vec3(float(x), float(y), float(z));
                        vec3 point = random3r(i_st + neighbor);

        //                point = 0.5 + 0.5*sin(6.2831*point);
                        vec3 diff = neighbor + point - f_st;
                        float dist = length(diff);
                        if (m_dist < dist) {
                            m_dist = dist;
                            m_point = point;
                        }
                    }
                }
            }

            outputColor = vec4(vec3(random3r(m_point)), 1.0);
        })glsl";

    constexpr static char flatVert[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;
        layout (location = 1) in vec3 a_normal;

        uniform mat4 MVP;

        out vec3 v_pos;

        void main() {
            v_pos = a_normal;
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";

    // From StyleBlit example
    constexpr static char flatfrag[] = R"glsl(#version 300 es
        precision highp float;

        #define BLEND_RADIUS 1

        in vec3 v_pos;
        out vec4 outputColor;

        void main()
        {
            outputColor = vec4(vec3(dot(vec3(0.0),v_pos)), 1.0);

        })glsl";

    ////////////////////////////////////////////////////////////////////////////////

    // TODO make these parameters
    ShaderProgram shaderFlat = ShaderProgram(flatVert, flatfrag);
    ShaderProgram shaderMatcap = ShaderProgram("shader/matcap.vert", "shader/matcap.frag", true);
    ShaderProgram shaderNormals = ShaderProgram("shader/normal.vert", "shader/normal.frag", true);
    ShaderProgram shader2 = ShaderProgram("shader/mesh.vert", "shader/styleblit-main.frag", true);
//    ShaderProgram shad = ShaderProgram("shader/mesh.vert", "shader/styleblit-blend-km.frag", true);
    ShaderProgram shad = ShaderProgram("shader/mesh.vert", "shader/styleblit-blend-linear.frag", true);
//    ShaderProgram shad = ShaderProgram("shader/mesh.vert", "shader/styleblit-blend-hsv.frag", true);
//    ShaderProgram shad = ShaderProgram("shader/mesh.vert", "shader/styleblit-blend-km.frag", true);
//    ShaderProgram shad = ShaderProgram(vertSource4, fragSource4);

    FrameBuffer fbo = FrameBuffer(1920, 1080);
    FrameBuffer fbo2 = FrameBuffer(1920, 1080);

    GLuint styleTextureId = 0; // Component data
    GLuint styleTexture2Id = 1;
    GLuint sphereNormalsTextureId = 2;
    GLuint jitterTextureId = 3;
    GLuint c_tableId = 4;

    bool renderAsScreenSpaceImageEffect = false;

public:

    StyleBlitSystem()
    {
    }

    void start()
    {
        fbo.addColorTexture(GL_RGBA, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE);
        fbo.addDepthTexture(GL_NEAREST, GL_NEAREST);
        fbo.unbind();

        fbo2.addColorTexture(GL_RGBA, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE);
//        fbo2.addDepthTexture(GL_NEAREST, GL_NEAREST);
        fbo2.unbind();

        // TODO make these parameters
//        styleTextureId = loadTexture("../../../../assets/styleblit/0.png", GL_RGBA, 200, GL_NEAREST, GL_REPEAT);
        styleTextureId = loadTexture("../../../../assets/my_source/my_source_2.png", GL_RGBA, 200, GL_NEAREST, GL_REPEAT);
        styleTexture2Id = loadTexture("../../../../assets/my_source/my_source_2.png", GL_RGBA, 200, GL_NEAREST, GL_REPEAT);
        sphereNormalsTextureId = loadTexture("../../../../assets/styleblit/normals.png", GL_RGBA, 200, GL_NEAREST, GL_REPEAT);
        jitterTextureId = loadTexture("../../../../assets/images/jitter.png", GL_RGBA, 256, GL_NEAREST, GL_REPEAT);
        c_tableId = loadTexture("../../../../assets/images/c_table.jpeg", GL_RGB, 4096, GL_NEAREST, GL_REPEAT);
    }

    void update(entt::registry &registry, float deltaTime)
    {
        PROFILE_FUNCTION();

//        if (INPUT::KEYBOARD::pressed(GLFW_KEY_T)) { threshold -= deltaTime * 0.15; std::cout << "Threshold: " << threshold << std::endl; }
//        if (INPUT::KEYBOARD::pressed(GLFW_KEY_Y)) { threshold += deltaTime * 0.15; std::cout << "Threshold: " << threshold << std::endl; }
//
//        if (INPUT::KEYBOARD::pressed(GLFW_KEY_G)) { blend = max(0.0f, blend - deltaTime); std::cout << "Blend: " << blend << std::endl; }
//        if (INPUT::KEYBOARD::pressed(GLFW_KEY_H)) { blend = min(1.0f, blend + deltaTime); std::cout << "Blend: " << blend << std::endl; }
    }

    void renderFlat(entt::registry &registry, Camera &camera, int type)
    {
        PROFILE_FUNCTION();
        auto view = registry.view<transform, styleblit, meshdata>();

        shaderFlat.use();

        for(auto [entity, transform, style, meshd]: view.each()) {
            if (style.type == type) {
                glUniformMatrix4fv(shaderFlat.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * transform)[0][0]);
                meshd.sharedmesh->render();
            }
        };
    }


    void renderMatcap(entt::registry &registry, Camera &camera, int type)
    {
        PROFILE_FUNCTION();
        auto view = registry.view<transform, styleblit, meshdata>();

        shaderMatcap.use();
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, styleTextureId); glUniform1i(shaderMatcap.uniformLocation("u_matcap"), 0);

        for(auto [entity, transform, style, meshd]: view.each()) {
            if (style.type == type) {
                glUniformMatrix4fv(shaderMatcap.uniformLocation("u_view"), 1, GL_FALSE, &(camera.view * transform)[0][0]);
                glUniformMatrix4fv(shaderMatcap.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * transform)[0][0]);
                meshd.sharedmesh->render();
            }
        };
    }

    void renderNormals(entt::registry &registry, Camera &camera, int type)
    {
        PROFILE_FUNCTION();
        auto view = registry.view<transform, styleblit, meshdata>();

        for(auto [entity, transform, style, meshd]: view.each()) { meshd.sharedmesh->vertexBuffer->bind(); } // TODO: this is a tmp hack

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        shaderNormals.use();
        for(auto [entity, transform, style, meshd]: view.each()) {
            if (style.type == type) {
                glUniformMatrix4fv(shaderNormals.uniformLocation("normalMatrix"), 1, GL_FALSE, &(glm::transpose(glm::inverse(camera.view * transform)))[0][0]);
                glUniformMatrix4fv(shaderNormals.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * transform)[0][0]);
                meshd.sharedmesh->render();
            }
        }
    }


    void renderShader2(entt::registry &registry, Camera &camera, int type)
    {
        PROFILE_FUNCTION();
        auto view = registry.view<transform, styleblit, meshdata>();

        shader2.use();
        fbo.colorTexture->bind(0, shader2, "u_target");
        shader2.setTexture(sphereNormalsTextureId, "u_source", 1);
        shader2.setTexture(jitterTextureId, "u_jitterTable", 2);

        glUniform2f(shader2.uniformLocation("u_targetSize"), 1920, 1080);
        glUniform2f(shader2.uniformLocation("u_sourceSize"), 200, 200);
        glUniform1f(shader2.uniformLocation("u_time"), timetracker);
        timetracker += 0.01f;

        if (renderAsScreenSpaceImageEffect)
        {
            glUniformMatrix4fv(shader2.uniformLocation("MVP"), 1, GL_FALSE, &(MAT4::IDENTITY)[0][0]);
            VertexBuffer::drawFullScreenImageEffect(); //drawFullscreenTriangle(glGetAttribLocation(shader2.programId,"a_pos"));
        }
        else
        {
            for(auto [entity, transform, style, meshd]: view.each()) {
                if (style.type == type) {
                    glUniform1f(shader2.uniformLocation("u_threshold"), style.threshold);

                    glUniformMatrix4fv(shader2.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * transform)[0][0]);
                    meshd.sharedmesh->render();
                }
            }
        }
    }

    void renderShaderFinal(entt::registry &registry, Camera &camera, int type)
    {
        PROFILE_FUNCTION();
        auto view = registry.view<transform, styleblit, meshdata>();

//        ShaderProgram * shader = (type == 3) ? &shader3 : (type == 4)? &shader4 : &shader5;
        ShaderProgram * shader = &shad;

        shader->use();
        fbo2.colorTexture->bind(0, *shader, "u_NNF");
        shader->setTexture(styleTextureId, "u_sourceStyle", 1);
        shader->setTexture(styleTexture2Id, "u_sourceStyle2", 2);
        fbo.colorTexture->bind(3, *shader, "u_targetMask");
        shader->setTexture(c_tableId, "c_table", 4);

        glUniform2f(shader->uniformLocation("u_targetSize"), 1920, 1080);
        glUniform2f(shader->uniformLocation("u_sourceSize"), 200, 200);


        if (renderAsScreenSpaceImageEffect)
        {
            glUniformMatrix4fv(shader->uniformLocation("MVP"), 1, GL_FALSE, &(MAT4::IDENTITY)[0][0]);
            VertexBuffer::drawFullScreenImageEffect();
        }
        else
        {
            for (auto[entity, transform, style, meshd]: view.each()) {
                if (style.type == type) {
                    glUniform1f(shader->uniformLocation("u_blend"), style.blend);

                    glUniformMatrix4fv(shader->uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * transform)[0][0]);
                    meshd.sharedmesh->render();
                }
            }
        }

//        static GLubyte *pixels = NULL;
//        pixels = static_cast<GLubyte *>(realloc(pixels, GL_RGB * sizeof(GLubyte)));
//        glReadPixels(0, 0, 1920, 1920, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    }

    void render(entt::registry &registry, Camera &camera)
    {
        PROFILE_FUNCTION();

        renderFlat(registry, camera, -1);

        //

        renderMatcap(registry, camera, 0);

        //

        renderNormals(registry, camera, 1);

        //

        fbo.cleanBind();
            renderNormals(registry, camera, 2);
        fbo.unbind();

        renderShader2(registry, camera, 2);

        //

        fbo.cleanBind();
            renderNormals(registry, camera, 3);
        fbo.unbind();
        fbo2.bind();
            renderShader2(registry, camera, 3);
        fbo2.unbind();
        renderShaderFinal(registry, camera, 3);

        fbo.cleanBind();
        renderNormals(registry, camera, 4);
        fbo.unbind();
        fbo2.bind();
        renderShader2(registry, camera, 4);
        fbo2.unbind();
        renderShaderFinal(registry, camera, 4);

        fbo.cleanBind();
        renderNormals(registry, camera, 5);
        fbo.unbind();
        fbo2.bind();
        renderShader2(registry, camera, 5);
        fbo2.unbind();
        renderShaderFinal(registry, camera, 5);
    }
};





#include "../util/external/mixbox.h"

inline void float2ByteInt (float* in, int* out) {
    out[0] = (int)(in[0]*255);
    out[1] = (int)(in[1]*255);
    out[2] = (int)(in[2]*255);
}

constexpr static char g_vertSource[] = R"glsl(#version 300 es
    layout (location = 0) in vec3 a_pos;
    void main() { gl_Position = vec4(a_pos, 1.0); })glsl";



inline vec3 mixboxMix(vec4 input) {
    FrameBuffer fb = FrameBuffer(1, 1);
    fb.addColorTexture(GL_RGBA, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE);

    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision highp float;

        out vec4 outputColor;
        uniform vec4 u_input;

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

        void main() { outputColor = vec4(mixBox(u_input), 1.0); } )glsl";

    ShaderProgram shader = ShaderProgram(g_vertSource, fragSource);

    fb.cleanBind();
        shader.use();
        glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(MAT4::IDENTITY)[0][0]);
        glUniform4f(shader.uniformLocation("u_input"), input[0], input[1], input[2], input[3]);

        VertexBuffer::drawFullScreenImageEffect();

        static GLubyte *pixels = NULL;
        pixels = static_cast<GLubyte *>(realloc(pixels, GL_RGB * sizeof(GLubyte)));
        glReadPixels(0, 0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    fb.unbind();
    return vec3(pixels[0], pixels[1], pixels[2]);
}
inline bool mixboxMixTest(vec4 input, float epsilon = 1.0f) {
    float c[4] = {input[0], input[1], input[2], input[3]};
    float mixrgb[3] = {0.0f, 0.0f, 0.0f};
    int rgb[3] = {0, 0, 0};

    mix(c, mixrgb);
    float2ByteInt(mixrgb, rgb);
    vec3 result = mixboxMix(input);

    fprintf(stderr, "a %f %f %f\n", mixrgb[0],mixrgb[1],mixrgb[2]);
    fprintf(stderr, "b %d %d %d\n", rgb[0],rgb[1],rgb[2]);
    fprintf(stderr, "c %.1f %.1f %.1f\n", result[0],result[1],result[2]);

    return  abs(result[0] - rgb[0]) <= epsilon &&
            abs(result[1] - rgb[1]) <= epsilon &&
            abs(result[2] - rgb[2]) <= epsilon;
}
TEST(mixboxMix1, return mixboxMixTest(vec4(1.0, 0.0, 0.0, 0.0)); )
TEST(mixboxMix2, return mixboxMixTest(vec4(0.0, 1.0, 0.0, 0.0)); )
TEST(mixboxMix3, return mixboxMixTest(vec4(0.0, 0.0, 1.0, 0.0)); )
TEST(mixboxMix4, return mixboxMixTest(vec4(0.0, 0.0, 0.0, 1.0)); )
TEST(mixboxMix5, return mixboxMixTest(vec4(0.5, 0.5, 0.0, 0.0)); )
TEST(mixboxMix6, return mixboxMixTest(vec4(0.3, 0.3, 0.3, 0.1)); )

//TEST(mixboxMix7, return mixboxMixTest(vec4(1.0, 1.0, 1.0, 1.0)); ) // this test fails because opengl output is clamped between 0 and 255




inline vec3 mixboxLatent(vec4 input) {
    FrameBuffer fb = FrameBuffer(1, 1);
    fb.addColorTexture(GL_RGBA, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE);

    GLuint c_tableId = loadTexture("../../../../assets/images/c_table.jpeg", GL_RGB, 4096, GL_NEAREST, GL_REPEAT);

    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision highp float;

        out vec4 outputColor;

        uniform sampler2D c_table;
        uniform vec4 u_input;
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
            vec3 xyz = clamp(rgb, 0.0f, 1.0f) * 255.0f;

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

        void main() {
            mat3 out_latent = mixbox_srgb32f_to_latent(u_input.xyz);
            vec3 result = vec3(out_latent[0][0], out_latent[1][0], out_latent[2][0]);
            outputColor = vec4(result, 1.0);
        })glsl";

    ShaderProgram shader = ShaderProgram(g_vertSource, fragSource);

    fb.cleanBind();
    shader.use();
    shader.setTexture(c_tableId, "c_table", 0);
    glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(MAT4::IDENTITY)[0][0]);
    glUniform4f(shader.uniformLocation("u_input"), input[0], input[1], input[2], input[3]);

    VertexBuffer::drawFullScreenImageEffect();

    static GLubyte *pixels = NULL;
    pixels = static_cast<GLubyte *>(realloc(pixels, GL_RGB * sizeof(GLubyte)));
    glReadPixels(0, 0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    fb.unbind();
    return vec3(pixels[0], pixels[1], pixels[2]);
}
inline bool mixboxLatentTest(vec4 input, float epsilon = 1.0f) {
    float l[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    mixbox_srgb32f_to_latent(input[0], input[1], input[2], l);
    vec3 result = mixboxLatent(input);

    fprintf(stderr, "a %f %f %f %f %f %f %f\n", l[0]*255.0,l[1]*255.0,l[2]*255.0,l[3]*255.0,l[4]*255.0,l[5]*255.0,l[6]*255.0);
    fprintf(stderr, "c %.1f %.1f %.1f\n", result[0],result[1],result[2]);

    return  abs(result[0] - l[0]*255.0) <= epsilon &&
            abs(result[1] - l[1]*255.0) <= epsilon &&
            abs(result[2] - l[2]*255.0) <= epsilon;
}
// c_table corner RGB values
// 125 67  63      0   68  187
// 113 9   0       0   0   0
//TEST(mixboxLatent0, return mixboxLatentTest(vec4(0.0, 0.0, 0.0, 0.0), 8.0f); )
//TEST(mixboxLatent1, return mixboxLatentTest(vec4(1.0, 0.0, 0.0, 0.0), 8.0f); ) // TODO: improve precision
//TEST(mixboxLatent2, return mixboxLatentTest(vec4(0.0, 1.0, 0.0, 0.0), 8.0f); ) // TODO: improve precision
//TEST(mixboxLatent3, return mixboxLatentTest(vec4(0.0, 0.0, 1.0, 0.0), 8.0f); ) // TODO: improve precision
//TEST(mixboxLatent4, return mixboxLatentTest(vec4(1.0, 1.0, 1.0, 1.0), 8.0f); ) // TODO: improve precision


inline vec3 mixboxLerp(vec4 a, vec4 b, vec4 c) {
    FrameBuffer fb = FrameBuffer(1, 1);
    fb.addColorTexture(GL_RGBA, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE);

    GLuint c_tableId = loadTexture("../../../../assets/images/c_table.jpeg", GL_RGB, 4096, GL_NEAREST, GL_REPEAT);

    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision highp float;

        out vec4 outputColor;

        uniform sampler2D c_table;
        uniform vec4 u_input;
        uniform vec4 u_input2;
        uniform vec4 u_input3;
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

        void main() {
            outputColor = vec4(mixbox_lerp_srgb32f(u_input.xyz, u_input2.xyz, u_input3.x), 1.0);
        })glsl";

    ShaderProgram shader = ShaderProgram(g_vertSource, fragSource);

    fb.cleanBind();
    shader.use();
    shader.setTexture(c_tableId, "c_table", 0);
    glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(MAT4::IDENTITY)[0][0]);
    glUniform4f(shader.uniformLocation("u_input"), a[0], a[1], a[2], a[3]);
    glUniform4f(shader.uniformLocation("u_input2"), b[0], b[1], b[2], b[3]);
    glUniform4f(shader.uniformLocation("u_input3"), c[0], c[1], c[2], c[3]);

    VertexBuffer::drawFullScreenImageEffect();

    static GLubyte *pixels = NULL;
    pixels = static_cast<GLubyte *>(realloc(pixels, GL_RGB * sizeof(GLubyte)));
    glReadPixels(0, 0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    fb.unbind();
    return vec3(pixels[0], pixels[1], pixels[2]);
}
inline bool mixboxLerpTest(vec3 a, vec3 b, float t, float epsilon = 1.0f) {
    float out[3] = {0.0f, 0.0f, 0.0f};

    mixbox_lerp_srgb32f(a[0], a[1], a[2],  b[0], b[1], b[2],  t, &out[0], &out[1], &out[2]);
    vec3 result = mixboxLerp(vec4(a, 0.0), vec4(b, 0.0), vec4(t));

    fprintf(stderr, "a %f %f %f\n", out[0]*255.0,out[1]*255.0,out[2]*255.0);
    fprintf(stderr, "c %.1f %.1f %.1f\n", result[0],result[1],result[2]);

    return  abs(result[0] - out[0]*255.0) <= epsilon &&
            abs(result[1] - out[1]*255.0) <= epsilon &&
            abs(result[2] - out[2]*255.0) <= epsilon;
}
TEST(mixboxLerp0, return mixboxLerpTest(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), 0.5,  5.0f); )
TEST(mixboxLerp1, return mixboxLerpTest(vec3(1.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), 0.5,  5.0f); ) // TODO: improve precision
TEST(mixboxLerp2, return mixboxLerpTest(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), 0.5,  5.0f); ) // TODO: improve precision
TEST(mixboxLerp3, return mixboxLerpTest(vec3(0.0, 1.0, 1.0), vec3(1.0, 0.0, 1.0), 0.5,  5.0f); ) // TODO: improve precision