local projectName = "GGSTModManager"

add_requires("vcpkg::glaze 4.2.2")

target(projectName)
    add_packages("vcpkg::glaze")
    add_rules("ue4ss.mod")

    add_includedirs("include", { public = true })
    add_headerfiles("include/**.hpp")
    add_headerfiles("include/**.h")

    add_files("src/dllmain.cpp")
    add_files("src/safetyhook.cpp")
    add_files("src/ModManager.cpp")
    add_files("src/d3d11hook.cpp")
