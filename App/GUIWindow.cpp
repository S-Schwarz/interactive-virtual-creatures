//
// Created by steven on 9/14/21.
//

#include <iostream>
#include "GUIWindow.h"

void gui_framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);

    auto winPtr = (ivc::GUIWindow*) glfwGetWindowUserPointer(window);
    auto screen = winPtr->getScreen();
    if(screen){
        screen->resize_callback_event(width,height);
        winPtr->resize();
    }

}

void gui_mouse_callback(GLFWwindow* window, double xpos, double ypos){
    glfwMakeContextCurrent(window);

    auto winPtr = (ivc::GUIWindow*)glfwGetWindowUserPointer(window);
    winPtr->getScreen()->cursor_pos_callback_event(xpos, ypos);
}

void gui_key_event_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    glfwMakeContextCurrent(window);

    auto winPtr = (ivc::GUIWindow*)glfwGetWindowUserPointer(window);
    winPtr->handleKeyInput(key,action);

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
    glfwSetKeyCallback(m_guiWindow, gui_key_event_callback);
    glfwSetWindowUserPointer(m_guiWindow, this);

    // GUI -------------

    m_guiScreen = new nanogui::Screen();
    m_guiScreen->initialize(m_guiWindow, true);

    m_fitnessGraph = m_guiScreen->add<nanogui::Graph>("Fitness Graph");
    m_fitnessGraph->set_stroke_color(nanogui::Color(255,0,0,255));

    m_updateButton = m_guiScreen->add<nanogui::Button>("Update");
    m_updateButton->set_callback([this]{this->update();});
    m_updateButton->set_tooltip("Update the configuration");

    m_cpgBox = m_guiScreen->add<nanogui::IntBox<unsigned int>>();
    m_cpgBox->set_editable(false);
    m_cpgBox->set_value(CREATURES_PER_GENERATION);
    m_cpgBox->set_min_max_values(1,1000);

    m_spgBox = m_guiScreen->add<nanogui::IntBox<unsigned int>>();
    m_spgBox->set_editable(false);
    m_spgBox->set_value(STEPS_PER_GENERATION);
    m_spgBox->set_min_max_values(1000,5000);

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

void ivc::GUIWindow::update() {
    m_config->m_creaturesPerGeneration = m_cpgBox->value();
}

void ivc::GUIWindow::resize() {

    auto screenWidth = m_guiScreen->width();
    auto screenHeight = m_guiScreen->height();

    auto graphSize = nanogui::Vector2i(screenWidth,screenHeight/4);
    auto graphPos = nanogui::Vector2i(0,screenHeight - screenHeight/4);
    m_fitnessGraph->set_fixed_size(graphSize);
    m_fitnessGraph->set_position(graphPos);

    auto updateButtonSize = nanogui::Vector2i(screenWidth/2, screenHeight/16);
    auto updateButtonPos = nanogui::Vector2i(screenWidth/2 - updateButtonSize.x()/2, graphPos.y() - (int)(updateButtonSize.y() * 1.5));
    m_updateButton->set_fixed_size(updateButtonSize);
    m_updateButton->set_position(updateButtonPos);

    auto cpgBoxSize = nanogui::Vector2i(screenWidth/2, screenHeight/16);
    auto cpgBoxPos = nanogui::Vector2i(screenWidth/2 - cpgBoxSize.x()/2, updateButtonPos.y() - (int)(cpgBoxSize.y() * 1.5));
    m_cpgBox->set_fixed_size(cpgBoxSize);
    m_cpgBox->set_position(cpgBoxPos);

    auto spgBoxSize = nanogui::Vector2i(screenWidth/2, screenHeight/16);
    auto spgBoxPos = nanogui::Vector2i(screenWidth/2 - spgBoxSize.x()/2, cpgBoxPos.y() - (int)(spgBoxSize.y() * 1.5));
    m_spgBox->set_fixed_size(spgBoxSize);
    m_spgBox->set_position(spgBoxPos);

    m_guiScreen->perform_layout();

}

void ivc::GUIWindow::setConfig(ivc::EvoConfig* config) {
    m_config = config;
}

nanogui::IntBox<unsigned int> *ivc::GUIWindow::getCPGBox() {
    return m_cpgBox;
}

void ivc::GUIWindow::handleKeyInput(int key, int action) {

    if(m_cpgBox->focused()){
        if(key == GLFW_KEY_UP && action == GLFW_PRESS){
            if(glfwGetKey(m_guiWindow, GLFW_KEY_LEFT_SHIFT))
                m_cpgBox->set_value(m_cpgBox->value()+10);
            else
                m_cpgBox->set_value(m_cpgBox->value()+1);
        }
        if(key == GLFW_KEY_DOWN && action == GLFW_PRESS){
            if(glfwGetKey(m_guiWindow, GLFW_KEY_LEFT_SHIFT))
                m_cpgBox->set_value(m_cpgBox->value()-10);
            else
                m_cpgBox->set_value(m_cpgBox->value()-1);
        }
    }

    if(m_spgBox->focused()){
        if(key == GLFW_KEY_UP && action == GLFW_PRESS){
            if(glfwGetKey(m_guiWindow, GLFW_KEY_LEFT_SHIFT))
                m_spgBox->set_value(m_spgBox->value()+10);
            else
                m_spgBox->set_value(m_spgBox->value()+1);
        }
        if(key == GLFW_KEY_DOWN && action == GLFW_PRESS){
            if(glfwGetKey(m_guiWindow, GLFW_KEY_LEFT_SHIFT))
                m_spgBox->set_value(m_spgBox->value()-10);
            else
                m_spgBox->set_value(m_spgBox->value()-1);
        }
    }

}
