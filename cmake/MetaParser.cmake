#use for cmake function argument parser
include(CMakeParseArguments)

function(meta_parser_prebuild)
    set(ONE_VALUE_ARGS
        TARGET #target name
        GENERATED_DIR #generated directory
        SOURCE_ROOT #source root
        MODULE_HEADER #module header
        OUT_MODULE_SOURCE #out reference variable, out module source
        OUT_GENERATED_FILES #out reference variable, out generated files
        OUT_INC #out reference variable
        OUT_SRC #out reference variable
    )

    set(MULTI_VALUE_ARGS
        HEADER_FILES
    )

    #parser function argument, and then add prefiex PREBUILD_META to argument values
    cmake_parse_arguments(PREBUILD_META "" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    #source file for this reflection module
    set(MODULE_SOURCE "${PREBUILD_META_GENERATED_DIR}/Module.${PREBUILD_META_TARGET}.Generated.cpp")

    #output the source file
    #set the reflection module source variable to parent scope
    set(${PREBUILD_META_OUT_MODULE_SOURCE} ${MODULE_SOURCE} PARENT_SCOPE)

    set(GENERATED_FILES "${MODULE_SOURCE}")
    set(GENERATED_HEADERS "")
    set(GENERATED_SOURCES "${MODULE_SOURCE}")

    #exclude the module header from the included headers
    list(REMOVE_ITEM PREBUILD_META_HEADER_FILES "${PREBUILD_META_SOURCE_ROOT}/${PREBUILD_META_MODULE_HEADER}")

    set(${PREBUILD_META_OUT_SRC} ${${PREBUILD_META_OUT_SRC}} ${MODULE_SOURCE} PARENT_SCOPE)
       
    #iterator over the header files
    foreach(HEADER ${PREBUILD_META_HEADER_FILES})
        #message(${HEADER})
        get_filename_component(DIRECTORY_NAME ${HEADER} DIRECTORY)
        get_filename_component(BASE_NAME ${HEADER} NAME_WE)
        get_filename_component(EXTENSION ${HEADER} EXT)

        #make sure the path is absolute
        if(NOT IS_ABSOLUTE ${HEADER})
            set(DIRECTORY_NAME "${PREBUILD_META_SOURCE_ROOT}/${DIRECTORY_NAME}")
        endif()

        #skip hpp files, etc for template's implement file
        if(NOT "${EXTENSION}" STREQUAL ".hpp")
            # get the relative dir, then generate the reflection header
            file(RELATIVE_PATH RELATIVE
                ${PREBUILD_META_SOURCE_ROOT}
                "${DIRECTORY_NAME}/${BASE_NAME}.Generated"
            )

            #message("source root:")
            #message("${PREBUILD_META_SOURCE_ROOT}")
            #
            #message("generated directory name:")
            #message("${DIRECTORY_NAME}/${BASE_NAME}.Generated")

            set(GENERATED_HEADER "${PREBUILD_META_GENERATED_DIR}/${RELATIVE}.h")
            set(GENERATED_SOURCE "${PREBUILD_META_GENERATED_DIR}/${RELATIVE}.cpp")

            #add the variable
            list(APPEND GENERATED_FILES ${GENERATED_HEADER} ${GENERATED_SOURCE})
            list(APPEND GENERATED_HEADERS ${GENERATED_HEADER})
            list(APPEND GENERATED_SOURCES ${GENERATED_SOURCE})
        endif()
    endforeach()

    #filter
    source_group(".Generated" FILES ${GENERATED_FILES})

    set(${PREBUILD_META_OUT_GENERATED_FILES} "${GENERATED_FILES}" PARENT_SCOPE)
    set(${PREBUILD_META_OUT_INC} ${${PREBUILD_META_OUT_INC}} ${GENERATED_HEADERS} PARENT_SCOPE)
    set(${PREBUILD_META_OUT_SRC} ${${PREBUILD_META_OUT_SRC}} ${GENERATED_SOURCES} PARENT_SCOPE)
endfunction()

function(meta_parser_build)
    set(ONE_VALUE_ARGS
        TARGET # reflection target
        SOURCE_ROOT # original source root
        SOURCE_FILE # contains all need parsed header
        MODULE_HEADER # module header
        MODULE_SOURCE_FILE # module source 
        GENERATED_DIR # generated directory
        PCH_NAME # pch name
        PARSER_EXECUTABLE) # parser executable

    set(MULTI_VALUE_ARGS
        DEFINES
        INCLUDES # -I Parameter
        GENERATED_FILES # generated files
        HEADER_FILES) # depend source header

    cmake_parse_arguments(BUILD_META "" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    # get the property value to DIRECTORIES variable
    get_property(DIRECTORIES TARGET ${BUILD_META_TARGET} PROPERTY INCLUDE_DIRECTORIES)

    set(RAW_INCLUDES ${DIRECTORIES} ${BUILD_META_INCLUDES})

    set(INCLUDES "")

    #foreach(item ${DIRECTORIES})
    #    message(${item})
    #endforeach()

    #foreach(item ${BUILD_META_INCLUDES})
    #    message(${item})
    #endforeach()

    # connect current to last string
    foreach(INC ${RAW_INCLUDES})
        set(INCLUDES "${INCLUDES}${INC}\n")
    endforeach()

    set(INCLUDES_FILE "${BUILD_META_GENERATED_DIR}/Module.${BUILD_META_TARGET}.Includes.txt")

    message("clang -I Parameter")
    foreach(item ${INCLUDES})
        message(${item})
    endforeach()
    message("clang -I Parameter")

    # write the ${INCLUDES} content to ${INCLUDES_FILE}
    file(WRITE ${INCLUDES_FILE} ${INCLUDES})

    set(DEFINES ${BUILD_META_DEFINES})

    # replace the space string to empty string
    string(REPLACE " " "" DEFINES_TRIMMED "${DEFINES}")

    if("${DEFINES_TRIMMED}" STREQUAL "")
        set(DEFINES_SWITCH )
    else()
        set(DEFINES_SWITCH --defines "${DEFINES}")
    endif()

    # pch
    # pch source include pch header
    if(NOT "${BUILD_META_PCH_NAME}" STREQUAL "")
        set(EMPTY_SOURCE_CONTENTS "#include \"${BUILD_META_PCH_NAME}.h\"")
        set(PCH_SWITCH \"${BUILD_META_PCH_NAME}.h\")
    else()
        set(EMPTY_SOURCE_CONTENTS "")
        set(PCH_SWITCH )
    endif()

    list(REMOVE_ITEM BUILD_META_GENERATED_FILES "${BUILD_META_SOURCE_ROOT}/${BUILD_META_MODULE_HEADER}")

    foreach(GENERATED_FILE ${BUILD_META_GENERATED_FILES})
        get_filename_component(EXTENSION ${GENERATED_FILE} EXT)

        # we have to create the files, as they might not be written to
        if(NOT EXISTS ${GENERATED_FILE})
            if("${EXTENSION}" STREQUAL ".Generated.h")
                file(WRITE ${GENERATED_FILE} "")
            else()
                file(WRITE ${GENERATED_FILE} ${EMPTY_SOURCE_CONTENTS})
            endif()
        endif()
    endforeach()

    # message(${CMAKE_SOURCE_DIR}/${PCH_SWITCH})

    message(${CMAKE_SOURCE_DIR}/${BUILD_META_PCH_NAME}.h)

    # message(${BUILD_META_HEADER_FILES})

    # add the command that generates the header and source files
    add_custom_command(
        OUTPUT ${BUILD_META_GENERATED_FILES} # output these generated files
        DEPENDS ${BUILD_META_HEADER_FILES} # depend the need parsed header files
        COMMAND ${CMAKE_COMMAND} -E echo "********************"
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/${BUILD_META_PCH_NAME}.h ${BUILD_META_GENERATED_DIR} # copy pch
        COMMAND call ${BUILD_META_PARSER_EXECUTABLE} # parser executable
        "${BUILD_META_TARGET}" # target name
        "${BUILD_META_SOURCE_ROOT}" # source root
        "${BUILD_META_SOURCE_ROOT}/${BUILD_META_SOURCE_FILE}" # one header file contains all need parsed header file
        "${BUILD_META_SOURCE_ROOT}/${BUILD_META_MODULE_HEADER}" # module header
        "${BUILD_META_MODULE_SOURCE_FILE}" # module source file
        "${BUILD_META_GENERATED_DIR}" # generated directory
        "${INCLUDES_FILE}" # have some -I parameter, include files, etc system include file
        "${PCH_SWITCH}" # pch 
        COMMAND ${CMAKE_COMMAND} -E echo "********************"
    )   
endfunction()