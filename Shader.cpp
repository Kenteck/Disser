# include "Shader.h"
const char* VertexShaderSourceRing = "#version 400 core\n"
///////////////////data position/////////////////////////////
"layout (location = 0) in vec3 aPos;\n"
/////////////////used data//////////////////////////////////////
//"uniform mat4 model;\n"
//"uniform mat4 view;\n"
//"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"gl_Position = gl_ModelViewProjectionMatrix * vec4(aPos, 1.0);\n"
"}\0";

const char* FragmentShaderSourceRing = "#version 400 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\0";


//////////////////////////////////////////////////////Particles////////////////////////////////////////////////
const char* VertexShaderSourceParticle = "#version 400 core\n"
///////////////////data position/////////////////////////////
"layout (location = 0) in vec3 aPos;\n"
/////////////////used data//////////////////////////////////////
//"uniform mat4 view;\n"
//"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"gl_Position = gl_ModelViewProjectionMatrix * vec4(aPos, 1.0);\n"
"gl_PointSize = 10.0;\n"
"}\0";

const char* FragmentShaderSourceParticle = "#version 400 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
"}\0";


unsigned int InitRingShader(Logger* log)
{
	log->LogInfo("Ring shader initialization: started");
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &VertexShaderSourceRing, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		log->LogError("ERROR VERTEX SHADER SPHERE COMPILATION: " + std::string(infoLog));
	}


	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &FragmentShaderSourceRing, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		log->LogError("ERROR FRAGMENT SHADER SPHERE COMPILATION: " + std::string(infoLog));
	}

	///////////////////////////////////////////shader program///////////////////////////////////////////
	unsigned int ShaderProgram;
	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, vertexShader);
	glAttachShader(ShaderProgram, fragmentShader);
	glLinkProgram(ShaderProgram);

	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(ShaderProgram, 512, NULL, infoLog);
		log->LogError("ERROR SHADER RING PROGRAM LINK: " + std::string(infoLog));
	}


	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	log->LogInfo("Ring shader initialization: finished");
	return ShaderProgram;
}


unsigned int InitParticleShader(Logger* log)
{
	log->LogInfo("Particle shader initialization: started");
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
	}


	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &FragmentShaderSourceParticle, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		log->LogError("ERROR FRAGMENT SHADER PARTICLE COMPILATION: " + std::string(infoLog));
	}

	///////////////////////////////////////////shader program///////////////////////////////////////////
	unsigned int ShaderProgram;
	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, vertexShader);
	glAttachShader(ShaderProgram, fragmentShader);
	glLinkProgram(ShaderProgram);

	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(ShaderProgram, 512, NULL, infoLog);
		log->LogError("ERROR SHADER PARTICLE PROGRAM LINK: " + std::string(infoLog));
	}


	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	log->LogInfo("Particle shader initialization: finished");
	return ShaderProgram;
}