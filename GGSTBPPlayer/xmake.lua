target("GGSTBPPlayer")
    set_kind("shared")
    set_languages("cxx20")
    set_exceptions("cxx")
    add_files("src/*.cpp")
    add_deps(
        "UE4SS", { public = true }
    )