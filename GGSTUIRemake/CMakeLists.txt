project(GGSTUIRemake)

set(TARGET GGSTUIRemake)
project(${TARGET})

set(${TARGET}_Sources
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dllmain.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/REDGameMode_CharaSelectRE.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/REDGameState_CharaSelectRE.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/safetyhook.cpp"
        )

add_library(${TARGET} SHARED ${${TARGET}_Sources})
target_include_directories(${TARGET} PRIVATE "include")
target_link_libraries(${TARGET} PUBLIC UE4SS)
