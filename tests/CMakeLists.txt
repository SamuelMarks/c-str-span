get_filename_component(EXEC_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(EXEC_NAME "${EXEC_NAME}_exec")

#=======================#
# Dependencies download #
#=======================#

set(GREATEST_SHA256 "b3f89ecad2301c64c580f328cc03a74d92f4cd4bf42ab18d3d4bfb7b8be5b948")
set(DOWNLOAD_DIR "${PROJECT_BINARY_DIR}/test_downloads")

file(MAKE_DIRECTORY "${DOWNLOAD_DIR}")
set(GREATEST_FILE "${DOWNLOAD_DIR}/greatest.h")
set(GREATEST_URL "https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h")
file(DOWNLOAD "${GREATEST_URL}" "${GREATEST_FILE}"
        EXPECTED_HASH "SHA256=${GREATEST_SHA256}")

#===========#
# Configure #
#===========#

if (NOT DEFINED TARGET_ARCH)
    set(TARGET_ARCH ${CMAKE_HOST_SYSTEM_PROCESSOR})
endif (NOT DEFINED TARGET_ARCH)

configure_file("configure.h.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_configure.h" @ONLY)

#=============#
# Test target #
#=============#

set(Header_Files "test_lib.h" "test_az_span.h")
source_group("${EXEC_NAME} Header Files" FILES "${Header_Files}")

set(Source_Files "test.c" "test_az_span.c")
source_group("${EXEC_NAME} Source Files" FILES "${Source_Files}")

add_executable("${EXEC_NAME}" "${Header_Files}" "${Source_Files}")

include(GNUInstallDirs)
target_include_directories(
        "${EXEC_NAME}"
        PRIVATE
        "$<BUILD_INTERFACE:${DOWNLOAD_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

if (DEFINED VCPKG_TOOLCHAIN AND VCPKG_TOOLCHAIN)
    find_package(cmocka CONFIG REQUIRED)
    set(cmocka_lib "cmocka::cmocka")
else ()
    include("${CMAKE_SOURCE_DIR}/cmake/modules/FetchCMocka.cmake")
    include(FetchContent)
    FetchContent_Declare(
            cmocka
            GIT_REPOSITORY https://git.cryptomilk.org/projects/cmocka.git
            GIT_TAG        cmocka-1.1.5
            GIT_SHALLOW    1
    )

    set(WITH_STATIC_LIB ON CACHE BOOL "CMocka: Build with a static library" FORCE)
    set(WITH_CMOCKERY_SUPPORT OFF CACHE BOOL "CMocka: Install a cmockery header" FORCE)
    set(WITH_EXAMPLES OFF CACHE BOOL "CMocka: Build examples" FORCE)
    set(UNIT_TESTING OFF CACHE BOOL "CMocka: Build with unit testing" FORCE)
    set(PICKY_DEVELOPER OFF CACHE BOOL "CMocka: Build with picky developer flags" FORCE)

    # Download cmocka, and execute its cmakelists.txt
    FetchContent_MakeAvailable(cmocka)

    set(cmocka_lib "cmocka-static")
endif ()

target_link_libraries(
        "${EXEC_NAME}"
        PRIVATE
        "${PROJECT_NAME}_compiler_flags"
        "${PROJECT_NAME}"
        "${cmocka_lib}"
)
# if (MSVC)
#     target_compile_definitions("${EXEC_NAME}" PRIVATE _CRT_SECURE_NO_WARNINGS)
# endif (MSVC)

#=======#
# CTest #
#=======#

add_test(NAME "${EXEC_NAME}"
         COMMAND "${EXEC_NAME}"
         WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
