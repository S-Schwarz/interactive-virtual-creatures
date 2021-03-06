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

void gui_character_callback(GLFWwindow* window, unsigned int codepoint)
{
    glfwMakeContextCurrent(window);

    auto winPtr = (ivc::GUIWindow*)glfwGetWindowUserPointer(window);
    winPtr->handleCharInput(codepoint);

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
    glfwSetCharCallback(m_guiWindow, gui_character_callback);

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

    auto buttonLayout =
            new nanogui::GridLayout(nanogui::Orientation::Horizontal, 5,
                                    nanogui::Alignment::Middle, 0, 0);

    m_buttonWidget = m_guiScreen->add<nanogui::Widget>();
    m_buttonWidget->set_layout(buttonLayout);

    m_updateButton = m_buttonWidget->add<nanogui::Button>("Update");
    m_updateButton->set_callback([this]{this->update();});

    m_startButton = m_buttonWidget->add<nanogui::Button>("Start");
    m_startButton->set_callback([this]{
        if(this->m_config->m_running){
            this->m_config->m_shouldEnd = true;
            this->m_startButton->set_caption("Start");
        }else{
            this->m_config->m_shouldStart = true;
            this->m_startButton->set_caption("End");
        }
    });

    m_saveButton = m_buttonWidget->add<nanogui::Button>("Save");
    m_saveButton->set_callback([this]{
        m_config->m_shouldSave = true;
        m_config->m_shouldEnd = true;
        this->m_startButton->set_caption("Start");
        m_config->m_fileName = m_fileNameBox->value();
    });

    m_loadButton = m_buttonWidget->add<nanogui::Button>("Load");
    m_loadButton->set_callback([this]{
        m_config->m_shouldLoad = true;
        m_config->m_shouldStart = true;
        this->m_startButton->set_caption("End");
        m_config->m_fileName = m_fileNameBox->value();
    });

    m_fileNameBox = m_buttonWidget->add<nanogui::TextBox>("");
    m_fileNameBox->set_editable(false);
    m_fileNameBox->set_fixed_size(nanogui::Vector2i(300, 50));

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

    auto displayLabel = m_evoConstantsWidget->add<nanogui::Label>("# displayed");
    m_displayNumBox = m_evoConstantsWidget->add<nanogui::IntBox<unsigned int>>();
    m_displayNumBox->set_editable(false);
    m_displayNumBox->set_value(1);
    m_displayNumBox->set_min_max_values(1,100);

    auto s = m_evoConstantsWidget->add<nanogui::Label>("Creature offset");
    m_creatureOffsetBox = m_evoConstantsWidget->add<nanogui::IntBox<unsigned int>>();
    m_creatureOffsetBox->set_value(30);

    auto se = m_evoConstantsWidget->add<nanogui::Label>("Use user selection");
    m_userSelectionBox = m_evoConstantsWidget->add<nanogui::CheckBox>("");

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

    auto generalLabel = m_fitnessConfigWidget->add<nanogui::Label>("Only general neurons");
    m_useGeneralNeurons = m_fitnessConfigWidget->add<nanogui::CheckBox>("");

    auto osciLabel = m_fitnessConfigWidget->add<nanogui::Label>("No oscillators");
    m_noOscis = m_fitnessConfigWidget->add<nanogui::CheckBox>("");

    auto targetLabel = m_fitnessConfigWidget->add<nanogui::Label>("Use targets");
    m_useTargetBox = m_fitnessConfigWidget->add<nanogui::CheckBox>("");

    auto rewardLabel = m_fitnessConfigWidget->add<nanogui::Label>("Use rewards");
    m_useRewardsBox = m_fitnessConfigWidget->add<nanogui::CheckBox>("");

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

    auto noveltyWidthLabel = m_noveltyConfigWidget->add<nanogui::Label>("search width");
    m_noveltyWidthBox = m_noveltyConfigWidget->add<nanogui::IntBox<unsigned int>>();
    m_noveltyWidthBox->set_min_max_values(1, 1000);
    m_noveltyWidthBox->set_value(20);

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

void ivc::GUIWindow::updateGraphs(std::vector<std::shared_ptr<EvoData>> dataVec) {
    float bestLargestDistance = -INFINITY;
    float worstLargestDistance = INFINITY;
    float bestFitnessScore = -INFINITY;
    float worstFitnessScore = INFINITY;
    float bestNoveltyScore = -INFINITY;
    float worstNoveltyScore = INFINITY;
    for(auto data : dataVec){
        if(data->m_largestDistanceTravelled > bestLargestDistance)
            bestLargestDistance = data->m_largestDistanceTravelled;
        if(data->m_largestDistanceTravelled < worstLargestDistance)
            worstLargestDistance = data->m_largestDistanceTravelled;

        if(data->m_bestFitnessScore > bestFitnessScore)
            bestFitnessScore = data->m_bestFitnessScore;
        if(data->m_bestFitnessScore < worstFitnessScore)
            worstFitnessScore = data->m_bestFitnessScore;

        if(data->m_bestNoveltyScore > bestNoveltyScore)
            bestNoveltyScore = data->m_bestNoveltyScore;
        if(data->m_bestNoveltyScore < worstNoveltyScore)
            worstNoveltyScore = data->m_bestNoveltyScore;
    }
    std::vector<float> distanceValueVec;
    std::vector<float> fitnessValueVec;
    std::vector<float> noveltyValueVec;
    for(auto data : dataVec){
        auto normalized = Mutator::normalize(data->m_largestDistanceTravelled, worstLargestDistance, bestLargestDistance);
        distanceValueVec.push_back(normalized);
        normalized = Mutator::normalize(data->m_bestFitnessScore, worstFitnessScore, bestFitnessScore);
        fitnessValueVec.push_back(normalized);
        normalized = Mutator::normalize(data->m_bestNoveltyScore, worstNoveltyScore, bestNoveltyScore);
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
    m_config->m_numberDisplayedCreatures = m_displayNumBox->value();
    m_config->m_useSidewaysMP = m_sidewaysCheckbox->checked();
    m_config->m_sidewaysMultiplier = m_sidewaysBox->value();
    m_config->m_forceDiversity = m_ForceDiversityCheckbox->checked();
    m_config->m_lockMorph = m_lockMorphCheckbox->checked();
    m_config->m_useNoveltySearch = m_noveltyCheckbox->checked();
    m_config->m_noveltyNearestNeighbors = m_neighborsBox->value();
    m_config->m_noveltyInterval = m_noveltyIntevallBox->value();
    m_config->m_noveltyWidth = m_noveltyWidthBox->value();
    m_config->m_useGeneralNeurons = m_useGeneralNeurons->checked();
    m_config->m_forbidOscis = m_noOscis->checked();
    m_config->m_useTarget = m_useTargetBox->checked();
    m_config->m_useRewards = m_useTargetBox->checked();
    m_config->m_userSelection = m_userSelectionBox->checked();
    m_config->m_creatureOffset = m_creatureOffsetBox->value();

    if(m_config->m_useTarget && m_config->m_useRewards){
        m_config->m_useRewards = false;
        m_useRewardsBox->set_checked(false);
    }

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

    m_config->m_clearBestVec = true;
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

    auto configurationWidgetSize = nanogui::Vector2i(screenWidth, screenHeight/2);
    auto configurationWidgetPos = nanogui::Vector2i(0, 0);
    m_configurationWidget->set_fixed_size(configurationWidgetSize);
    m_configurationWidget->set_position(configurationWidgetPos);

    auto buttonWidgetSize = nanogui::Vector2i(screenWidth, screenHeight/4);
    auto buttonWidgetPos = nanogui::Vector2i(0, (int)(configurationWidgetSize.y() * 1));
    m_buttonWidget->set_fixed_size(buttonWidgetSize);
    m_buttonWidget->set_position(buttonWidgetPos);

    m_guiScreen->perform_layout();

}

void ivc::GUIWindow::setConfig(std::shared_ptr<EvoConfig> config) {
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
    updateIntBox(m_displayNumBox,key,action,1);
    updateFloatBox(m_sidewaysBox,key,action,0.05f);
    updateIntBox(m_neighborsBox,key,action,1);
    updateIntBox(m_noveltyIntevallBox,key,action,10);
    updateIntBox(m_noveltyWidthBox,key,action,1);
    updateIntBox(m_creatureOffsetBox,key,action,1);

    if(m_fileNameBox->focused()){
        if(key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS){
            auto val = m_fileNameBox->value();
            if(val.size() > 0){
                val.pop_back();
                m_fileNameBox->set_value(val);
            }
        }
    }

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

ivc::GUIWindow::~GUIWindow() {

    delete m_guiScreen;

}

void ivc::GUIWindow::handleCharInput(unsigned int codepoint) {

    if(m_fileNameBox->focused()){
        m_fileNameBox->set_value(m_fileNameBox->value() + (char)codepoint);
    }

}
