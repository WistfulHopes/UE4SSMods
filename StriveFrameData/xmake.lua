local projectName = "GGSTFrameViewer"

target(projectName)
    add_rules("ue4ss.mod")

    add_includedirs("include", { public = true })
    add_headerfiles("include/**.hpp")
    add_headerfiles("include/**.h")

    add_files("src/dllmain.cpp")
    add_files("src/arcsys.cpp")
    add_files("src/bbscript.cpp")
    add_files("src/bind_watcher.cpp")
    add_files("src/draw_utils.cpp")
    add_files("src/framebar.cpp")
    add_files("src/hitboxes.cpp")
    add_files("src/menu.cpp")
    add_files("src/sigscan.cpp")