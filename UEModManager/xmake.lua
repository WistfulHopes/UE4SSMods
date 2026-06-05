local projectName = "GGSTModManager"

add_requires("vcpkg::glaze 4.2.2")
add_requires("minhook")

target(projectName)
    add_packages("vcpkg::glaze", "minhook")
    add_rules("ue4ss.mod")
    
    add_includedirs("include", { public = true })
    add_headerfiles("include/**.hpp")
    add_headerfiles("include/**.h")

    add_files("src/dllmain.cpp")
    add_files("src/safetyhook.cpp")
    add_files("src/ModManager.cpp")
    add_files("src/kiero.cpp")
