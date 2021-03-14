#include "Particles.h"

void Particles::Init(std::shared_ptr<Logger> log)
{
	this->log = log;
	log->LogInfo("Setup Particles in GPU");
	log->LogInfo("Setup Particles in GPU: Vertices");
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	log->LogInfo("Setup Particles in GPU: Vertices: finished");

	log->LogInfo("Setup Particles in GPU: Shaders");
	particleShaderProgram = InitParticleShader(log);
	log->LogInfo("Setup Particles in GPU: Shaders: finished");

	log->LogInfo("Setup Particles in GPU: finished");
}

void Particles::Render()
{
	glUseProgram(particleShaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

Particles::~Particles()
{

}