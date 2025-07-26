local projectName = "CSharpLoader"

target(projectName)
    add_rules("ue4ss.mod")

    add_includedirs("include", { public = true })
    add_headerfiles("include/**.hpp")
    add_headerfiles("include/**.h")

    add_files("src/DotNetLibrary.cpp")
