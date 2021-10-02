//
// Created by steven on 9/14/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_GUIWINDOW_H
#define INTERACTIVE_VIRTUAL_CREATURES_GUIWINDOW_H

#include <GLFW/glfw3.h>
#include <nanogui/nanogui.h>
#include "../Evolution/EvoData.h"
#include "../Evolution/EvoConfig.h"

namespace ivc{
    class GUIWindow {
        private:
            GLFWwindow* m_guiWindow = nullptr;
            nanogui::Screen* m_guiScreen = nullptr;

            nanogui::Graph* m_distanceGraph = nullptr;
            nanogui::Graph* m_fitnessGraph = nullptr;
            nanogui::Graph* m_noveltyGraph = nullptr;

            nanogui::Button* m_updateButton = nullptr;
            nanogui::Button* m_startButton = nullptr;

            nanogui::Widget* m_configurationWidget = nullptr;

            nanogui::Widget* m_evoConstantsWidget = nullptr;
            nanogui::IntBox<unsigned int>* m_cpgBox = nullptr;
            nanogui::IntBox<unsigned int>* m_spgBox = nullptr;
            nanogui::FloatBox<float>* m_mutChanceBox = nullptr;
            nanogui::FloatBox<float>* m_reflChanceBox = nullptr;
            nanogui::IntBox<unsigned int>* m_displayNumBox = nullptr;

            nanogui::Widget* m_fitnessConfigWidget = nullptr;
            nanogui::CheckBox* m_sidewaysCheckbox = nullptr;
            nanogui::FloatBox<float>* m_sidewaysBox = nullptr;
            nanogui::CheckBox* m_ForceDiversityCheckbox = nullptr;
            nanogui::CheckBox* m_lockMorphCheckbox = nullptr;

            nanogui::Widget* m_noveltyConfigWidget = nullptr;
            nanogui::CheckBox* m_noveltyCheckbox = nullptr;
            nanogui::CheckBox* m_noveltyArchiveCheckbox = nullptr;
            nanogui::IntBox<unsigned int>* m_neighborsBox = nullptr;
            nanogui::IntBox<unsigned int>* m_noveltyIntevallBox = nullptr;
            nanogui::CheckBox* m_useXAxisCheckbox = nullptr;
            nanogui::CheckBox* m_useZAxisCheckbox = nullptr;

            nanogui::Label* m_fitnessFunctionLabel = nullptr;

            EvoConfig* m_config = nullptr;

            void update();
            void updateIntBox(nanogui::IntBox<unsigned int>*, int, int, int);
            void updateFloatBox(nanogui::FloatBox<float>*, int, int, float);
        public:
            GUIWindow(int,int);
            void resize();
            void draw();
            nanogui::Screen* getScreen();
            void updateGraphs(std::vector<std::shared_ptr<EvoData>>);
            void setConfig(EvoConfig*);
            nanogui::IntBox<unsigned int>* getCPGBox();
            void handleKeyInput(int,int);
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_GUIWINDOW_H
