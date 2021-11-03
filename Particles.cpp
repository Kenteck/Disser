#include "Particles.h"
#include "cuda_helper_functions.cu"
#include "kernel.cuh"

float length(float first, float second)
{
	return sqrtf(pow(first, 2) + pow(second, 2));
}

void Particles::Init()
{
	log->LogInfo("Setup Particles");
	
	InitVertices();
	InitVelocities();
	
	log->LogInfo("Setup Particles: Shaders");
	particleShaderProgram = InitParticleShader(log);
	log->LogInfo("Setup Particles: Shaders: finished");

	log->LogInfo("Setup Particles: finished");
	InitCUDA();
}

void Particles::InitVertices()
{
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
	}
	log->LogInfo("Setup Particles: Vertices: finished");
}

void Particles::InitVelocities()
{
	log->LogInfo("Setup Particles: Velocities");
	std::default_random_engine generator;
	switch (configs.m_typeDistribution)
	{
		case Configuration::VelocityDistribution::UNIFORM:
		{
			std::uniform_real_distribution<float> uni_distribution(0, 2 * configs.m_mean);
			for (size_t i = 0; i < configs.m_NumberOfParticles; i++) {
				m_module[i] = uni_distribution(generator);
			}
		}

		case Configuration::VelocityDistribution::POISSON:
		{
			std::poisson_distribution<int> poi_distribution(configs.m_mean);
			for (size_t i = 0; i < configs.m_NumberOfParticles; i++) {
				m_module[i] = poi_distribution(generator);
			}
		}

		case Configuration::VelocityDistribution::NORMAL:
		{
			std::normal_distribution<float> nor_distribution(configs.m_mean, configs.m_sttDev);
			for (size_t i = 0; i < configs.m_NumberOfParticles; i++) {
				m_module[i] = nor_distribution(generator);
			}
		}
	}
	for (int i = 0; i < configs.m_NumberOfParticles; i++) {
		double phi = 2. * configs.m_PI * ((double)(rand()) / RAND_MAX);
		m_velocity[2 * i] = m_module[i] * cos(phi);
		m_velocity[2 * i + 1] = m_module[i] * sin(phi);

		if (local_max < m_module[i]) local_max = m_module[i];
		if (local_min > m_module[i]) local_min = m_module[i];
	}
		
	log->LogInfo("Setup Particles: Velocities: finished");
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
	allocateArray((void**)&m_dMomentum, sizeof(float) * configs.m_NumberOfParticles);

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

	allocateArray((void**)&m_dRandom, configs.m_NumberOfParticles * sizeof(curandState));

	fillColors();
	
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

void Particles::fillColors()
{
	log->LogInfo("Setup Colors: started");
	createColorVBO(configs.m_NumberOfParticles * 3 * sizeof(float));
	registerGLBufferObject(colorVBO, &m_cuda_colorvbo_resource);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
 	float* data = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	
	for (uint i = 0; i < configs.m_NumberOfParticles; i++)
	{
		// The faster particles are red, the slower are blue
		data[3 * i] = (m_module[i] - local_min) / (local_max - local_min);
		data[3 * i + 1] = 0.1f;
		data[3 * i + 2] = 1.0f - data[3 * i];
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	log->LogInfo("Setup Colors: finished");
}

void Particles::createColorVBO(unsigned int size)
{
	log->LogInfo("Setup Color VBO: started");
	glGenBuffers(1, &colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	log->LogInfo("Setup Color VBO: finished");
}

void Particles::Render()
{
	Move();

	glColor3f(1, 1, 1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_COLOR_ARRAY);

	glPointSize(configs.m_pointSize);
	glDrawArrays(GL_POINTS, 0, configs.m_NumberOfParticles);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glUseProgram(0);

	if (m_expFinish) {
		glutDestroyWindow(glutGetWindow());
		glutPostRedisplay();
	}
}

void Particles::Move()
{
	float* dPos;
	float currentResult = 0;

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
		configs.m_numGridCells,
		m_dRandom);
	 
	// calculate momentum of the System
	
	currentResult = integrateMomentumOfSystem(
						dPos,
						m_dVel,
						m_dMomentum,
						configs.m_NumberOfParticles);

	
	if (abs(currentResult - m_previousResult) < currentResult * 0.01) {
		if (++m_momentumCounter >= 50)
			m_expFinish = true; 
	}
	else {
		m_momentumCounter = 0;
	}


	m_previousResult = currentResult;

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

void Particles::Dump()
{
	copyArrayFromDevice(m_vertices, 0, &m_cuda_posvbo_resource, sizeof(float) * 2 * configs.m_NumberOfParticles);

	std::ofstream file(m_dmpFile, std::ios::trunc);
	if (file.is_open()) {
		file << "Distribution type: ";
		switch (configs.m_typeDistribution) {
		case Configuration::VelocityDistribution::NORMAL:
			file << "Normal\n";
			break;
		case Configuration::VelocityDistribution::UNIFORM:
			file << "Uniform\n";
			break;
		case Configuration::VelocityDistribution::POISSON:
			file << "Poisson\n";
			file.close();
		}
		file << "Number of particles: " << configs.m_NumberOfParticles << std::endl;
		file << "Number of circles: " << configs.m_numberOfCircles << std::endl;
		float minDist, maxDist;
		for (int circle = 0; circle < configs.m_numberOfCircles; circle++) {
			minDist = (configs.m_radius / configs.m_numberOfCircles) * circle;
			maxDist = (configs.m_radius / configs.m_numberOfCircles) * (circle + 1);
			int particlesCount = 0;

			for (int particle = 0; particle < configs.m_NumberOfParticles; particle++) {
				float dist = length(m_vertices[2 * particle], m_vertices[2 * particle + 1]);
				if (((minDist < dist) && (maxDist >= dist)) || ((circle+1 == configs.m_numberOfCircles) && (minDist < dist)))
					particlesCount++;
			}

			file << "Circle " << circle + 1 << ": " << particlesCount << std::endl;
		}
	}
	log->LogInfo("Particle data dumped");
}

Particles::~Particles()
{
	delete[] m_vertices;
	delete[] m_velocity;
	delete[] m_module;
	delete[] m_colors;
	delete[] m_hCellStart;
	delete[] m_hCellEnd;

	freeArray(m_dVel);
	freeArray(m_dSortedPos);
	freeArray(m_dSortedVel);

	freeArray(m_dGridParticleHash);
	freeArray(m_dGridParticleIndex);
	freeArray(m_dCellStart);
	freeArray(m_dCellEnd);
	unregisterGLBufferObject(m_cuda_colorvbo_resource);
	unregisterGLBufferObject(m_cuda_posvbo_resource);
	glDeleteBuffers(1, (const GLuint*)&VBO);
	glDeleteBuffers(1, (const GLuint*)&colorVBO);
}