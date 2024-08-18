local projectName = "MVCICharaEnable"

target(projectName)
    add_rules("ue4ss.mod")

    add_files("src/dllmain.cpp")