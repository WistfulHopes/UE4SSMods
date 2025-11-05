project(SuzieMod)

set(TARGET SuzieMod)
project(${TARGET})

set(${TARGET}_Sources
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dllmain.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Suzie.cpp"
        )

add_library(${TARGET} SHARED ${${TARGET}_Sources})
target_include_directories(${TARGET} PRIVATE "include")
target_link_libraries(${TARGET} PUBLIC UE4SS)
