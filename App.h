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

namespace ivc {

    class App {
        private:
            GLFWwindow* m_window;
            bool m_shouldClose = false;
            bool isInitialized = false;

            Shader* m_shader = nullptr;
            unsigned int m_VAO;

            void processInput();
            PhysicsWorld* m_physicsWorld = nullptr;
        public:
            bool shouldClose();

            int init();
            int render();
            int close();
    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_APP_H
