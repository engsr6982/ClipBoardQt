add_rules("mode.release", "mode.debug")

-- This Qt app:
--    Windows:
--      MSVC:
--          xmake f -c --qt=C:\Qt\6.7.2\msvc2019_64
--      Mingw:
--          xmake f -p mingw -a x86_64 --qt=D:/software/Qt5.14.2/5.14.2/mingw73_64 --mingw=D:/software/Qt5.14.2/Tools/mingw730_64 --toolchain=mingw
--    Linux:
--      Todo


add_requires(
    "nlohmann_json 3.11.3",
    "fmt 10.2.1"
)


if not has_config("vs_runtime") then 
    set_runtimes("MD")
end 


target("ClipBoardQt")
    add_cxflags(
        "/utf-8"
    )
    add_defines(
        "NOMINMAX",
        "UNICODE"
    )
    set_symbols("debug")

    add_rules("qt.widgetapp")

    add_files(
        "src/*.cpp",
        "src/*.h",
        "src/*.ui"
    )
    add_includedirs("src")
    add_headerfiles("src/*.h")

    add_packages(
        "nlohmann_json",
        "fmt"
    )
