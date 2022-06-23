//
// Created by sneeuwpop on 20-6-19.
//

#pragma once

#include <glad/glad.h>
#include "../util/io/file_watch.h"

class ShaderProgram
{
private:

public:
#if linux
    FileWatch vertFW = FileWatch();
    const char * vertPath = nullptr;
    FileWatch fragFW = FileWatch();
    const char * fragPath = nullptr;
#endif

    GLuint programId; // TODO: make private

    ShaderProgram(const char* vertexSource, const char* fragSource, bool useSourceAsPath = false);
    void setupShader(const char* vertexSource, const char* fragSource, bool useSourceAsPath = false);
    ~ShaderProgram();

    void use();
//    GLuint getId() { return programId; }

    GLint uniformLocation(const char* name) const;
    void setTexture(GLuint textureId, const char* shaderName, int index);

#if linux
    void reload();
#endif
};