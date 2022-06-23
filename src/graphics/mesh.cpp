//
// Created by sneeuwpop on 20-7-19.
//

#include <iostream>
#include "mesh.h"
#include "../util/debug/nice_error.h"
#include "../util/math/math.h"
#include "../util/external/tiny_obj_loader.h"

using namespace MATH;

Mesh* Mesh::quad()
{
    Mesh* mesh = new Mesh(4, 6);
    mesh->vertices.insert(mesh->vertices.begin(), {
            -1, -1,  0,
            -1,  1,  0,
             1,  1,  0,
             1, -1,  0,     });
    mesh->indicies.insert(mesh->indicies.begin(), {2, 1, 0, 0, 3, 2});
    return mesh;
}

Mesh* Mesh::triangle()
{
    Mesh* mesh = new Mesh(3, 3);
    mesh->vertices.insert(mesh->vertices.begin(), {
            -1, -1,  0,
             1, -1,  0,
             0,  1,  0, });
    mesh->indicies.insert(mesh->indicies.begin(), {0,1,2} );
    return mesh;
}

Mesh *Mesh::cube()
{
    Mesh* mesh = new Mesh(12*3, 0);
    mesh->vertices.insert(mesh->vertices.begin(), { // TODO optimize
            //  x,    y,    z       x,    y,    z       x,    y,    z
            -1.0f,-1.0f,-1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,
             1.0f, 1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,
             1.0f,-1.0f, 1.0f,  -1.0f,-1.0f,-1.0f,   1.0f,-1.0f,-1.0f,
             1.0f, 1.0f,-1.0f,   1.0f,-1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,-1.0f,
             1.0f,-1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,   1.0f,-1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,   1.0f,-1.0f,-1.0f,   1.0f, 1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,   1.0f, 1.0f, 1.0f,   1.0f,-1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,   1.0f, 1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,
             1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,-1.0f,  -1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,   1.0f,-1.0f, 1.0f
    });
//    mesh->indicies.insert(mesh->indicies.begin(), {0,1,2, 3,4,5, 6,7,8, 9,10,11, 12,13,14, 15,16,17, 18,19,20, 21,22,23, 24,25,26, 27,28,29, 30,31,32, 33,34,35});
    return mesh;
}

Mesh *Mesh::cube_n()
{
    Mesh* mesh = new Mesh(12*3, 0, VA_POSITION_NORMAL);
    mesh->vertices.insert(mesh->vertices.begin(), { // TODO optimize
            //  x,    y,    z                            x,    y,    z                            x,    y,    z
            -1.0f,-1.0f,-1.0f,   -1.0f,-1.0f,-1.0f,    -1.0f,-1.0f, 1.0f, -1.0f,-1.0f, 1.0f,    -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f,-1.0f,    1.0f, 1.0f,-1.0f,    -1.0f,-1.0f,-1.0f, -1.0f,-1.0f,-1.0f,    -1.0f, 1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,
            1.0f,-1.0f, 1.0f,    1.0f,-1.0f, 1.0f,    -1.0f,-1.0f,-1.0f, -1.0f,-1.0f,-1.0f,     1.0f,-1.0f,-1.0f,   1.0f,-1.0f,-1.0f,
            1.0f, 1.0f,-1.0f,    1.0f, 1.0f,-1.0f,     1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f,    -1.0f,-1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,   -1.0f,-1.0f,-1.0f,    -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,    -1.0f, 1.0f,-1.0f, -1.0f, 1.0f,-1.0f,
            1.0f,-1.0f, 1.0f,    1.0f,-1.0f, 1.0f,    -1.0f,-1.0f, 1.0f, -1.0f,-1.0f, 1.0f,    -1.0f,-1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,   -1.0f, 1.0f, 1.0f,    -1.0f,-1.0f, 1.0f, -1.0f,-1.0f, 1.0f,     1.0f,-1.0f, 1.0f,  1.0f,-1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f,     1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f,     1.0f, 1.0f,-1.0f,   1.0f, 1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,    1.0f,-1.0f,-1.0f,     1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,     1.0f,-1.0f, 1.0f,   1.0f,-1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f,     1.0f, 1.0f,-1.0f,  1.0f, 1.0f,-1.0f,    -1.0f, 1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,
            1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f,    -1.0f, 1.0f,-1.0f, -1.0f, 1.0f,-1.0f,    -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f,    -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,     1.0f,-1.0f, 1.0f,   1.0f,-1.0f, 1.0f
    });
//    mesh->indicies.insert(mesh->indicies.begin(), {0,1,2, 3,4,5, 6,7,8, 9,10,11, 12,13,14, 15,16,17, 18,19,20, 21,22,23, 24,25,26, 27,28,29, 30,31,32, 33,34,35});
    return mesh;
}

Mesh *Mesh::skybox()
{
    Mesh* mesh = new Mesh(12*3, 0);
    mesh->vertices.insert(mesh->vertices.begin(), {
            -1.0f,  1.0f, -1.0f,   -1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,    1.0f,  1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,   -1.0f, -1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,    1.0f, -1.0f,  1.0f,    1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,    1.0f,  1.0f, -1.0f,    1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,    1.0f, -1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,    1.0f,  1.0f, -1.0f,    1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f,  1.0f
    });
//    mesh->indicies.insert(mesh->indicies.begin(), {0,1,2, 3,4,5, 6,7,8, 9,10,11, 12,13,14, 15,16,17, 18,19,20, 21,22,23, 24,25,26, 27,28,29, 30,31,32, 33,34,35});
    return mesh;
}

Mesh *Mesh::sphere(int rings, bool inverse, bool add_normals)
{
    Mesh* mesh = new Mesh(0, 0, add_normals ? VA_POSITION_NORMAL : VA_POSITION);
    float step = 2*PI / rings;

    for (float phi=-0.5*PI; phi < 0.5*PI - 0.01; phi+=step) // [-90,90]
    {
        bool notFirstPhiLoop = abs(-0.5 * PI - phi) > 0.01,
             notLastPhiLoop  = phi + step < 0.5 * PI - 0.01;

        for (float theta=0; theta < 2*PI - 0.01; theta+=step) // [0,360]
        {
            vec3 a1 = vec3(cos(phi) * cos(theta), sin(phi), cos(phi)*sin(theta));
            vec3 a2 = vec3(cos(phi) * cos(theta+step), sin(phi), cos(phi)*sin(theta+step));
            vec3 b1 = vec3(cos(phi+step) * cos(theta), sin(phi+step), cos(phi+step)*sin(theta));
            vec3 b2 = vec3(cos(phi+step) * cos(theta+step), sin(phi+step), cos(phi+step)*sin(theta+step));

            if (add_normals) {
                if (inverse) {
                    if (notFirstPhiLoop) { mesh->vertices.insert(mesh->vertices.end(), {a1.x,a1.y,a1.z,a1.x,a1.y,a1.z, a2.x,a2.y,a2.z,a2.x,a2.y,a2.z, b1.x,b1.y,b1.z,b1.x,b1.y,b1.z}); }
                    if (notLastPhiLoop)  { mesh->vertices.insert(mesh->vertices.end(), {b2.x,b2.y,b2.z,b2.x,b2.y,b2.z, b1.x,b1.y,b1.z,b1.x,b1.y,b1.z, a2.x,a2.y,a2.z,a2.x,a2.y,a2.z}); }
                } else {
                    if (notFirstPhiLoop) { mesh->vertices.insert(mesh->vertices.end(), {a1.x,a1.y,a1.z,a1.x,a1.y,a1.z, b1.x,b1.y,b1.z,b1.x,b1.y,b1.z, a2.x,a2.y,a2.z,a2.x,a2.y,a2.z}); }
                    if (notLastPhiLoop)  { mesh->vertices.insert(mesh->vertices.end(), {b2.x,b2.y,b2.z,b2.x,b2.y,b2.z, a2.x,a2.y,a2.z,a2.x,a2.y,a2.z, b1.x,b1.y,b1.z,b1.x,b1.y,b1.z}); }
                }
            } else {
                if (inverse) {
                    if (notFirstPhiLoop) { mesh->vertices.insert(mesh->vertices.end(), {a1.x,a1.y,a1.z, a2.x,a2.y,a2.z, b1.x,b1.y,b1.z}); }
                    if (notLastPhiLoop)  { mesh->vertices.insert(mesh->vertices.end(), {b2.x,b2.y,b2.z, b1.x,b1.y,b1.z, a2.x,a2.y,a2.z}); }
                } else {
                    if (notFirstPhiLoop) { mesh->vertices.insert(mesh->vertices.end(), {a1.x,a1.y,a1.z, b1.x,b1.y,b1.z, a2.x,a2.y,a2.z}); }
                    if (notLastPhiLoop)  { mesh->vertices.insert(mesh->vertices.end(), {b2.x,b2.y,b2.z, a2.x,a2.y,a2.z, b1.x,b1.y,b1.z}); }
                }
            }
        }
    }
    mesh->nrOfVerts = mesh->vertices.size() / mesh->attributes.getVertSize();
    return mesh;
}
Mesh *Mesh::sphere_n(int rings, bool inverse) { return sphere(rings, inverse, true); }

Mesh *Mesh::loadObjFile(std::string filename) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;                 // TODO: don't hardcore the url here
    std::string path = std::string("../../../../assets/"+filename+".obj");
    std::cout << "Loading model: " << path << std::endl;
    bool ret = tinyobj::LoadObj(&attrib,&shapes,&materials,&err,path.c_str(),"",true);

    // An index contains 2 indicies that point to the 2 vectors.
    // attrib.vertices =            [verticies, ...]
    // attrib.normals =             [normals, ...]
    // shapes.at(i).mesh.indices =  [(vertex_index, normal_index), (a,b), ...]

    if (!err.empty()) { nice_error(err); return 0; }
    if (!ret) { nice_error("TinyObj loading error"); return 0; }

    bool has_normals = (attrib.normals.size() != 0);
    Mesh* mesh = new Mesh(0, 0, has_normals ? VA_POSITION_NORMAL : VA_POSITION);

    std::cout << "vertices: " << attrib.vertices.size() << std::endl;
    std::cout << "normals: " << attrib.normals.size() << std::endl;
    std::cout << "indices: " << shapes.at(0).mesh.indices.size() << std::endl;

    for (int s=0; s<attrib.vertices.size(); s+=3)
    {
        mesh->vertices.insert(mesh->vertices.end(), {attrib.vertices.at(s+0), attrib.vertices.at(s+1), attrib.vertices.at(s+2)});
        if (has_normals == false) { continue; }
        mesh->vertices.insert(mesh->vertices.end(), {attrib.vertices.at(s+0), attrib.vertices.at(s+1), attrib.vertices.at(s+2)}); // temporary fill with data
    }
    mesh->nrOfVerts = mesh->vertices.size() / mesh->attributes.getVertSize();


    for (auto & shape : shapes)
    {
        for (auto & indice : shape.mesh.indices)
        {
            mesh->indicies.push_back(indice.vertex_index);

            if (has_normals == false) { continue; }
            int vertexIndex = indice.vertex_index;
            int normalIndex = indice.normal_index;
            mesh->vertices.at(vertexIndex*6 + 3) = attrib.normals.at(normalIndex*3 + 0); // fill the data we temporary filled in
            mesh->vertices.at(vertexIndex*6 + 4) = attrib.normals.at(normalIndex*3 + 1);
            mesh->vertices.at(vertexIndex*6 + 5) = attrib.normals.at(normalIndex*3 + 2);
        }
    }
    mesh->nrOfIndices = mesh->indicies.size();
//    mesh->nrOfIndices = 0;
    return mesh;
}
Mesh *Mesh::golem() { return loadObjFile("golem"); }
Mesh *Mesh::bunny() { return loadObjFile("bunny"); }
//Mesh *Mesh::bunny() { return loadObjFile("forestALL"); }
Mesh *Mesh::tyra() { return loadObjFile("tyra"); }
Mesh *Mesh::armadillo() { return loadObjFile("armadillo"); }
Mesh *Mesh::dragon() { return loadObjFile("dragon"); }

void Mesh::render()
{
    if (!vertexBuffer)
        throw nice_error("You need to upload this mesh to a vertexBuffer first");

    vertexBuffer->bind();

    if (nrOfIndices != 0)
        glDrawElements(renderMode, nrOfIndices, GL_UNSIGNED_INT, (void*)0);
    else
        glDrawArrays(renderMode, 0, nrOfVerts);
}


