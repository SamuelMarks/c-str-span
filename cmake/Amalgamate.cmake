# amalgamate.cmake

set(C_STR_SPAN_DIR "${SRC_DIR}")
set(OUT_FILE "${OUT_FILE}")

set(PROCESSED_FILES "")

# Recursive function to resolve local includes
function(resolve_includes FILE_PATH OUTPUT_VAR)
    get_filename_component(FILE_NAME "${FILE_PATH}" NAME)
    list(FIND PROCESSED_FILES "${FILE_NAME}" _idx)
    if (_idx GREATER -1)
        set(${OUTPUT_VAR} "/* already included: ${FILE_NAME} */\n" PARENT_SCOPE)
        return()
    endif()
    
    list(APPEND PROCESSED_FILES "${FILE_NAME}")
    set(PROCESSED_FILES "${PROCESSED_FILES}" PARENT_SCOPE)

    file(READ "${FILE_PATH}" file_content)
    
    string(REGEX MATCHALL "#include[ \t]+\"c_str_[^\"]+\"" matches "${file_content}")
    # We must deduplicate matches to avoid string(REPLACE) messing up if we replace a duplicate later
    if (matches)
        list(REMOVE_DUPLICATES matches)
    endif()

    foreach(match IN LISTS matches)
        # Extract the filename
        string(REGEX MATCH "\"c_str_[^\"]+\"" file_name_match "${match}")
        string(REPLACE "\"" "" inc_file "${file_name_match}")
        
        if(EXISTS "${C_STR_SPAN_DIR}/${inc_file}")
            resolve_includes("${C_STR_SPAN_DIR}/${inc_file}" sub_content)
            string(REPLACE "${match}" "${sub_content}" file_content "${file_content}")
        else()
            string(REPLACE "${match}" "/* ${match} (stripped) */" file_content "${file_content}")
        endif()
    endforeach()
    
    set(${OUTPUT_VAR} "/* ========================================================================= */\n/* Begin: ${FILE_NAME} */\n/* ========================================================================= */\n${file_content}\n" PARENT_SCOPE)
endfunction()

# -------------------------------------------------------------
# Gather Headers
# -------------------------------------------------------------
# We start with ALL headers except stdint/stdbool (which get resolved where they are included)
set(TOP_HEADERS
    "c_str_span_types.h"
    "c_str_result.h"
    "c_str_result_internal.h"
    "c_str_precondition.h"
    "c_str_precondition_internal.h"
    "c_str_hex_private.h"
    "c_str_span.h"
    "c_str_span_internal.h"
    "c_str_span_private.h"
    "c_str_span_printf.h"
)

set(ALL_HEADERS_CONTENT "")
foreach(hdr IN LISTS TOP_HEADERS)
    resolve_includes("${C_STR_SPAN_DIR}/${hdr}" resolved)
    string(APPEND ALL_HEADERS_CONTENT "${resolved}")
endforeach()

# -------------------------------------------------------------
# Gather Sources
# -------------------------------------------------------------
set(SOURCES
    "c_str_span.c"
    "c_str_span_precondition.c"
    "c_str_span_printf.c"
)

set(ALL_SOURCES_CONTENT "")
foreach(src IN LISTS SOURCES)
    file(READ "${C_STR_SPAN_DIR}/${src}" file_content)
    
    string(REGEX MATCHALL "#include[ \t]+\"c_str_[^\"]+\"" matches "${file_content}")
    if (matches)
        list(REMOVE_DUPLICATES matches)
    endif()
    foreach(match IN LISTS matches)
        string(REPLACE "${match}" "/* ${match} (amalgamated) */" file_content "${file_content}")
    endforeach()
    
    string(APPEND ALL_SOURCES_CONTENT "/* ========================================================================= */\n/* Begin: ${src} */\n/* ========================================================================= */\n${file_content}\n")
endforeach()

# -------------------------------------------------------------
# Write Output
# -------------------------------------------------------------
file(WRITE "${OUT_FILE}" "/*
 * c-str-span Amalgamation (STB Style)
 * 
 * To use this library, do this in *one* C or C++ file:
 *   #define C_STR_SPAN_IMPLEMENTATION
 *   #include \"c_str_span_amalgamation.h\"
 */

#ifndef C_STR_SPAN_AMALGAMATION_H
#define C_STR_SPAN_AMALGAMATION_H

#ifndef C_STR_SPAN_EXPORT
#define C_STR_SPAN_EXPORT
#endif

${ALL_HEADERS_CONTENT}

#endif /* C_STR_SPAN_AMALGAMATION_H */

#ifdef C_STR_SPAN_IMPLEMENTATION

${ALL_SOURCES_CONTENT}

#endif /* C_STR_SPAN_IMPLEMENTATION */
")
