target("codegen")
    set_kind("static")
    add_files("*.c")
    add_includedirs("$(projectdir)/include")
    add_includedirs("./")
    if is_arch("i386") then
        add_files("i386/*.c")
    end
    if is_arch("x86_64") then
        add_files("x86_64/*.c")
    end
    if is_arch("armv7*") then
        add_files("arm/*.c")
    end
    if is_arch("arm64*") then
        add_files("arm64/*.c")
    end
