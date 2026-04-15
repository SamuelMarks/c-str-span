get_filename_component(EXEC_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(EXEC_NAME "${PROJECT_NAME}_${EXEC_NAME}")

#=======================#
# Dependencies download #
#=======================#

include(FetchContent)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/patch_greatest.cmake" "file(READ CMakeLists.txt content)\nstring(REPLACE \"CMAKE_SOURCE_DIR\" \"CMAKE_CURRENT_SOURCE_DIR\" content \"\${content}\")\nfile(WRITE CMakeLists.txt \"\${content}\")")

FetchContent_Declare(
        greatest
        GIT_REPOSITORY https://github.com/SamuelMarks/greatest.git
        GIT_TAG        master
        PATCH_COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/patch_greatest.cmake"
)
set(OLD_BUILD_TESTING ${BUILD_TESTING})
set(BUILD_TESTING OFF)
FetchContent_MakeAvailable(greatest)
set(BUILD_TESTING ${OLD_BUILD_TESTING})

if (NOT TARGET greatest)
    add_library(greatest INTERFACE)
    target_include_directories(greatest INTERFACE "${greatest_SOURCE_DIR}")
endif ()

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

set(Source_Files "test.c" "test_az_span.c" "test_coverage.c")
source_group("${EXEC_NAME} Source Files" FILES "${Source_Files}")

add_executable("${EXEC_NAME}" "${Header_Files}" "${Source_Files}")

target_include_directories(
        "${EXEC_NAME}"
        PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(
        "${EXEC_NAME}"
        PRIVATE
        "${PROJECT_NAME}_compiler_flags"
        "${PROJECT_NAME}"
        greatest
)

if (MSVC)
    target_compile_definitions("${EXEC_NAME}" PRIVATE _CRT_SECURE_NO_WARNINGS)
endif (MSVC)

#=======#
# CTest #
#=======#

add_test(NAME "${EXEC_NAME}"
         COMMAND "${EXEC_NAME}"
         WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
