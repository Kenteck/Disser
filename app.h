#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Logger.h"
#include <string>
#include "Particles.h"

//Consts
#define W_HEIGHT 1080
#define W_WIDTH 1920

class app
{
	void InitGLFW();
	void InitGLAD();
	void InitOpenGL();
	Particles* InitParticles(std::shared_ptr<Logger>);

	// OpenGl functions
	void static framebuffer_size_callback(GLFWwindow*, int, int);
	void processInput(GLFWwindow*);

public:
	void init() {
		log->Init();
		InitGLFW();
		InitGLAD();
		InitOpenGL();
		particles = InitParticles(log);
	}

	void run();

	~app();

	// Logger
	std::shared_ptr<Logger> log = std::make_shared<Logger>();
	
	// Window
	GLFWwindow* window = nullptr;

	// Particles
	Particles* particles = nullptr;
};