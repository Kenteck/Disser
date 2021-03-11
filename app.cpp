#include "app.h"

void app::InitGLFW()
{
    log->LogInfo("GLFW initialization: start");
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Simulation", nullptr, nullptr);
    if (!window)
    {
        log->LogError("Failed to create GLFW window");
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, app::framebuffer_size_callback);
    log->LogInfo("GLFW initialization: finished");
}

void app::InitGLAD()
{
    log->LogInfo("GLAD initialization: start");
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        log->LogError("Failed to initialize GLAD");
        throw std::runtime_error("Failed to initialize GLAD");
    }
    log->LogInfo("GLAD initialization: finished");
}

void app::InitOpenGL()
{
    glViewport(0, 0, W_WIDTH, W_HEIGHT);
    log->LogInfo("The size of window was changed to: " + std::to_string(W_WIDTH) + " " + std::to_string(W_HEIGHT));
}

void app::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void app::processInput(GLFWwindow* window)
{
    if (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        log->LogInfo("Escape pressed, application finishing...");
        glfwSetWindowShouldClose(window, true);
    }
}


app::~app()
{
    glfwTerminate();
}

std::unique_ptr<Particles> app::InitParticles(std::shared_ptr<Logger> log)
{
    std::unique_ptr<Particles> particles_obj = std::make_unique<Particles>();
    particles_obj->Init(log);
    return particles_obj;
}

void app::run()
{
    log->LogInfo("Main Loop starting");
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        processInput(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}