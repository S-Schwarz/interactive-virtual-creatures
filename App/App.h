//
// Created by st on 6/25/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_APP_H
#define INTERACTIVE_VIRTUAL_CREATURES_APP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "../Res/Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Physics/LiveEnvironment.h"
#include "Camera.h"
#include "../Res/ShapeHandler.h"

#include "../Constants.h"
#include "../Evolution/Evolver.h"
#include "NeuronVisualizer.h"
#include "GUIWindow.h"

#include <nanogui/nanogui.h>
#include <memory>

namespace ivc {

    class App {
        private:
            GLFWwindow* m_liveWindow = nullptr;
            Camera m_camera = Camera(glm::vec3(0.0f, 10.0f, 10.0f));
            bool m_shouldClose = false;
            bool m_isInitialized = false;
            bool m_physicsPaused = true;
            bool m_cursorDisabled = false;

            std::shared_ptr<Shader> m_liveShader = nullptr;
            std::shared_ptr<Shader> m_neuronShader = nullptr;

            std::shared_ptr<NeuronVisualizer> m_neuronVisualizer = nullptr;

            GLFWwindow* m_neuronWindow = nullptr;

            std::shared_ptr<GUIWindow> m_guiWindow = nullptr;

            std::shared_ptr<LiveEnvironment> m_liveEnvironment = nullptr;
            std::shared_ptr<Evolver> m_evolver = nullptr;
            std::shared_ptr<EvoConfig> m_evoConfig = nullptr;
            std::thread* m_evolutionThread = nullptr;

            const int c_WIDTH = 800, c_HEIGHT = 600;
            int m_windowWidth = c_WIDTH, m_windowHeight = c_HEIGHT;
            bool m_firstMouseMovement = true;
            double m_lastMousePosX = c_WIDTH / 2, m_lastMousePosY = c_HEIGHT / 2;

            float m_deltaTime = 0.0f;
            float m_lastTime= 0.0f;
            float m_accumulator  = 0.0f;

            unsigned int m_lastGenNum = 0;

            glm::mat4 m_projectionMatrix;

            unsigned int m_blockTexture;

            bool liveEnvInitialized = false;
            std::shared_ptr<PhysicsBase> m_physicsBase = nullptr;

            glm::vec3 m_newObjectPos = glm::vec3(0,0.5,0);
            glm::vec3 m_newObjectScale = glm::vec3(1,1,1);

            void processInput();
            int drawShape(Shape shape, glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec4 color, bool wireframe);
            void initLiveWindow();
            void initShadersAndTextures();
            void initGUIWindow();
            void initNeuronWindow();
            void drawLiveWindow();
            void drawPath(std::vector<PxVec3>);
        public:
            bool shouldClose();

            int init();
            int update();
            int close();
            Camera* getCamera();
            bool firstMouseMovement();
            void firstMouseMoveDone();
            std::pair<double,double> getLastMousePos();
            void setLastMousePos(double,double);
            void setWindowSize(int w, int h);
            GLFWwindow* getLiveWindow();
            void addToScale(glm::vec3);
    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_APP_H
