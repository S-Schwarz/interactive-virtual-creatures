//
// Created by steven on 9/3/21.
//

#include "NeuronVisualizer.h"
#include "../Res/ShapeHandler.h"

ivc::NeuronVisualizer::NeuronVisualizer(GLFWwindow* w, Shader* s) {
    m_window = w;
    m_shader = s;

    glGenVertexArrays(1, &lineVAO);
    glBindVertexArray(lineVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ivc::NeuronVisualizer::updateVisualizer(ivc::PhysicalCreature* c) {
    m_gatePosMap = {};
    m_neuronPosVec = {};
    m_sensorPosVec = {};
    m_contactPosVec = {};
    m_effectorPosVec = {};

    m_creature = c;

    auto neuronVec = m_creature->getActiveNeurons();
    auto sensorVec = m_creature->getActiveJointSensors();
    auto contactVec = m_creature->getActiveContactSensors();
    auto effectorVec = m_creature->getJointEffectors();

    printf("SENSORS: %i\n", sensorVec.size());

    int maxHeight = neuronVec.size();

    if(sensorVec.size() + contactVec.size() > maxHeight)
        maxHeight = sensorVec.size() + contactVec.size();

    if(effectorVec.size() > maxHeight)
        maxHeight = effectorVec.size();

    m_yPos = (1.f/maxHeight);
    m_ySize = m_yPos * 0.5f;
    m_xSize = (1.f/3.f) * 0.6f;

    int i = 0;
    for(auto neuron : neuronVec){
        auto position = glm::vec3(0,(1 - m_yPos) - m_yPos * 2 * i,0);
        m_neuronPosVec[neuron] = position;
        auto inVec = neuron->getInputs();
        for(auto in : inVec){
            m_gatePosMap[in] = {position,glm::vec3(INFINITY)};
        }
        ++i;
    }

    i = 0;
    for(auto sensor : sensorVec){
        auto position = glm::vec3(-1+(1.f/3.f),(1 - m_yPos) - m_yPos * 2 * i,0);
        m_sensorPosVec[sensor.first] = position;
        ++i;
    }

    for(auto contact : contactVec){
        auto position = glm::vec3(-1+(1.f/3.f),(1 - m_yPos) - m_yPos * 2 * i,0);
        m_contactPosVec[contact.first] = position;
        ++i;
    }

    i = 0;
    for(auto effector : effectorVec){
        auto position = glm::vec3(1-(1.f/3.f),(1 - m_yPos) - m_yPos * 2 * i,0);
        m_effectorPosVec[effector] = position;
        auto inVec = effector->getInputs();
        for(auto in : inVec){
            m_gatePosMap[in] = {position,glm::vec3(INFINITY)};
        }
        ++i;
    }

    //create connections
    for(auto const& [id,value] : m_gatePosMap) {
        bool found = false;

        for (auto const&[neuron, pos]: m_neuronPosVec) {
            if (found)
                break;
            if (neuron->getOutputID() == id) {
                m_gatePosMap[id] = {value.first, pos};
                found = true;
            }
        }

        for (auto const&[sensor, pos]: m_sensorPosVec) {
            if (found)
                break;
            for (auto sensor_id: sensor->getOutputIDs()) {
                if (sensor_id == id) {
                    m_gatePosMap[id] = {value.first, pos};
                    found = true;
                    break;
                }
            }
        }

        for (auto const&[contact, pos]: m_contactPosVec) {
            if (found)
                break;
            for (auto contact_id: contact->getOutputIDs()) {
                if (contact_id == id) {
                    m_gatePosMap[id] = {value.first, pos};
                    found = true;
                    break;
                }
            }
        }


    }


}

void ivc::NeuronVisualizer::draw() {
    glfwMakeContextCurrent(m_window);
    glClearColor(0, 0, 0, 255);
    glClear(GL_COLOR_BUFFER_BIT);
    m_shader->use();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glm::vec3 scale;

    m_shader->setVec4("drawColor", glm::vec4(255,255,255,255));
    for(auto const& [gate,pos] : m_gatePosMap){
        glm::mat4 model = glm::mat4(1.0f);
        m_shader->setMat4("model", model);

        float lineVertices[] = {
                pos.first.x, pos.first.y, pos.first.z,
                pos.second.x, pos.second.y, pos.second.z
        };

        glBindVertexArray(lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_LINES, 0, 2);
    }

    ShapeHandler::bindNeuronVAO();

    m_shader->setVec4("drawColor", glm::vec4(0,0,180,255));
    for(auto const& [neuron,pos] : m_neuronPosVec){
        scale = glm::vec3(m_xSize,m_ySize,1);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, scale);

        m_shader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    m_shader->setVec4("drawColor", glm::vec4(0,180,0,255));
    for(auto const& [sensor,pos] : m_sensorPosVec){
        scale = glm::vec3(m_xSize,m_ySize,1);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, scale);

        m_shader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    m_shader->setVec4("drawColor", glm::vec4(180,0,0,255));
    for(auto const& [contact,pos] : m_contactPosVec){
        scale = glm::vec3(m_xSize,m_ySize,1);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, scale);

        m_shader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    m_shader->setVec4("drawColor", glm::vec4(180,0,200,255));
    for(auto const& [effector,pos] : m_effectorPosVec){
        scale = glm::vec3(m_xSize,m_ySize,1);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, scale);

        m_shader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glfwSwapBuffers(m_window);
}
