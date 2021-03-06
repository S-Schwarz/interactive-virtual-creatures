//
// Created by steven on 9/3/21.
//

#include "NeuronVisualizer.h"
#include "../Res/ShapeHandler.h"



ivc::NeuronVisualizer::NeuronVisualizer(GLFWwindow* w, std::shared_ptr<Shader> s) {
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

    m_textShader = std::make_shared<Shader>("../Res/text.vert", "../Res/text.frag");
}

void ivc::NeuronVisualizer::updateVisualizer(std::shared_ptr<PhysicalCreature> c) {
    m_requestMap = {};
    m_connectionMap = {};
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

    int maxHeight = neuronVec.size();

    if(sensorVec.size() + contactVec.size() > maxHeight)
        maxHeight = sensorVec.size() + contactVec.size();

    if(effectorVec.size() > maxHeight)
        maxHeight = effectorVec.size();

    m_yPos = (1.f/((float)maxHeight+0.5f));
    m_ySize = m_yPos * 0.5f;
    if(m_ySize < MAX_NODE_WIDTH){
        m_xSize = m_ySize;
    }else{
        m_xSize = MAX_NODE_WIDTH;
    }


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
            m_requestMap[in].push_back(position);
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

        auto type = effector->getType();
        int index = 0;
        for(auto in : inVec){
            position += yOffset;
            // only add requests for active ins
            if(index == ((type+1)%3)){
                m_requestMap[in].push_back(position);
            }
            index++;
        }

        ++i;
    }

    //create connections
    for(auto const& [id,value] : m_requestMap) {
        bool found = false;

        for (auto const&[neuron, pos]: m_neuronPosMap) {
            if (found)
                break;
            if (neuron->getOutputID() == id) {
                auto position = pos + glm::vec3(m_xSize,0,0);
                std::vector<std::pair<glm::vec3, glm::vec3>> connVec;
                for(auto p : m_requestMap[id])
                    connVec.emplace_back(p, position);
                m_connectionMap[id] = connVec;
                found = true;
            }
        }

        for (auto const&[sensor, pos]: m_sensorPosMap) {
            if (found)
                break;
            auto position = pos + glm::vec3(m_xSize ,0,0);
            if (sensor->getOutputID() == id) {
                std::vector<std::pair<glm::vec3, glm::vec3>> connVec;
                for(auto p : m_requestMap[id])
                    connVec.emplace_back(p, position);
                m_connectionMap[id] = connVec;
                found = true;
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
                    std::vector<std::pair<glm::vec3, glm::vec3>> connVec;
                    for(auto p : m_requestMap[id])
                        connVec.emplace_back(p, position);
                    m_connectionMap[id] = connVec;
                    found = true;
                    break;
                }
            }
        }


    }

    //draw();

    //printf("TEST\n");
}

void ivc::NeuronVisualizer::draw() {
    glfwMakeContextCurrent(m_window);
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    m_shader->use();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glm::vec3 scale;

    unsigned int nnCounter = 0;

    for(auto const& [gate,connVec] : m_connectionMap) {
        glm::mat4 model = glm::mat4(1.0f);
        m_shader->setMat4("model", model);

        for (auto pos : connVec) {


            float lineVertices[] = {
                    pos.first.x, pos.first.y, pos.first.z,
                    pos.second.x, pos.second.y, pos.second.z
            };

            PxVec3 extraPos1 = PxVec3(-m_xSize * 1.2f, pos.second.y - 1.8f * m_ySize, 0);
            PxVec3 extraPos2 = PxVec3(m_xSize * 1.2f, pos.second.y - 1.8f * m_ySize, 0);

            float bypassVertices[] = {
                    pos.first.x, pos.first.y, pos.first.z,
                    extraPos2.x, extraPos2.y, extraPos2.z,
                    extraPos2.x, extraPos2.y, extraPos2.z,
                    extraPos1.x, extraPos1.y, extraPos1.z,
                    extraPos1.x, extraPos1.y, extraPos1.z,
                    pos.second.x, pos.second.y, pos.second.z
            };

            PxVec3 nnPos1 = PxVec3(pos.second.x + 0.2f * m_xSize, pos.second.y, 0);
            PxVec3 nnPos2 = PxVec3(pos.second.x + 0.2f * m_xSize, pos.second.y + 1.2f * m_ySize, 0);
            PxVec3 nnPos3 = PxVec3(pos.first.x - 0.2f * m_xSize - nnCounter * 0.025f, pos.second.y + 1.2f * m_ySize, 0);
            PxVec3 nnPos4 = PxVec3(pos.first.x - 0.2f * m_xSize - nnCounter * 0.025f, pos.first.y, 0);

            float nnVertices[] = {
                    pos.second.x, pos.second.y, pos.second.z,
                    nnPos1.x, nnPos1.y, nnPos1.z,

                    nnPos1.x, nnPos1.y, nnPos1.z,
                    nnPos2.x, nnPos2.y, nnPos2.z,

                    nnPos2.x, nnPos2.y, nnPos2.z,
                    nnPos3.x, nnPos3.y, nnPos3.z,

                    nnPos3.x, nnPos3.y, nnPos3.z,
                    nnPos4.x, nnPos4.y, nnPos4.z,

                    nnPos4.x, nnPos4.y, nnPos4.z,
                    pos.first.x, pos.first.y, pos.first.z
            };

            bool bypassNeuron = false;
            bool nnConn = false;
            if (pos.second.x == -1 + (1.f / 3.f) + m_xSize && pos.first.x == 1 - (1.f / 3.f) - m_xSize) {
                bypassNeuron = true;
            } else if (pos.second.x == m_xSize && pos.first.x == -m_xSize) {
                nnConn = true;
                nnCounter++;
            }

            //modify line look by value
            auto val = m_gatePtrMap[gate]->getValue();
            if (val >= 0)
                m_shader->setVec4("drawColor", glm::vec4(0, 1.0f, 0, std::max(val, 0.2f)));
            else
                m_shader->setVec4("drawColor", glm::vec4(1.0f, 0, 0, std::max(abs(val), 0.2f)));

            glLineWidth(3);

            glBindVertexArray(m_lineVAO);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

            if (bypassNeuron) {
                glBufferData(GL_ARRAY_BUFFER, sizeof(bypassVertices), bypassVertices, GL_STATIC_DRAW);
            } else if (nnConn) {
                glBufferData(GL_ARRAY_BUFFER, sizeof(nnVertices), nnVertices, GL_STATIC_DRAW);
            } else {
                glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
            }

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(0);

            if (bypassNeuron) {
                glDrawArrays(GL_LINES, 0, 6);
            } else if (nnConn) {
                glDrawArrays(GL_LINES, 0, 10);
            } else {
                glDrawArrays(GL_LINES, 0, 2);
            }
        }
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

void ivc::NeuronVisualizer::drawText(std::shared_ptr<Neuron> neuron, glm::mat4 model) {
    char toDraw = ' ';

    switch(neuron->getType()){
        case EYE:
            toDraw = 'E';
            break;
        case CONSTANT:
            toDraw = 'C';
            break;
        case SINE_OSCI:
            toDraw = 'O';
            break;
        case SAW_OSCI:
            toDraw = 'Z';
            break;
        case SIGN:
            toDraw = 'V';
            break;
        case ABS:
            toDraw = 'A';
            break;
        case SIN:
            toDraw = 'S';
            break;
        case SAW:
            toDraw = 'W';
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
        case SIGMOID:
            toDraw = '1';
            break;
        case TANH:
            toDraw = '2';
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
