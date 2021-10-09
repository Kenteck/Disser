#include "Ring.h"
void Ring::Init()
{
	ringShaderProgram = InitRingShader(log);
}

void Ring::Render()
{
    glUseProgram(ringShaderProgram);
    glBegin(GL_LINE_LOOP);
    for (int ii = 0; ii < configs.m_ringSegments; ii++) {
        float theta = 2.0f * 3.1415926f * float(ii) / float(configs.m_ringSegments);
        float x = configs.m_radius* cosf(theta);
        float y = configs.m_radius * sinf(theta);
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glUseProgram(0);
}