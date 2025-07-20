local projectName = "SuzieMod"

add_requires("raw_pdb", { debug = is_mode_debug(), configs = { runtimes = get_mode_runtimes() } })

target(projectName)
    add_rules("ue4ss.mod")

    add_includedirs("include", { public = true })
    add_headerfiles("include/**.hpp")

    add_files("src/dllmain.cpp", "src/Suzie.cpp")