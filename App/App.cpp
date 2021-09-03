//
// Created by st on 6/25/21.
//

#include "App.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Res/stb_image.h"

glm::vec4 COLOR_GREEN(0.0f, 0.8f, 0.3f, 1.0f);
glm::vec4 COLOR_RED(0.8f, 0.2f, 0.3f, 1.0f);

glm::vec4 COLOR_PLANE(0.0f, 0.5f, 0.2f, 1.0f);
glm::vec4 COLOR_CLEAR(0.0f, 0.7f, 0.9f, 1.0f);

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
             ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
             type, severity, message );

}

void backgroundEvolution(ivc::Evolver* evo){
    evo->startContinuousEvolution();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);

    auto appPtr = (ivc::App*) glfwGetWindowUserPointer(window);
    if(window == appPtr->getLiveWindow())
        appPtr->setWindowSize(width,height);
    else
        appPtr->getGUIScreen()->resize_callback_event(width,height);
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

    if (glfwGetKey(m_liveWindow, GLFW_KEY_R) == GLFW_PRESS)
        m_liveEnvironment->resetCreaturePosition();

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

    if(appPtr->getLiveWindow() != window){
        appPtr->getGUIScreen()->cursor_pos_callback_event(xpos, ypos);
        return;
    }

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

    PhysicsBase* physicsBase = new PhysicsBase();
    physicsBase->init();

    Evolver* evolver = new Evolver();
    m_evolver = evolver;
    evolver->init(physicsBase);

    m_evolutionThread = new std::thread(backgroundEvolution, evolver);

    PhysicsScene* liveScene = new PhysicsScene();

    RootMorphNode* firstCreature = nullptr;
    while(firstCreature == nullptr){
        firstCreature = evolver->getCurrentBest();
    }

    liveScene->init(physicsBase,firstCreature);

    m_liveEnvironment = new LiveEnvironment();
    m_liveEnvironment->init(liveScene);

    initLiveWindow();
    initNeuronWindow();
    initGUIWindow();
    initShadersAndextures();

    m_neuronVisualizer = new NeuronVisualizer(m_neuronWindow,m_neuronShader);
    m_neuronVisualizer->updateVisualizer(m_liveEnvironment->getCreature());

    isInitialized = true;

    return 0;
}

int ivc::App::update() {

    if(!isInitialized || m_shouldClose)
        return -1;

    if(glfwWindowShouldClose(m_liveWindow) || glfwWindowShouldClose(m_guiWindow)){
        m_shouldClose = true;
        return -1;
    }

    processInput();

    float currentTime = glfwGetTime();
    m_deltaTime = currentTime - m_lastTime;
    m_lastTime = currentTime;

    // Physics simulation ---------

    if(!m_physicsPaused){
        m_accumulator += m_deltaTime;

        while (m_accumulator >= SIMULATION_STEP_SIZE){
            m_liveEnvironment->simulate();
            m_accumulator -= SIMULATION_STEP_SIZE;
        }
    }

    // insert new creature ------------

    auto currentGenNum = m_evolver->getNumberGenerations();
    if(currentGenNum != m_lastGenNum && currentGenNum % 2 == 0){
        auto newCreature = m_evolver->getCurrentBest();
        if(newCreature != nullptr){
            printf("INSERTING NEW CREATURE INTO LIVE SCENE\n");
            m_liveEnvironment->insertNewCreature(newCreature);
            m_neuronVisualizer->updateVisualizer(m_liveEnvironment->getCreature());
            m_lastGenNum = currentGenNum;
        }
    }

    // ---------------------------
    updateFitnessGraph(m_evolver->getEvoDataVec());
    //----------------------------

    drawLiveWindow();
    m_neuronVisualizer->draw();
    drawGUIWindow();
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

    if(!isInitialized)
        return -1;

    delete m_liveShader;
    m_evolver->stopEvolution();
    m_evolutionThread->join();
    m_liveEnvironment->destroy();
    delete m_liveEnvironment;
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

    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        throw std::bad_exception();
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, c_WIDTH, c_HEIGHT);
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );

    glfwSetWindowUserPointer(m_liveWindow, this);

    ShapeHandler::initLiveShapes();

}

void ivc::App::initShadersAndextures() {

    glfwMakeContextCurrent(m_liveWindow);
    m_liveShader = new Shader("../Res/shader.vert", "../Res/shader.frag");

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
    m_neuronShader = new Shader("../Res/neuron.vert", "../Res/neuron.frag");

}

void ivc::App::initGUIWindow() {

    // Create Window
    m_guiWindow = glfwCreateWindow(c_WIDTH, c_HEIGHT, "Interactive Virtual Creatures", nullptr, nullptr);
    if (m_guiWindow == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::bad_exception();
    }
    glfwMakeContextCurrent(m_guiWindow);
    glfwSetFramebufferSizeCallback(m_guiWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(m_guiWindow, mouse_callback);
    glfwSetMouseButtonCallback(m_guiWindow,
                               [](GLFWwindow *window, int button, int action, int modifiers) {
                                   auto appPtr = (ivc::App*) glfwGetWindowUserPointer(window);
                                   appPtr->getGUIScreen()->mouse_button_callback_event(button, action, modifiers);
                               }
    );
    glfwSetWindowUserPointer(m_guiWindow, this);

    // GUI -------------

    guiScreen = new nanogui::Screen();
    guiScreen->initialize(m_guiWindow, true);

    m_fitnessGraph = guiScreen->add<nanogui::Graph>("Fitness Graph");
    m_fitnessGraph->set_size(nanogui::Vector2i(400,100));
    m_fitnessGraph->set_stroke_color(nanogui::Color(255,0,0,255));

    guiScreen->set_visible(true);
    guiScreen->perform_layout();

    //----------------

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

    //-------------------------

    drawShape(BOX, glm::vec3(0,0,-240),glm::quat(),glm::vec3(0.1,0.1,500.0), COLOR_RED, false);
    drawShape(BOX, glm::vec3(0,0,0),glm::quat(),glm::vec3(20.0,0.1,0.1), COLOR_RED, false);

    // PHYSX OBJECTS ----------
    for(auto body : m_liveEnvironment->getBodyParts()){
        auto transform = body->getGlobalPose();

        glm::vec3 posVec = glm::vec3(transform.p.x, transform.p.y, transform.p.z);

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

        if(m_physicsPaused)
            drawShape(TEXTURED_BOX, posVec, rotQuat, scaleVec, COLOR_RED, false);
        else
            drawShape(TEXTURED_BOX, posVec, rotQuat, scaleVec, COLOR_GREEN, false);
    }

    // PLANE ------------------
    //TODO: ???
    auto rigidStatic = m_liveEnvironment->getFloorPlane();
    PxPlane* plane = (PxPlane*)rigidStatic;
    auto transformA = rigidStatic->getGlobalPose();
    auto transformB = PxTransformFromPlaneEquation(*plane);

    glm::vec3 posVec = glm::vec3(transformA.p.x, transformA.p.y, transformA.p.z);

    glm::quat rotQuat = glm::quat(transformB.q.w, transformB.q.x, transformB.q.y, transformB.q.z);

    drawShape(PLANE, posVec, rotQuat, glm::vec3(1000.0f, 0.0f, 1000.0f), COLOR_PLANE, false);

    glfwSwapBuffers(m_liveWindow);
}

void ivc::App::drawGUIWindow() {
    glfwMakeContextCurrent(m_guiWindow);
    glClearColor(0, 0, 0, 255);
    glClear(GL_COLOR_BUFFER_BIT);
    guiScreen->draw_contents();
    guiScreen->draw_widgets();
    glfwSwapBuffers(m_guiWindow);
}

GLFWwindow *ivc::App::getLiveWindow() {
    return m_liveWindow;
}

nanogui::Screen *ivc::App::getGUIScreen() {
    return guiScreen;
}

void ivc::App::updateFitnessGraph(std::vector<EvoData*> dataVec) {

    float bestScore = -INFINITY;
    float worstScore = INFINITY;

    for(auto data : dataVec){
        if(data->getBestScore() > bestScore)
            bestScore = data->getBestScore();
        if(data->getBestScore() < worstScore)
            worstScore = data->getBestScore();
    }

    std::vector<float> valueVec;
    for(auto data : dataVec){
        auto normalized = (data->getBestScore()-worstScore)/(bestScore-worstScore);
        valueVec.push_back(normalized);
    }

    m_fitnessGraph->set_values(valueVec);

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
