cmake_minimum_required(VERSION 3.20)

set(MINIAUDIO_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/externals)

project(MiniVoice VERSION 1.0)

if(MSVC)
    add_compile_definitions(
            $<$<BOOL:${BUILD_SHARED_LIBS}>:${PROJECT_NAME}_EXPORTS>
            $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${PROJECT_NAME}_STATIC>
    )
endif()

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)

MACRO(header_directories return_list includes_base_folder extention)
    FILE(GLOB_RECURSE new_list ${includes_base_folder}/*.${extention})
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

header_directories(INCLUDES ${PROJECT_SOURCE_DIR}/include/ hpp)

message("src files:")
foreach(file ${SRCS})
    message(STATUS ${file})
endforeach()

message("include directories:")
foreach(dir ${INCLUDES})
    message(STATUS ${dir})
endforeach()

if(NOT EXISTS ${MINIAUDIO_INCLUDE_DIRS})
    find_path(MINIAUDIO_INCLUDE_DIRS "miniaudio.h")
endif()

add_library(${PROJECT_NAME} SHARED ${SRCS})

set_target_properties(${PROJECT_NAME} PROPERTIES
        DEBUG_POSTFIX "d"
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/lib"
)

target_include_directories(${PROJECT_NAME} PRIVATE ${MINIAUDIO_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "MINIVOICE_EXPORTS")
endif()