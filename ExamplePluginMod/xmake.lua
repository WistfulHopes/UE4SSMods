local projectName = "ExamplePluginMod"

target(projectName)
    add_rules("ue4ss.mod")
    
    add_files("src/dllmain.cpp")

    add_deps(
        "Function", "ModuleManagerMod"
    )