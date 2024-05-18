local projectName = "GBVSRollback"

target(projectName)
    add_rules("ue4ss.mod")

    add_includedirs("include", { public = true })
    add_headerfiles("include/**.hpp")

    add_files("src/dllmain.cpp")
    add_files("src/PatternFinder.cpp")
    add_files("src/BattleState.cpp")
    add_files("src/Particles.cpp")