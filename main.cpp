//
// Created by st on 6/19/21.
//

#define _DEBUG

#include <iostream>

#include "PxPhysicsAPI.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


using namespace physx;

static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;

int physxFunc(){
    PxFoundation* mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
    if (!mFoundation) {
        printf("PxCreateFoundation failed!");
        return -1;
    }

    bool recordMemoryAllocations = true;

    PxPhysics* mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), recordMemoryAllocations,
                                          nullptr);
    if (!mPhysics) {
        printf("PxCreatePhysics failed!");
        return -1;
    }

    PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(0);

    PxScene* mScene = mPhysics->createScene(sceneDesc);

    PxMaterial* mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    PxVec3 normalVec(0, 1, 0);
    PxVec3 normal = normalVec.getNormalized();
    PxRigidStatic* plane = PxCreatePlane(*mPhysics, PxPlane(normal, 0), *mMaterial);
    mScene->addActor(*plane);

    PxShape* boxShape = mPhysics->createShape(PxBoxGeometry(0.5, 0.5, 0.5), *mMaterial);
    PxTransform transform(PxVec3(0, 10, 0));
    PxRigidDynamic* body = mPhysics->createRigidDynamic(transform);
    body->attachShape(*boxShape);
    PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
    mScene->addActor(*body);
    boxShape->release();

    for (int i = 0; i < 10000; ++i) {
        mScene->simulate(0.01f);
        mScene->fetchResults(true);
    }

    body->release();
    mPhysics->release();
    mFoundation->release();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(){

    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Interactive Virtual Creatures", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    if(physxFunc() < 0)
        return -1;

    return 0;
}