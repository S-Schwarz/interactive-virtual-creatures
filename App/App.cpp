//
// Created by st on 6/25/21.
//

#include "App.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Res/stb_image.h"

void backgroundEvolution(std::shared_ptr<ivc::Evolver> evo){
    evo->startContinuousEvolution();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);

    auto appPtr = (ivc::App*) glfwGetWindowUserPointer(window);
    if(window == appPtr->getLiveWindow())
        appPtr->setWindowSize(width,height);
}

void ivc::App::setWindowSize(int w, int h) {
    m_windowWidth = w;
    m_windowHeight = h;
}

void ivc::App::processInput()
{
    if(glfwGetKey(m_liveWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(m_liveWindow, true);
        m_shouldClose = true;
    }

    if (glfwGetKey(m_liveWindow, GLFW_KEY_W) == GLFW_PRESS)
        m_camera.ProcessKeyboard(FORWARD, m_deltaTime);
    if (glfwGetKey(m_liveWindow, GLFW_KEY_S) == GLFW_PRESS)
        m_camera.ProcessKeyboard(BACKWARD, m_deltaTime);
    if (glfwGetKey(m_liveWindow, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.ProcessKeyboard(LEFT, m_deltaTime);
    if (glfwGetKey(m_liveWindow, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.ProcessKeyboard(RIGHT, m_deltaTime);


    if (glfwGetKey(m_liveWindow, GLFW_KEY_R) == GLFW_PRESS){
        m_liveEnvironment->resetCreaturePosition();
        m_evoConfig->m_clearBestVec = true;
    }

    static int oldMoveSelectorLeftState = GLFW_RELEASE;
    int newMoveSelectorLeftState = glfwGetKey(m_liveWindow, GLFW_KEY_LEFT);
    if(oldMoveSelectorLeftState == GLFW_RELEASE && newMoveSelectorLeftState == GLFW_PRESS){
        --m_selector;
        if(m_selector < 0){
            m_selector = m_offsetVec.size()-1;
        }
    }
    oldMoveSelectorLeftState = newMoveSelectorLeftState;

    static int oldMoveSelectorRightState = GLFW_RELEASE;
    int newMoveSelectorRightState = glfwGetKey(m_liveWindow, GLFW_KEY_RIGHT);
    if(newMoveSelectorRightState == GLFW_RELEASE && oldMoveSelectorRightState == GLFW_PRESS){
        ++m_selector;
        if(m_selector >= m_offsetVec.size()){
            m_selector = 0;
        }
    }
    oldMoveSelectorRightState = newMoveSelectorRightState;


    static int oldSelectorModeState = GLFW_RELEASE;
    int newSelectorModeState = glfwGetKey(m_liveWindow, GLFW_KEY_H);
    if(newSelectorModeState == GLFW_RELEASE && oldSelectorModeState == GLFW_PRESS){
        m_inSelectorMode = !m_inSelectorMode;
        if(m_inSelectorMode)
            m_inBuildMode = false;
    }
    oldSelectorModeState = newSelectorModeState;

    static int oldBuildModeState = GLFW_RELEASE;
    int newBuildModeState = glfwGetKey(m_liveWindow, GLFW_KEY_B);
    if(newBuildModeState == GLFW_RELEASE && oldBuildModeState == GLFW_PRESS){
        m_inBuildMode = !m_inBuildMode;
        if(m_inBuildMode)
            m_inSelectorMode = false;
    }
    oldBuildModeState = newBuildModeState;

    static int oldPlaceState = GLFW_RELEASE;
    int newPlaceState = glfwGetKey(m_liveWindow, GLFW_KEY_SPACE);
    if(newPlaceState == GLFW_RELEASE && oldPlaceState == GLFW_PRESS){
        if(m_inBuildMode){
            if(glfwGetKey(m_liveWindow, GLFW_KEY_LEFT_CONTROL)){
                m_evoConfig->m_foodVec.emplace_back(m_newObjectPos, m_newObjectScale);
                m_evoConfig->m_clearBestVec = true;
            }else{
                m_evoConfig->m_objVec.emplace_back(m_newObjectPos, m_newObjectScale);
                m_evoConfig->m_clearBestVec = true;
            }
        }else if(m_inSelectorMode){
            m_evolver->setUserSelection(m_offsetVec[m_selector].first->getRootNode());
        }

    }
    oldPlaceState = newPlaceState;

    static int oldPauseState = GLFW_RELEASE;
    int newPauseState = glfwGetKey(m_liveWindow, GLFW_KEY_P);
    if(newPauseState == GLFW_RELEASE && oldPauseState == GLFW_PRESS)
        m_physicsPaused = !m_physicsPaused;
    oldPauseState = newPauseState;

    static int oldCursorState = GLFW_RELEASE;
    int newCursorState = glfwGetKey(m_liveWindow, GLFW_KEY_C);
    if(newCursorState == GLFW_RELEASE && oldCursorState == GLFW_PRESS){
        m_cursorDisabled = !m_cursorDisabled;
        if(m_cursorDisabled)
            glfwSetInputMode(m_liveWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(m_liveWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    oldCursorState = newCursorState;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){

    glfwMakeContextCurrent(window);

    auto appPtr = (ivc::App*)glfwGetWindowUserPointer(window);

    double lastX = appPtr->getLastMousePos().first;
    double lastY = appPtr->getLastMousePos().second;

    if (appPtr->firstMouseMovement())
    {
        lastX = xpos;
        lastY = ypos;
        appPtr->firstMouseMoveDone();
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    appPtr->setLastMousePos(xpos,ypos);

    appPtr->getCamera()->ProcessMouseMovement(xoffset, yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

    auto appPtr = (ivc::App*)glfwGetWindowUserPointer(window);

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        appPtr->addToScale(glm::vec3(0,0,yoffset * 0.25f));
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        appPtr->addToScale(glm::vec3(yoffset * 0.25f,0,0));
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        appPtr->addToScale(glm::vec3(0,yoffset * 0.25f,0));

}


bool ivc::App::firstMouseMovement() {
    return m_firstMouseMovement;
}

void ivc::App::firstMouseMoveDone() {
    m_firstMouseMovement = false;
}

std::pair<double, double> ivc::App::getLastMousePos() {
    return {m_lastMousePosX,m_lastMousePosY};
}

void ivc::App::setLastMousePos(double x, double y) {
    m_lastMousePosX = x;
    m_lastMousePosY = y;
}

Camera * ivc::App::getCamera() {
    return &m_camera;
}

bool ivc::App::shouldClose() {
    return m_shouldClose;
}

int ivc::App::init(){

    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //--------------

    m_physicsBase = std::make_shared<PhysicsBase>();
    m_physicsBase->init();

    m_evoConfig = std::make_shared<EvoConfig>();

    m_evolver = std::make_shared<Evolver>();
    m_evolver->init(m_physicsBase, m_evoConfig);

    m_liveEnvironment = std::make_shared<LiveEnvironment>();

    initLiveWindow();
    initNeuronWindow();
    initGUIWindow();
    m_guiWindow->setConfig(m_evoConfig);
    initShadersAndTextures();

    m_neuronVisualizer = std::make_shared<NeuronVisualizer>(m_neuronWindow,m_neuronShader);

    m_isInitialized = true;

    return 0;
}

int ivc::App::update() {

    if(!m_isInitialized || m_shouldClose)
        return -1;

    if(glfwWindowShouldClose(m_liveWindow)){
        m_shouldClose = true;
        return -1;
    }

    processInput();

    if(m_evoConfig->m_shouldStart){
        if(m_evoConfig->m_shouldLoad){
            auto fileName = m_evoConfig->m_fileName;
            if (!fileName.empty()) {
                auto loadedNode = loadFrom(fileName);
                if(loadedNode){
                    m_evolver->loadStartNode(loadedNode);
                }
            }
        }
        m_evolutionThread = new std::thread(backgroundEvolution, m_evolver);
        m_evoConfig->m_running = true;
        m_evoConfig->m_shouldStart = false;
        m_evoConfig->m_shouldLoad = false;
    }

    if(m_evoConfig->m_shouldEnd){
        m_evolutionThread->join();

        if(m_evoConfig->m_shouldSave) {
            auto fileName = m_evoConfig->m_fileName;
            if (!fileName.empty()) {
                saveEvoData(m_evolver->getEvoDataVec(), fileName);
                saveTo(m_liveEnvironment->getBestNode().get(), fileName);
            }
        }
        m_evolver->clean();
        m_liveEnvironment->clean();

        m_evoConfig->m_shouldSave = false;
        m_evoConfig->m_running = false;
        m_evoConfig->m_shouldEnd = false;
    }

    float currentTime = glfwGetTime();
    m_deltaTime = currentTime - m_lastTime;
    m_lastTime = currentTime;

    // Physics simulation ---------

    if(liveEnvInitialized || !m_physicsPaused){
        m_accumulator += m_deltaTime;

        while (m_accumulator >= SIMULATION_STEP_SIZE){
            m_liveEnvironment->simulate();
            m_accumulator -= SIMULATION_STEP_SIZE;
        }
    }

    // -----------------------------

    auto frontVec = m_camera.Front;
    auto normalVec = glm::vec3(0,1,0);

    if(glm::dot(frontVec,normalVec) != 0){
        m_newObjectPos = m_camera.Position + m_camera.Front*(glm::dot((glm::vec3(0,0,0) - m_camera.Position),normalVec)) / (glm::dot(frontVec,normalVec));
        m_newObjectPos += glm::vec3(0,m_newObjectScale.y * 0.5,0);
    }

    // insert new creature ------------
    if(liveEnvInitialized){
        auto currentGenNum = m_evolver->getNumberGenerations();
        if(m_evoConfig->m_refreshLiveEnvironment || m_liveEnvironment->getBestNode() == nullptr || (currentGenNum != m_lastGenNum && currentGenNum % 4 == 0)){
            auto newCreatureVector = m_evolver->getCurrentBestVector();
            if(!newCreatureVector.empty()){
                m_evoConfig->m_refreshLiveEnvironment = false;
                printf("INSERTING %i NEW CREATURES INTO LIVE SCENE %p\n", newCreatureVector.size(), newCreatureVector[0].first.get());
                m_liveEnvironment->insertNewCreatures(newCreatureVector);
                m_neuronVisualizer->updateVisualizer(m_liveEnvironment->getBestCreature());
                m_lastGenNum = currentGenNum;
            }
        }
        m_guiWindow->updateGraphs(m_evolver->getEvoDataVec());

        drawLiveWindow();
        m_neuronVisualizer->draw();
    }else{
        auto currentCreatureVec = m_evolver->getCurrentBestVector();
        if(!currentCreatureVec.empty()){
            m_liveEnvironment->init(m_physicsBase, currentCreatureVec, m_evoConfig);
            m_neuronVisualizer->updateVisualizer(m_liveEnvironment->getBestCreature());
            liveEnvInitialized = true;
        }
    }

    //----------------------------
    m_guiWindow->draw();
    //-------------------------

    glfwPollEvents();

    return 0;

}

int ivc::App::drawShape(Shape shape, glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec4 color, bool wireframe) {

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = model * glm::mat4_cast(rotation);
        model = glm::scale(model, scale);

        m_liveShader->setMat4("model", model);
        m_liveShader->setVec4("drawColor", color);

        if(wireframe){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }else{
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        switch(shape){
            case BOX:
                ShapeHandler::bindBoxVAO();
                m_liveShader->setBool("drawTexture", false);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                break;
            case PLANE:
                ShapeHandler::bindPlaneVAO();
                m_liveShader->setBool("drawTexture", false);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                break;
            case TEXTURED_BOX:
                ShapeHandler::bindTexturedBoxVAO();
                m_liveShader->setBool("drawTexture", true);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                break;
            default:
                return -1;
        }

        return 0;

}

int ivc::App::close() {

    if(!m_isInitialized)
        return -1;

    m_evolutionThread->join();
    delete m_evolutionThread;
    glfwTerminate();

    return 0;

}

void ivc::App::initLiveWindow() {

    // Create Window
    m_liveWindow = glfwCreateWindow(c_WIDTH, c_HEIGHT, "Interactive Virtual Creatures", nullptr, nullptr);
    if (m_liveWindow == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::bad_exception();
    }
    glfwMakeContextCurrent(m_liveWindow);
    glfwSetFramebufferSizeCallback(m_liveWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(m_liveWindow, mouse_callback);
    glfwSetScrollCallback(m_liveWindow, scroll_callback);

    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        throw std::bad_exception();
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, c_WIDTH, c_HEIGHT);

    glfwSetWindowUserPointer(m_liveWindow, this);

    ShapeHandler::initLiveShapes();

}

void ivc::App::initShadersAndTextures() {

    glfwMakeContextCurrent(m_liveWindow);
    m_liveShader = std::make_shared<Shader>("../Res/shader.vert", "../Res/shader.frag");

    //load textures
    glGenTextures(1,&m_blockTexture);
    glBindTexture(GL_TEXTURE_2D, m_blockTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("../Res/boxTexture2.png", &width, &height, &nrChannels, 0);
    if(data){
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        printf("Failed to load texture\n");
    }

    stbi_image_free(data);

    glfwMakeContextCurrent(m_neuronWindow);
    m_neuronShader = std::make_shared<Shader>("../Res/neuron.vert", "../Res/neuron.frag");

}

void ivc::App::initGUIWindow() {

    m_guiWindow = std::make_shared<GUIWindow>(c_WIDTH,c_HEIGHT);

}

void ivc::App::drawLiveWindow() {
    glfwMakeContextCurrent(m_liveWindow);
    m_projectionMatrix = glm::perspective(glm::radians(45.0f), (float)m_windowWidth / (float)m_windowHeight, 0.1f, 1000.0f);
    m_liveShader->use();
    glm::mat4 view = m_camera.GetViewMatrix();
    m_liveShader->setMat4("view", view);

    int viewLoc = glGetUniformLocation(m_liveShader->ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    int projectionLoc = glGetUniformLocation(m_liveShader->ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));

    glClearColor(COLOR_CLEAR.r, COLOR_CLEAR.g, COLOR_CLEAR.b, COLOR_CLEAR.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw new object ----------------------

    if(m_inBuildMode)
        drawShape(BOX, m_newObjectPos,glm::quat(),m_newObjectScale, COLOR_RED, true);

    // draw all scenes
    // calc offsets
    int index = 0;
    int sign = 1;
    auto liveSceneVec = m_liveEnvironment->getSceneVec();
    m_offsetVec.clear();
    bool flipped = false;
    for(const auto& [scene,path] : liveSceneVec){
        float offset = sign * index * m_evoConfig->m_creatureOffset;
        m_offsetVec.push_back({scene, {path, offset}});
        index += sign;
        if(!flipped && index > std::floor(liveSceneVec.size()/2)){
            sign = -1;
            index =  std::ceil(liveSceneVec.size()/2);
            if(liveSceneVec.size()%2==0){
                --index;
            }
            flipped = true;
        }
    }
/*
    if(m_selector >= m_offsetVec.size()){
        m_selector = m_offsetVec.size()-1;
    }else if(m_selector < 0){
        m_selector = 0;
    }
*/
    // draw
    int selectionIndex = 0;
    for(const auto& [scene, pair] : m_offsetVec){
        auto offset = pair.second;

        // draw selection box
        if(m_inSelectorMode && selectionIndex == m_selector){
            drawShape(BOX, glm::vec3(offset, 2.5f, 0), glm::quat(), glm::vec3(5.0, 5.0, 5.0), COLOR_CLEAR, true);
        }

        if(m_evoConfig->m_useNoveltySearch){
            // draw novelty search box
            auto archive = m_evolver->getNoveltyArchive();
            auto width = (float)m_evoConfig->m_noveltyWidth;

            for(auto vec : archive){
                auto pos = vec.back();
                drawShape(BOX, glm::vec3(pos.x + offset,pos.y,pos.z),glm::quat(),glm::vec3(0.2,0.2,0.2), COLOR_RED, false);
            }

            drawShape(BOX, glm::vec3(-width / 2 + offset, 0, -250), glm::quat(), glm::vec3(0.1, 0.1, 500.0), COLOR_RED, false);
            drawShape(BOX, glm::vec3(width / 2 + offset, 0, -250), glm::quat(), glm::vec3(0.1, 0.1, 500.0), COLOR_RED, false);

            drawShape(BOX, glm::vec3(offset, 0, 0), glm::quat(), glm::vec3(width, 0.1, 0.1), COLOR_RED, false);

        }else {
            drawShape(BOX, glm::vec3(offset, 0, -240), glm::quat(), glm::vec3(0.1, 0.1, 500.0), COLOR_RED, false);
            drawShape(BOX, glm::vec3(offset, 0, 0), glm::quat(), glm::vec3(20.0, 0.1, 0.1), COLOR_RED, false);
        }

        for( auto pair : m_evoConfig->getObjVec()){
            auto pos = glm::vec3(pair.first.x + offset, pair.first.y, pair.first.z);
            drawShape(TEXTURED_BOX, pos, glm::quat(), pair.second, COLOR_RED, false);
        }
        for( auto pair : m_evoConfig->m_foodVec){
            auto pos = glm::vec3(pair.first.x + offset, pair.first.y, pair.first.z);
            drawShape(BOX, pos, glm::quat(), pair.second, COLOR_CLEAR, false);
        }

        // draw vision
        auto pose = scene->getCreature()->getRootLink()->getGlobalPose();
        auto forwardVec = pose.q.rotate(physx::PxVec3(0, 0, -1));
        auto position = pose.p;
        auto length = scene->getRootNode()->getViewDistance();
        auto glmForward = glm::vec3(forwardVec.x, forwardVec.y, forwardVec.z);
        drawShape(BOX, glm::vec3(position.x + offset, position.y, position.z) + glmForward*(length/2), glm::quat(pose.q.w, pose.q.x, pose.q.y, pose.q.z), glm::vec3(0.1, 0.1, length), COLOR_CLEAR, false);

        for(auto body : scene->getBodyParts()){
            auto transform = body->getGlobalPose();

            glm::vec3 posVec = glm::vec3(transform.p.x + offset, transform.p.y, transform.p.z);

            glm::quat rotQuat = glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);

            //TODO: ???
            glm::vec3 scaleVec;

            auto numShapes = body->getNbShapes();
            PxShape **shapes = new PxShape*[numShapes];
            body->getShapes(shapes, numShapes * sizeof(PxShape));
            for(int i = 0; i < numShapes; ++i){
                if(std::strcmp(shapes[i]->getName(),"draw") == 0){
                    PxBoxGeometry* bodyGeom = new PxBoxGeometry();
                    shapes[i]->getBoxGeometry(*bodyGeom);
                    scaleVec = glm::vec3(bodyGeom->halfExtents.x * 2, bodyGeom->halfExtents.y * 2, bodyGeom->halfExtents.z * 2);
                    delete bodyGeom;
                    break;
                }
            }
            delete[] shapes;

            drawPath(pair.first, offset);

            //m_liveShader->setBool("transparent", bodyPair.second.empty());
            drawShape(TEXTURED_BOX, posVec, rotQuat, scaleVec, COLOR_RED, false);
        }
        ++selectionIndex;
    }

    // PLANE ------------------

    if(m_liveEnvironment->getBestCreature() != nullptr){
        //TODO: ???
        auto rigidStatic = m_liveEnvironment->getFloorPlane();
        PxPlane* plane = (PxPlane*)rigidStatic;
        auto transformA = rigidStatic->getGlobalPose();
        auto transformB = PxTransformFromPlaneEquation(*plane);

        glm::vec3 posVec = glm::vec3(transformA.p.x, transformA.p.y, transformA.p.z);

        glm::quat rotQuat = glm::quat(transformB.q.w, transformB.q.x, transformB.q.y, transformB.q.z);

        drawShape(PLANE, posVec, rotQuat, glm::vec3(1000.0f, 0.0f, 1000.0f), COLOR_PLANE, false);
    }

    glfwSwapBuffers(m_liveWindow);
}

GLFWwindow *ivc::App::getLiveWindow() {
    return m_liveWindow;
}

void ivc::App::initNeuronWindow() {

    // Create Window
    m_neuronWindow = glfwCreateWindow(c_WIDTH, c_HEIGHT, "Interactive Virtual Creatures", nullptr, nullptr);
    if (m_neuronWindow == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::bad_exception();
    }
    glfwMakeContextCurrent(m_neuronWindow);
    glfwSetFramebufferSizeCallback(m_neuronWindow, framebuffer_size_callback);

    glViewport(0, 0, c_WIDTH, c_HEIGHT);
    glfwSetWindowUserPointer(m_neuronWindow, this);

    ShapeHandler::initNeuronShapes();

}

void ivc::App::drawPath(std::vector<PxVec3> posVec, float offset) {

    for(const auto& pos : posVec){
        drawShape(BOX, glm::vec3(pos.x + offset,pos.y,pos.z),glm::quat(),glm::vec3(0.2,0.2,0.2), COLOR_YELLOW, false);
    }

}

void ivc::App::addToScale(glm::vec3 vec) {

    if(m_inBuildMode){
        m_newObjectScale += vec;

        if(m_newObjectScale.x < 0.25f)
            m_newObjectScale = glm::vec3(0.25f, m_newObjectScale.y, m_newObjectScale.z);
        if(m_newObjectScale.y < 0.25f)
            m_newObjectScale = glm::vec3(m_newObjectScale.x, 0.25f, m_newObjectScale.z);
        if(m_newObjectScale.z < 0.25f)
            m_newObjectScale = glm::vec3(m_newObjectScale.x, m_newObjectScale.y, 0.25f);
    }

}

void ivc::App::saveTo(ivc::BaseNode* node, std::string fileName) {
    auto bestDNA = DNA(node);
    std::ofstream outfile(fileName + ".dna");
    boost::archive::text_oarchive archive(outfile);
    archive << bestDNA;
}

std::shared_ptr<ivc::BaseNode> ivc::App::loadFrom(std::string fileName) {
    DNA loadedDNA;
    std::ifstream infile(fileName + ".dna");
    boost::archive::text_iarchive archive(infile);
    archive >> loadedDNA;

    return loadedDNA.toRootNode();
}

void ivc::App::saveEvoData(std::vector<std::shared_ptr<EvoData>> dataVec, std::string fileName) {

    std::ofstream myFile;
    myFile.open (fileName + ".csv");
    myFile << "file;gen;bDis;wDis;aDis;bFit;wFit;aFit;bNov;wNov;aNov;bCom;wCom;aCom;bNeu;wNeu;aNeu;bBra;wBra;aBra;bJoi;wJoi;aJoi;bCon;wCon;aCon;cCom;cNeu;cBra;cJoi;cCon\n";

    for(const auto& data : dataVec){

        myFile << fileName << ";" << data->getGeneration() << ";";
        myFile << data->m_largestDistanceTravelled << ";" << data->m_worstDistanceTravelled << ";" << data->m_averageDistanceTravelled << ";";
        myFile << data->m_bestFitnessScore << ";" << data->m_worstFitnessScore << ";" << data->m_averageFitnessScore << ";";
        myFile << data->m_bestNoveltyScore << ";" << data->m_worstNoveltyScore << ";" << data->m_averageNoveltyScore << ";";

        myFile << data->m_highestComplexity << ";" << data->m_lowestComplexity << ";" << data->m_averageComplexity << ";";
        myFile << data->m_mostActiveNeurons << ";" << data->m_leastActiveNeurons << ";" << data->m_averageActiveNeurons << ";";
        myFile << data->m_mostActiveBrainNeurons << ";" << data->m_leastActiveBrainNeurons << ";" << data->m_averageActiveBrainNeurons << ";";
        myFile << data->m_mostActiveJointSensors << ";" << data->m_leastActiveJointSensors << ";" << data->m_averageActiveJointSensors << ";";
        myFile << data->m_mostActiveContactSensors << ";" << data->m_leastActiveContactSensors << ";" << data->m_averageActiveContactSensors << ";";

        myFile << data->m_complexityOfCurrentBest << ";" << data->m_neuronsOfCurrentBest << ";" << data->m_brainOfCurrentBest << ";" << data->m_jointsOfCurrentBest << ";" << data->m_contactsOfCurrentBest << "\n";

    }

    myFile.close();

}
