#pragma once
#include "GL/glew.h"
#include "GL/freeglut.h"
#include <memory>
#include "Logger.h"
#include "Shader.h"

class Particles
{

    unsigned int VAO, VBO;
    unsigned int particleShaderProgram = NULL;
public:
    Particles() {}
    Logger* log = Logger::GetLogger();
    void Init();
    void Render();
    ~Particles();
};

