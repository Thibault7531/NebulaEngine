# Cmake file for Nebula Engine target


# Dependencies
get_directory_property(old_dir_compile_options COMPILE_OPTIONS)
add_compile_options(-w)
add_subdirectory(vendor)
set_directory_properties(PROPERTIES COMPILE_OPTIONS "${old_dir_compile_options}")

add_subdirectory(source/Platform)
add_subdirectory(source/Core)