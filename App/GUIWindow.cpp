//
// Created by steven on 9/14/21.
//

#include <iostream>
#include "GUIWindow.h"

void gui_framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);

    auto winPtr = (ivc::GUIWindow*) glfwGetWindowUserPointer(window);
    winPtr->getScreen()->resize_callback_event(width,height);
}

void gui_mouse_callback(GLFWwindow* window, double xpos, double ypos){
    glfwMakeContextCurrent(window);

    auto winPtr = (ivc::GUIWindow*)glfwGetWindowUserPointer(window);
    winPtr->getScreen()->cursor_pos_callback_event(xpos, ypos);
}

ivc::GUIWindow::GUIWindow(int width, int height) {
    // Create Window
    m_guiWindow = glfwCreateWindow(width, height, "Interactive Virtual Creatures", nullptr, nullptr);
    if (m_guiWindow == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::bad_exception();
    }
    glfwMakeContextCurrent(m_guiWindow);
    glfwSetFramebufferSizeCallback(m_guiWindow, gui_framebuffer_size_callback);
    glfwSetCursorPosCallback(m_guiWindow, gui_mouse_callback);
    glfwSetMouseButtonCallback(m_guiWindow,
                               [](GLFWwindow *window, int button, int action, int modifiers) {
                                   auto winPtr = (ivc::GUIWindow*) glfwGetWindowUserPointer(window);
                                   winPtr->getScreen()->mouse_button_callback_event(button, action, modifiers);
                               }
    );
    glfwSetWindowUserPointer(m_guiWindow, this);

    // GUI -------------

    m_guiScreen = new nanogui::Screen();
    m_guiScreen->initialize(m_guiWindow, true);

    m_fitnessGraph = m_guiScreen->add<nanogui::Graph>("Fitness Graph");
    m_fitnessGraph->set_size(nanogui::Vector2i(400,100));
    m_fitnessGraph->set_stroke_color(nanogui::Color(255,0,0,255));

    m_guiScreen->set_visible(true);
    m_guiScreen->perform_layout();

    //----------------
}

void ivc::GUIWindow::draw() {
    glfwMakeContextCurrent(m_guiWindow);
    glClearColor(0, 0, 0, 255);
    glClear(GL_COLOR_BUFFER_BIT);
    m_guiScreen->draw_contents();
    m_guiScreen->draw_widgets();
    glfwSwapBuffers(m_guiWindow);
}

nanogui::Screen* ivc::GUIWindow::getScreen() {
    return m_guiScreen;
}

void ivc::GUIWindow::updateFitnessGraph(std::vector<EvoData*> dataVec) {
    float bestScore = -INFINITY;
    float worstScore = INFINITY;

    for(auto data : dataVec){
        if(data->getBestScore() > bestScore)
            bestScore = data->getBestScore();
        if(data->getBestScore() < worstScore)
            worstScore = data->getBestScore();
    }

    std::vector<float> valueVec;
    for(auto data : dataVec){
        auto normalized = (data->getBestScore()-worstScore)/(bestScore-worstScore);
        valueVec.push_back(normalized);
    }

    m_fitnessGraph->set_values(valueVec);
}
