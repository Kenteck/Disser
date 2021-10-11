#pragma once

struct Configuration
{
	const unsigned long m_NumberOfParticles = 100000;
	const float m_PI = 3.1415926f;
	const float m_radius = 28.0f;
	const int m_ringSegments = 100;
	const float dt = 1.0f / 60.0f;
	const float m_pointSize = 2.0f;
};

static Configuration configs;