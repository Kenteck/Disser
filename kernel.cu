#include <Windows.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include "thrust/device_ptr.h"
#include "thrust/for_each.h"
#include "thrust/iterator/zip_iterator.h"
#include "helper_math.h"
#include "kernel.cuh"

typedef unsigned int uint;

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


        if (pos.x > 1.4f)
        {
            vel.x *= -1;
        }

        if (pos.x < -1.4f)
        {
            vel.x *= -1;
        }

        if (pos.y > 1.4f)
        {
            vel.y *= -1;
        }

        if (pos.y < -1.4f)
        {
            vel.y *= -1;
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