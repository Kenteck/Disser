#include "Particles.h"
#include "cuda_helper_functions.cu"
#include "kernel.cuh"

void Particles::Init()
{
	log->LogInfo("Setup Particles");
	
	log->LogInfo("Setup Particles: Vertices");
	srand(time(NULL));
	for (size_t i = 0; i < configs.m_NumberOfParticles; i++) {
		float u = (float)rand() / (RAND_MAX);
		float v = (float)rand() / (RAND_MAX);
		float theta = u * 2.0 * configs.m_PI;
		float phi = acos(2.0 * v - 1.0);
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);
		float sinPhi = sin(phi);
		//Random coordinates within sphere
		m_vertices[2 * i] = cbrt((float)rand() / (RAND_MAX)) * configs.m_radius * sinPhi * cosTheta;
		m_vertices[2 * i + 1] = cbrt((float)rand() / (RAND_MAX)) * configs.m_radius * sinPhi * sinTheta;
		//Random velocities
		m_velocity[2 * i] = ((float)rand() / (RAND_MAX) * 2 - 1) * configs.m_maxVel;
		m_velocity[2 * i + 1] = ((float)rand() / (RAND_MAX) * 2 - 1) * configs.m_maxVel;
	}

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

	m_hCellStart = new uint[configs.m_numGridCells];
	memset(m_hCellStart, 0, configs.m_numGridCells * sizeof(uint));

	m_hCellEnd = new uint[configs.m_numGridCells];
	memset(m_hCellEnd, 0, configs.m_numGridCells * sizeof(uint));

	allocateArray((void**)&m_dSortedPos, memSize);
	allocateArray((void**)&m_dSortedVel, memSize);

	allocateArray((void**)&m_dGridParticleHash, configs.m_NumberOfParticles * sizeof(uint));
	allocateArray((void**)&m_dGridParticleIndex, configs.m_NumberOfParticles * sizeof(uint));

	allocateArray((void**)&m_dCellStart, configs.m_numGridCells * sizeof(uint));
	allocateArray((void**)&m_dCellEnd, configs.m_numGridCells * sizeof(uint));

	SetArray(POSITION, m_vertices, 0, configs.m_NumberOfParticles);
	SetArray(VELOCITY, m_velocity, 0, configs.m_NumberOfParticles);
	setParameters(&configs);
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
	glPointSize(configs.m_pointSize);
	
	glUseProgram(particleShaderProgram);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);

	glDrawArrays(GL_POINTS, 0, configs.m_NumberOfParticles);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

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
	
	// calculate grid hash
	calcHash(
		m_dGridParticleHash,
		m_dGridParticleIndex,
		dPos,
		configs.m_NumberOfParticles);

	//// sort particles based on hash
	sortParticles(m_dGridParticleHash, m_dGridParticleIndex, configs.m_NumberOfParticles);

	// reorder particle arrays into sorted order and
// find start and end of each cell
	reorderDataAndFindCellStart(
		m_dCellStart,
		m_dCellEnd,
		m_dSortedPos,
		m_dSortedVel,
		m_dGridParticleHash,
		m_dGridParticleIndex,
		dPos,
		m_dVel,
		configs.m_NumberOfParticles,
		configs.m_numGridCells);

	// process collisions
	collide(
		m_dVel,
		m_dSortedPos,
		m_dSortedVel,
		m_dGridParticleIndex,
		m_dCellStart,
		m_dCellEnd,
		configs.m_NumberOfParticles,
		configs.m_numGridCells);


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