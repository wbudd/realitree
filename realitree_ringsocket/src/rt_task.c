// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

#include "rt_task.h"

#define RT_TASKS_ALLOC_C 1000
#define RT_TASKS_REALLOC_FACTOR 1.5

static struct rt_task *tasks = NULL;
static size_t tasks_elem_c = 0;
static size_t task_c = 0;

void init_tasks(
    void
) {
    tasks_elem_c = RT_TASKS_ALLOC_C;
    tasks = calloc(tasks_elem_c, sizeof(struct rt_task));
}

rt_ret load_tasks(
    ckv_t * ckv
) {
    struct ckv_map * maps = NULL;
    RT_GUARD_CKV(ckv_get_maps(ckv, (ckv_arg_maps){
        .key = "tasks",
        .dst = &maps,
        .elem_c = &task_c
    }));
    if (task_c > tasks_elem_c) {
        size_t new_elem_c = RT_TASKS_REALLOC_FACTOR * task_c;
        tasks = realloc(tasks, new_elem_c * sizeof(struct rt_task));
        memset(tasks + tasks_elem_c, 0, new_elem_c - tasks_elem_c);
        tasks_elem_c = new_elem_c;
    }
    for (size_t i = 0; i < task_c; i++) {
        struct rt_task * task = tasks + i;
        struct ckv_map * map = maps + i;
        RT_GUARD_CKV(ckv_get_uint32(ckv, (ckv_arg_uint32){
            .map = map,
            .key = "id",
            .dst = &task->id,
            .is_required = true
        }));
        RT_GUARD_CKV(ckv_get_str(ckv, (ckv_arg_str){
            .map = map,
            .key = "description",
            .dst = &task->description
        }));
        RT_GUARD_CKV(ckv_get_str(ckv, (ckv_arg_str){
            .map = map,
            .key = "afterword",
            .dst = &task->afterword
        }));
        RT_GUARD_CKV(ckv_get_str(ckv, (ckv_arg_str){
            .map = map,
            .key = "tags",
            .dst = &task->tags
        }));
        {
            size_t switch_c = 0;
            RT_GUARD_CKV(ckv_get_uint32s(ckv, (ckv_arg_uint32s){
                .map = map,
                .key = "switch_times",
                .dst = &task->switch_times,
                .elem_c = &switch_c
            }));
            task->switch_c = switch_c; // from size_t to uint32_t
        }
        RT_GUARD_CKV(ckv_get_uint32(ckv, (ckv_arg_uint32){
            .map = map,
            .key = "earliest",
            .dst = &task->earliest
        }));
        RT_GUARD_CKV(ckv_get_uint32(ckv, (ckv_arg_uint32){
            .map = map,
            .key = "latest",
            .dst = &task->latest
        }));
        RT_GUARD_CKV(ckv_get_uint8(ckv, (ckv_arg_uint8){
            .map = map,
            .key = "status",
            .dst = &task->status
        }));
        if (task->status > 2) {
            RS_LOG(LOG_ERR, "Invalid status: %u", task->status);
            return RT_FATAL;
        }
    }
    return RT_OK;
}

rt_ret store_tasks(
    ckv_t * ckv
) {
    struct ckv_map * maps = NULL;
    char const * keys[] = {"id", "description", "afterword", "tags",
        "switch_times", "switch_c", "earliest", "latest", "status"};
    RT_GUARD_CKV(ckv_set_maps(ckv, NULL, "tasks", &maps, task_c, keys,
        CKV_ELEM_C(keys)));
    for (size_t i = 0; i < task_c; i++) {
        struct rt_task * task = tasks + i;
        struct ckv_map * map = maps + i;
        RT_GUARD_CKV(ckv_set_uint32(ckv, map, "id", task->id));
        RT_GUARD_CKV(ckv_set_str(ckv, map, "description",
            task->description));
        if (task->afterword) {
            RT_GUARD_CKV(ckv_set_str(ckv, map, "afterword",
                task->afterword));
        }
        if (task->tags) {
            RT_GUARD_CKV(ckv_set_str(ckv, map, "tags", task->tags));
        }
        if (task->switch_times) {
            RT_GUARD_CKV(ckv_set_uint32s(ckv, map, "switch_times",
                task->switch_times, task->switch_c));
        }
        RT_GUARD_CKV(ckv_set_uint32(ckv, map, "earliest", task->earliest));
        if (task->latest) {
            RT_GUARD_CKV(ckv_set_uint32(ckv, map, "latest", task->latest));
        }
        RT_GUARD_CKV(ckv_set_uint8(ckv, map, "status", task->status));
    }
    return CKV_OK;
}
