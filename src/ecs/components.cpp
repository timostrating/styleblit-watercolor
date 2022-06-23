//
// Created by sneeuwpop on 07-01-22.
//

#include "../graphics/mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

typedef mat4 transform;

struct styleblit {
    int type;
    float blend = 0.5f;
    float threshold = 24.0f;
};

struct meshdata {
    SharedMesh sharedmesh;
};

