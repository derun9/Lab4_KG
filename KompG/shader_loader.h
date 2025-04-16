#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <GL/glew.h>
#include <string>

GLuint createShader(const char* vertexPath, const char* fragmentPath);
void setUniformFloat(GLuint program, const char* name, float value);
void setUniformVec3(GLuint program, const char* name, float x, float y, float z);
void setUniformVec4(GLuint program, const char* name, float x, float y, float z, float w);
void setUniformInt(GLuint program, const char* name, int value);

#endif
