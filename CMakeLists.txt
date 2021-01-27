#
# The Server Component Utilities
#
# Copyright (C) 2020, HENSOLDT Cyber GmbH
#

cmake_minimum_required(VERSION 3.17)

#-------------------------------------------------------------------------------
project(lib_server C)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME}
    INTERFACE
        "src/ContextMgr.c"
        "src/HandleMgr.c"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE
        "include"
)

if (DEBUG_CONFIG_H_FILE)

    target_compile_definitions(${PROJECT_NAME}
        INTERFACE
            "DEBUG_CONFIG_H_FILE=${DEBUG_CONFIG_H_FILE}"
    )

endif()
