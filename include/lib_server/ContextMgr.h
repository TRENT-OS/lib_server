/*
 * Copyright (C) 2020, HENSOLDT Cyber GmbH
 */

/**
 * @file
 * @brief The ContextMgr manages client contexts of RPC server components
 */

#pragma once

#include "OS_Error.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Forward declaration
typedef struct ContextMgr_ClientSlot ContextMgr_ClientSlot_t;

/**
 * Type of a seL4 client ID
 */
typedef uint32_t ContextMgr_CID_t;

/**
 * These functions will be called by the ContextMgr to alloc/free a client
 * context. Note that client contexts will be allocated the first time a
 * get() is performed, and not before.
 */
typedef struct
{
    OS_Error_t (*init)(
        const ContextMgr_CID_t cid, void** mem);
    OS_Error_t (*free)(
        const ContextMgr_CID_t cid, void* mem);
} ContextMgr_MemoryFuncs_t;

/**
 * Context of context manager, needs to be allocated by user of ContextMgr.
 */
typedef struct
{
    size_t max;
    ContextMgr_ClientSlot_t* slots;
    ContextMgr_MemoryFuncs_t memFns;
} ContextMgr_t;

/**
 * @brief Initialize a context manager instance
 *
 * Initialize a context manager instance with memory alloc/free callbacks and the
 * max number of expected contexts. Will internally alloc as many slots for
 * contexts as indicated by \p max.
 *
 * @return an error code
 * @retval OS_SUCCESS if operation succeeded
 * @retval OS_ERROR_INVALID_PARAMETER if a parameter was missing or invalid
 * @retval OS_ERROR_INSUFFICIENT_SPACE if the required amount of handles results
 *  in a greater need of memory (than the one passed).
 */
OS_Error_t
ContextMgr_init(
    ContextMgr_t*                   self,   /**< [in]   pointer to context
                                                        manager */
    const ContextMgr_MemoryFuncs_t* memFns, /**< [in]   client context alloc/free
                                                        callbacks */
    const size_t                    max     /**< [in]   maximum amount of contexts
                                                        expected */
);

/**
 * @brief Free a context manager instance
 *
 * Free context manager memory; will call the free() callback on all already
 * allocated client memories.
 *
 * In a typical scenario (e.g., in a component) this will probably never be
 * called, but it is still good practice to have it.
 *
 * @return an error code
 * @retval OS_SUCCESS if operation succeeded
 * @retval OS_ERROR_INVALID_PARAMETER if a parameter was missing or invalid
 */
OS_Error_t
ContextMgr_free(
    ContextMgr_t* self  /**< [in] pointer to context manager */
);

/**
 * @brief Get a client context based on its ID
 *
 * This function obtains a pointer to a client memory context, based a unique
 * client ID (CID) parameter.
 *
 * If there is currently no memory allocated for that CID, the context manager
 * will use the init() callback to allocate a client context; then the newly
 * allocated memory will be returned.
 *
 * @return an error code
 * @retval OS_SUCCESS if operation succeeded
 * @retval OS_ERROR_INVALID_PARAMETER if a parameter was missing or invalid
 * @retval OS_ERROR_INSUFFICIENT_SPACE if there is no slot assigned to the
 *  \p CID but there are no more free slots left.
 */
OS_Error_t
ContextMgr_get(
    ContextMgr_t*          self,    /**< [in]   pointer to context manager */
    const ContextMgr_CID_t cid,     /**< [in]   unique client ID to use for
                                                lookup */
    void**                 ctx      /**< [out]  pointer which will be set to client
                                                context mem */
);
