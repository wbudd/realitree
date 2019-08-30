// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

#include "rt_task.h"

#define RT_TASKS_ALLOC_C 1000
#define RT_TASKS_REALLOC_FACTOR 1.5

static struct rt_task * tasks = NULL;
static size_t tasks_elem_c = 0;
static size_t task_c = 0;

void init_tasks(
    void
) {
    tasks_elem_c = RT_TASKS_ALLOC_C;
    tasks = calloc(tasks_elem_c, sizeof(struct rt_task));
}

rt_ret load_tasks(
    jg_t * jg,
    jg_obj_get_t * root_obj
) {
    jg_arr_get_t * arr = NULL;
    size_t elem_c = 0;
    RT_GUARD_JG(jg_obj_get_arr_defa(jg, root_obj, "tasks", NULL, &arr,
        &elem_c));
    if (elem_c > tasks_elem_c) {
        size_t new_elem_c = RT_TASKS_REALLOC_FACTOR * elem_c;
        tasks = realloc(tasks, new_elem_c * sizeof(struct rt_task));
        memset(tasks + tasks_elem_c, 0, new_elem_c - tasks_elem_c);
        tasks_elem_c = new_elem_c;
    }
    for (size_t i = 0; i < elem_c; i++) {
        jg_obj_get_t * obj = NULL;
        RT_GUARD_JG(jg_arr_get_obj(jg, arr, i, NULL, &obj));
        struct rt_task * task = tasks + i;
        RT_GUARD_JG(jg_obj_get_uint32(jg, obj, "id", NULL, &task->id));
        RT_GUARD_JG(jg_obj_get_str(jg, obj, "description", NULL,
            &task->description));
        RT_GUARD_JG(jg_obj_get_str(jg, obj, "afterword", NULL,
            &task->afterword));
        RT_GUARD_JG(jg_obj_get_str(jg, obj, "tags", NULL, &task->tags));
        {
            jg_arr_get_t * child_arr = NULL;
            size_t child_elem_c = 0;
            RT_GUARD_JG(jg_obj_get_arr(jg, obj, "switch_times", NULL,
                &child_arr, &child_elem_c));
            for (size_t j = 0; j < child_elem_c; j++) {
                RT_GUARD_JG(jg_arr_get_uint32(jg, child_arr, j, NULL,
                    task->switch_times + j));
            }
            task->switch_c = child_elem_c; // from size_t to uint32_t
        }
        RT_GUARD_JG(jg_obj_get_uint32(jg, obj, "earliest", NULL,
            &task->earliest));
        RT_GUARD_JG(jg_obj_get_uint32(jg, obj, "latest", NULL,
            &task->earliest));
        RT_GUARD_JG(jg_obj_get_uint8(jg, obj, "status", &(jg_obj_uint8){
            .max = &(uint8_t){2},
            .max_reason = "Only task status codes 0 through 2 are recognized."
        }, &task->status));
    }
    return RT_OK;
}

rt_ret store_tasks(
    jg_t * jg,
    jg_obj_set_t * root_obj
) {
    if (!task_c) {
        return JG_OK;
    }
    jg_arr_set_t * arr = NULL;
    RT_GUARD_JG(jg_obj_set_arr(jg, root_obj, "tasks", &arr));
    for (size_t i = 0; i < task_c; i++) {
        jg_obj_set_t * obj = NULL;
        RT_GUARD_JG(jg_arr_set_obj(jg, arr, &obj));
        struct rt_task * task = tasks + i;
        RT_GUARD_JG(jg_obj_set_uint32(jg, obj, "id", task->id));
        RT_GUARD_JG(jg_obj_set_str(jg, obj, "description", task->description));
        if (task->afterword) {
            RT_GUARD_JG(jg_obj_set_str(jg, obj, "afterword", task->afterword));
        }
        if (task->tags) {
            RT_GUARD_JG(jg_obj_set_str(jg, obj, "tags", task->tags));
        }
        if (task->switch_times) {
            jg_arr_set_t * a = NULL;
            RT_GUARD_JG(jg_obj_set_arr(jg, obj, "switch_times", &a));
            for (size_t i = 0; i < task->switch_c; i++) {
                RT_GUARD_JG(jg_arr_set_uint32(jg, a, task->switch_times[i]));
            }
        }
        RT_GUARD_JG(jg_obj_set_uint32(jg, obj, "earliest", task->earliest));
        if (task->latest) {
            RT_GUARD_JG(jg_obj_set_uint32(jg, obj, "latest", task->latest));
        }
        RT_GUARD_JG(jg_obj_set_uint8(jg, obj, "status", task->status));
    }
    return JG_OK;
}
