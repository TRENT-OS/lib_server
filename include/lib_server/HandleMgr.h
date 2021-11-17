/*
 * Copyright (C) 2020, HENSOLDT Cyber GmbH
 */

/**
 * @file
 * @brief The HandleMgr manages lists of handles
 */

#pragma once

#include "OS_Error.h"
#include "lib_utils/PointerVector.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef void* HandleMgr_Handle_t;

typedef struct HandleMgr
{
    PointerVector vector;
}
HandleMgr_t;

#define HandleMgr_SIZE_OF_BUFFER(numItems)\
    PointerVector_SIZE_OF_BUFFER(numItems)

/**
 * @brief Initialize a handle manager instance
 *
 * Initialize a handle manager instance with a set of handle manager lists,
 * to which handles can be added/removed.
 *
 * @param self (required) pointer to handle manager
 * @param buffer (required) memory buffer to store handles
 * @param bufSize (required) memory buffer to store handles
 * @param capacityNumHandles capacity in number of elements. This is an
 * input/output parameter. If NULL then it is simply ignored, otherwise the
 * init will check the required number of elements against the size of the
 * memory buffer. If the memory is not sufficient then
 *  OS_ERROR_INSUFFICIENT_SPACE will be returned otherwise the maximum capacity
 * of the container beneath will be returned.
 *
 * @return an error code
 * @retval OS_SUCCESS if operation succeeded
 * @retval OS_ERROR_INVALID_PARAMETER if a parameter was missing or invalid
 * @retval OS_ERROR_INSUFFICIENT_SPACE if the required amount of handles results
 * in a greater need of memory (than the one passed).
 */
OS_Error_t
HandleMgr_init(
    HandleMgr_t* self,
    void* buffer,
    size_t bufSize,
    size_t* capacityNumHandles);

/**
 * @brief Free a handle manager instance
 *
 * @param self (required) handle manager
 *
 * @return an error code
 * @retval OS_SUCCESS if operation succeeded
 * @retval OS_ERROR_INVALID_PARAMETER if a parameter was missing or invalid
 */
OS_Error_t
HandleMgr_free(
    HandleMgr_t* self);

/**
 * @brief Add handle to manager
 *
 * @param self (required) handle manager
 * @param handle (required) handle
 *
 * @return an error code
 * @retval OS_SUCCESS if operation succeeded
 * @retval OS_ERROR_OPERATION_DENIED handle is duplicated
 * @retval OS_ERROR_INVALID_PARAMETER if a parameter was missing or invalid
 * @retval OS_ERROR_INSUFFICIENT_SPACE if an allocation failed
 */
OS_Error_t
HandleMgr_add(
    HandleMgr_t* self,
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
 *          OS_CryptoKey_import(
 *              &keyHandle,
 *              hCrypto,
 *              &data),
 *          (HandleMgr_Handle_t*) &keyHandle);
 * \endcode
 *
 * @param self (required) handle manager
 * @param ret (required) error code of outer function
 * @param handle (required) pointer to handle
 *
 * @return an error code
 * @retval OS_SUCCESS if operation succeeded
 * @retval OS_ERROR_OPERATION_DENIED handle is duplicated
 * @retval OS_ERROR_INVALID_PARAMETER if a parameter was missing or invalid
 * @retval OS_ERROR_INSUFFICIENT_SPACE if an allocation failed
 */
__attribute__((unused))
static OS_Error_t
HandleMgr_addOnSuccess(
    HandleMgr_t*        self,
    const OS_Error_t    ret,
    HandleMgr_Handle_t* handle)
{
    return (NULL == handle)    ? OS_ERROR_INVALID_PARAMETER :
           (OS_SUCCESS == ret) ? HandleMgr_add(self, *handle) : ret;
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
    HandleMgr_t*        self,
    HandleMgr_Handle_t  handle);

/**
 * @brief (Conditionally) Remove handle from manager
 *
 * This function behaves as the one above, it does perform the remove only if
 * the input parameter \p ret is OS_SUCCESS. If it is not, the function simply
 * returns the value of \p ret. This allows to "stack" the HandleMgr with other
 * functions (e.g., free() of some object inidcated by the handle).
 *
 * @param self (required) handle manager
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
    HandleMgr_t*        self,
    const OS_Error_t    ret,
    HandleMgr_Handle_t  handle)
{
    return ret != OS_SUCCESS ? ret : HandleMgr_remove(self, handle);
}

/**
 * @brief Validate handle
 *
 * This function looks up a handle in the list indicated by the respective
 * \p id. If the handle is in that list, the handle is simply returned. If
 * the handle is NOT in the list, this function returns NULL. This behavior
 * allows to stack this function with other functionality.
 *
 * @param self (required) handle manager
 * @param id  (required) id of handle
 * @param handle (required) handle
 *
 * @return return \p handle if handle is known, NULL otherwise
 */
HandleMgr_Handle_t
HandleMgr_validate(
    HandleMgr_t* self,
    HandleMgr_Handle_t handle);
