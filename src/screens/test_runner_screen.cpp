//
// Created by sneeuwpop on 17-01-20.
//

#include <glad/glad.h>
#include "../util/interfaces/screen.h"
#include "../graphics/flying_camera.h"
#include "../util/input/keyboard.h"
#include "../ecs/style_blit_system.cpp"
#include "../../external/entt/src/entt/entity/registry.hpp"
#include "../util/debug/profiler.h"
#include "../util/debug/gl_debug.h"


class TestRunnerScreen : public Screen
{
    FlyingCamera camera = FlyingCamera();

    Gizmos gizmos;

    SharedMesh golem = SharedMesh(Mesh::golem());
    SharedMesh bunny = SharedMesh(Mesh::bunny());
    SharedMesh tyra = SharedMesh(Mesh::tyra());
    SharedMesh armadillo = SharedMesh(Mesh::armadillo());
    SharedMesh dragon = SharedMesh(Mesh::dragon());

    entt::registry registry;
    StyleBlitSystem system = StyleBlitSystem();

public:
    TestRunnerScreen()
    {
        PROFILE_FUNCTION();

        VertexBuffer::uploadSingleMesh(golem);
        VertexBuffer::uploadSingleMesh(bunny);
        VertexBuffer::uploadSingleMesh(tyra);
        VertexBuffer::uploadSingleMesh(armadillo);
        VertexBuffer::uploadSingleMesh(dragon);

        const auto entity1 = registry.create();
        registry.emplace<transform>(entity1, translate(mat4(1), vec3(0, 0, 0)));
        registry.emplace<styleblit>(entity1, 4, 0.0f, 50.0f);
        registry.emplace<meshdata>(entity1, golem);

        const auto entity2 = registry.create();
        registry.emplace<transform>(entity2, translate(mat4(1), vec3(10, 0, 0)));
        registry.emplace<styleblit>(entity2, 4, 0.0f, 50.0f);
        registry.emplace<meshdata>(entity2, bunny);

        const auto entity3 = registry.create();
        registry.emplace<transform>(entity3, translate(mat4(1), vec3(20, 0, 0)));
        registry.emplace<styleblit>(entity3, 4, 0.0f, 50.0f);
        registry.emplace<meshdata>(entity3, tyra);

        const auto entity4 = registry.create();
        registry.emplace<transform>(entity4, translate(mat4(1), vec3(30, 0, 0)));
        registry.emplace<styleblit>(entity4, 4, 0.0f, 50.0f);
        registry.emplace<meshdata>(entity4, armadillo);

        const auto entity5 = registry.create();
        registry.emplace<transform>(entity5, translate(mat4(1), vec3(40, 0, 0)));
        registry.emplace<styleblit>(entity5, 4, 0.0f, 50.0f);
        registry.emplace<meshdata>(entity5, dragon);



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
//            camera.position = vec3(0, 1, 5 + cos(time*0.6));
//            camera.position = vec3(0, 8, 10);
            camera.position = vec3(static_cast<int>(framenr / 200.0) * 10.0, 1, 5 + 4 * cos(framenr / 100.0f *  2.0f * MATH::PI));   // for testing 200 frames
//            camera.position = vec3(0, 1, 2.75);
//            camera.lookAt(vec3(2, 1, 0));
            camera.lookAt(vec3(static_cast<int>(framenr / 200.0) * 10.0, 1, 0));                                                       // the center
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