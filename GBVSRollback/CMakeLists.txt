project(GBVSRollback)

set(TARGET GBVSRollback)
project(${TARGET})

add_subdirectory(GekkoNet)

set(${TARGET}_Includes
        "${CMAKE_CURRENT_SOURCE_DIR}/include/BattleState.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/CXXBYTE.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/Particles.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/safetyhook.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/struct_util.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/Unreal.hpp"
        )

set(${TARGET}_Sources
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dllmain.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/BattleState.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Particles.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/safetyhook.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Unreal.cpp"
        )

add_library(${TARGET} SHARED ${${TARGET}_Sources} ${${TARGET}_Includes})
target_include_directories(${TARGET} PRIVATE "include")
target_link_libraries(${TARGET} PUBLIC UE4SS)
