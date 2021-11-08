#include "configs.h"

Configuration configs;
// The order of configs: noice, interactionDistance, typeofDist, mean, deviation, tryNumber
void setConfig(int* argc, char** argv)
{
	configs.m_noice = atof(argv[1]);
	configs.m_interactionDistance = atof(argv[2]);
	configs.m_typeDistribution = atoi(argv[3]);
	configs.m_mean = atof(argv[4]);
	configs.m_sttDev = atof(argv[5]);
	configs.m_tryNumber = atoi(argv[6]);
}