cmake_minimum_required(VERSION 3.11.4)

project(c_str_span VERSION 0.0.1 LANGUAGES C)

option(PRECONDITIONS "Enable precondition checking" ON)
if (NOT PRECONDITIONS)
    add_compile_definitions(AZ_NO_PRECONDITION_CHECKING)
endif ()

#=============#
# CMake Toggles #
#=============#

# LTO toggle
option(ENABLE_LTO "Enable Link-Time Optimization (LTO)" OFF)
if(ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported)
    if(ipo_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

# Charsets toggle
option(ENABLE_UNICODE "Enable UNICODE support" ON)
if(ENABLE_UNICODE)
    add_compile_definitions(UNICODE _UNICODE)
endif()

# MSVC specific toggles
if(MSVC)
    # Threading toggle
    option(ENABLE_MULTITHREADING "Enable Multi-threading (MSVC)" ON)

    # CRT Linkage toggle
    option(USE_STATIC_CRT "Use static CRT (/MT, /MTd)" OFF)

    # RTC toggles
    option(ENABLE_RTC1 "Enable Basic Runtime Checks (/RTC1)" OFF)
    option(ENABLE_RTCS "Enable Stack Frame Runtime Checks (/RTCs)" OFF)
    option(ENABLE_RTCU "Enable Uninitialized Variable Runtime Checks (/RTCu)" OFF)

    # Use CMake's built-in CMP0091 for CRT linkage
    if(POLICY CMP0091)
        cmake_policy(SET CMP0091 NEW)
        if(USE_STATIC_CRT)
            if(ENABLE_MULTITHREADING)
                set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
            else()
                # CMake CMP0091 doesn't have SingleThreaded, MSVC removed it.
                set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
            endif()
        else()
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        endif()
    endif()

    # Configure RTC flags
    set(RTC_FLAGS "")
    if(ENABLE_RTC1)
        list(APPEND RTC_FLAGS "/RTC1")
    endif()
    if(ENABLE_RTCS AND NOT ENABLE_RTC1)
        list(APPEND RTC_FLAGS "/RTCs")
    endif()
    if(ENABLE_RTCU AND NOT ENABLE_RTC1)
        list(APPEND RTC_FLAGS "/RTCu")
    endif()
    if(RTC_FLAGS)
        add_compile_options("$<$<CONFIG:Debug>:${RTC_FLAGS}>")
    endif()

    if(NOT ENABLE_MULTITHREADING AND NOT USE_STATIC_CRT)
        # Note: Single-threaded CRT (/ML) was removed in VS 2005, but we provide option as requested
    endif()
endif()

#===============#
# Compiler args #
#===============#

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
set(CMAKE_C_STANDARD 90 CACHE STRING "")

add_library("${PROJECT_NAME}_compiler_flags" INTERFACE)
if (NOT DEFINED MSVC_VERSION
        OR MSVC_VERSION STRGREATER "1900" # 2015
        OR NOT (CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom"))
    target_compile_features("${PROJECT_NAME}_compiler_flags" INTERFACE "c_std_${CMAKE_C_STANDARD}")
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15")
        set(gcc_like "$<COMPILE_LANG_AND_ID:C,CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
        set(msvc "$<COMPILE_LANG_AND_ID:C,CXX,MSVC>")
        target_compile_options(
                "${PROJECT_NAME}_compiler_flags"
                INTERFACE
                "$<$<AND:${gcc_like},$<CONFIG:Debug>>:$<BUILD_INTERFACE:-Wshadow;-Wformat=2;-Wall;-Wno-missing-braces;-Wno-long-long;-pedantic;-fprofile-arcs;-ftest-coverage>>"
                "$<${msvc}:$<BUILD_INTERFACE:-W4;-WX;-Zi;-permissive->>"
        )
        target_link_options(
                "${PROJECT_NAME}_compiler_flags"
                INTERFACE
                "$<$<AND:${gcc_like},$<CONFIG:Debug>>:$<BUILD_INTERFACE:--coverage>>"
        )
    endif (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15")
endif ()
# Set the build directories
if (CMAKE_SYSTEM_NAME STREQUAL "Windows"
        OR CMAKE_SYSTEM_NAME STREQUAL "CYGWIN"
        OR CMAKE_SYSTEM_NAME MATCHES "MINGW.*")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
else ()
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif ()

#===========#
# Configure #
#===========#

# configure a header file to pass the version number only
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
        "${PROJECT_NAME}Config.h"
)

#=========#
# Install #
#=========#

include(GNUInstallDirs)

#=============#
# Sub-targets #
#=============#

add_subdirectory("${PROJECT_NAME}")

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(BUILD_TESTING_${PROJECT_NAME} "Build the tests for c_str_span" ON)
else()
    option(BUILD_TESTING_${PROJECT_NAME} "Build the tests for c_str_span" OFF)
endif()

if (BUILD_TESTING_${PROJECT_NAME})
    include(CTest)
    if (BUILD_TESTING)
        message(STATUS "TESTS ENABLED")
        add_subdirectory("${PROJECT_NAME}/tests")
        file(WRITE "${CMAKE_BINARY_DIR}/CTestCustom.cmake"
            "set(CTEST_CUSTOM_COVERAGE_EXCLUDE\n"
            "    \".*tests/.*\"\n"
            "    \".*test\\\\.c\"\n"
            ")\n"
        )
    else()
        message(STATUS "TESTS DISABLED: BUILD_TESTING is OFF")
    endif()
else()
    message(STATUS "TESTS DISABLED: BUILD_TESTING_${PROJECT_NAME} is OFF")
endif()

install(
        FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.h"
        TYPE "INCLUDE"
)
if (NOT CMAKE_CROSSCOMPILING)
    include(InstallRequiredSystemLibraries)
endif()
set(CPACK_BUNDLE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "SamuelMarks")
set(CPACK_PACKAGE_DESCRIPTION "UTF-8 replacement for C strings, supporting zero-copy use-cases (non-null-terminated).")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION}")
if (APPLE)
    set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Info.plist")
    set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Info.plist")
    set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CustomVolumeIcon.icns")
endif (APPLE)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE-MIT")
set(CPACK_PACKAGE_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/cmake/README.txt")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Welcome.txt")
set(CPACK_PACKAGE_CONTACT "https://github.com/SamuelMarks/c-str-span")

include(CPack)
include(CMakePackageConfigHelpers)

# generate the config file that is includes the exports
configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# generate the version file for the config file
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}"
        COMPATIBILITY AnyNewerVersion
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")
