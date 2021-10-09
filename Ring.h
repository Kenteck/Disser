#pragma once
#include "configs.h"
#include "Logger.h"
#include "Shader.h"
class Ring
{
	unsigned int ringShaderProgram;

	Logger* log = Logger::GetLogger();
public:
	void Init();
	void Render();
};