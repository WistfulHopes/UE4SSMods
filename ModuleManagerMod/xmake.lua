local projectName = "ModuleManagerMod"

add_requires("raw_pdb", { debug = is_mode_debug(), configs = { runtimes = get_mode_runtimes() } })

target(projectName)
    add_rules("ue4ss.mod")

    add_includedirs("include", { public = true })
    add_headerfiles("include/**.hpp")

    add_files("src/ModuleManager.cpp")
    add_files("src/SymbolResolver.cpp")

    add_packages("raw_pdb")