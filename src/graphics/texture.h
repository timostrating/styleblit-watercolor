//
// Created by sneeuwpop on 24-10-19.
//

#pragma once


#include <glad/glad.h>
#include "shader_program.h"

class Texture
{

public:
    GLuint textureId;
    int width, height;

    static Texture fromAssetFile(const char *imgPath, GLint textureWrapping = GL_REPEAT, GLint textureInterpolation = GL_LINEAR);
    static Texture testCheckerboard();

    /// \param imgPath
    /// \param textureWrapping GL_REPEAT, ...
    /// \param textureInterpolation GL_LINEAR, ...
    /// \param format GL_RGB, GL_RGBA, ...
    Texture(const char *imgPath, GLint textureWrapping = GL_REPEAT, GLint textureInterpolation = GL_LINEAR, GLint format = GL_RGB);

    Texture(GLuint textureId, int width, int height) : textureId(textureId), width(width), height(height) {}

    ~Texture();

    void bind(unsigned int textureI);
    void bind(unsigned int textureI, const ShaderProgram &shader, const char *name);

    void renderGUI();
};


