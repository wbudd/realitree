// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

#pragma once

#include <ringsocket.h>
#include <ckv.h>

typedef enum {
    RT_FATAL = -1,
    RT_OK = 0,
    RT_PROJECT_ID_NOT_FOUND = 4000,
    RT_PROJECT_INVALID_SOURCE = 4001,
    RT_PROJECT_INVALID_TARGET = 4002
} rt_ret;

#define RT_GUARD(rt_call) do { \
    rt_ret ret = (rt_call); \
    if (ret != RT_OK) { \
        return ret; \
    } \
} while (0)

#define RT_GUARD_CKV(ckv_call) do { \
    if ((ckv_call) != CKV_OK) { \
        RS_LOG(LOG_ERR, "libckv error: %s", ckv_get_err_str(ckv)); \
        return RT_FATAL; \
    } \
} while (0)

struct rt_project {
    struct rt_task * * tagged_tasks;
    struct rt_project * next;
    struct rt_project * child;
    char * title;
    char * description;
    uint32_t id;
    uint8_t is_collapsed;
};

struct rt_task {
    char * description;
    char * afterword;
    char * tags;
    uint32_t * switch_times;
    uint32_t switch_c;
    uint32_t earliest;
    uint32_t latest;
    uint32_t id;
    uint8_t status;
    //uint8_t access_profile;
};

// Global flag to mark whether or not anything has changed since the last
// dump_to_file(). (Global, but of course not exported outside of the app SO.)
extern bool rt_has_changed;
