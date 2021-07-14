cmake_minimum_required(VERSION 3.19)
project(interactive_virtual_creatures)

add_compile_definitions(_DEBUG)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-lglfw3 -Wl,--no-as-needed -ldl -lGL -lX11 -pthread -lXrandr -lXi ")

add_subdirectory(../glfw ./glfw)

INCLUDE_DIRECTORIES(~/private/PhysX/physx/include ~/private/PhysX/pxshared/include ~/private/glfw/include ~/private/GLAD/include ~/private/glm)
LINK_DIRECTORIES(~/private/PhysX/physx/bin/linux.clang/debug ~/private/glfw-build/src)

add_executable(interactive_virtual_creatures
        main.cpp Res/glad.c App/App.h App/App.cpp World/PhysicsWorld.h World/PhysicsWorld.cpp Res/ShapeHandler.h Res/ShapeHandler.cpp
        Body/RootMorphNode.h Body/RootMorphNode.cpp Body/MorphNode.h Body/MorphNode.cpp Control/NeuronCluster.h Control/NeuronCluster.cpp Constants.h
        Body/BaseNode.h Body/BaseNode.cpp Body/PhysicalCreature.h Body/PhysicalCreature.cpp Control/Neurons/Neuron.h Control/Neurons/Neuron.cpp Control/Neurons/TwoInputs/Neuron_Sum.h Control/Neurons/TwoInputs/Neuron_Sum.cpp
        Control/Neurons/TwoInputs/Neuron_Min.h Control/Neurons/TwoInputs/Neuron_Min.cpp Control/Neurons/TwoInputs/Neuron_Max.h Control/Neurons/TwoInputs/Neuron_Max.cpp Control/Gate.h Control/Gate.cpp
        Control/Joints/JointSensor.h Control/Joints/JointSensor.cpp Control/Joints/JointEffector.h Control/Joints/JointEffector.cpp Control/IDHandler.h Control/IDHandler.cpp Control/Neurons/TwoInputs/Neuron_TwoInputs.h Control/Neurons/TwoInputs/Neuron_TwoInputs.cpp Control/Neurons/NeuronFactory.cpp Control/Neurons/NeuronFactory.h)

TARGET_LINK_LIBRARIES(interactive_virtual_creatures glfw3.a PhysXExtensions_static_64.a PhysX_static_64.a PhysXPvdSDK_static_64.a PhysXCommon_static_64.a PhysXFoundation_static_64.a  )