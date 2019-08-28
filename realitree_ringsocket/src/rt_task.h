// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

#pragma once

#include "realitree.h"

void init_tasks(
    void
);

rt_ret load_tasks(
    jg_t * jg,
    jg_obj_get_t * root_obj
);

rt_ret store_tasks(
    jg_t * jg,
    jg_obj_set_t * root_obj
);
