#pragma once

#include "../../external/entt/src/entt/entity/registry.hpp"
#include "../graphics/camera.h"

class ECS_System
{
public:
    virtual void start() {};
    virtual void update(entt::registry &registry, float deltaTime) {};
    virtual void render(entt::registry &registry, Camera &camera) {};
    virtual void resize(int width, int height) {};
};