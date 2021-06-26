//
// Created by st on 6/25/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_APP_H
#define INTERACTIVE_VIRTUAL_CREATURES_APP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "PhysicsWorld.h"
#include "Camera.h"

namespace ivc {

    class App {
        private:
            GLFWwindow* m_window;
            Camera m_camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
            bool m_shouldClose = false;
            bool isInitialized = false;

            Shader* m_shader = nullptr;
            unsigned int m_VAO;

            void processInput();
            PhysicsWorld* m_physicsWorld = nullptr;

            const int c_WIDTH = 800, c_HEIGHT = 600;
            bool m_firstMouseMovement = true;
            double m_lastMousePosX = c_WIDTH / 2, m_lastMousePosY = c_HEIGHT / 2;
        public:
            bool shouldClose();

            int init();
            int render();
            int close();
            Camera* getCamera();
            bool firstMouseMovement();
            void firstMouseMoveDone();
            std::pair<double,double> getLastMousePos();
            void setLastMousePos(double,double);
    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_APP_H
