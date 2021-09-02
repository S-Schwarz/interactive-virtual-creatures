//
// Created by st on 6/28/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_SHAPEHANDLER_H
#define INTERACTIVE_VIRTUAL_CREATURES_SHAPEHANDLER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ivc {

    class ShapeHandler {

    private:
        ShapeHandler();

        static unsigned int s_boxVAO, s_planeVAO, s_texturedBoxVAO, s_neuronVAO;

    public:
        static int initLiveShapes();
        static int initNeuronShapes();
        static int bindBoxVAO();
        static int bindPlaneVAO();
        static int bindTexturedBoxVAO();
        static int bindNeuronVAO();
    };

    enum Shape{
        BOX,
        PLANE,
        TEXTURED_BOX
    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_SHAPEHANDLER_H
