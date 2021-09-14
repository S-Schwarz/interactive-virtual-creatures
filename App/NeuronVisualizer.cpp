//
// Created by steven on 9/3/21.
//

#include "NeuronVisualizer.h"
#include "../Res/ShapeHandler.h"



ivc::NeuronVisualizer::NeuronVisualizer(GLFWwindow* w, Shader* s) {
    m_window = w;
    m_shader = s;

    glGenVertexArrays(1, &m_lineVAO);
    glBindVertexArray(m_lineVAO);
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, "../Res/FantasqueSansMono-Regular.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                (unsigned int)face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glGenVertexArrays(1, &text_VAO);
    glGenBuffers(1, &text_VBO);
    glBindVertexArray(text_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, text_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_textShader = new Shader("../Res/text.vert", "../Res/text.frag");
}

void ivc::NeuronVisualizer::updateVisualizer(ivc::PhysicalCreature* c) {
    m_gatePosMap = {};
    m_neuronPosMap = {};
    m_sensorPosMap = {};
    m_contactPosMap = {};
    m_effectorPosMap = {};

    m_gatePtrMap = c->getGateMap();

    m_creature = c;

    auto neuronVec = m_creature->getActiveNeurons();
    auto sensorVec = m_creature->getActiveJointSensors();
    auto contactVec = m_creature->getActiveContactSensors();
    auto effectorVec = m_creature->getJointEffectors();

    printf("SENSORS: %lu\n", sensorVec.size());

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
        m_neuronPosMap[neuron] = position;
        position += glm::vec3(-m_xSize ,0,0);
        auto inVec = neuron->getInputs();
        auto yOffset = glm::vec3(0,(-m_ySize*2)/(inVec.size()+1),0);
        position += glm::vec3(0,m_ySize,0);
        for(auto in : inVec){
            position += yOffset;
            m_gatePosMap[in] = {position,glm::vec3(INFINITY)};
        }
        ++i;
    }

    i = 0;
    for(auto sensor : sensorVec){
        auto position = glm::vec3(-1+(1.f/3.f),(1 - m_yPos) - m_yPos * 2 * i,0);
        m_sensorPosMap[sensor.first] = position;
        ++i;
    }

    for(auto contact : contactVec){
        auto position = glm::vec3(-1+(1.f/3.f),(1 - m_yPos) - m_yPos * 2 * i,0);
        m_contactPosMap[contact.first] = position;
        ++i;
    }

    i = 0;
    for(auto effector : effectorVec){
        auto position = glm::vec3(1-(1.f/3.f),(1 - m_yPos) - m_yPos * 2 * i,0);
        m_effectorPosMap[effector] = position;
        position += glm::vec3(-m_xSize ,0,0);
        auto inVec = effector->getInputs();
        auto yOffset = glm::vec3(0,(-m_ySize*2)/(inVec.size()+1),0);
        position += glm::vec3(0,m_ySize,0);
        for(auto in : inVec){
            position += yOffset;
            m_gatePosMap[in] = {position,glm::vec3(INFINITY)};
        }
        ++i;
    }

    //create connections
    for(auto const& [id,value] : m_gatePosMap) {
        bool found = false;

        for (auto const&[neuron, pos]: m_neuronPosMap) {
            if (found)
                break;
            if (neuron->getOutputID() == id) {
                auto position = pos + glm::vec3(m_xSize,0,0);
                m_gatePosMap[id] = {value.first, position};
                found = true;
            }
        }

        for (auto const&[sensor, pos]: m_sensorPosMap) {
            if (found)
                break;
            auto outVec = sensor->getOutputIDs();
            auto position = pos + glm::vec3(m_xSize ,m_ySize,0);
            auto yOffset = glm::vec3(0,(-m_ySize*2)/(outVec.size()+1),0);
            for (auto sensor_id: outVec) {
                position += yOffset;
                if (sensor_id == id) {
                    m_gatePosMap[id] = {value.first, position};
                    found = true;
                    break;
                }
            }
        }

        for (auto const&[contact, pos]: m_contactPosMap) {
            if (found)
                break;
            auto outVec = contact->getOutputIDs();
            auto position = pos + glm::vec3(m_xSize ,m_ySize,0);
            auto yOffset = glm::vec3(0,(-m_ySize*2)/(outVec.size()+1),0);
            for (auto contact_id: outVec) {
                position += yOffset;
                if (contact_id == id) {
                    m_gatePosMap[id] = {value.first, position};
                    found = true;
                    break;
                }
            }
        }


    }


}

void ivc::NeuronVisualizer::draw() {
    glfwMakeContextCurrent(m_window);
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    m_shader->use();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glm::vec3 scale;

    for(auto const& [gate,pos] : m_gatePosMap){
        glm::mat4 model = glm::mat4(1.0f);
        m_shader->setMat4("model", model);

        float lineVertices[] = {
                pos.first.x, pos.first.y, pos.first.z,
                pos.second.x, pos.second.y, pos.second.z
        };

        //modify line look by value
        auto val = m_gatePtrMap[gate]->getValue();
        if(val >= 0)
            m_shader->setVec4("drawColor", glm::vec4(0,1.0f,0,std::max(val,0.2f)));
        else
            m_shader->setVec4("drawColor", glm::vec4(1.0f,0,0,std::max(abs(val),0.2f)));

        glLineWidth(5);

        glBindVertexArray(m_lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_LINES, 0, 2);
    }

    glLineWidth(1);
    ShapeHandler::bindNeuronVAO();

    m_shader->setVec4("drawColor", glm::vec4(0,0,0.5f,1.0f));
    for(auto const& [neuron,pos] : m_neuronPosMap){
        m_shader->use();
        ShapeHandler::bindNeuronVAO();
        scale = glm::vec3(m_xSize,m_ySize,1);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, scale);

        m_shader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        drawText(neuron,model);
    }

    m_shader->use();
    ShapeHandler::bindNeuronVAO();
    m_shader->setVec4("drawColor", glm::vec4(0,0.5f,0,1.0f));
    for(auto const& [sensor,pos] : m_sensorPosMap){
        scale = glm::vec3(m_xSize,m_ySize,1);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, scale);

        m_shader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    m_shader->setVec4("drawColor", glm::vec4(0.5f,0,0,1.0f));
    for(auto const& [contact,pos] : m_contactPosMap){
        scale = glm::vec3(m_xSize,m_ySize,1);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, scale);

        m_shader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    m_shader->setVec4("drawColor", glm::vec4(0.5f,0,0.5f,1.0f));
    for(auto const& [effector,pos] : m_effectorPosMap){
        scale = glm::vec3(m_xSize,m_ySize,1);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, scale);

        m_shader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glfwSwapBuffers(m_window);
}

void ivc::NeuronVisualizer::drawText(ivc::Neuron* neuron, glm::mat4 model) {
    char toDraw = ' ';

    switch(neuron->getType()){
        case CONSTANT:
            toDraw = 'C';
            break;
        case SINE_OSCI:
            toDraw = 'O';
            break;
        case SIGN:
            toDraw = 'V';
            break;
        case ABS:
            toDraw = 'A';
            break;
        case SIN:
            toDraw = 'S';
        case SINE_OSCI_ONE_IN:
            toDraw = 'O';
            break;
        case DELAY:
            toDraw = 'D';
            break;
        case SUM:
            toDraw = '+';
            break;
        case MAX:
            toDraw = 'X';
            break;
        case MIN:
            toDraw = 'N';
            break;
        case PRODUCT:
            toDraw = '*';
            break;
        case DIVIDE:
            toDraw = '/';
            break;
        case SUM_THRESHOLD:
            toDraw = 'T';
            break;
        case GREATER_THAN:
            toDraw = '>';
            break;
        case IF_THEN_ELSE:
            toDraw = '?';
            break;
        default:
            throw std::invalid_argument("INVALID NEURON TYPE");
    }

    model = glm::scale(model, glm::vec3(0.2f,0.2f,1.0f));

    m_textShader->use();
    glUniform3f(glGetUniformLocation(m_textShader->ID, "textColor"), 1.0f, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(text_VAO);
    m_textShader->setMat4("model", model);

    Character ch = Characters[toDraw];
    float vertices[6][4] = {
            {-1.0f, 1.0f,  0.0f, 0.0f},
            {-1.0f, -1.0f, 0.0f, 1.0f},
            {1.0f,  -1.0f, 1.0f, 1.0f},

            {-1.0f, 1.0f,  0.0f, 0.0f},
            {1.0f,  -1.0f, 1.0f, 1.0f},
            {1.0f,  1.0f,  1.0f, 0.0f}
    };

    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    glBindBuffer(GL_ARRAY_BUFFER, text_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
