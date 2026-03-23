set_project("nes")
set_xmakever("3.0.0")
add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
else
    set_strip("all")
    set_symbols("hidden")
    set_optimize("fastest")
end

set_warnings("allextra")
set_languages("c11")

if is_plat("wasm") then
    add_requires("emscripten")
    set_toolchains("emcc@emscripten")
end

-- TODO
-- if is_host("windows") then
-- elseif is_host("linux") then
-- elseif is_host("macosx") then
-- else
-- end

-- [[ add SDL3 ]]
add_requires("libsdl3")
add_packages("libsdl3")

target("nes", function ()
    set_kind("binary")

    local nes_dir = "../.."
    add_includedirs(nes_dir .. "/inc")
    add_files(nes_dir .. "/src/**.c")

    add_includedirs("port")
    add_files("port/*.c")

    add_files("main.c")

end)
