#pragma once

struct Configuration
{
	const unsigned long m_NumberOfParticles = 500;
	const float m_PI = 3.1415926f;
	const float m_radius = 1.4f;
	const int m_ringSegments = 100;
	const float m_dt = 1.0f / 60.0f;
};

static Configuration configs;