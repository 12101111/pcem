includes("slirp")

target("network")
    set_kind("static")
    add_files("*.c")
    add_includedirs("$(projectdir)/include")
    add_deps("slirp")
    if is_os("windows") then
        add_syslinks("wsock32","iphlpapi")
    end
