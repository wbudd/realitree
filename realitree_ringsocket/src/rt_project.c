// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

#include "rt_project.h"

static struct rt_project root_project[1] = {0};

// Prototype required here because load_child_projects() and load_project()
// call each other in mutual recursion.
static rt_ret load_project(
    ckv_t * ckv,
    struct ckv_map * map,
	struct rt_project * project
);

static rt_ret load_child_projects(
    ckv_t * ckv,
    struct ckv_map * map,
    struct rt_project * project,
	char const * key
) {
	struct ckv_map * child_maps = NULL;
	size_t child_map_c = 0;
	RT_GUARD_CKV(ckv_get_maps(ckv, (ckv_arg_maps){
		.map = map,
		.key = key,
		.dst = &child_maps,
		.elem_c = &child_map_c
	}));
	if (!child_map_c) {
		return RT_OK;
	}
	project = project->child = calloc(1, sizeof(struct rt_project));
	for (;; project = project->next = calloc(1, sizeof(struct rt_project))) {
		RT_GUARD(load_project(ckv, child_maps++, project));
		if (!--child_map_c) {
			return RT_OK;
		}
	}
}

static rt_ret load_project(
    ckv_t * ckv,
    struct ckv_map * map,
	struct rt_project * project
) {
	RT_GUARD_CKV(ckv_get_uint32(ckv, (ckv_arg_uint32){
		.map = map,
		.key = "id",
		.dst = &project->id,
		.is_required = true
	}));
	RT_GUARD_CKV(ckv_get_str(ckv, (ckv_arg_str){
		.map = map,
		.key = "title",
		.dst = &project->title
	}));
	RT_GUARD_CKV(ckv_get_str(ckv, (ckv_arg_str){
		.map = map,
		.key = "description",
		.dst = &project->description
	}));
	{
		bool is_collapsed = false;
		RT_GUARD_CKV(ckv_get_bool(ckv, (ckv_arg_bool){
			.map = map,
			.key = "is_collapsed",
			.dst = &is_collapsed
		}));
		project->is_collapsed = is_collapsed; // from bool type to uint8_t
	}
	return load_child_projects(ckv, map, project, "children");
}

rt_ret load_projects(
    ckv_t * ckv
) {
	return load_child_projects(ckv, NULL, root_project, "projects");
}

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
