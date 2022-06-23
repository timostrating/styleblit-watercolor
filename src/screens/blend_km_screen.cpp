#include <glad/glad.h>
#include "../util/interfaces/screen.h"
#include "../graphics/flying_camera.h"
#include "../util/input/keyboard.h"
#include "../ecs/style_blit_system.cpp"
#include "../../external/entt/src/entt/entity/registry.hpp"
#include "../util/debug/profiler.h"
#include "../util/debug/gl_debug.h"

class BlendKmScreen : public Screen
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
    BlendKmScreen()
    {
        PROFILE_FUNCTION();

        VertexBuffer::uploadSingleMesh(crystal);
        VertexBuffer::uploadSingleMesh(sphere);
        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(armadillo);
        VertexBuffer::uploadSingleMesh(tyra);
        VertexBuffer::uploadSingleMesh(bunny);
        VertexBuffer::uploadSingleMesh(golem);

        for (int i=-1; i<=5; i++) {
            const auto entityArmadillo = registry.create();
            registry.emplace<transform>(entityArmadillo, translate(mat4(1), vec3(0, i*2 -1, 0)));
            registry.emplace<styleblit>(entityArmadillo, i);
            registry.emplace<meshdata>(entityArmadillo, armadillo);
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
            camera.position = vec3(0, 8, 10 + cos(time*0.6));
//            camera.position = vec3(0, 1, 5 + 4 * cos(framenr / 100.0f *  2.0f * MATH::PI));
//            camera.position = vec3(0, 1, 2.75);
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