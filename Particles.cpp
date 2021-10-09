#include "Particles.h"

void Particles::Init()
{
	log->LogInfo("Setup Particles in GPU");
	log->LogInfo("Setup Particles in GPU: Vertices");
	
	log->LogInfo("Setup Particles in GPU: Vertices: finished");

	log->LogInfo("Setup Particles in GPU: Shaders");
	//particleShaderProgram = InitParticleShader(log);
	log->LogInfo("Setup Particles in GPU: Shaders: finished");

	log->LogInfo("Setup Particles in GPU: finished");\
	
}

void Particles::Render()
{
	//glMatrixMode(GL_MODELVIEW);

	glTranslatef(0.0f, 0.0f, -1.0f);
	//glPopMatrix()
	//gluLookAt(0.0f, 0.0f, 10.0f,
	 //   0.0f, 0.0f, 0.0f,
	 //   0.0f, 1.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glutWireCube(0.25);
}

Particles::~Particles()
{

}