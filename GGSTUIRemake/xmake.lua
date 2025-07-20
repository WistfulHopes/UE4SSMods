local projectName = "GGSTUIRemake"

target(projectName)
    add_rules("ue4ss.mod")

    add_includedirs("include", { public = true })
    add_headerfiles("include/**.hpp")
    add_headerfiles("include/**.h")

    add_files("src/dllmain.cpp")
    add_files("src/REDGameMode_CharaSelectRE.cpp")
    add_files("src/REDGameState_CharaSelectRE.cpp")
    add_files("src/safetyhook.cpp")