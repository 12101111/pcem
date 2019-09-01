set_project("PCem")
set_version("14.0.0")
set_xmakever("2.2.5")

add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    add_defines("DEBUG")
    set_symbols("debug")
    set_optimize("none")
end
if is_mode("release") then
    add_defines("RELEASE_BUILD")
    set_symbols("hidden")
    set_strip("all")
    add_cxflags("-fomit-frame-pointer")
    add_mxflags("-fomit-frame-pointer")
    set_optimize("fastest")
end

if is_os("macosx") then
    add_defines("PCEM_RENDER_WITH_TIMER")
    add_defines("PCEM_RENDER_TIMER_LOOP")
end

set_warnings("all")

option("network")
    set_default(false)
    set_showmenu(true)
    add_defines("USE_NETWORKING")

includes("src/codegen")
includes("src/network")

target("pcem")
    set_kind("binary")
    set_default(true)
    add_includedirs("include")
    add_files("src/cdrom/*.c")
    add_files("src/cdrom/*.cc")
    add_files("src/device/*.c")
    add_files("src/disk/*.c")
    add_files("src/joystick/*.c")
    add_files("src/keyboard/*.c")
    add_files("src/model/*.c")
    add_files("src/mouse/*.c")
    add_files("src/sound/**.c")
    add_files("src/sound/**.cc")
    add_files("src/video/*.c")
    add_files("src/wx/*.c|wx-sdl2-display-win.c")
    add_files("src/wx/*.cc")
    add_files("src/wx/*.cpp")
    add_files("src/*.c")
    add_deps("codegen")
    if has_config("network") then
        add_deps("network")
    end
    if is_os("macosx") then 
        add_frameworks("OpenGL","OpenAL")
    else 
        add_syslinks("GL","openal")
    end
    add_syslinks("pthread","SDL2")
    on_load(function (target)
        local wxcxflags,err = os.iorun("wx-config --cxxflags")
        target:add("cxflags",wxcxflags)
        local wxlibs,err2 = os.iorun("wx-config --libs");
        target:add("ldflags",wxlibs)
    end)
