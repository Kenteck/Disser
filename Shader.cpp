#include "Shader.h"
//////////////////////////////////////////////////////Particles////////////////////////////////////////////////
const char* VertexShaderSourceParticle = "#version 450 core\n"
///////////////////data position/////////////////////////////
"layout (location = 0) in vec3 aPos;\n"
/////////////////used data//////////////////////////////////////
"void main()\n"
"{\n"
"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); \n"
"}\0";

const char* FragmentShaderSourceParticle = "#version 450 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\0";

////////////////////////////////////////////////Particles///////////////////////////////////////////////////
unsigned int InitParticleShader(std::shared_ptr<Logger> log)
{
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &VertexShaderSourceParticle, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		log->LogError("ERROR VERTEX SHADER PARTICLE COMPILATION: " + std::string(infoLog));
		throw std::runtime_error("ERROR VERTEX SHADER PARTICLE COMPILATION: " + std::string(infoLog));
	}
	else
		log->LogInfo("Vertex shader particle compilation successful");


	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &FragmentShaderSourceParticle, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		log->LogError("ERROR FRAGMENT SHADER PARTICLE COMPILATION: " + std::string(infoLog));
		throw std::runtime_error("ERROR FRAGMENT SHADER PARTICLE COMPILATION: " + std::string(infoLog));
	} else 
		log->LogInfo("Fragment shader particle compilation successful");

	///////////////////////////////////////////shader program///////////////////////////////////////////
	unsigned int ShaderProgram;
	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, vertexShader);
	glAttachShader(ShaderProgram, fragmentShader);
	glLinkProgram(ShaderProgram);

	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(ShaderProgram, 512, NULL, infoLog);
		log->LogError("ERROR SHADER PROGRAM LINK: " + std::string(infoLog));
		throw std::runtime_error("ERROR SHADER PROGRAM LINK: " + std::string(infoLog));
	} else
		log->LogInfo("Shader program link successful");


	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return ShaderProgram;
}