/*
 * Copyright (C) 2020-2024, HENSOLDT Cyber GmbH
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * For commercial licensing, contact: info.cyber@hensoldt.net
 */

#include <gtest/gtest.h>

extern "C"
{
#include "lib_server/ContextMgr.h"
}

class Test_ContextMgr : public testing::Test
{
    protected:
};

// Dummy context struct
typedef struct
{
    ContextMgr_CID_t cid;
} ClientCtx_t;

// Keep track of alloc/free
static size_t initNum = 0;
static size_t freeNum = 0;

// Private functions -----------------------------------------------------------

static OS_Error_t
initClient(
    const ContextMgr_CID_t cid,
    void**                 mem)
{
    ClientCtx_t* p;

    p = (ClientCtx_t*) calloc(1, sizeof(ClientCtx_t));
    assert(p != NULL);
    p->cid = cid;

    *mem = p;

    initNum++;

    return OS_SUCCESS;
}

static OS_Error_t
freeClient(
    const ContextMgr_CID_t cid,
    void*                  mem)
{
    free(mem);

    freeNum++;

    return OS_SUCCESS;
}

const ContextMgr_MemoryFuncs_t fns =
{
    .init = initClient,
    .free = freeClient
};

#define MAX_CLIENTS 8

// Test functions --------------------------------------------------------------

TEST(Test_ContextMgr, init_free_pos)
{
    ContextMgr_t hMgr;

    initNum = freeNum = 0;
    ASSERT_EQ(OS_SUCCESS, ContextMgr_init(&hMgr, &fns, MAX_CLIENTS));
    ASSERT_EQ(OS_SUCCESS, ContextMgr_free(&hMgr));

    // We should not have any allocations without calls to get()
    ASSERT_EQ(initNum, 0);
    ASSERT_EQ(freeNum, 0);
}

TEST(Test_ContextMgr, init_neg)
{
    ContextMgr_t hMgr;
    ContextMgr_MemoryFuncs_t myFns;

    // Empty pointers
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, ContextMgr_init(NULL, &fns, MAX_CLIENTS));
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, ContextMgr_init(&hMgr, NULL,
                                                          MAX_CLIENTS));

    // Empty callbacks
    myFns = fns;
    myFns.free = NULL;
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, ContextMgr_init(&hMgr, &myFns,
                                                          MAX_CLIENTS));
    myFns = fns;
    myFns.init = NULL;
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, ContextMgr_init(&hMgr, &myFns,
                                                          MAX_CLIENTS));

    // Invalid number of contexts
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, ContextMgr_init(&hMgr, &fns, 0));
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, ContextMgr_init(&hMgr, &fns, 1025));
}

TEST(Test_ContextMgr, free_neg)
{
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, ContextMgr_free(NULL));
}

TEST(Test_ContextMgr, get_pos)
{
    ContextMgr_t hMgr;
    ClientCtx_t* ctx;

    initNum = freeNum = 0;
    ASSERT_EQ(OS_SUCCESS, ContextMgr_init(&hMgr, &fns, MAX_CLIENTS));

    // Get all contexts, this should allocate them
    for (size_t i = 0; i < MAX_CLIENTS; i++)
    {
        ASSERT_EQ(OS_SUCCESS, ContextMgr_get(&hMgr, i, (void**)&ctx));
        ASSERT_EQ(ctx->cid, i);
    }
    ASSERT_EQ(initNum, MAX_CLIENTS);

    // Get contexts again, there should be no further allocation
    for (size_t i = 0; i < MAX_CLIENTS; i++)
    {
        ASSERT_EQ(OS_SUCCESS, ContextMgr_get(&hMgr, i, (void**)&ctx));
        ASSERT_EQ(ctx->cid, i);
    }
    ASSERT_EQ(initNum, MAX_CLIENTS);

    // Free mgr instance and check all contexts were free'd as well
    ASSERT_EQ(OS_SUCCESS, ContextMgr_free(&hMgr));
    ASSERT_EQ(freeNum, MAX_CLIENTS);
}

TEST(Test_ContextMgr, get_neg)
{
    ContextMgr_t hMgr;
    ClientCtx_t* ctx;

    ASSERT_EQ(OS_SUCCESS, ContextMgr_init(&hMgr, &fns, 2));

    // Try empty pointers
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, ContextMgr_get(NULL,  0, (void**)&ctx));
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, ContextMgr_get(&hMgr, 0, NULL));

    // Try to get more contexts than allowed
    ASSERT_EQ(OS_SUCCESS, ContextMgr_get(&hMgr, 0, (void**)&ctx));
    ASSERT_EQ(OS_SUCCESS, ContextMgr_get(&hMgr, 1, (void**)&ctx));
    ASSERT_EQ(OS_ERROR_INSUFFICIENT_SPACE, ContextMgr_get(&hMgr, 2, (void**)&ctx));

    ASSERT_EQ(OS_SUCCESS, ContextMgr_free(&hMgr));
}
