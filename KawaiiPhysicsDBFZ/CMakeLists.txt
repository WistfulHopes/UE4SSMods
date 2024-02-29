cmake_minimum_required(VERSION 3.18)

set(TARGET KawaiiPhysicsDBFZ)
project(${TARGET})

set(${TARGET}_Sources
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dllmain.cpp"
        )

add_library(${TARGET} SHARED ${${TARGET}_Sources})
target_link_libraries(${TARGET} PUBLIC UE4SS)