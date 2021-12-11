cmake_minimum_required(VERSION 3.16)
project(vAmiga C CXX)

if (MSVC)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Supported build configurations" FORCE)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /permissive- /Zi /Zo /sdl /W4 /wd4100 /wd4201 /wd4324 /wd4458")

    if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /JMC")
    endif()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    add_definitions("-D_CRT_SECURE_NO_WARNINGS")

else()
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_EXTENSIONS False)

    set(COMMON_FLAGS "-O3 -ggdb")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts")
    endif()
endif()

if (WIN32)
    add_definitions("-DWIN32 -D_WIN32 -DUNICODE -D_UNICODE -D_WIN32_WINNT=0x600")
elseif (${CMAKE_HOST_SYSTEM_VERSION} MATCHES ".*-WSL2$")
    message(WARNING "Adding WSL2 mouse hack")
    add_definitions("-DWSL2_MOUSE_HACK")
endif()

if (WIN32)
    include(ExternalProject)
    ExternalProject_Add(SDL2_LIB
        URL "https://www.libsdl.org/release/SDL2-devel-2.0.18-VC.zip"
        URL_HASH SHA256=D4A56E2EE7C0EAE2EF0D511201D3BD38C5AB255662E4B571D4D630762473BF42
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_property(SDL2_LIB SOURCE_DIR)
    set(SDL2_INCLUDE_DIRS "${SOURCE_DIR}/include")
    add_library(SDL2 STATIC IMPORTED GLOBAL)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION "${SOURCE_DIR}/lib/x64/SDL2.lib")
    else()
        set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION "${SOURCE_DIR}/lib/x86/SDL2.lib")
    endif()
    set (SDL2_LIBRARIES SDL2)
else()
    find_package(SDL2 REQUIRED)
endif()

add_subdirectory("vAmiga/Emulator" "vAmigaCore")

add_executable(vAmiga main.cpp)
target_include_directories(vAmiga PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(vAmiga vAmigaCore ${SDL2_LIBRARIES})
if (NOT WIN32)
    target_link_libraries(vAmiga pthread)
else()
    target_link_libraries(vAmiga ws2_32)
endif()
