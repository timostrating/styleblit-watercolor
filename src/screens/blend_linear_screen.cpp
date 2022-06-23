#include <glad/glad.h>
#include "../util/interfaces/screen.h"
#include "../graphics/flying_camera.h"
#include "../util/input/keyboard.h"
#include "../ecs/style_blit_system.cpp"
#include "../../external/entt/src/entt/entity/registry.hpp"
#include "../util/debug/profiler.h"
#include "../util/debug/gl_debug.h"

class BlendLinearScreen : public Screen
{
    FlyingCamera camera = FlyingCamera();

    Gizmos gizmos;
    SharedMesh crystal = SharedMesh(Mesh::sphere_n(4));
    SharedMesh sphere = SharedMesh(Mesh::sphere_n());
    SharedMesh cube = SharedMesh(Mesh::cube_n());
    SharedMesh armadillo = SharedMesh(Mesh::armadillo());
    SharedMesh tyra = SharedMesh(Mesh::tyra());
    SharedMesh bunny = SharedMesh(Mesh::bunny());
    SharedMesh golem = SharedMesh(Mesh::golem());


    entt::registry registry;
    StyleBlitSystem system = StyleBlitSystem();

public:
    BlendLinearScreen()
    {
        PROFILE_FUNCTION();

        VertexBuffer::uploadSingleMesh(crystal);
        VertexBuffer::uploadSingleMesh(sphere);
        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(armadillo);
        VertexBuffer::uploadSingleMesh(tyra);
        VertexBuffer::uploadSingleMesh(bunny);
        VertexBuffer::uploadSingleMesh(golem);

        for (float f=0; f<=1.01; f+=0.1f) {
            const auto entityLinear = registry.create();
            registry.emplace<transform>(entityLinear, translate(mat4(1), vec3(f * 10.0f - 5.0f, 2.0f, 0)));
            registry.emplace<styleblit>(entityLinear, 4, f);
            registry.emplace<meshdata>(entityLinear, armadillo);

            const auto entityhsv = registry.create();
            registry.emplace<transform>(entityhsv, translate(mat4(1), vec3(f * 10.0f - 5.0f, 0.0f, 0)));
            registry.emplace<styleblit>(entityhsv, 3, f);
            registry.emplace<meshdata>(entityhsv, armadillo);

            const auto entityKM = registry.create();
            registry.emplace<transform>(entityKM, translate(mat4(1), vec3(f * 10.0f - 5.0f, -2.0f, 0)));
            registry.emplace<styleblit>(entityKM, 5, f);
            registry.emplace<meshdata>(entityKM, armadillo);
        }

        system.start();
    }

    double time = 0;
    bool debugUpdate = false;
    bool dragUpdate = false;
    float framenr = 0.0;

    void render(double deltaTime)
    {
        PROFILE_FUNCTION();

        system.update(registry, deltaTime);
        time += deltaTime;
        framenr += 1.0;

//        glClearColor(61.0/255.0, 130.0/255.0, 184.0/255.0, 1.0);
        glClearColor(255.0/255.0, 255.0/255.0, 255.0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (INPUT::KEYBOARD::pressed(GLFW_KEY_1)) debugUpdate = true;
        if (INPUT::KEYBOARD::pressed(GLFW_KEY_2)) dragUpdate = true;

        if (dragUpdate) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else if (debugUpdate) {
            camera.debugUpdate(deltaTime);
            camera.debugDraw();
        } else {
//            camera.position = vec3(0, 8, 10 + cos(time*0.6));
//            camera.position = vec3(0, 1, 5 + 4 * cos(framenr / 100.0f *  2.0f * MATH::PI));
            camera.position = vec3(0, 1, 10);
//            camera.lookAt(vec3(2, 1, 0));
            camera.lookAt(vec3(0, 1, 0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        system.render(registry, camera);
    }

    void resize(int width, int height)
    {
        PROFILE_FUNCTION();
        camera.resize(width, height);
    }
};