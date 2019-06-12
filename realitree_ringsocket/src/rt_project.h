// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

#pragma once

#include "realitree.h"

rt_ret load_projects(
    ckv_t * ckv
);

void send_projects(
    rs_t * rs
);

rt_ret add_project(
    rs_t * rs,
    uint32_t parent_id,
    uint32_t id
);

rt_ret move_project(
    rs_t * rs,
    uint32_t old_parent_id,
    uint32_t new_parent_id,
    uint16_t old_i,
    uint16_t new_i
);

rt_ret collapse_project(
    rs_t * rs,
    uint32_t id,
    uint8_t is_collapsed
);

rt_ret title_project(
    rs_t * rs,
    uint32_t id,
    char * title,
    size_t strlen
);

rt_ret describe_project(
    rs_t * rs,
    uint32_t id,
    uint32_t crc32,
    uint16_t splice_i,
    uint16_t delete_c,
    char * diff_str,
    size_t strlen
);

// todo: complete_project()

rt_ret abort_project(
    rs_t * rs,
    uint32_t parent_id,
    uint16_t i
);
