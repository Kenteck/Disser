#include <Windows.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include "thrust/device_ptr.h"
#include "thrust/for_each.h"
#include "thrust/iterator/zip_iterator.h"
#include "helper_math.h"
#include "configs.h"
#include "kernel.cuh"

typedef unsigned int uint;

// simulation parameters in constant memory
__constant__ struct Configuration params;

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
            float dt1 = fabs(sqrt(fabs(SQRT)) - dot(pos, vel) / dot(vel, vel));
            
            float2 pos_1 = pos + vel * dt1;
            float2 vel_1 = vel;

            float2 nx = -pos_1 / params.m_radius;

            float vel_n = dot(vel_1, nx);

            float2 vel_1_n = vel_1 - 2 * nx * vel_n;

            vel = vel_1_n;

            pos = pos_1 + vel * dt1;
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