#include "Particles.h"
#include "cuda_helper_functions.cu"
#include "kernel.cuh"

void Particles::Init()
{
	log->LogInfo("Setup Particles");
	
	log->LogInfo("Setup Particles: Vertices");
	m_vertices[0] = 0.0f;
	m_vertices[1] = 1.4f;

	m_velocity[0] = 1.0f;
	m_velocity[1] = -1.0f;
	log->LogInfo("Setup Particles: Vertices: finished");

	
	log->LogInfo("Setup Particles: Shaders");
	particleShaderProgram = InitParticleShader(log);
	log->LogInfo("Setup Particles: Shaders: finished");

	log->LogInfo("Setup Particles: finished");
	InitCUDA();
}

void Particles::InitCUDA()
{
	log->LogInfo("Setup Particles CUDA: started");

	cudaInit();

	// allocate GPU data
	unsigned int memSize = sizeof(float) * 2 * configs.m_NumberOfParticles;
	createVBO(memSize);
	registerGLBufferObject(VBO, &m_cuda_posvbo_resource);

	allocateArray((void**)&m_dVel, memSize);

	SetArray(POSITION, m_vertices, 0, configs.m_NumberOfParticles);
	SetArray(VELOCITY, m_velocity, 0, configs.m_NumberOfParticles);

	log->LogInfo("Setup Particles CUDA: finished");
}

void Particles::createVBO(unsigned int size)
{
	log->LogInfo("Setup VBOs: started");
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	log->LogInfo("Setup VBOs: finished");
}

void Particles::Render()
{
	Move();

	glColor3f(1, 1, 1);
	glPointSize(10.0f);
	
	glUseProgram(particleShaderProgram);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);

	glDrawArrays(GL_POINTS, 0, configs.m_NumberOfParticles);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	/*
	glBufferSubData(GL_ARRAY_BUFFER, 0, configs.m_NumberOfParticles * 2 * sizeof(float), m_vertices);
	glBindVertexArray(VAO);
	glPointSize(10.0f);
	glDrawArrays(GL_POINTS, 0, configs.m_NumberOfParticles);
	glBindVertexArray(0);*/
	glUseProgram(0);
}

void Particles::Move()
{
	float* dPos;

	dPos = (float*)mapGLBufferObject(&m_cuda_posvbo_resource);

	// integrate
	integrateSystem(
		dPos,
		m_dVel,
		configs.dt,
		configs.m_NumberOfParticles);

	unmapGLBufferObject(m_cuda_posvbo_resource);
}

void Particles::SetArray(ParticleArray array, const float* data, int start, int count)
{
	switch (array)
	{
		default:
		case POSITION:
		{
			unregisterGLBufferObject(m_cuda_posvbo_resource);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, start * 2 * sizeof(float), count * 2 * sizeof(float), data);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			registerGLBufferObject(VBO, &m_cuda_posvbo_resource);
		}
		break;
	
		case VELOCITY:
			copyArrayToDevice(m_dVel, data, start * 2 * sizeof(float), count * 2 * sizeof(float));
		break;
	}
}

Particles::~Particles()
{

}