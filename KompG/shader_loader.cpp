#include "shader_loader.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string loadShaderSource(const char* filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint createShader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = loadShaderSource(vertexPath);
    std::string fragmentCode = loadShaderSource(fragmentPath);
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    // Проверка на ошибки компиляции для вершинного шейдера
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);
    // Проверка на ошибки компиляции для фрагментного шейдера
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Проверка на ошибки линковки программы
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

void setUniformFloat(GLuint program, const char* name, float value) {
    GLint location = glGetUniformLocation(program, name);
    if (location == -1) {
        std::cerr << "ERROR: Uniform " << name << " not found!" << std::endl;
    }
    else {
        glUniform1f(location, value);
    }
}

void setUniformVec3(GLuint program, const char* name, float x, float y, float z) {
    GLint location = glGetUniformLocation(program, name);
    if (location == -1) {
        std::cerr << "ERROR: Uniform " << name << " not found!" << std::endl;
    }
    else {
        glUniform3f(location, x, y, z);
    }
}

void setUniformVec4(GLuint program, const char* name, float x, float y, float z, float w) {
    GLint location = glGetUniformLocation(program, name);
    if (location == -1) {
        std::cerr << "ERROR: Uniform " << name << " not found!" << std::endl;
    }
    else {
        glUniform4f(location, x, y, z, w);
    }
}

void setUniformInt(GLuint program, const char* name, int value) {
    GLint location = glGetUniformLocation(program, name);
    if (location == -1) {
        std::cerr << "ERROR: Uniform " << name << " not found!" << std::endl;
    }
    else {
        glUniform1i(location, value);
    }
}
