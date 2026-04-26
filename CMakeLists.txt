cmake_minimum_required(VERSION 3.16)

project(RVault C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)




add_executable(rvault
        src/source/rvault_random.c
        src/source/rvault_crypto.c
        src/source/rvault_auth.cpp
        src/source/rvault_main.cpp
        src/source/rvault_platform.c
        src/headers/rvault_random.h
        src/headers/rvault_crypto.h
        src/headers/rvault_auth.h
        src/headers/rvault_platform.h
        src/headers/rvault_file.h
        src/headers/rvault_constants.h
        src/source/rvault_file.cpp
        src/headers/rvault_vault.h
        src/source/rvault_session.cpp
        src/headers/rvault_session.h
        src/source/rvault_auth.cpp
        src/headers/rvault_exception.h
        src/source/rvault_exception.cpp
        src/headers/rvault_ui.h
        src/source/rvault_ui.cpp
)

# link clip
set(CLIP_ENABLE_IMAGE OFF CACHE BOOL "" FORCE)
add_subdirectory(external_lib/clip)
target_link_libraries(rvault PRIVATE clip)

# Find and link libsodium
find_package(unofficial-sodium CONFIG QUIET)
if(unofficial-sodium_FOUND)
    target_link_libraries(rvault PRIVATE unofficial-sodium::sodium)
else()
    find_path(SODIUM_INCLUDE_DIR sodium.h)
    find_library(SODIUM_LIBRARY sodium)
    if(NOT SODIUM_INCLUDE_DIR OR NOT SODIUM_LIBRARY)
        message(FATAL_ERROR "libsodium not found. Install libsodium-dev.")
    endif()
    target_include_directories(rvault PRIVATE ${SODIUM_INCLUDE_DIR})
    target_link_libraries(rvault PRIVATE ${SODIUM_LIBRARY})
endif()

#Windows specific
if(WIN32)
    target_link_libraries(rvault PRIVATE bcrypt)
endif()

