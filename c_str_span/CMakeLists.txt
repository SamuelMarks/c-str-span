get_filename_component(LIBRARY_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

set(Header_Files
        "c_str_hex_private.h"
        "c_str_precondition.h"
        "c_str_precondition_internal.h"
        "c_str_result.h"
        "c_str_result_internal.h"
        "c_str_span.h"
        "c_str_span_internal.h"
        "c_str_span_printf.h"
        "c_str_span_private.h"
        "c_str_span_stdbool.h"
        "c_str_span_types.h"
)
if (MSVC_VERSION STRLESS "1600") # MSVC 2010
    list(APPEND Header_Files "c_str_span_stdint.h")
endif (MSVC_VERSION STRLESS "1600")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files
        "c_str_span.c"
        "c_str_span_precondition.c"
        "c_str_span_printf.c"
)
source_group("Source Files" FILES "${Source_Files}")

add_library("${LIBRARY_NAME}" "${Header_Files}" "${Source_Files}")

target_link_libraries("${LIBRARY_NAME}" PUBLIC "${PROJECT_NAME}_compiler_flags")

include(GenerateExportHeader)
set(_export_file "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_export.h")
generate_export_header("${LIBRARY_NAME}" EXPORT_FILE_NAME "${_export_file}")

include(GNUInstallDirs)
target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# if (MSVC)
#     target_compile_definitions("${LIBRARY_NAME}" PRIVATE _CRT_SECURE_NO_WARNINGS)
# endif (MSVC)

#=========#
# Install #
#=========#

# setup the version numbering
set_property(TARGET "${LIBRARY_NAME}" PROPERTY VERSION "${${PROJECT_NAME}_VERSION}")
set_property(TARGET "${LIBRARY_NAME}" PROPERTY SOVERSION "${${PROJECT_NAME}_VERSION_MAJOR}")

install(FILES ${Header_Files} "${_export_file}"
        TYPE "INCLUDE")


#====================#
# Package to install #
#====================#

set(installable_libs "${LIBRARY_NAME}" "${PROJECT_NAME}_compiler_flags")
if (TARGET "${DEPENDANT_LIBRARY}")
    list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif (TARGET "${DEPENDANT_LIBRARY}")
install(TARGETS ${installable_libs}
        EXPORT "${LIBRARY_NAME}Targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(EXPORT "${LIBRARY_NAME}Targets" DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")
