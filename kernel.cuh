#include <curand_kernel.h>

extern "C"
{
    void integrateSystem(float* pos, float* vel, float deltaTime, uint numParticles);

    void calcHash(uint* gridParticleHash, uint* gridParticleIndex, float* pos, uint numParticles);

    void sortParticles(uint* dGridParticleHash, uint* dGridParticleIndex, uint numParticles);

    void reorderDataAndFindCellStart(uint* cellStart, uint* cellEnd, float* sortedPos, float* sortedVel, uint* gridParticleHash, uint* gridParticleIndex, float* oldPos, float* oldVel, uint numParticles, uint numCells);
    
    void collide(float* newVel, float* sortedPos, float* sortedVel, uint* gridParticleIndex, uint* cellStart, uint* cellEnd, uint numParticles, uint numCells, curandState* states);
}