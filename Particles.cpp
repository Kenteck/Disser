#include "Particles.h"

void Particles::Init()
{
	log->LogInfo("Setup Particles in GPU");
	log->LogInfo("Setup Particles in GPU: Vertices");
	m_vertices[0] = 0.0f;
	m_vertices[1] = 1.4f;

	m_velocity[0] = 0.05f;
	m_velocity[1] = -0.01f;
	log->LogInfo("Setup Particles in GPU: Vertices: finished");

	log->LogInfo("Setup VBOs: started");
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, configs.m_NumberOfParticles * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	log->LogInfo("Setup VBOs: finished");
	
	log->LogInfo("Setup Particles in GPU: Shaders");
	particleShaderProgram = InitParticleShader(log);
	log->LogInfo("Setup Particles in GPU: Shaders: finished");

	log->LogInfo("Setup Particles in GPU: finished");\
	
}

void Particles::Render()
{
	//glMatrixMode(GL_MODELVIEW);
	Move();
	
	//glPopMatrix()
	//gluLookAt(0.0f, 0.0f, 10.0f,
	 //   0.0f, 0.0f, 0.0f,
	 //   0.0f, 1.0f, 0.0f);
	glUseProgram(particleShaderProgram);
	glBufferSubData(GL_ARRAY_BUFFER, 0, configs.m_NumberOfParticles * 2 * sizeof(float), m_vertices);
	glBindVertexArray(VAO);
	glPointSize(10.0f);
	glDrawArrays(GL_POINTS, 0, configs.m_NumberOfParticles);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Particles::Move()
{
	if (fabs(m_vertices[0]) > configs.m_radius)
		m_velocity[0] *= -1;

	if (fabs(m_vertices[1]) > configs.m_radius)
		m_velocity[1] *= -1;

	m_vertices[0] += m_velocity[0];
	m_vertices[1] += m_velocity[1];
}

Particles::~Particles()
{

}