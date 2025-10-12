project(GGSTBPPlayer)

set(TARGET GGSTBPPlayer)
project(${TARGET})

set(${TARGET}_Sources
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dllmain.cpp"
        )

add_library(${TARGET} SHARED ${${TARGET}_Sources})
target_link_libraries(${TARGET} PUBLIC UE4SS)
