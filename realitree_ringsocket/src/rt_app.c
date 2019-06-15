// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

#include "rt_project.h"
#include "rt_storage.h"
#include "rt_task.h"

#define RT_STRLEN_MIN 0
#define RT_STRLEN_MAX_DIFF 1024
#define RT_STRLEN_MAX_TITLE 64

static uint32_t client_offset = 1;

rt_ret app_init(
    void
) {
    init_tasks();
    return load_from_storage(&client_offset);
}

rt_ret send_all(
    rs_t * rs
) {
    rs_w_uint32_hton(rs, client_offset);
    client_offset += 0x10000;
    //send_tasks(rs);
    send_projects(rs);
    rs_to_cur(rs, false, NULL, 0);
    return RT_OK;
}

rt_ret peer_open(
    rs_t * rs
) {
    RS_LOG(LOG_DEBUG);
    return send_all(rs);
}

rt_ret peer_close(
    rs_t * rs
) {
    (void) rs;
    RS_LOG(LOG_DEBUG);
    return RT_OK;
}

int64_t timer(
    void
) {
    RS_LOG(LOG_DEBUG);
    return 1000000; // Call this function again in 1 second
}

RS_APP(
    RS_INIT(app_init),
    RS_OPEN(peer_open),
    RS_READ_SWITCH(
        RS_CASE_BIN(0, send_all),
        RS_CASE_BIN(10, add_project,
            RS_NTOH(uint32_t), // parent_id
            RS_NTOH(uint32_t)  // id
        ),
        RS_CASE_BIN(11, move_project,
            RS_NTOH(uint32_t), // old_parent_id
            RS_NTOH(uint32_t), // new_parent_id
            RS_NTOH(uint16_t), // old_i
            RS_NTOH(uint16_t)  // new_i
        ),
        RS_CASE_BIN(12, collapse_project,
            RS_NTOH(uint32_t), // id
            RS_NET(uint8_t)    // is_collapsed
        ),
        RS_CASE_BIN(13, title_project,
            RS_NTOH(uint32_t), // id
            RS_STR_HEAP( // title -- allocate on the heap to store as-is
                RT_STRLEN_MIN, // (Note: in bytes, not UTF-8 char count)
                RT_STRLEN_MAX_TITLE
            )
        ),
        RS_CASE_BIN(14, describe_project,
            RS_NTOH(uint32_t), // id
            RS_NTOH(uint32_t), // crc32
            RS_NTOH(uint16_t), // splice_i
            RS_NTOH(uint16_t), // delete_c
            RS_STR( // diff_str -- temp var, so use a VLA instead of the heap
                RT_STRLEN_MIN,
                RT_STRLEN_MAX_DIFF
            )
        ), // todo: complete_project()
        RS_CASE_BIN(15, abort_project,
            RS_NTOH(uint32_t), // parent_id
            RS_NTOH(uint16_t) // i
        )
    ),
    RS_CLOSE(peer_close),
    RS_TIMER_SLEEP(timer)
);
