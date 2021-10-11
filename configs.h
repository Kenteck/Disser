#pragma once

struct Configuration
{
	const unsigned long m_NumberOfParticles = 10000;
	const int m_coeff = 1;
	const float m_PI = 3.1415926f;
	const float m_radius = 28.0f;
	const int m_ringSegments = 100;
	const float dt = m_coeff * 1.0f / 30.0f;
	const float m_pointSize = 1.5f;
};

static Configuration configs;