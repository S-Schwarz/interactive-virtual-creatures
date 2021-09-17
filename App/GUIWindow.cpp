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

    auto layout =
            new nanogui::GridLayout(nanogui::Orientation::Horizontal, 2,
                                    nanogui::Alignment::Middle, 0, 0);
    layout->set_col_alignment(
            { nanogui::Alignment::Maximum, nanogui::Alignment::Fill });

    m_evoConstantsWidget = m_guiScreen->add<nanogui::Widget>();
    m_evoConstantsWidget->set_tooltip("TEST");
    m_evoConstantsWidget->set_layout(layout);

    auto cpgLabel = m_evoConstantsWidget->add<nanogui::Label>("CPG");

    m_cpgBox = m_evoConstantsWidget->add<nanogui::IntBox<unsigned int>>();
    m_cpgBox->set_editable(false);
    m_cpgBox->set_value(CREATURES_PER_GENERATION);
    m_cpgBox->set_min_max_values(1,1000);

    auto spgLabel = m_evoConstantsWidget->add<nanogui::Label>("SPG");

    m_spgBox = m_evoConstantsWidget->add<nanogui::IntBox<unsigned int>>();
    m_spgBox->set_editable(false);
    m_spgBox->set_value(STEPS_PER_GENERATION);
    m_spgBox->set_min_max_values(1000,5000);

    m_fitnessConfigWidget = m_guiScreen->add<nanogui::Widget>();
    m_fitnessConfigWidget->set_layout(layout);

    auto sideWaysLabel = m_fitnessConfigWidget->add<nanogui::Label>("Punish sideways movement");

    m_sidewaysCheckbox = m_fitnessConfigWidget->add<nanogui::CheckBox>("");

    auto sideWaysMultiLabel = m_fitnessConfigWidget->add<nanogui::Label>("Multiplier");

    m_sidewaysBox = m_fitnessConfigWidget->add<nanogui::FloatBox<float>>();
    m_sidewaysBox->set_min_max_values(0.0f, 10.0f);
    m_sidewaysBox->set_value(0.1f);

    m_guiScreen->set_visible(true);
    resize();

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
    m_config->m_stepsPerGeneration = m_spgBox->value();
    m_config->m_useSidewaysMP = m_sidewaysCheckbox->checked();
    m_config->m_sidewaysMultiplier = m_sidewaysBox->value();
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

    auto constantsWidgetSize = nanogui::Vector2i(screenWidth/4, screenHeight/8);
    auto constantsWidgetPos = nanogui::Vector2i(0, 0);
    m_evoConstantsWidget->set_fixed_size(constantsWidgetSize);
    m_evoConstantsWidget->set_position(constantsWidgetPos);

    auto fitnessWidgetSize = nanogui::Vector2i(screenWidth/4, screenHeight/8);
    auto fitnessWidgetPos = nanogui::Vector2i(constantsWidgetPos.x() + constantsWidgetSize.x(), 0);
    m_fitnessConfigWidget->set_fixed_size(fitnessWidgetSize);
    m_fitnessConfigWidget->set_position(fitnessWidgetPos);

    m_guiScreen->perform_layout();

}

void ivc::GUIWindow::setConfig(ivc::EvoConfig* config) {
    m_config = config;
}

nanogui::IntBox<unsigned int> *ivc::GUIWindow::getCPGBox() {
    return m_cpgBox;
}

void ivc::GUIWindow::handleKeyInput(int key, int action) {

    updateIntBox(m_cpgBox,key,action,10);
    updateIntBox(m_spgBox,key,action,10);
    updateFloatBox(m_sidewaysBox,key,action,0.05f);

}

void ivc::GUIWindow::updateIntBox(nanogui::IntBox<unsigned int>* box, int key, int action, int baseIncrement) {
    if(box->focused()){
        if(key == GLFW_KEY_UP && action == GLFW_PRESS){
            if(glfwGetKey(m_guiWindow, GLFW_KEY_LEFT_SHIFT))
                box->set_value(box->value()+baseIncrement*10);
            else
                box->set_value(box->value()+baseIncrement);
        }
        if(key == GLFW_KEY_DOWN && action == GLFW_PRESS){
            if(glfwGetKey(m_guiWindow, GLFW_KEY_LEFT_SHIFT))
                box->set_value(box->value()-baseIncrement*10);
            else
                box->set_value(box->value()-baseIncrement);
        }
    }
}

void ivc::GUIWindow::updateFloatBox(nanogui::FloatBox<float>* box, int key, int action, float baseIncrement) {
    if(box->focused()){
        if(key == GLFW_KEY_UP && action == GLFW_PRESS){
            if(glfwGetKey(m_guiWindow, GLFW_KEY_LEFT_SHIFT))
                box->set_value(box->value()+baseIncrement*10);
            else
                box->set_value(box->value()+baseIncrement);
        }
        if(key == GLFW_KEY_DOWN && action == GLFW_PRESS){
            if(glfwGetKey(m_guiWindow, GLFW_KEY_LEFT_SHIFT))
                box->set_value(box->value()-baseIncrement*10);
            else
                box->set_value(box->value()-baseIncrement);
        }
    }
}