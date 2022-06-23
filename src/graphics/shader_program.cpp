//
// Created by sneeuwpop on 20-6-19.
//

#include <iostream>
#include <glad/glad.h>
#include <vector>
#include "shader_program.h"
#include "../util/io/file.h"
#include "../util/debug/nice_error.h"


void validateShader(GLint shaderId)
{
    GLint isCompiled = 0;
    GLint logLength = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    if (!isCompiled || logLength > 1) // A driver may append a null terminator to a log in case there is no log
    {
        std::vector<char> logText(logLength+1);
        glGetShaderInfoLog(shaderId, logLength, NULL, &logText[0]);
        std::cout << "validateShader() ERROR: isCompiled = "<< ((isCompiled)?"TRUE":"FALSE") <<"; log = " << &logText[0] << "\n";
        throw nice_error("Shader error");
    }
}

void validateProgram(GLint programId)
{
    GLint isCompiled = 0;
    GLint logLength = 0;
    glGetProgramiv(programId, GL_LINK_STATUS, &isCompiled);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
    if (!isCompiled || logLength > 1) // A driver may append a null terminator to a log in case there is no log
    {
        std::vector<char> logText(logLength+1);
        glGetProgramInfoLog(programId, logLength, NULL, &logText[0]);
        std::cerr << "validateProgram() ERROR: isCompiled = "<< ((isCompiled)?"TRUE":"FALSE") <<"; log = '" << &logText[0] << "'\n";
        throw nice_error("ShaderProgram error");
    }
}

GLuint compileShader(GLenum shaderType, const char *src)
{
    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &src, NULL);
    glCompileShader(shaderId);

    validateShader(shaderId);

    return shaderId;
}

void ShaderProgram::setupShader(const char *vertexSource, const char *fragSource, bool useAsPaths) {
    programId = glCreateProgram();

    GLuint vs, fs;
    if (useAsPaths)
    {
//        fragSource = std::string("#version 300 es\nprecision mediump float;").append(File::readAssetAsString("shaders/lib/SHARED.frag")).append(File::readAssetAsString(fragPath)).c_str();
#if linux
        // TODO: remove the hardcoded string
        vertPath = vertexSource;
        vertFW.setFileToWatch(std::string("../../../../assets/").append(vertexSource).c_str());

        fragPath = fragSource;
        fragFW.setFileToWatch(std::string("../../../../assets/").append(fragSource).c_str());
#endif
        vs = compileShader(GL_VERTEX_SHADER, File::readAssetAsString(vertexSource).c_str());
        fs = compileShader(GL_FRAGMENT_SHADER,  File::readAssetAsString(fragSource).c_str());
    }
    else
    {
        vs = compileShader(GL_VERTEX_SHADER, vertexSource);
        fs = compileShader(GL_FRAGMENT_SHADER, fragSource);
    }


    glAttachShader(programId, vs);
    glAttachShader(programId, fs);

//    glBindAttribLocation(programId, 0, "a_pos"); // TODO:
    glLinkProgram(programId);

    validateProgram(programId);

    glDetachShader(programId, vs);
    glDetachShader(programId, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

ShaderProgram::ShaderProgram(const char *vertexSource, const char *fragSource, bool useAsPaths)
{
    setupShader(vertexSource, fragSource, useAsPaths);
}

#if linux
void ShaderProgram::reload()
{
    // TODO: I suppose there is a better way than creating a new shader but this is just for testing
    if (vertPath && fragPath)
        setupShader(vertPath, fragPath, true);
}
#endif

void ShaderProgram::use()
{
#if linux
    if (vertFW.hasChanged() || fragFW.hasChanged())
    {
        std::cout << "Reloading Shader: " << programId << " \n";
        reload();
    }
#endif
    glUseProgram(programId);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(programId);
}

GLint ShaderProgram::uniformLocation(const char *name) const
{
    return glGetUniformLocation(programId, name);
}

void ShaderProgram::setTexture(GLuint textureId, const char* shaderName, int index)
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D,textureId);
    glUniform1i(uniformLocation(shaderName), index);
}