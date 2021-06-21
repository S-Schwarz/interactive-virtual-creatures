cmake_minimum_required(VERSION 3.19)
project(interactive_virtual_creatures)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-pthread -Wl,--no-as-needed -ldl")

INCLUDE_DIRECTORIES(~/private/PhysX/physx/include ~/private/PhysX/pxshared/include)
LINK_DIRECTORIES(~/private/PhysX/physx/bin/linux.clang/debug)

add_executable(interactive_virtual_creatures
        main.cpp)

TARGET_LINK_LIBRARIES(interactive_virtual_creatures PhysXExtensions_static_64.a PhysX_static_64.a PhysXPvdSDK_static_64.a PhysXCommon_static_64.a PhysXFoundation_static_64.a  )