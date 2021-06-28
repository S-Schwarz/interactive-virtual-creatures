//
// Created by st on 6/25/21.
//

#include "App.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void ivc::App::processInput()
{
    if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(m_window, true);
        m_shouldClose = true;
    }

    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera.ProcessKeyboard(FORWARD, m_deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera.ProcessKeyboard(BACKWARD, m_deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.ProcessKeyboard(LEFT, m_deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.ProcessKeyboard(RIGHT, m_deltaTime);

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){

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

    // Create Window
    m_window = glfwCreateWindow(800, 600, "Interactive Virtual Creatures", NULL, NULL);
    if (m_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetCursorPosCallback(m_window, mouse_callback);


    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, c_WIDTH, c_HEIGHT);

    m_shader = new Shader("../shader.vert", "../shader.frag");

    ShapeHandler::initShapes();

    glfwSetWindowUserPointer(m_window, this);

    m_physicsWorld = new PhysicsWorld();
    m_physicsWorld->init();

    isInitialized = true;

    return 0;
}

glm::vec3 cubePosition = glm::vec3( 0.0f,  0.0f,  0.0f);

int ivc::App::render() {

    if(!isInitialized || m_shouldClose)
        return -1;

    if(glfwWindowShouldClose(m_window)){
        m_shouldClose = true;
        return -1;
    }

    float currentTime = glfwGetTime();
    m_deltaTime = currentTime - m_lastTime;
    m_lastTime = currentTime;

    m_accumulator += m_deltaTime;

    processInput();

    // Physics simulation ---------

    float physicsStep = m_physicsWorld->getStepSize();
    while (m_accumulator >= physicsStep){
        m_physicsWorld->simulate();
        m_accumulator -= physicsStep;
    }

    // ----------------------------

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    glm::mat4 view = m_camera.GetViewMatrix();
    m_shader->setMat4("view", view);

    int viewLoc = glGetUniformLocation(m_shader->ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    int projectionLoc = glGetUniformLocation(m_shader->ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader->use();

    //-------------------------

    // PHYSX OBJECTS ----------
    for(auto body : m_physicsWorld->getRigidDynamics()){
        auto transform = body->getGlobalPose();

        glm::vec3 posVec = glm::vec3(transform.p.x, transform.p.y, transform.p.z);

        glm::quat rotQuat = glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);

        //TODO: ???
        PxShape* bodyShape = nullptr;
        body->getShapes(&bodyShape, sizeof(bodyShape),0);
        PxBoxGeometry* bodyGeom = new PxBoxGeometry();
        bodyShape->getBoxGeometry(*bodyGeom);
        glm::vec3 scaleVec = glm::vec3(bodyGeom->halfExtents.x * 2, bodyGeom->halfExtents.y * 2, bodyGeom->halfExtents.z * 2);

        drawShape(BOX, posVec, rotQuat, scaleVec, glm::vec4(0.0f, 0.8f, 0.3f, 1.0f), true);
    }

    // PLANE ------------------
    for(auto rigidStatic : m_physicsWorld->getRigidStatics()){
        //TODO: ???
        PxPlane* plane = (PxPlane*)rigidStatic;
        auto transformA = rigidStatic->getGlobalPose();
        auto transformB = PxTransformFromPlaneEquation(*plane);

        glm::vec3 posVec = glm::vec3(transformA.p.x, transformA.p.y, transformA.p.z);

        glm::quat rotQuat = glm::quat(transformB.q.w, transformB.q.x, transformB.q.y, transformB.q.z);

        drawShape(PLANE, posVec, rotQuat, glm::vec3(1000.0f, 0.0f, 1000.0f), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), false);
    }

    //-------------------------

    glfwSwapBuffers(m_window);
    glfwPollEvents();

    return 0;

}

int ivc::App::drawShape(Shape shape, glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec4 color, bool wireframe) {

        switch(shape){
            case BOX:
                ShapeHandler::bindBoxVAO();
                break;
            case PLANE:
                ShapeHandler::bindPlaneVAO();
                break;
            default:
                return -1;
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = model * glm::mat4_cast(rotation);
        model = glm::scale(model, scale);

        m_shader->setMat4("model", model);
        m_shader->setVec4("drawColor", color);

        if(wireframe){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }else{
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glDrawArrays(GL_TRIANGLES, 0, 36);

        return 0;

}

int ivc::App::close() {

    if(!isInitialized)
        return -1;

    m_physicsWorld->destroy();
    glfwTerminate();

    return 0;

}