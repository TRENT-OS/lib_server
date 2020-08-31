/*
 * Copyright (C) 2020, Hensoldt Cyber GmbH
 */

#include "lib_server/HandleMgr.h"
#include "lib_util/PointerVector.h"

#include <string.h>

struct HandleMgr
{
    PointerVector* handles;
    size_t num;
};

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
HandleMgr_init(
    HandleMgr_t** mgr,
    const size_t  num)
{
    OS_Error_t err;
    HandleMgr_t* self;

    if (NULL == mgr || 0 == num)
    {
        return OS_ERROR_INVALID_PARAMETER;
    }

    if ((self = calloc(1, sizeof(HandleMgr_t))) == NULL)
    {
        return OS_ERROR_INSUFFICIENT_SPACE;
    }
    if ((self->handles = calloc(num, sizeof(PointerVector))) == NULL)
    {
        err = OS_ERROR_INSUFFICIENT_SPACE;
        goto err0;
    }

    for (size_t i = 0; i < num; i++)
    {
        if (!PointerVector_ctor(&self->handles[i], 1))
        {
            err = OS_ERROR_INSUFFICIENT_SPACE;
            goto err1;
        }
    }

    self->num = num;

    *mgr = self;

    return OS_SUCCESS;

err1:
    free(self->handles);
err0:
    free(self);

    return err;
}

OS_Error_t
HandleMgr_free(
    HandleMgr_t* mgr)
{
    if (NULL == mgr)
    {
        return OS_ERROR_INVALID_PARAMETER;
    }

    free(mgr->handles);
    free(mgr);

    return OS_SUCCESS;
}

OS_Error_t
HandleMgr_add(
    HandleMgr_t* const mgr,
    const size_t       id,
    HandleMgr_Handle_t handle)
{
    PointerVector* vec;

    if (NULL == mgr || id >= mgr->num || NULL == handle)
    {
        return OS_ERROR_INVALID_PARAMETER;
    }

    vec = &mgr->handles[id];

    return !PointerVector_pushBack(vec, handle) ?
           OS_ERROR_INSUFFICIENT_SPACE : OS_SUCCESS;
}

OS_Error_t
HandleMgr_remove(
    HandleMgr_t* const mgr,
    const size_t       id,
    HandleMgr_Handle_t handle)
{
    PointerVector* vec;
    size_t idx;

    if (NULL == mgr || id >= mgr->num || NULL == handle)
    {
        return OS_ERROR_INVALID_PARAMETER;
    }

    vec = &mgr->handles[id];

    if ((idx = find(vec, handle)) ==  HANDLE_NOT_FOUND)
    {
        return OS_ERROR_INVALID_HANDLE;
    }

    PointerVector_replaceElementAt(vec, idx, PointerVector_getBack(vec));
    PointerVector_popBack(vec);

    return OS_SUCCESS;
}

HandleMgr_Handle_t
HandleMgr_validate(
    HandleMgr_t* const mgr,
    const size_t       id,
    HandleMgr_Handle_t handle)
{
    if (NULL == mgr || id >= mgr->num)
    {
        return NULL;
    }
    // Let NULL pointers simply pass through
    if (NULL == handle)
    {
        return NULL;
    }

    return find(&mgr->handles[id], handle) != HANDLE_NOT_FOUND ? handle : NULL;
}
