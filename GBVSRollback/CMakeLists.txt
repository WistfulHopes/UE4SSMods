project(GBVSRollback)

set(TARGET GBVSRollback)
project(${TARGET})

add_subdirectory(deps/GekkoNet)

set_property(TARGET GekkoNet PROPERTY MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Game__Debug__Win64,CasePreserving__Debug__Win64,LessEqual421__Debug__Win64>:Debug>DLL")

set(${TARGET}_Includes
        "${CMAKE_CURRENT_SOURCE_DIR}/include/BattleState.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/CXXBYTE.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/Particles.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/safetyhook.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/struct_util.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/Unreal.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/Pawn.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/RollbackContext.hpp"
        )

set(${TARGET}_Sources
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dllmain.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/BattleState.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Particles.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/safetyhook.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Unreal.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Pawn.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/RollbackContext.cpp"
        )

set(${TARGET}_LZ4_Sources
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/lz4/lib/lz4.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/lz4/lib/xxhash.c"
        )

add_library(${TARGET} SHARED ${${TARGET}_Sources} ${${TARGET}_Includes} ${${TARGET}_LZ4_Sources})
target_include_directories(${TARGET} PRIVATE "include" "deps/lz4/lib")
target_link_libraries(${TARGET} PUBLIC UE4SS GekkoNet)
