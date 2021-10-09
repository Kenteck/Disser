#pragma once

struct Configuration
{
	const unsigned long m_NumberOfParticles = 60000;
	const float m_PI = 3.1415926f;
	const float m_radius = 0.25f;
	const int m_ringSegments = 100;
};

static Configuration configs;