project(GGSTModManager)

set(TARGET GGSTModManager)
project(${TARGET})

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG v1.3.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(minhook)
# Uses suppress_third_party_warnings() from cmake/modules/ThirdPartyWarnings.cmake
suppress_third_party_warnings(minhook)

set(${TARGET}_Sources
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dllmain.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/safetyhook.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ModManager.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/kiero.cpp"
        )

add_library(${TARGET} SHARED ${${TARGET}_Sources})
target_include_directories(${TARGET} PRIVATE "include")
target_link_libraries(${TARGET} PUBLIC UE4SS)
target_link_libraries(${TARGET} PUBLIC ImGui glaze::glaze minhook)