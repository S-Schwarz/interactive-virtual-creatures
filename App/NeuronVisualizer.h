//
// Created by steven on 9/3/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURONVISUALIZER_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURONVISUALIZER_H


#include "../Body/PhysicalCreature.h"
#include "../Res/Shader.h"
#include <GLFW/glfw3.h>

#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace ivc{
    struct Character {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };

    class NeuronVisualizer {
    private:
        std::map<char, Character> Characters;

        GLFWwindow* m_window = nullptr;
        Shader* m_shader = nullptr;
        Shader* m_textShader = nullptr;
        unsigned int text_VAO, text_VBO;

        PhysicalCreature* m_creature = nullptr;

        std::map<unsigned long, std::pair<glm::vec3, glm::vec3>> m_gatePosMap;
        std::map<unsigned long, Gate*> m_gatePtrMap;
        std::map<Neuron*, glm::vec3> m_neuronPosMap;
        std::map<JointSensor*, glm::vec3> m_sensorPosMap;
        std::map<ContactSensor*, glm::vec3> m_contactPosMap;
        std::map<JointEffector*, glm::vec3> m_effectorPosMap;

        unsigned int m_lineVAO;
        unsigned int m_VBO;

        float m_yPos = 0;
        float m_ySize = 0;
        float m_xSize = 0;

        void drawText(Neuron*,glm::mat4);
    public:
        NeuronVisualizer(GLFWwindow*, Shader*);
        void updateVisualizer(PhysicalCreature*);
        void draw();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURONVISUALIZER_H
