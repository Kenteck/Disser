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

    //GPU data
    float* m_dVel;
    float* m_dPos;

    struct cudaGraphicsResource* m_cuda_posvbo_resource; // handles OpenGL-CUDA exchange

    enum ParticleArray
    {
        POSITION,
        VELOCITY,
    };

public:
    Particles() {}
    Logger* log = Logger::GetLogger();
    void Init();
    void InitCUDA();
    void createVBO(unsigned int);
    void Render();
    void Move();
    void SetArray(ParticleArray, const float*, int, int);
    ~Particles();
};

extern "C" void cudaInit();
extern "C" void allocateArray(void** devPtr, size_t size);
extern "C" void freeArray(void* devPtr);
extern "C" void threadSync();
extern "C" void copyArrayToDevice(void* device, const void* host, int offset, int size);
extern "C" void registerGLBufferObject(unsigned int vbo, struct cudaGraphicsResource** cuda_vbo_resource);
extern "C" void unregisterGLBufferObject(struct cudaGraphicsResource* cuda_vbo_resource);
extern "C" void* mapGLBufferObject(struct cudaGraphicsResource** cuda_vbo_resource);
extern "C" void unmapGLBufferObject(struct cudaGraphicsResource* cuda_vbo_resource);
extern "C" void setParameters(Configuration*);