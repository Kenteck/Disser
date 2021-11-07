#include <Windows.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include "thrust/device_ptr.h"
#include "thrust/for_each.h"
#include "thrust/iterator/zip_iterator.h"
#include "thrust/sort.h"
#include "helper_math.h"
#include "configs.h"
#include "kernel.cuh"
#include <curand_kernel.h>
#include <curand.h>
#include <cooperative_groups.h>

namespace cg = cooperative_groups;

typedef unsigned int uint;

// This will output the proper error string when calling cudaGetLastError
#define getLastCudaError(msg) __getLastCudaError(msg, __FILE__, __LINE__)
static const char* _cudaGetErrorEnum(cudaError_t error) {
    return cudaGetErrorName(error);
}
#define checkCudaErrors(val) check((val), #val, __FILE__, __LINE__)
template <typename T>
void check(T result, char const* const func, const char* const file,
    int const line) {
    if (result) {
        fprintf(stderr, "CUDA error at %s:%d code=%d(%s) \"%s\" \n", file, line,
            static_cast<unsigned int>(result), _cudaGetErrorEnum(result), func);
        exit(EXIT_FAILURE);
    }
}

inline void __getLastCudaError(const char* errorMessage, const char* file,
    const int line) {
    cudaError_t err = cudaGetLastError();

    if (cudaSuccess != err) {
        fprintf(stderr,
            "%s(%i) : getLastCudaError() CUDA error :"
            " %s : (%d) %s.\n",
            file, line, errorMessage, static_cast<int>(err),
            cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }
}

// simulation parameters in constant memory
__device__ __constant__ struct Configuration params;

struct integrate_functor
{
    float deltaTime;

    __host__ __device__
        integrate_functor(float delta_time) : deltaTime(delta_time) {}

    template <typename Tuple>
    __device__
        void operator()(Tuple t)
    {
        volatile float2 posData = thrust::get<0>(t);
        volatile float2 velData = thrust::get<1>(t);
        float2 pos = make_float2(posData.x, posData.y);
        float2 vel = make_float2(velData.x, velData.y);

        pos += vel * deltaTime;

        if (dot(pos, pos) >= params.m_radius * params.m_radius) {
            float SQRT = (dot(pos, vel) / dot(vel, vel)) * (dot(pos, vel) / dot(vel, vel)) + ((params.m_radius * params.m_radius - dot(pos, pos)) / dot(vel, vel));
            float dtPart = fabs(sqrt(fabs(SQRT)) - dot(pos, vel) / dot(vel, vel));
            
            float2 posTMP = pos + vel * dtPart;
            float2 velTMP = vel;
            float HypotVelocity = length(vel);

            float2 normal = -posTMP / params.m_radius;

            float velNormal = dot(velTMP, normal);

            float2 velPart = velTMP - 2 * normal * velNormal;

            vel = velPart;

            pos = posTMP + vel * dtPart;
            
            vel = (vel / length(vel)) * HypotVelocity; // comepnsation of the float multiplication
        }

        // store new position and velocity
        
        thrust::get<0>(t) = pos;
        thrust::get<1>(t) = vel;
    }
};

void integrateSystem(float* pos, float* vel, float deltaTime, uint numParticles)
{
    thrust::device_ptr<float2> d_pos2((float2*)pos);
    thrust::device_ptr<float2> d_vel2((float2*)vel);
    thrust::for_each(
        thrust::make_zip_iterator(thrust::make_tuple(d_pos2, d_vel2)),
        thrust::make_zip_iterator(thrust::make_tuple(d_pos2 + numParticles, d_vel2 + numParticles)),
        integrate_functor(deltaTime));
}

struct momentum_functor
{
    template <typename Tuple>
    __device__
        void operator()(Tuple t)
    {
        volatile float2 posData = thrust::get<0>(t);
        volatile float2 velData = thrust::get<1>(t);
        volatile float momentumData = thrust::get<2>(t);
        
        float2 pos = make_float2(posData.x, posData.y);
        float2 vel = make_float2(velData.x, velData.y);

        float momentum = pos.x * vel.y - pos.y * vel.x;

        thrust::get<2>(t) = momentum;
    }
};

float integrateMomentumOfSystem(float* pos, float* vel, float* momentum, uint numParticles)
{
    thrust::device_ptr<float2> d_pos((float2*)pos);
    thrust::device_ptr<float2> d_vel((float2*)vel);
    thrust::device_ptr<float> d_momentum((float*)momentum);
    thrust::for_each(
        thrust::make_zip_iterator(thrust::make_tuple(d_pos, d_vel, d_momentum)),
        thrust::make_zip_iterator(thrust::make_tuple(d_pos + numParticles, d_vel + numParticles, d_momentum + numParticles)),
        momentum_functor());
    return thrust::reduce(d_momentum, d_momentum + numParticles, 0);;
}


//Round a / b to nearest higher integer value
uint iDivUp(uint a, uint b)
{
    return (a % b != 0) ? (a / b + 1) : (a / b);
}

// compute grid and thread block size for a given number of elements
void computeGridSize(uint n, uint blockSize, uint& numBlocks, uint& numThreads)
{
    numThreads = min(blockSize, n);
    numBlocks = iDivUp(n, numThreads);
}

// calculate position in uniform grid
__device__ int2 calcGridPos(float2 p)
{
    int2 gridPos;
    gridPos.x = floor((p.x) / params.m_sizeOfCell);
    gridPos.y = floor((p.y) / params.m_sizeOfCell);
    return gridPos;
}

// calculate address in grid from position (clamping to edges)
__device__ uint calcGridHash(int2 gridPos)
{
    gridPos.x = gridPos.x & (params.m_gridSize - 1);  // wrap grid, assumes size is power of 2
    gridPos.y = gridPos.y & (params.m_gridSize - 1);
    return __umul24(gridPos.y, params.m_gridSize) + gridPos.x;
}


// calculate grid hash value for each particle
__global__ void calcHashD(uint* gridParticleHash,  // output
    uint* gridParticleIndex, // output
    float2* pos,               // input: positions
    uint    numParticles)
{
    uint index = __umul24(blockIdx.x, blockDim.x) + threadIdx.x;

    if (index >= numParticles) return;

    volatile float2 p = pos[index];

    // get address in grid
    int2 gridPos = calcGridPos(make_float2(p.x, p.y));
    uint hash = calcGridHash(gridPos);

    // store grid hash and particle index
    gridParticleHash[index] = hash;
    gridParticleIndex[index] = index;
}

void calcHash(uint* gridParticleHash,
    uint* gridParticleIndex,
    float* pos,
    uint    numParticles)
{
    uint numThreads, numBlocks;
    computeGridSize(numParticles, 1024, numBlocks, numThreads);
    
    // execute the kernel
    calcHashD << < numBlocks, numThreads >> > (gridParticleHash,
        gridParticleIndex,
        (float2*)pos,
        numParticles);
    
    // check if kernel invocation generated an error
    getLastCudaError("Kernel execution failed");
}

void sortParticles(uint* dGridParticleHash, uint* dGridParticleIndex, uint numParticles)
{
    thrust::sort_by_key(thrust::device_ptr<uint>(dGridParticleHash),
        thrust::device_ptr<uint>(dGridParticleHash + numParticles),
        thrust::device_ptr<uint>(dGridParticleIndex));
}

// rearrange particle data into sorted order, and find the start of each cell
// in the sorted hash array
__global__
void reorderDataAndFindCellStartD(uint* cellStart,        // output: cell start index
    uint* cellEnd,          // output: cell end index
    float2* sortedPos,        // output: sorted positions
    float2* sortedVel,        // output: sorted velocities
    uint* gridParticleHash, // input: sorted grid hashes
    uint* gridParticleIndex,// input: sorted particle indices
    float2* oldPos,           // input: sorted position array
    float2* oldVel,           // input: sorted velocity array
    uint    numParticles)
{
    // Handle to thread block group
    
    cg::thread_block cta = cg::this_thread_block();
    extern __shared__ uint sharedHash[];    // blockSize + 1 elements
    uint index = __umul24(blockIdx.x, blockDim.x) + threadIdx.x;

    uint hash;

    // handle case when no. of particles not multiple of block size
    if (index < numParticles)
    {
        hash = gridParticleHash[index];

        // Load hash data into shared memory so that we can look
        // at neighboring particle's hash value without loading
        // two hash values per thread
        sharedHash[threadIdx.x + 1] = hash;

        if (index > 0 && threadIdx.x == 0)
        {
            // first thread in block must load neighbor particle hash
            sharedHash[0] = gridParticleHash[index - 1];
        }
    }

    cg::sync(cta);

    if (index < numParticles)
    {
        // If this particle has a different cell index to the previous
        // particle then it must be the first particle in the cell,
        // so store the index of this particle in the cell.
        // As it isn't the first particle, it must also be the cell end of
        // the previous particle's cell

        if (index == 0 || hash != sharedHash[threadIdx.x])
        {
            cellStart[hash] = index;

            if (index > 0)
                cellEnd[sharedHash[threadIdx.x]] = index;
        }

        if (index == numParticles - 1)
        {
            cellEnd[hash] = index + 1;
        }

        // Now use the sorted index to reorder the pos and vel data
        uint sortedIndex = gridParticleIndex[index];
        float2 pos = oldPos[sortedIndex];
        float2 vel = oldVel[sortedIndex];

        sortedPos[index] = pos;
        sortedVel[index] = vel;
    }


}

void reorderDataAndFindCellStart(uint* cellStart,
    uint* cellEnd,
    float* sortedPos,
    float* sortedVel,
    uint* gridParticleHash,
    uint* gridParticleIndex,
    float* oldPos,
    float* oldVel,
    uint   numParticles,
    uint   numCells)
{
    uint numThreads, numBlocks;
    computeGridSize(numParticles, 1024, numBlocks, numThreads);

    // set all cells to empty
    checkCudaErrors(cudaMemset(cellStart, 0xffffffff, numCells * sizeof(uint)));

    uint smemSize = sizeof(uint) * (numThreads + 1);
    reorderDataAndFindCellStartD << < numBlocks, numThreads, smemSize >> > (
        cellStart,
        cellEnd,
        (float2*)sortedPos,
        (float2*)sortedVel,
        gridParticleHash,
        gridParticleIndex,
        (float2*)oldPos,
        (float2*)oldVel,
        numParticles);
    getLastCudaError("Kernel execution failed: reorderDataAndFindCellStartD");

}

// collide a particle against all other particles in a given cell
__device__
float2 collideCell(int2 gridPos,
    uint    index,
    float2* oldPos,
    float2* oldVel,
    uint* cellStart,
    uint* cellEnd)
{
    uint gridHash = calcGridHash(gridPos);

    // get start of bucket for this cell
    uint startIndex = cellStart[gridHash];

    float2 force = make_float2(0.0f, 0.0f);

    if (startIndex != 0xffffffff)          // cell is not empty
    {
        // iterate over particles in this cell
        uint endIndex = cellEnd[gridHash];

        for (uint j = startIndex; j < endIndex; j++)
        {
            if (index != j) {
                float dist = length(oldPos[j] - oldPos[index]);
                if (dist <= params.m_interactionDistance) {
                    float Hypot = length(oldVel[j]);
                    force += oldVel[j] / Hypot;
                }
            }
        }
    }

    return force;
}

__global__
void collideD(float2* newVel,               // output: new velocity
    float2* oldPos,               // input: sorted positions
    float2* oldVel,               // input: sorted velocities
    uint* gridParticleIndex,    // input: sorted particle indices
    uint* cellStart,
    uint* cellEnd,
    uint  numParticles,
    curandState* states)
{
    uint index = __mul24(blockIdx.x, blockDim.x) + threadIdx.x;

    if (index >= numParticles) return;

    curand_init(index, index, 0, &states[index]);   // 	Initialize CURAND

    // read particle data from sorted arrays
    float2 pos = oldPos[index];
    float2 vel = oldVel[index];

    // get address in grid
    int2 gridPos = calcGridPos(pos);

    // examine neighbouring cells
    float2 force = make_float2(0.0f, 0.0f);

    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            int2 neighbourPos = gridPos + make_int2(x, y);
            force += collideCell(neighbourPos, index, oldPos, oldVel, cellStart, cellEnd);
        }
    }

    if (length(force) > 1e-5) {
        force.x += params.m_noice * (1 - 2 * (curand_normal(&states[index])));
        force.y += params.m_noice * (1 - 2 * (curand_normal(&states[index])));
        float HypotForce = length(force);

        float2 Ort = force / HypotForce;

        float HypotVelocity = length(vel);

        vel = Ort * HypotVelocity;
    }
    else 
    {
        float HypotVelocity = length(vel);

        vel.x += params.m_noice * (1 - 2 * (curand_normal(&states[index])));
        vel.y += params.m_noice * (1 - 2 * (curand_normal(&states[index])));

        float HypotForce = length(vel);

        float2 Ort = vel / HypotForce;

        vel = Ort * HypotVelocity;
    }
    // write new velocity back to original unsorted location
    uint originalIndex = gridParticleIndex[index];
    newVel[originalIndex] = vel;
}


void collide(float* newVel,
    float* sortedPos,
    float* sortedVel,
    uint* gridParticleIndex,
    uint* cellStart,
    uint* cellEnd,
    uint   numParticles,
    uint   numCells,
    curandState* states)
{

    // thread per particle
    uint numThreads, numBlocks;
    computeGridSize(numParticles, 256, numBlocks, numThreads);

    // execute the kernel
    collideD << < numBlocks, numThreads >> > ((float2*)newVel,
        (float2*)sortedPos,
        (float2*)sortedVel,
        gridParticleIndex,
        cellStart,
        cellEnd,
        numParticles,
        states);

    // check if kernel invocation generated an error
    getLastCudaError("Kernel execution failed");

}

void setParameters(Configuration* hostParams)
{
    // copy parameters to constant memory
    checkCudaErrors(cudaMemcpyToSymbol(params, hostParams, sizeof(Configuration)));
}