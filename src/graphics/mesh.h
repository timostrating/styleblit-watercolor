//
// Created by sneeuwpop on 28-6-19.
//

#pragma once

#include <glad/glad.h>
#include <vector>
#include <memory>
#include "vertex_buffer.h"
#include "vert_attributes.h"

class VertexBuffer;
class Mesh;
typedef std::shared_ptr<Mesh> SharedMesh;

class Mesh
{
public:
    std::vector<float> vertices = {};
    std::vector<int> indicies = {};

    VertexBuffer *vertexBuffer = nullptr;

    GLenum renderMode = GL_TRIANGLES;

    unsigned int nrOfVerts = 0;
    unsigned int nrOfIndices = 0;

    VertAttributes attributes;

    static Mesh* quad();
    static Mesh* triangle();
    static Mesh* cube();
    static Mesh* cube_n();
    static Mesh* skybox();
    static Mesh* sphere(int rings = 10, bool inverse = false, bool addNormals = false);
    static Mesh* sphere_n(int rings = 10, bool inverse = false);
    static Mesh* loadObjFile(std::string filename);
    static Mesh* golem();
    static Mesh* bunny();
    static Mesh* tyra();
    static Mesh* armadillo();
    static Mesh* dragon();

    Mesh(unsigned int nrOfVerts, unsigned int nrOfIndices, VertAttributes attributes = VA_POSITION) : nrOfVerts(nrOfVerts), nrOfIndices(nrOfIndices), attributes(attributes) {};

    void render();
};
