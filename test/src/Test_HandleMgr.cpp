/*
 *  Copyright (C) 2019, Hensoldt Cyber GmbH
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

TEST(Test_HandleMgr, init_free_pos)
{
    HandleMgr_t* hMgr;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(hMgr));
}

TEST(Test_HandleMgr, init_neg)
{
    HandleMgr_t* hMgr;

    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_init(NULL, 1));
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_init(&hMgr, 0));
}

TEST(Test_HandleMgr, free_neg)
{
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_free(NULL));
}

TEST(Test_HandleMgr, add_pos)
{
    HandleMgr_t* hMgr;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_add(hMgr, 0, (HandleMgr_Handle_t) 1));
    // Handle was really added
    ASSERT_EQ((void*)1, HandleMgr_validate(hMgr, 0, (HandleMgr_Handle_t) 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(hMgr));
}

TEST(Test_HandleMgr, add_neg)
{
    HandleMgr_t* hMgr;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, 1));

    // Empty ctx
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_add(
                  NULL, 0, (HandleMgr_Handle_t) 1));
    // Non existent ID
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_add(
                  hMgr, 1, (HandleMgr_Handle_t) 1));
    // NULL handle
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_add(hMgr, 0, NULL));

    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(hMgr));
}

TEST(Test_HandleMgr, addOnSuccess_pos)
{
    HandleMgr_t* hMgr;
    HandleMgr_Handle_t h = (HandleMgr_Handle_t)1;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, 1));
    // Add sucessfully
    ASSERT_EQ(OS_SUCCESS, HandleMgr_addOnSuccess(hMgr, 0, OS_SUCCESS, &h));
    // Handle was added
    ASSERT_EQ(h, HandleMgr_validate(hMgr, 0, h));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(hMgr));
}

TEST(Test_HandleMgr, addOnSuccess_neg)
{
    HandleMgr_t* hMgr;
    HandleMgr_Handle_t h = (HandleMgr_Handle_t)1;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, 1));

    // Empty ctx
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_addOnSuccess(
                  NULL, 0, OS_SUCCESS, &h));
    // Invalid ID
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_addOnSuccess(
                  hMgr, 1, OS_SUCCESS, &h));
    // Input error != OS_SUCCESS
    ASSERT_EQ(OS_ERROR_ABORTED, HandleMgr_addOnSuccess(
                  hMgr, 0, OS_ERROR_ABORTED,  &h));
    // NULL handle pointer
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_addOnSuccess(
                  hMgr, 0, OS_SUCCESS, NULL));

    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(hMgr));
}

TEST(Test_HandleMgr, remove_pos)
{
    HandleMgr_t* hMgr;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_add(hMgr, 0, (HandleMgr_Handle_t) 1));
    // Handle was really removed
    ASSERT_EQ((void*)1, HandleMgr_validate(hMgr, 0, (HandleMgr_Handle_t) 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_remove(hMgr, 0, (HandleMgr_Handle_t) 1));
    // Handle was really removed
    ASSERT_EQ((void*)0, HandleMgr_validate(hMgr, 0, (HandleMgr_Handle_t) 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(hMgr));
}

TEST(Test_HandleMgr, remove_neg)
{
    HandleMgr_t* hMgr;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, 1));

    // Empty ctx
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_remove(
                  NULL, 0, (HandleMgr_Handle_t) 1));
    // Non existent ID
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_remove(
                  hMgr, 1, (HandleMgr_Handle_t) 1));
    // NULL handle
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_remove(
                  hMgr, 0, NULL));
    // Handle was never added
    ASSERT_EQ(OS_ERROR_INVALID_HANDLE, HandleMgr_remove(
                  hMgr, 0, (HandleMgr_Handle_t) 1));

    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(hMgr));
}

TEST(Test_HandleMgr, removeOnSuccess_pos)
{
    HandleMgr_t* hMgr;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_add(hMgr, 0, (HandleMgr_Handle_t) 1));
    // Handle was really added
    ASSERT_EQ((void*)1, HandleMgr_validate(hMgr, 0, (HandleMgr_Handle_t) 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_removeOnSuccess(
                  hMgr, 0, OS_SUCCESS, (HandleMgr_Handle_t) 1));
    // Handle was really removed
    ASSERT_EQ((void*)0, HandleMgr_validate(hMgr, 0, (HandleMgr_Handle_t) 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(hMgr));
}

TEST(Test_HandleMgr, removeOnSuccess_neg)
{
    HandleMgr_t* hMgr;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_add(hMgr, 0, (HandleMgr_Handle_t) 1));

    // Empty ctx
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_removeOnSuccess(
                  NULL, 0, OS_SUCCESS, (HandleMgr_Handle_t) 1));
    // Non existent ID
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_removeOnSuccess(
                  hMgr, 1, OS_SUCCESS, (HandleMgr_Handle_t) 1));
    // Input error != OS_SUCCESS
    ASSERT_EQ(OS_ERROR_ABORTED, HandleMgr_removeOnSuccess(
                  hMgr, 0,   OS_ERROR_ABORTED, (HandleMgr_Handle_t) 1));
    // NULL handle
    ASSERT_EQ(OS_ERROR_INVALID_PARAMETER, HandleMgr_removeOnSuccess(
                  hMgr, 0, OS_SUCCESS, NULL));

    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(hMgr));
}

TEST(Test_HandleMgr, validate_pos)
{
    HandleMgr_t* hMgr;

    ASSERT_EQ(OS_SUCCESS, HandleMgr_init(&hMgr, 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_add(hMgr, 0, (HandleMgr_Handle_t) 1));
    ASSERT_NE((void*)0, HandleMgr_validate(hMgr, 0, (HandleMgr_Handle_t) 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_remove(hMgr, 0, (HandleMgr_Handle_t) 1));
    // Non existent handle resolves to NULL
    ASSERT_EQ((void*)0, HandleMgr_validate(hMgr, 0, (HandleMgr_Handle_t) 1));
    ASSERT_EQ(OS_SUCCESS, HandleMgr_free(hMgr));
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
