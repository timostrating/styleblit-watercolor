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


class TmpScreen : public Screen
{
    FlyingCamera camera = FlyingCamera();

    Gizmos gizmos;
    SharedMesh crystal = SharedMesh(Mesh::sphere_n(4));
    SharedMesh sphere = SharedMesh(Mesh::sphere_n());
    SharedMesh cube = SharedMesh(Mesh::cube_n());
    SharedMesh golem = SharedMesh(Mesh::golem());
    SharedMesh bunny = SharedMesh(Mesh::bunny());
    SharedMesh tyra = SharedMesh(Mesh::tyra());
    SharedMesh armadillo = SharedMesh(Mesh::armadillo());
    SharedMesh dragon = SharedMesh(Mesh::dragon());

    entt::registry registry;
    StyleBlitSystem system = StyleBlitSystem();

public:
    TmpScreen()
    {
        PROFILE_FUNCTION();

        VertexBuffer::uploadSingleMesh(crystal);
        VertexBuffer::uploadSingleMesh(sphere);
        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(golem);
        VertexBuffer::uploadSingleMesh(bunny);
        VertexBuffer::uploadSingleMesh(tyra);
        VertexBuffer::uploadSingleMesh(armadillo);
        VertexBuffer::uploadSingleMesh(dragon);

        for (int i=-1; i<=5; i++) {
            const auto entitySphere = registry.create();
            registry.emplace<transform>(entitySphere, translate(mat4(1), vec3(-9, i*2, 0)));
            registry.emplace<styleblit>(entitySphere, i, 0.0f, 50.0f);
            registry.emplace<meshdata>(entitySphere, sphere);

            const auto entityCube = registry.create();
            registry.emplace<transform>(entityCube, translate(mat4(1), vec3(-6, i*2, 0)));
            registry.emplace<styleblit>(entityCube, i, 0.0f, 50.0f);
            registry.emplace<meshdata>(entityCube, cube);

            const auto entityGolem = registry.create();
            registry.emplace<transform>(entityGolem, translate(mat4(1), vec3(-3, i*2 -1, 0)));
            registry.emplace<styleblit>(entityGolem, i, 0.0f, 50.0f);
            registry.emplace<meshdata>(entityGolem, golem);

            const auto entityBunny = registry.create();
            registry.emplace<transform>(entityBunny, translate(mat4(1), vec3(0, i*2 -1, 0)));
            registry.emplace<styleblit>(entityBunny, i, 0.0f, 50.0f);
            registry.emplace<meshdata>(entityBunny, bunny);

            const auto entityTyra = registry.create();
            registry.emplace<transform>(entityTyra, translate(mat4(1), vec3(3, i*2 -1, 0)));
            registry.emplace<styleblit>(entityTyra, i, 0.0f, 50.0f);
            registry.emplace<meshdata>(entityTyra, tyra);

            const auto entityArmadillo = registry.create();
            registry.emplace<transform>(entityArmadillo, translate(mat4(1), vec3(6, i*2 -1, 0)));
            registry.emplace<styleblit>(entityArmadillo, i, 0.0f, 50.0f);
            registry.emplace<meshdata>(entityArmadillo, armadillo);

            const auto entityDragon = registry.create();
            registry.emplace<transform>(entityDragon, translate(mat4(1), vec3(9, i*2 -1, 0)));
            registry.emplace<styleblit>(entityDragon, i, 0.0f, 50.0f);
            registry.emplace<meshdata>(entityDragon, dragon);
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
            camera.position = vec3(0, 5, 17 + cos(time*0.6));
//            camera.position = vec3(0, 8, 10);
//            camera.position = vec3(0, 1, 5 + 4 * cos(framenr / 100.0f *  2.0f * MATH::PI));
//            camera.position = vec3(0, 1, 2.75);
            camera.lookAt(vec3(0, 5, 0));
//            camera.lookAt(vec3(0, 1, 0));
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