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
        main.cpp glad.c App.h App.cpp PhysicsWorld.h PhysicsWorld.cpp ShapeHandler.h ShapeHandler.cpp
        RootMorphNode.h RootMorphNode.cpp MorphNode.h MorphNode.cpp NeuronCluster.h NeuronCluster.cpp Constants.h)

TARGET_LINK_LIBRARIES(interactive_virtual_creatures glfw3.a PhysXExtensions_static_64.a PhysX_static_64.a PhysXPvdSDK_static_64.a PhysXCommon_static_64.a PhysXFoundation_static_64.a  )