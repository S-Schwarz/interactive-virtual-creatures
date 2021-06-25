//
// Created by st on 6/25/21.
//

#include "App.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void ivc::App::processInput()
{
    if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(m_window, true);
        m_shouldClose = true;
    }

}

bool ivc::App::shouldClose() {
    return m_shouldClose;
}

int ivc::App::init(){

    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create Window
    m_window = glfwCreateWindow(800, 600, "Interactive Virtual Creatures", NULL, NULL);
    if (m_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    isInitialized = true;

    return 0;
}

int ivc::App::render() {

    if(!isInitialized || glfwWindowShouldClose(m_window) || m_shouldClose)
        return -1;

    processInput();

    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(m_window);
    glfwPollEvents();

    return 0;

}

int ivc::App::close() {

    if(!isInitialized)
        return -1;

    glfwTerminate();

    return 0;

}