#include "Particles.h"

void Particles::Init()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, NUMBER_OF_PARTICLES * 3 * sizeof(float), NULL, GL_STREAM_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void Particles::Render()
{
	glBufferSubData(GL_ARRAY_BUFFER, 0, NUMBER_OF_PARTICLES * 3 * sizeof(float), vertices);
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, NUMBER_OF_PARTICLES);
	glBindVertexArray(0);
}