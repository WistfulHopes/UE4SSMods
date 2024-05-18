local projectName = "MVCIViewer"

target(projectName)
    add_rules("ue4ss.mod")

    add_includedirs("include", { public = true })
    add_headerfiles("include/**.hpp")
    
    add_files("src/dllmain.cpp")
    add_files("src/math_util.cpp")