//
// Created by steven on 9/3/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURONVISUALIZER_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURONVISUALIZER_H


#include "../Body/PhysicalCreature.h"
#include "../Res/Shader.h"
#include <GLFW/glfw3.h>

#include <map>

namespace ivc{
    class NeuronVisualizer {
    private:
        GLFWwindow* m_window = nullptr;
        Shader* m_shader = nullptr;

        PhysicalCreature* m_creature = nullptr;

        std::map<unsigned long, std::pair<glm::vec3, glm::vec3>> m_gatePosMap;
        std::map<Neuron*, glm::vec3> m_neuronPosVec;
        std::map<JointSensor*, glm::vec3> m_sensorPosVec;
        std::map<ContactSensor*, glm::vec3> m_contactPosVec;
        std::map<JointEffector*, glm::vec3> m_effectorPosVec;

        float m_yPos = 0;
        float m_ySize = 0;
        float m_xSize = 0;
    public:
        NeuronVisualizer(GLFWwindow*, Shader*);
        void updateVisualizer(PhysicalCreature*);
        void draw();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURONVISUALIZER_H
