cmake_minimum_required(VERSION 3.19)
project(interactive_virtual_creatures)

add_compile_definitions(NDEBUG)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-lglfw3 -Wl,--no-as-needed -ldl -lGL -lX11 -pthread -lXrandr -lXi ")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Res/glfw)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Res/PhysX/physx/include ${CMAKE_CURRENT_SOURCE_DIR}/Res/PhysX/pxshared/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Res/glfw/include ${CMAKE_CURRENT_SOURCE_DIR}/Res/GLAD/include ${CMAKE_CURRENT_SOURCE_DIR}/Res/glm)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Res/PhysX/physx/bin/linux.clang/release ${CMAKE_CURRENT_SOURCE_DIR}/Res/glfw-build/src)

add_executable(interactive_virtual_creatures
        main.cpp Res/glad.c App/App.h App/App.cpp Res/ShapeHandler.h Res/ShapeHandler.cpp
        Body/RootMorphNode.h Body/RootMorphNode.cpp Body/MorphNode.h Body/MorphNode.cpp Control/NeuronCluster.h Control/NeuronCluster.cpp Constants.h
        Body/BaseNode.h Body/BaseNode.cpp Body/PhysicalCreature.h Body/PhysicalCreature.cpp Control/Neuron.h Control/Neuron.cpp
        Control/Gate.h Control/Gate.cpp
        Control/Joints/JointSensor.h Control/Joints/JointSensor.cpp Control/Joints/JointEffector.h Control/Joints/JointEffector.cpp Control/IDHandler.h Control/IDHandler.cpp
        Mutator.h Physics/PhysicsBase.cpp Physics/PhysicsBase.h Physics/PhysicsScene.cpp Physics/PhysicsScene.h Physics/LiveEnvironment.cpp Physics/LiveEnvironment.h
        Evolution/Evolver.cpp Evolution/Evolver.h Res/stb_image.h Control/ContactSensor.cpp Control/ContactSensor.h Physics/ContactReporter.h Evolution/EvoData.cpp Evolution/EvoData.h)

TARGET_LINK_LIBRARIES(interactive_virtual_creatures glfw3.a PhysXExtensions_static_64.a PhysX_static_64.a PhysXPvdSDK_static_64.a PhysXCommon_static_64.a PhysXFoundation_static_64.a  )