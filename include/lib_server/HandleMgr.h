/*
 * Copyright (C) 2020, Hensoldt Cyber GmbH
 */

/**
 * @file
 * @brief The HandleMgr manages lists of handles
 */

#pragma once

#include "OS_Error.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef void* const HandleMgr_Handle_t;
typedef struct HandleMgr HandleMgr_t;

#define HandleMgr_GET_SIZE_BY_CAPACITY(capacity)\
    PointerVector_GET_SIZE_BY_CAPACITY(capacity)

/**
 * @brief Initialize a handle manager instance
 *
 * Initialize a handle manager instance with a set of handle manager lists,
 * to which handles can be added/removed.
 *
 * @param mgr (required) pointer to handle manager
 * @param num (required) number of different handle types
 *
 * @return an error code
 * @retval OS_SUCCESS if operation succeeded
 * @retval OS_ERROR_INVALID_PARAMETER if a parameter was missing or invalid
 * @retval OS_ERROR_INSUFFICIENT_SPACE if an allocation failed
 */
OS_Error_t
HandleMgr_init(
    HandleMgr_t** mgr,
    const size_t  num);

/**
 * @brief Free a handle manager instance
 *
 * @param mgr (required) handle manager
 *
 * @return an error code
 * @retval OS_SUCCESS if operation succeeded
 * @retval OS_ERROR_INVALID_PARAMETER if a parameter was missing or invalid
 */
OS_Error_t
HandleMgr_free(
    HandleMgr_t* mgr);

/**
 * @brief Add handle to manager
 *
 * @param mgr (required) handle manager
 * @param id  (required) id of handle
 * @param handle (required) handle
 *
 * @return an error code
 * @retval OS_SUCCESS if operation succeeded
 * @retval OS_ERROR_INVALID_PARAMETER if a parameter was missing or invalid
 * @retval OS_ERROR_INSUFFICIENT_SPACE if an allocation failed
 */
OS_Error_t
HandleMgr_add(
    HandleMgr_t* const mgr,
    const size_t       id,
    HandleMgr_Handle_t handle);

/**
 * @brief (Conditionally) Add handle to manager
 *
 * This function behaves as the one above, it does perform the add only if the
 * input parameter \p ret is OS_SUCCESS. If it is not, the function simply
 * returns the value of \p ret. This allows to "stack" the HandleMgr with other
 * functions (e.g., init() of some other object).
 *
 * In contrast to the normal add(), we need to pass the address to the handle
 * here, not the value itself. This is because there is no defined order of
 * evaluating arguments in C, so dereferencing the handle/pointer in the
 * arglist of the calling functions would not necessarily return the assigned
 * handle value, see the example below:
 *
 * \code{.c}
 * OS_CryptoKey_Handle_t keyHandle;
 * err = HandleMgr_addOnSuccess(
 *          handleMgr,
 *          HND_KEY,
 *          OS_CryptoKey_import(
 *              &keyHandle,
 *              hCrypto,
 *              &data),
 *          (HandleMgr_Handle_t*) &keyHandle);
 * \endcode
 *
 * @param mgr (required) handle manager
 * @param id  (required) id of handle
 * @param ret (required) error code of outer function
 * @param handle (required) pointer to handle
 *
 * @return an error code
 * @retval OS_SUCCESS if operation succeeded
 * @retval OS_ERROR_INVALID_PARAMETER if a parameter was missing or invalid
 * @retval OS_ERROR_INSUFFICIENT_SPACE if an allocation failed
 */
__attribute__((unused))
static OS_Error_t
HandleMgr_addOnSuccess(
    HandleMgr_t* const  mgr,
    const size_t        id,
    const OS_Error_t    ret,
    HandleMgr_Handle_t* handle)
{
    return (NULL == handle)    ? OS_ERROR_INVALID_PARAMETER :
           (OS_SUCCESS == ret) ? HandleMgr_add(mgr, id, *handle) : ret;
}

/**
 * @brief Remove handle from manager
 *
 * @param mgr (required) handle manager
 * @param id  (required) id of handle
 * @param handle (required) handle
 *
 * @return an error code
 * @retval OS_SUCCESS if operation succeeded
 * @retval OS_ERROR_INVALID_PARAMETER if a parameter was missing or invalid
 */
OS_Error_t
HandleMgr_remove(
    HandleMgr_t* const mgr,
    const size_t       id,
    HandleMgr_Handle_t handle);

/**
 * @brief (Conditionally) Remove handle from manager
 *
 * This function behaves as the one above, it does perform the remove only if
 * the input parameter \p ret is OS_SUCCESS. If it is not, the function simply
 * returns the value of \p ret. This allows to "stack" the HandleMgr with other
 * functions (e.g., free() of some object inidcated by the handle).
 *
 * @param mgr (required) handle manager
 * @param id  (required) id of handle
 * @param ret (required) error code of outer function
 * @param handle (required) handle
 *
 * @return an error code
 * @retval OS_SUCCESS if operation succeeded
 * @retval OS_ERROR_INVALID_PARAMETER if a parameter was missing or invalid
 * @retval \p ret in case if \p ret != OS_SUCCESS
 */
__attribute__((unused))
static OS_Error_t
HandleMgr_removeOnSuccess(
    HandleMgr_t* const mgr,
    const size_t       id,
    const OS_Error_t   ret,
    HandleMgr_Handle_t handle)
{
    return ret != OS_SUCCESS ? ret : HandleMgr_remove(mgr, id, handle);
}

/**
 * @brief Validate handle
 *
 * This function looks up a handle in the list indicated by the respective
 * \p id. If the handle is in that list, the handle is simply returned. If
 * the handle is NOT in the list, this function returns NULL. This behavior
 * allows to stack this function with other functionality.
 *
 * @param mgr (required) handle manager
 * @param id  (required) id of handle
 * @param handle (required) handle
 *
 * @return return \p handle if handle is known, NULL otherwise
 */
HandleMgr_Handle_t
HandleMgr_validate(
    HandleMgr_t* const mgr,
    const size_t       id,
    HandleMgr_Handle_t handle);
