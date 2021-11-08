#pragma once
#include "GL/glew.h"
#include "GL/freeglut.h"
#include <memory>
#include "Logger.h"
#include "Shader.h"
#include "configs.h"
#include <curand_kernel.h>
#include <random>
#include <fstream>

#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
extern Configuration configs;

class Particles
{
    unsigned int VAO, VBO, colorVBO;
    unsigned int particleShaderProgram = NULL;
    float* m_vertices = new float[configs.m_NumberOfParticles * 2];
    float* m_velocity = new float[configs.m_NumberOfParticles * 2];
    float* m_module = new float[configs.m_NumberOfParticles];
    float* m_colors = new float[configs.m_NumberOfParticles * 3];

    float local_max = -configs.m_mean;
    float local_min = configs.m_mean;

    int m_momentumCounter = 0;
    float m_previousResult = 0;
    bool m_expFinish = false;

    //GPU data
    float* m_dVel;
    float* m_dPos;
    float* m_dMomentum;

    struct cudaGraphicsResource* m_cuda_posvbo_resource; // handles OpenGL-CUDA exchange
    struct cudaGraphicsResource* m_cuda_colorvbo_resource;

    // grid data for sorting method
    uint* m_dGridParticleHash; // grid hash value for each particle
    uint* m_dGridParticleIndex;// particle index for each particle
    uint* m_dCellStart;        // index of start of each cell in sorted list
    uint* m_dCellEnd;          // index of end of cell

    uint* m_hCellStart;
    uint* m_hCellEnd;

    float* m_dSortedPos;
    float* m_dSortedVel;

    curandState* m_dRandom;

    std::clock_t start = std::clock();
    const float duration = 10 * 60;  // Max seconds of simulation

    enum ParticleArray
    {
        POSITION,
        VELOCITY
    };

    std::string m_dmpFile = "ParticlesPos.txt";

public:
    Particles() {}
    Logger* log = Logger::GetLogger();
    void Init();
    void InitVertices();
    void InitVelocities();
    void InitCUDA();
    void createVBO(unsigned int);
    void createColorVBO(unsigned int);
    void fillColors();
    void Render();
    void Move();
    void SetArray(ParticleArray, const float*, int, int);
    void checkMomentum(float currentMomentum);
    void checkTime();
    void savePicture();
    void Dump();
    ~Particles();
};

extern "C" void cudaInit();
extern "C" void allocateArray(void** devPtr, size_t size);
extern "C" void freeArray(void* devPtr);
extern "C" void threadSync();
extern "C" void copyArrayToDevice(void* device, const void* host, int offset, int size);
extern "C" void copyArrayFromDevice(void* , const void* , struct cudaGraphicsResource** , int);
extern "C" void registerGLBufferObject(unsigned int vbo, struct cudaGraphicsResource** cuda_vbo_resource);
extern "C" void unregisterGLBufferObject(struct cudaGraphicsResource* cuda_vbo_resource);
extern "C" void* mapGLBufferObject(struct cudaGraphicsResource** cuda_vbo_resource);
extern "C" void unmapGLBufferObject(struct cudaGraphicsResource* cuda_vbo_resource);
extern "C" void setParameters(Configuration*);