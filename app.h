#pragma once
#include "GL/glew.h"
#include "GL/wglew.h"
#include "GL/freeglut.h"
#include <iostream>
#include "Logger.h"
#include <string>
#include "Particles.h"
#include "Ring.h"
#include "configs.h"
//Consts
#define W_HEIGHT 1080
#define W_WIDTH 1920

class app
{

	void InitGL(int* argc, char** argv);
	void InitGlut(int* argc, char** argv);
	void InitGlew();
	void InitOpenGL();
	void InitCamera();
	void InitParticles();
	void InitRing();

public:
	void init(int* argc, char** argv) {
		if (*argc > 1)
			setConfig(argc, argv);
		log->Init();
		InitGL(argc, argv);
		InitCamera();
		InitParticles();
		InitRing();
	}

	void run();

	// Logger
	Logger* log = Logger::GetLogger();
};