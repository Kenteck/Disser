#include <windows.h>
#include "cuda_func.h"
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include "configs.h"

static const char* _cudaGetErrorEnum(cudaError_t error) {
    return cudaGetErrorName(error);
}

template <typename T>
void check(T result, char const* const func, const char* const file,
    int const line) {
    if (result) {
        fprintf(stderr, "CUDA error at %s:%d code=%d(%s) \"%s\" \n", file, line,
            static_cast<unsigned int>(result), _cudaGetErrorEnum(result), func);
        exit(EXIT_FAILURE);
    }
}

#define checkCudaErrors(val) check((val), #val, __FILE__, __LINE__)

// simulation parameters in constant memory
__constant__ Configuration params;

typedef unsigned int uint;

extern "C"
{
    void cudaInit()
    {
        int devID = 0;
        int device_count = 0;
        printf("CUDA initizalition: started\n");

        checkCudaErrors(cudaGetDeviceCount(&device_count));
        printf("CUDA devices found: %i\n", device_count);

        if (device_count == 0) {
            fprintf(stderr,
                "gpuGetMaxGflopsDeviceId() CUDA error:"
                " no devices supporting CUDA.\n");
        }

        checkCudaErrors(cudaSetDevice(devID)); // Set the firts available device
        printf("CUDA initizalition: finished\n");
    }

    void allocateArray(void** devPtr, size_t size)
    {
        checkCudaErrors(cudaMalloc(devPtr, size));
    }

    void freeArray(void* devPtr)
    {
        checkCudaErrors(cudaFree(devPtr));
    }

    void threadSync()
    {
        checkCudaErrors(cudaDeviceSynchronize());
    }

    void copyArrayToDevice(void* device, const void* host, int offset, int size)
    {
        checkCudaErrors(cudaMemcpy((char*)device + offset, host, size, cudaMemcpyHostToDevice));
    }

    void registerGLBufferObject(uint vbo, struct cudaGraphicsResource** cuda_vbo_resource)
    {
        checkCudaErrors(cudaGraphicsGLRegisterBuffer(cuda_vbo_resource, vbo, cudaGraphicsMapFlagsNone));
    }

    void unregisterGLBufferObject(struct cudaGraphicsResource* cuda_vbo_resource)
    {
        checkCudaErrors(cudaGraphicsUnregisterResource(cuda_vbo_resource));
    }

    void* mapGLBufferObject(struct cudaGraphicsResource** cuda_vbo_resource)
    {
        void* ptr;
        checkCudaErrors(cudaGraphicsMapResources(1, cuda_vbo_resource, 0));
        size_t num_bytes;
        checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void**)&ptr, &num_bytes,
            *cuda_vbo_resource));
        return ptr;
    }

    void unmapGLBufferObject(struct cudaGraphicsResource* cuda_vbo_resource)
    {
        checkCudaErrors(cudaGraphicsUnmapResources(1, &cuda_vbo_resource, 0));
    }
}