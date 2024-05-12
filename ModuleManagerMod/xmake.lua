local projectName = "ModuleManagerMod"

add_requires("raw_pdb", { debug = is_mode_debug(), configs = { runtimes = get_mode_runtimes() } })

target(projectName)
    set_kind("shared")
    set_languages("cxx20")
    set_exceptions("cxx")

    add_includedirs("include", { public = true })
    add_headerfiles("include/**.hpp")

    add_files("src/ModuleManager.cpp")
    add_files("src/SymbolResolver.cpp")

    add_deps(
        "UE4SS"
    )

    add_packages("raw_pdb")

    
    on_load(function (target)
        import("build_configs", { rootdir = get_config("scriptsRoot") })
        build_configs:set_output_dir(target)
    end)
    
    on_config(function (target)
        import("build_configs", { rootdir = get_config("scriptsRoot") })
        build_configs:config(target)
    end)
    
    after_clean(function (target)
        import("build_configs", { rootdir = get_config("scriptsRoot") })
        build_configs:clean_output_dir(target)
    end)