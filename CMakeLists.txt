#
# Server Library
#
# Copyright (C) 2020-2024, HENSOLDT Cyber GmbH
# 
# SPDX-License-Identifier: GPL-2.0-or-later
#
# For commercial licensing, contact: info.cyber@hensoldt.net
#

cmake_minimum_required(VERSION 3.17)

#-------------------------------------------------------------------------------
project(lib_server C)

#-------------------------------------------------------------------------------
# LIBRARY
#-------------------------------------------------------------------------------
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

target_link_libraries(${PROJECT_NAME}
    INTERFACE
        os_core_api
        lib_debug
        lib_macros
        lib_utils
)

if (DEBUG_CONFIG_H_FILE)
    target_compile_definitions(${PROJECT_NAME}
        INTERFACE
            "DEBUG_CONFIG_H_FILE=${DEBUG_CONFIG_H_FILE}"
    )
endif ()

#-------------------------------------------------------------------------------
# TESTING
#-------------------------------------------------------------------------------
if (BUILD_TESTING)
    add_subdirectory(test)
endif ()
