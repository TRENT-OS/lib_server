/*
 * Copyright (C) 2020, HENSOLDT Cyber GmbH
 */

#include "lib_server/ContextMgr.h"
#include "lib_macros/Check.h"
#include "lib_util/PointerVector.h"

#include <string.h>

// These are pretty arbitrary, so we can provide between 1-1024 contexts; this
// can be changed without side effects (given that the memory is there..)
#define CONTEXTMGR_CONTEXTS_MIN 1
#define CONTEXTMGR_CONTEXTS_MAX 1024

// Client memory container
struct ContextMgr_ClientSlot
{
    bool inUse;
    ContextMgr_CID_t cid;
    void* mem;
};

// Public functions ------------------------------------------------------------

OS_Error_t
ContextMgr_init(
    ContextMgr_t*                   self,
    const ContextMgr_MemoryFuncs_t* memFns,
    const size_t                    max)
{
    CHECK_PTR_NOT_NULL(self);
    CHECK_PTR_NOT_NULL(memFns);
    CHECK_PTR_NOT_NULL(memFns->init);
    CHECK_PTR_NOT_NULL(memFns->free);
    CHECK_VALUE_IN_CLOSED_INTERVAL(max,
                                   CONTEXTMGR_CONTEXTS_MIN,
                                   CONTEXTMGR_CONTEXTS_MAX);

    self->memFns = *memFns;
    self->max    = max;

    // Allocate as many client slots as user requested
    if ((self->slots = calloc(max, sizeof(ContextMgr_ClientSlot_t))) == NULL)
    {
        Debug_LOG_ERROR("calloc() failed");
        return OS_ERROR_INSUFFICIENT_SPACE;
    }

    return OS_SUCCESS;
}

OS_Error_t
ContextMgr_free(
    ContextMgr_t* self)
{
    OS_Error_t err;
    ContextMgr_ClientSlot_t* slot;

    CHECK_PTR_NOT_NULL(self);

    // Go through client array and call respective free on those which were
    // allocated
    for (size_t i = 0; i < self->max; i++)
    {
        slot = &self->slots[i];
        if (slot->mem != NULL)
        {
            if ((err = self->memFns.free(slot->cid, slot->mem)) != OS_SUCCESS)
            {
                Debug_LOG_ERROR("free() callback failed on client (CID=%i) " \
                                "with %d, continuing", slot->cid, err);
            }
            slot->mem = NULL;
            slot->cid = 0;
            slot->inUse = false;
        }
    }

    free(self->slots);

    return OS_SUCCESS;
}

OS_Error_t
ContextMgr_get(
    ContextMgr_t*          self,
    const ContextMgr_CID_t cid,
    void**                 ctx)
{
#define INVALID_SLOT ((size_t) -1)
    OS_Error_t err;
    ContextMgr_ClientSlot_t* slot;
    size_t freeSlot;

    CHECK_PTR_NOT_NULL(self);
    CHECK_PTR_NOT_NULL(ctx);

    // Check if we already have a slot for this CID
    freeSlot = INVALID_SLOT;
    for (size_t i = 0; i < self->max; i++)
    {
        slot = &self->slots[i];
        if (slot->inUse)
        {
            if (slot->cid == cid)
            {
                Debug_ASSERT_PRINTFLN(slot->mem != NULL,
                                      "Client (CID=%i) context mem is NULL", cid);
                *ctx = slot->mem;
                return OS_SUCCESS;
            }
        }
        else
        {
            freeSlot = i;
        }
    }

    // CID was not assigned yet, so hopefully we have a free slot
    if (INVALID_SLOT != freeSlot)
    {
        slot = &self->slots[freeSlot];
        Debug_ASSERT_PRINTFLN(slot->mem == NULL,
                              "Client memory slot %Zd is unused, but not " \
                              "NULL", freeSlot);
        slot->cid = cid;
        slot->inUse = true;
        if ((err = self->memFns.init(cid, &slot->mem)) != OS_SUCCESS)
        {
            Debug_LOG_ERROR("init() callback failed on client (CID=%i) " \
                            "with %d", cid, err);
            return err;
        }
        *ctx = slot->mem;
        return OS_SUCCESS;
    }

    Debug_LOG_ERROR("Could not find free context slot for client (CID=%i)", cid);

    return OS_ERROR_INSUFFICIENT_SPACE;
}
