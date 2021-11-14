#pragma once
#include "vector_types.h"
#include <stdlib.h>

typedef unsigned int uint;

struct Configuration
{
	enum VelocityDistribution
	{
		UNIFORM,
		POISSON,
		NORMAL
	};

	// Simulation size
	const float m_radius = 100.0f;
	const int m_ringSegments = 100;
	const float m_PI = 3.1415926f;

	// Particles behavior parameters
	const unsigned long m_NumberOfParticles = 200000;
	const int m_coeff = 1;
	const float dt = m_coeff * 1.0f / 30.0f;
	const float m_pointSize = 1.5f;
	float m_interactionDistance = 15.0f;
	float m_noice = 6.0f;
	int m_typeDistribution = VelocityDistribution::NORMAL;
	float m_sttDev = 10.0f;
	float m_mean = 20.0f;
	
	// Performance parameteres
	const float m_sizeOfCell = m_interactionDistance;
	const int m_gridSize = (2 * m_radius) / m_sizeOfCell + 1;
	const int m_numGridCells = m_gridSize * m_gridSize;

	// Dump parameters
	const int m_numberOfCircles = 10;
	int m_tryNumber = 0;
};

void setConfig(int* argc, char** argv);

extern Configuration configs;