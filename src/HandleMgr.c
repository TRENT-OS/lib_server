/*
 * Copyright (C) 2020, Hensoldt Cyber GmbH
 */

#include "lib_server/HandleMgr.h"
#include <string.h>

#define HANDLE_NOT_FOUND ((size_t) -1)

// Private functions -----------------------------------------------------------

static size_t
find(
    PointerVector* v,
    const void*    h)
{
    size_t sz = PointerVector_getSize(v);

    for (size_t i = 0; i < sz; i++)
    {
        if (h == PointerVector_getElementAt(v, i))
        {
            return i;
        }
    }

    return HANDLE_NOT_FOUND;
}

// Public functions ------------------------------------------------------------

OS_Error_t
HandleMgr_init(HandleMgr_t* self,
               void* buffer,
               size_t bufSize,
               size_t* capacityNumHandles)
{
    if (NULL == self || NULL == buffer || 0 == bufSize)
    {
        return OS_ERROR_INVALID_PARAMETER;
    }

    size_t myCapacityNumHandles = bufSize / sizeof(HandleMgr_Handle_t);
    if (capacityNumHandles != NULL &&
        *capacityNumHandles > myCapacityNumHandles)
    {
        return OS_ERROR_INSUFFICIENT_SPACE;
    }

    if (!PointerVector_ctorStatic(&self->vector, buffer, myCapacityNumHandles))
    {
        return OS_ERROR_ABORTED;
    }

    if (capacityNumHandles != NULL)
    {
        *capacityNumHandles = myCapacityNumHandles;
    }

    return OS_SUCCESS;
}

OS_Error_t
HandleMgr_free(
    HandleMgr_t* self)
{
    if (NULL == self)
    {
        return OS_ERROR_INVALID_PARAMETER;
    }

    PointerVector_dtor(&self->vector);

    return OS_SUCCESS;
}

OS_Error_t
HandleMgr_add(
    HandleMgr_t* self,
    HandleMgr_Handle_t handle)
{
    if (NULL == self || NULL == handle)
    {
        return OS_ERROR_INVALID_PARAMETER;
    }

    if (find(&self->vector, handle) != HANDLE_NOT_FOUND)
    {
        return OS_ERROR_OPERATION_DENIED;
    }

    return !PointerVector_pushBack(&self->vector, handle) ?
           OS_ERROR_INSUFFICIENT_SPACE : OS_SUCCESS;
}

OS_Error_t
HandleMgr_remove(
    HandleMgr_t* self,
    HandleMgr_Handle_t handle)
{
    size_t idx;

    if (NULL == self || NULL == handle)
    {
        return OS_ERROR_INVALID_PARAMETER;
    }

    if ((idx = find(&self->vector, handle)) ==  HANDLE_NOT_FOUND)
    {
        return OS_ERROR_INVALID_HANDLE;
    }

    PointerVector_replaceElementAt(&self->vector,
                                   idx,
                                   PointerVector_getBack(&self->vector));
    PointerVector_popBack(&self->vector);

    return OS_SUCCESS;
}

HandleMgr_Handle_t
HandleMgr_validate(
    HandleMgr_t* self,
    HandleMgr_Handle_t handle)
{
    // Let NULL pointers simply pass through
    if (NULL == self || NULL == handle)
    {
        return NULL;
    }

    return find(&self->vector, handle) != HANDLE_NOT_FOUND ? handle : NULL;
}
