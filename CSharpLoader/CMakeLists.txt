cmake_minimum_required(VERSION 3.18)

set(TARGET CSharpLoader)
project(${TARGET})

set(${TARGET}_Sources
        "${CMAKE_CURRENT_SOURCE_DIR}/src/CoreCLR.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/DotNetLibrary.cpp"
        )

add_library(${TARGET} SHARED ${${TARGET}_Sources})
target_compile_features(${TARGET} PUBLIC cxx_std_23)
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${TARGET} PUBLIC UE4SS "${CMAKE_CURRENT_SOURCE_DIR}/lib-vc2022/libnethost.lib")