#pragma once
#include "vector_types.h"
typedef unsigned int uint;

struct Configuration
{
	const unsigned long m_NumberOfParticles = 100000;
	const int m_coeff = 1;
	const float m_PI = 3.1415926f;
	const float m_radius = 28.0f;
	const int m_ringSegments = 100;
	const float dt = m_coeff * 1.0f / 30.0f;
	const float m_pointSize = 3.0f;
	const float m_interactionDistance = 3.0f;
	const float m_sizeOfCell = 2 * m_interactionDistance;
	const int m_gridSize = (2 * m_radius) / m_sizeOfCell + 1;
	const int m_numGridCells = m_gridSize * m_gridSize;
	const float m_maxVel = 5.0f;
};

static Configuration configs;