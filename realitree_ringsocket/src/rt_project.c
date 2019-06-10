// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

#include "rt_project.h"

rt_ret add_project(
    rs_t * rs,
    uint32_t parent_id,
    uint32_t id
) {
    (void) rs;
    RS_LOG(LOG_DEBUG, "called with "
        "parent_id=%" PRIu32 " and "
        "id=%" PRIu32,
        parent_id,
        id
    );
    return RT_OK;
}

rt_ret move_project(
    rs_t * rs,
    uint32_t old_parent_id,
    uint32_t new_parent_id,
    uint16_t old_i,
    uint16_t new_i
) {
    (void) rs;
    RS_LOG(LOG_DEBUG, "called with "
        "old_parent_id=%" PRIu32 ", "
        "new_parent_id=%" PRIu32 ", "
        "old_i=%" PRIu16 ", and "
        "new_i=%" PRIu16,
        old_parent_id,
        new_parent_id,
        old_i,
        new_i
    );
    return RT_OK;
}

rt_ret collapse_project(
    rs_t * rs,
    uint32_t id,
    uint8_t is_collapsed
) {
    (void) rs;
    RS_LOG(LOG_DEBUG, "called with "
        "id=%" PRIu32 " and "
        "is_collapsed=%" PRIu8,
        id,
        is_collapsed
    );
    return RT_OK;
}

rt_ret title_project(
    rs_t * rs,
    uint32_t id,
    char * title,
    size_t strlen
) {
    (void) rs;
    RS_LOG(LOG_DEBUG, "called with "
        "id=%" PRIu32 ", "
        "title=%s, and "
        "strlen=%zu",
        id,
        title,
        strlen
    );
    return RT_OK;
}

rt_ret describe_project(
    rs_t * rs,
    uint32_t id,
    uint32_t crc32,
    uint16_t splice_i,
    uint16_t delete_c,
    char * diff_str,
    size_t strlen
) {
    (void) rs;
    RS_LOG(LOG_DEBUG, "called with "
        "id=%" PRIu32 ", "
        "crc32=%" PRIu32 ", "
        "splice_i=%" PRIu16 ", "
        "delete_c=%" PRIu16 ", "
        "diff_str=%s, and "
        "strlen=%zu",
        id,
        crc32,
        splice_i,
        delete_c,
        diff_str,
        strlen
    );
    return RT_OK;
}

// todo: complete_project()

rt_ret abort_project(
    rs_t * rs,
    uint32_t parent_id,
    uint16_t i
) {
    (void) rs;
    RS_LOG(LOG_DEBUG, "called with "
        "parent_id=%" PRIu32 " and "
        "i=%" PRIu16,
        parent_id,
        i
    );
    return RT_OK;
}
