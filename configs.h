#pragma once
#include "vector_types.h"
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
	const unsigned long m_NumberOfParticles = 100000;
	const int m_coeff = 1;
	const float dt = m_coeff * 1.0f / 30.0f;
	const float m_pointSize = 1.5f;
	const float m_interactionDistance = 10.0f;
	const float m_noice = 0.5f;
	const int m_typeDistribution = VelocityDistribution::NORMAL;
	const float m_sttDev = 5.0f;
	const float m_mean = 15.0f;
	
	// Performance parameteres
	const float m_sizeOfCell = m_interactionDistance;
	const int m_gridSize = (2 * m_radius) / m_sizeOfCell + 1;
	const int m_numGridCells = m_gridSize * m_gridSize;

	// Dump parameters
	const int m_numberOfCircles = 7;
};

extern Configuration configs;