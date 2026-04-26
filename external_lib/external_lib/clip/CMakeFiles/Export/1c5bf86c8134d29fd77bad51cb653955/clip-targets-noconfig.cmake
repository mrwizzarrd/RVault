#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "clip::clip" for configuration ""
set_property(TARGET clip::clip APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(clip::clip PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib64/libclip.a"
  )

list(APPEND _cmake_import_check_targets clip::clip )
list(APPEND _cmake_import_check_files_for_clip::clip "${_IMPORT_PREFIX}/lib64/libclip.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
