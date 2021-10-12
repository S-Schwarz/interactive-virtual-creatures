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
        std::shared_ptr<Shader> m_shader = nullptr;
        std::shared_ptr<Shader> m_textShader = nullptr;
        unsigned int text_VAO, text_VBO;

        std::shared_ptr<PhysicalCreature> m_creature = nullptr;

        //std::map<unsigned long, std::pair<glm::vec3, glm::vec3>> m_gatePosMap;
        std::map<unsigned long, std::vector<glm::vec3>> m_requestMap;
        std::map<unsigned long, std::vector<std::pair<glm::vec3, glm::vec3>>> m_connectionMap;
        std::map<unsigned long, std::shared_ptr<Gate>> m_gatePtrMap;
        std::map<std::shared_ptr<Neuron>, glm::vec3> m_neuronPosMap;
        std::map<std::shared_ptr<JointSensor>, glm::vec3> m_sensorPosMap;
        std::map<std::shared_ptr<ContactSensor>, glm::vec3> m_contactPosMap;
        std::map<std::shared_ptr<JointEffector>, glm::vec3> m_effectorPosMap;

        unsigned int m_lineVAO;
        unsigned int m_VBO;

        float m_yPos = 0;
        float m_ySize = 0;
        float m_xSize = 0;

        void drawText(std::shared_ptr<Neuron>,glm::mat4);
    public:
        NeuronVisualizer(GLFWwindow*, std::shared_ptr<Shader>);
        void updateVisualizer(std::shared_ptr<PhysicalCreature>);
        void draw();
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURONVISUALIZER_H
