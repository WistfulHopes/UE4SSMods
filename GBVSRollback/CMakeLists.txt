cmake_minimum_required(VERSION 3.18)

set(TARGET GBVSRollback)
project(${TARGET})

set(${TARGET}_Sources
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dllmain.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/PatternFinder.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/BattleState.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Particles.cpp"
        )

add_library(${TARGET} SHARED ${${TARGET}_Sources})
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${TARGET} PUBLIC UE4SS)
