/*
 * Copyright (C) 2019-2024, HENSOLDT Cyber GmbH
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * For commercial licensing, contact: info.cyber@hensoldt.net
 */

#include <gtest/gtest.h>

extern "C"
{
#include "lib_server/HandleMgr.h"
#include <stdint.h>
#include <limits.h>
}

class Test_HandleMgr : public testing::Test
{
    protected:
};

#define NUM_HANDLES 10

TEST(Test_HandleMgr, init_free_pos)
{
    HandleMgr_t hMgr;
    HandleMgr_Handle_t buffer[NUM_HANDLES];
    size_t numEl = NUM_HANDLES - 1;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, buffer, sizeof(buffer), &numEl));
    ASSERT_EQ(numEl, NUM_HANDLES);
    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(&hMgr));

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, buffer, sizeof(buffer), NULL));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(&hMgr));
}

TEST(Test_HandleMgr, init_neg)
{
    HandleMgr_t hMgr;
    HandleMgr_Handle_t buffer[NUM_HANDLES];

    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER,
              HandleMgr_init(NULL, buffer, sizeof(buffer), NULL));
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER,
              HandleMgr_init(&hMgr, NULL, NUM_HANDLES, NULL));
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER,
              HandleMgr_init(&hMgr, buffer, 0, NULL));
}

TEST(Test_HandleMgr, free_neg)
{
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_free(NULL));
}

TEST(Test_HandleMgr, add_pos)
{
    HandleMgr_t hMgr;
    HandleMgr_Handle_t buffer[NUM_HANDLES];
    size_t numEl = NUM_HANDLES;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, buffer, sizeof(buffer), &numEl));

    for (size_t i = 0; i < numEl; i++)
    {
        ASSERT_EQ(OS_SUCCESS,
                  HandleMgr_add(&hMgr, (HandleMgr_Handle_t) (i + 1)));
        // Handle was really added
        ASSERT_EQ((void*) (i + 1),
                  HandleMgr_validate(&hMgr, (HandleMgr_Handle_t) (i + 1)));
    }
    // test duplicates avoidance
    ASSERT_EQ(OS_ERROR_OPERATION_DENIED,
              HandleMgr_add(&hMgr, (HandleMgr_Handle_t) (NUM_HANDLES)));
    // test limit exceeded
    ASSERT_EQ(OS_ERROR_INSUFFICIENT_SPACE,
              HandleMgr_add(&hMgr, (HandleMgr_Handle_t) (NUM_HANDLES + 1)));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(&hMgr));
}

TEST(Test_HandleMgr, add_neg)
{
    HandleMgr_t hMgr;
    HandleMgr_Handle_t buffer[NUM_HANDLES];

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, buffer, sizeof(buffer), NULL));

    // Empty ctx
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_add(
                  NULL, (HandleMgr_Handle_t) 1));
    // NULL handle
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_add(&hMgr, NULL));

    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(&hMgr));
}

TEST(Test_HandleMgr, addOnSuccess_pos)
{
    HandleMgr_t hMgr;
    HandleMgr_Handle_t buffer[NUM_HANDLES];
    size_t numEl = NUM_HANDLES;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, buffer, sizeof(buffer), &numEl));

    for (size_t i = 0; i < numEl; i++)
    {
        HandleMgr_Handle_t handle = (HandleMgr_Handle_t) (i + 1);
        ASSERT_EQ(OS_SUCCESS,
                  HandleMgr_addOnSuccess(&hMgr,
                                         OS_SUCCESS,
                                         &handle));
        // Handle was really added
        ASSERT_EQ((void*) (i + 1),
                  HandleMgr_validate(&hMgr, handle));
    }
    ASSERT_EQ(OS_ERROR_INSUFFICIENT_SPACE,
              HandleMgr_add(&hMgr, (HandleMgr_Handle_t) (NUM_HANDLES + 1)));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(&hMgr));
}

TEST(Test_HandleMgr, addOnSuccess_neg)
{
    HandleMgr_t hMgr;
    HandleMgr_Handle_t h = (HandleMgr_Handle_t) 1;
    HandleMgr_Handle_t buffer[NUM_HANDLES];

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, buffer, sizeof(buffer), NULL));

    // Empty ctx
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_addOnSuccess(
                  NULL, OS_SUCCESS, &h));
    // Input error != OS_SUCCESS
    ASSERT_EQ(OS_ERROR_ABORTED, HandleMgr_addOnSuccess(
                  &hMgr, OS_ERROR_ABORTED,  &h));
    // NULL handle pointer
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_addOnSuccess(
                  &hMgr, OS_SUCCESS, NULL));

    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(&hMgr));
}

TEST(Test_HandleMgr, remove_pos)
{
    HandleMgr_t hMgr;
    HandleMgr_Handle_t buffer[NUM_HANDLES];
    size_t numEl = NUM_HANDLES;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, buffer, sizeof(buffer), &numEl));

    for (size_t i = 0; i < numEl; i++)
    {
        ASSERT_EQ(OS_SUCCESS,
                  HandleMgr_add(&hMgr, (HandleMgr_Handle_t) (i + 1)));
        // Handle was really added
        ASSERT_EQ((void*) (i + 1),
                  HandleMgr_validate(&hMgr, (HandleMgr_Handle_t) (i + 1)));
    }
    ASSERT_EQ(OS_ERROR_INSUFFICIENT_SPACE,
              HandleMgr_add(&hMgr, (HandleMgr_Handle_t) (NUM_HANDLES + 1)));

    ASSERT_EQ((void*)1, HandleMgr_validate(&hMgr, (HandleMgr_Handle_t) 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_remove(&hMgr, (HandleMgr_Handle_t) 1));
    // Handle was really removed
    ASSERT_EQ((void*)0, HandleMgr_validate(&hMgr, (HandleMgr_Handle_t) 1));
    // try to re-add
    ASSERT_EQ(OS_SUCCESS, HandleMgr_add(&hMgr, (HandleMgr_Handle_t) 1));
    // Handle was really re-added
    ASSERT_EQ((void*)1, HandleMgr_validate(&hMgr, (HandleMgr_Handle_t) 1));

    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(&hMgr));
}

TEST(Test_HandleMgr, remove_neg)
{
    HandleMgr_t hMgr;
    HandleMgr_Handle_t buffer[NUM_HANDLES];

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, buffer, sizeof(buffer), NULL));

    // Empty ctx
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_remove(
                  NULL, (HandleMgr_Handle_t) 1));
    // NULL handle
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_remove(
                  &hMgr, NULL));
    // Handle was never added
    ASSERT_EQ(OS_ERROR_INVALID_HANDLE, HandleMgr_remove(
                  &hMgr, (HandleMgr_Handle_t) 1));

    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(&hMgr));
}

TEST(Test_HandleMgr, removeOnSuccess_pos)
{
    HandleMgr_t hMgr;
    HandleMgr_Handle_t buffer[NUM_HANDLES];

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, buffer, sizeof(buffer), NULL));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_add(&hMgr, (HandleMgr_Handle_t) 1));
    // Handle was really added
    ASSERT_EQ((void*)1, HandleMgr_validate(&hMgr, (HandleMgr_Handle_t) 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_removeOnSuccess(
                  &hMgr, OS_SUCCESS, (HandleMgr_Handle_t) 1));
    // Handle was really removed
    ASSERT_EQ((void*)0, HandleMgr_validate(&hMgr, (HandleMgr_Handle_t) 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(&hMgr));
}

TEST(Test_HandleMgr, removeOnSuccess_neg)
{
    HandleMgr_t hMgr;
    HandleMgr_Handle_t buffer[NUM_HANDLES];

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, buffer, sizeof(buffer), NULL));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_add(&hMgr, (HandleMgr_Handle_t) 1));

    // Empty ctx
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_removeOnSuccess(
                  NULL, OS_SUCCESS, (HandleMgr_Handle_t) 1));
    // Input error != OS_SUCCESS
    ASSERT_EQ(OS_ERROR_ABORTED, HandleMgr_removeOnSuccess(
                  &hMgr, OS_ERROR_ABORTED, (HandleMgr_Handle_t) 1));
    // NULL handle
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_removeOnSuccess(
                  &hMgr, OS_SUCCESS, NULL));

    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(&hMgr));
}

TEST(Test_HandleMgr, validate_pos)
{
    HandleMgr_t hMgr;
    HandleMgr_Handle_t buffer[NUM_HANDLES];

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, buffer, sizeof(buffer), NULL));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_add(&hMgr, (HandleMgr_Handle_t) 1));
    ASSERT_NE((void*)0, HandleMgr_validate(&hMgr, (HandleMgr_Handle_t) 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_remove(&hMgr, (HandleMgr_Handle_t) 1));
    // Non existent handle resolves to NULL
    ASSERT_EQ((void*)0, HandleMgr_validate(&hMgr, (HandleMgr_Handle_t) 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(&hMgr));
}
