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
            nanogui::Graph* m_fitnessGraph = nullptr;
            nanogui::Button* m_updateButton = nullptr;

            nanogui::Widget* m_evoConstantsWidget = nullptr;
            nanogui::IntBox<unsigned int>* m_cpgBox = nullptr;
            nanogui::IntBox<unsigned int>* m_spgBox = nullptr;

            nanogui::Widget* m_fitnessConfigWidget = nullptr;
            nanogui::CheckBox* m_sidewaysCheckbox = nullptr;
            nanogui::FloatBox<float>* m_sidewaysBox = nullptr;

            EvoConfig* m_config = nullptr;

            void update();
            void updateIntBox(nanogui::IntBox<unsigned int>*, int, int, int);
            void updateFloatBox(nanogui::FloatBox<float>*, int, int, float);
        public:
            GUIWindow(int,int);
            void resize();
            void draw();
            nanogui::Screen* getScreen();
            void updateFitnessGraph(std::vector<EvoData*>);
            void setConfig(EvoConfig*);
            nanogui::IntBox<unsigned int>* getCPGBox();
            void handleKeyInput(int,int);
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_GUIWINDOW_H
