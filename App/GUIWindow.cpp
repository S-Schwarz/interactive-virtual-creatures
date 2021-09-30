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

    m_distanceGraph = m_guiScreen->add<nanogui::Graph>("Distance Graph");
    m_distanceGraph->set_stroke_color(nanogui::Color(255,0,0,255));

    m_fitnessGraph = m_guiScreen->add<nanogui::Graph>("Fitness Graph");
    m_fitnessGraph->set_stroke_color(nanogui::Color(255,0,0,255));

    m_noveltyGraph = m_guiScreen->add<nanogui::Graph>("Novelty Graph");
    m_noveltyGraph->set_stroke_color(nanogui::Color(255,0,0,255));

    m_fitnessFunctionLabel = m_guiScreen->add<nanogui::Label>("Fitness function: ");

    m_updateButton = m_guiScreen->add<nanogui::Button>("Update");
    m_updateButton->set_callback([this]{this->update();});
    m_updateButton->set_tooltip("Update the configuration");

    m_startButton = m_guiScreen->add<nanogui::Button>("Start");
    m_startButton->set_callback([this]{this->m_config->m_paused = false;});

    auto topLayout =
            new nanogui::GridLayout(nanogui::Orientation::Horizontal, 3,
                                    nanogui::Alignment::Middle, 0, 0);
    topLayout->set_col_alignment(
            { nanogui::Alignment::Maximum, nanogui::Alignment::Fill });

    m_configurationWidget = m_guiScreen->add<nanogui::Widget>();
    m_configurationWidget->set_layout(topLayout);

    auto layout =
            new nanogui::GridLayout(nanogui::Orientation::Horizontal, 2,
                                    nanogui::Alignment::Middle, 0, 0);
    layout->set_col_alignment(
            { nanogui::Alignment::Maximum, nanogui::Alignment::Fill });

    m_evoConstantsWidget = m_configurationWidget->add<nanogui::Widget>();
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

    auto mutChanceLabel = m_evoConstantsWidget->add<nanogui::Label>("Mutation Chance");
    m_mutChanceBox = m_evoConstantsWidget->add<nanogui::FloatBox<float>>();
    m_mutChanceBox->set_min_max_values(0.0f, 10.0f);
    m_mutChanceBox->set_value(STANDARD_MUTATION_CHANCE);

    auto reflChanceLabel = m_evoConstantsWidget->add<nanogui::Label>("Reflection Chance");
    m_reflChanceBox = m_evoConstantsWidget->add<nanogui::FloatBox<float>>();
    m_reflChanceBox->set_min_max_values(0.0f, 10.0f);
    m_reflChanceBox->set_value(MUTATE_REFLECTION_FLAG_CHANCE);

    m_fitnessConfigWidget = m_configurationWidget->add<nanogui::Widget>();
    m_fitnessConfigWidget->set_layout(layout);

    auto sideWaysLabel = m_fitnessConfigWidget->add<nanogui::Label>("Punish sideways movement");
    m_sidewaysCheckbox = m_fitnessConfigWidget->add<nanogui::CheckBox>("");

    auto sideWaysMultiLabel = m_fitnessConfigWidget->add<nanogui::Label>("Multiplier");
    m_sidewaysBox = m_fitnessConfigWidget->add<nanogui::FloatBox<float>>();
    m_sidewaysBox->set_min_max_values(0.0f, 10.0f);
    m_sidewaysBox->set_value(0.1f);

    auto forceDiversityLabel = m_fitnessConfigWidget->add<nanogui::Label>("Force diversity");
    m_ForceDiversityCheckbox = m_fitnessConfigWidget->add<nanogui::CheckBox>("");

    auto lockMorphLabel = m_fitnessConfigWidget->add<nanogui::Label>("Lock morphology");
    m_lockMorphCheckbox = m_fitnessConfigWidget->add<nanogui::CheckBox>("");

    m_noveltyConfigWidget = m_configurationWidget->add<nanogui::Widget>();
    m_noveltyConfigWidget->set_layout(layout);

    auto noveltyLabel = m_noveltyConfigWidget->add<nanogui::Label>("Use novelty search");
    m_noveltyCheckbox = m_noveltyConfigWidget->add<nanogui::CheckBox>("");

    auto noveltyNeighborsLabel = m_noveltyConfigWidget->add<nanogui::Label>("# neighbors");
    m_neighborsBox = m_noveltyConfigWidget->add<nanogui::IntBox<unsigned int>>();
    m_neighborsBox->set_min_max_values(1, 100);
    m_neighborsBox->set_value(10);

    auto noveltyIntervalLabel = m_noveltyConfigWidget->add<nanogui::Label>("sample interval");
    m_noveltyIntevallBox = m_noveltyConfigWidget->add<nanogui::IntBox<unsigned int>>();
    m_noveltyIntevallBox->set_min_max_values(1, 1000);
    m_noveltyIntevallBox->set_value(100);

    auto noveltyArchiveLabel = m_noveltyConfigWidget->add<nanogui::Label>("Only consider end position");
    m_noveltyArchiveCheckbox = m_noveltyConfigWidget->add<nanogui::CheckBox>("");

    auto xAxisLabel = m_noveltyConfigWidget->add<nanogui::Label>("Consider x-axis");
    m_useXAxisCheckbox = m_noveltyConfigWidget->add<nanogui::CheckBox>("");
    m_useXAxisCheckbox->set_checked(true);

    auto zAxisLabel = m_noveltyConfigWidget->add<nanogui::Label>("Consider z-axis");
    m_useZAxisCheckbox = m_noveltyConfigWidget->add<nanogui::CheckBox>("");
    m_useZAxisCheckbox->set_checked(true);

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

void ivc::GUIWindow::updateGraphs(std::vector<EvoData*> dataVec) {
    float bestLargestDistance = -INFINITY;
    float worstLargestDistance = INFINITY;
    float bestFitnessScore = -INFINITY;
    float worstFitnessScore = INFINITY;
    float bestNoveltyScore = -INFINITY;
    float worstNoveltyScore = INFINITY;
    for(auto data : dataVec){
        if(data->getLargestDistance() > bestLargestDistance)
            bestLargestDistance = data->getLargestDistance();
        if(data->getLargestDistance() < worstLargestDistance)
            worstLargestDistance = data->getLargestDistance();

        if(data->getBestFitnessScore() > bestFitnessScore)
            bestFitnessScore = data->getBestFitnessScore();
        if(data->getBestFitnessScore() < worstFitnessScore)
            worstFitnessScore = data->getBestFitnessScore();

        if(data->getBestNoveltyScore() > bestNoveltyScore)
            bestNoveltyScore = data->getBestNoveltyScore();
        if(data->getBestNoveltyScore() < worstNoveltyScore)
            worstNoveltyScore = data->getBestNoveltyScore();
    }
    std::vector<float> distanceValueVec;
    std::vector<float> fitnessValueVec;
    std::vector<float> noveltyValueVec;
    for(auto data : dataVec){
        auto normalized = Mutator::normalize(data->getLargestDistance(), worstLargestDistance, bestLargestDistance);
        distanceValueVec.push_back(normalized);
        normalized = Mutator::normalize(data->getBestFitnessScore(), worstFitnessScore, bestFitnessScore);
        fitnessValueVec.push_back(normalized);
        normalized = Mutator::normalize(data->getBestNoveltyScore(), worstNoveltyScore, bestNoveltyScore);
        noveltyValueVec.push_back(normalized);
    }
    m_distanceGraph->set_values(distanceValueVec);
    m_fitnessGraph->set_values(fitnessValueVec);
    m_noveltyGraph->set_values(noveltyValueVec);
}

void ivc::GUIWindow::update() {
    m_config->m_creaturesPerGeneration = m_cpgBox->value();
    m_config->m_stepsPerGeneration = m_spgBox->value();
    m_config->m_mutChance = m_mutChanceBox->value();
    m_config->m_reflChance = m_reflChanceBox->value();
    m_config->m_useSidewaysMP = m_sidewaysCheckbox->checked();
    m_config->m_sidewaysMultiplier = m_sidewaysBox->value();
    m_config->m_forceDiversity = m_ForceDiversityCheckbox->checked();
    m_config->m_lockMorph = m_lockMorphCheckbox->checked();
    m_config->m_useNoveltySearch = m_noveltyCheckbox->checked();
    m_config->m_noveltyNearestNeighbors = m_neighborsBox->value();
    m_config->m_noveltyInterval = m_noveltyIntevallBox->value();
    m_config->m_onlyUseEndPos = m_noveltyArchiveCheckbox->checked();
    m_config->m_useXAxis = m_useXAxisCheckbox->checked();
    m_config->m_useZAxis = m_useZAxisCheckbox->checked();

    if(!m_config->m_useNoveltySearch){
        std::string fitnessFunc = "Fitness function: ";
        fitnessFunc += "delta_z";
        if(m_sidewaysCheckbox->checked()){
            fitnessFunc += " - delta_x * ";
            fitnessFunc += std::to_string(m_sidewaysBox->value());
            while(fitnessFunc.back() == '0')
                fitnessFunc.pop_back();
        }

        m_fitnessFunctionLabel->set_caption(fitnessFunc);
    }else{
        m_fitnessFunctionLabel->set_caption("using novelty search");
    }


}

void ivc::GUIWindow::resize() {

    auto screenWidth = m_guiScreen->width();
    auto screenHeight = m_guiScreen->height();

    auto distanceGraphSize = nanogui::Vector2i(screenWidth / 3,screenHeight/4);
    auto distanceGraphPos = nanogui::Vector2i(0,screenHeight - screenHeight/4);
    m_distanceGraph->set_fixed_size(distanceGraphSize);
    m_distanceGraph->set_position(distanceGraphPos);

    auto fitnessGraphSize = nanogui::Vector2i(screenWidth / 3,screenHeight/4);
    auto fitnessGraphPos = nanogui::Vector2i(distanceGraphPos.x() + distanceGraphSize.x(), screenHeight - screenHeight/4);
    m_fitnessGraph->set_fixed_size(fitnessGraphSize);
    m_fitnessGraph->set_position(fitnessGraphPos);

    auto noveltyGraphSize = nanogui::Vector2i(screenWidth / 3,screenHeight/4);
    auto noveltyGraphPos = nanogui::Vector2i(fitnessGraphPos.x() + fitnessGraphSize.x(),screenHeight - screenHeight/4);
    m_noveltyGraph->set_fixed_size(noveltyGraphSize);
    m_noveltyGraph->set_position(noveltyGraphPos);

    auto funcLabelSize = nanogui::Vector2i(screenWidth/2, screenHeight/16);
    auto funcLabelPos = nanogui::Vector2i(screenWidth/2 - funcLabelSize.x()/2, fitnessGraphPos.y() - (int)(funcLabelSize.y() * 1.5));
    m_fitnessFunctionLabel->set_fixed_size(funcLabelSize);
    m_fitnessFunctionLabel->set_position(funcLabelPos);
    m_fitnessFunctionLabel->set_font_size(funcLabelSize.y() / 2);

    auto updateButtonSize = nanogui::Vector2i(screenWidth/4, screenHeight/16);
    auto updateButtonPos = nanogui::Vector2i(screenWidth/4, funcLabelPos.y() - (int)(updateButtonSize.y() * 1.5));
    m_updateButton->set_fixed_size(updateButtonSize);
    m_updateButton->set_position(updateButtonPos);

    auto startButtonSize = nanogui::Vector2i(screenWidth/4, screenHeight/16);
    auto startButtonPos = nanogui::Vector2i(updateButtonPos.x() + updateButtonSize.x(), funcLabelPos.y() - (int)(startButtonSize.y() * 1.5));
    m_startButton->set_fixed_size(startButtonSize);
    m_startButton->set_position(startButtonPos);

    auto configurationWidgetSize = nanogui::Vector2i(screenWidth, screenHeight/2);
    auto configurationWidgetPos = nanogui::Vector2i(0, 0);
    m_configurationWidget->set_fixed_size(configurationWidgetSize);
    m_configurationWidget->set_position(configurationWidgetPos);

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
    updateFloatBox(m_mutChanceBox,key,action,0.05f);
    updateFloatBox(m_reflChanceBox,key,action,0.05f);
    updateFloatBox(m_sidewaysBox,key,action,0.05f);
    updateIntBox(m_neighborsBox,key,action,1);
    updateIntBox(m_noveltyIntevallBox,key,action,10);

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