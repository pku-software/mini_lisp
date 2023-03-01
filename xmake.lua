add_rules("mode.debug", "mode.release")

target("mini_lisp")
  add_files("src/*.cpp")
  set_languages("c++20")
  set_targetdir("bin")
  if is_plat("wasm") then
    add_defines("WASM")
    add_links("embind")
    add_cxflags("-fwasm-exceptions", "-fexperimental-library")
    add_ldflags("-fwasm-exceptions")
  end
