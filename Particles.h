#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Logger.h"
// Consts


class Particles
{
    float vertices[9] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    unsigned int VAO, VBO;
    unsigned int particleShaderProgram;
public:
    Particles() {}
    std::shared_ptr<Logger> log;
    void Init(std::shared_ptr<Logger>);
    void Render();
    ~Particles();
};

