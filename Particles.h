#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Consts
#define NUMBER_OF_PARTICLES 3

class Particles
{
    float vertices[9] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    unsigned int VAO, VBO;
public:
    void Init();
    void Render();
};

