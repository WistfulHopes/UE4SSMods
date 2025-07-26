local projectName = "CSharpLoaderProxy"

target(projectName)
    add_rules("ue4ss.mod")

    add_deps("CSharpLoader")

    add_files("src/dllmain.cpp")
