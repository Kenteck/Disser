#include "app.h"
// Funcs to use in glut
void display();
void keyboardfunc(unsigned char key, int /*x*/, int /*y*/);
void GlutCleanup();

//Variables to use in glut
Logger* log_glut = Logger::GetLogger();
Particles* particles = nullptr;
Ring* ring = nullptr;

void app::InitGL(int* argc, char** argv)
{
    log->LogInfo("Initialization of OpenGL Started");

    InitGlut(argc, argv);
    InitGlew();
    InitOpenGL();
    
    log->LogInfo("Initialization of OpenGL finished successfully");
}

void app::InitGlut(int* argc, char** argv)
{
    log->LogInfo("Glut initialization: start");
    
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(W_WIDTH, W_HEIGHT);
    glutCreateWindow("Simulation");
    glutReportErrors();

    log->LogInfo("Glut initialization: finished");
}

void app::InitGlew()
{
    log->LogInfo("Glew initialization: start");

    glewInit();
    glEnable(GL_DEPTH_TEST);

    log->LogInfo("Glew initialization: finished");
}

void app::InitOpenGL()
{
    
    log->LogInfo("The size of window was changed to: " + std::to_string(W_WIDTH) + " " + std::to_string(W_HEIGHT));
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardfunc);
    glutCloseFunc(GlutCleanup);
    glutIdleFunc(display);
}

void app::InitCamera()
{
    log->LogInfo("Camera initialization: start");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float ratio = W_WIDTH * 1.0 / W_HEIGHT;

    gluPerspective(60.0f, ratio, 0.1f, 190.0f);
    glViewport(0, 0, W_WIDTH, W_HEIGHT);

    
    glMatrixMode(GL_MODELVIEW);
    
    log->LogInfo("Camera initialization: finished");
}
void app::InitParticles()
{
    log->LogInfo("Particles initialization: start");
    particles = new Particles();
    particles->Init();

    log->LogInfo("Particles initialization: finished");
}

void app::InitRing()
{
    log->LogInfo("Ring initialization: start");
    ring = new Ring();
    ring->Init();

    log->LogInfo("Ring initialization: finished");
}

void keyboardfunc(unsigned char key, int /*x*/, int /*y*/)
{
    switch (key)
    {
        case '\033':
        case 'q':
            glutDestroyWindow(glutGetWindow());
            return;
    }
    log_glut->LogInfo(std::string("The key: ") + std::string(reinterpret_cast<char*>(&key)) + std::string(" was pressed"));
    glutPostRedisplay();
}

void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -175.0f);
    
    ring->Render();
    particles->Render();
    
    glutSwapBuffers();
}

void app::run()
{
    log->LogInfo("Main Loop starting");
    glutMainLoop();
}

void GlutCleanup()
{
    log_glut->LogInfo("Exiting application");
    particles->Dump();
    log_glut->Flush();
}