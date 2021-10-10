#pragma once
#include "GL/glew.h"
#include "GL/freeglut.h"
#include <memory>
#include "Logger.h"
#include "Shader.h"
#include "configs.h"

class Particles
{

    unsigned int VAO, VBO;
    unsigned int particleShaderProgram = NULL;
    float* m_vertices = new float[configs.m_NumberOfParticles * 2];
    float* m_velocity = new float[configs.m_NumberOfParticles * 2];


public:
    Particles() {}
    Logger* log = Logger::GetLogger();
    void Init();
    void Render();
    void Move();
    ~Particles();
};

