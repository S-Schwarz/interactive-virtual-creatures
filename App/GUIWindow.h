//
// Created by steven on 9/14/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_GUIWINDOW_H
#define INTERACTIVE_VIRTUAL_CREATURES_GUIWINDOW_H

#include <GLFW/glfw3.h>
#include <nanogui/nanogui.h>
#include "../Evolution/EvoData.h"

namespace ivc{
    class GUIWindow {
        private:
            GLFWwindow* m_guiWindow = nullptr;
            nanogui::Screen* m_guiScreen = nullptr;
            nanogui::Graph* m_fitnessGraph = nullptr;
        public:
            GUIWindow(int,int);
            void draw();
            nanogui::Screen* getScreen();
            void updateFitnessGraph(std::vector<EvoData*>);
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_GUIWINDOW_H
