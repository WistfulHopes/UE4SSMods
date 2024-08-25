local projectName = "GBVSRollback"

target(projectName)
    after_load(function(target)
        -- Binary outputs are written to the `Binaries` dir.
        target:set("targetdir", path.join(os.projectdir(), "Binaries", get_config("mode"), target:name()))

        target:set("kind", "shared")
        target:set("languages", "cxx23")
        target:set("exceptions", "cxx")
        target:add("deps", "UE4SS")
        target:set("group", "mods")
    end)

    on_install(function(target)
        import("mods.install").install(target)
    end)

    add_includedirs("include", { public = true })
    add_headerfiles("include/**.hpp")
    add_headerfiles("include/**.h")

    add_files("src/dllmain.cpp")
    add_files("src/safetyhook.cpp")
    add_files("src/BattleState.cpp")
    add_files("src/Particles.cpp")
    add_files("src/Unreal.cpp")