// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

#include "rt_project.h"

static struct rt_project root_project[1] = {0};

// Prototype required here because load_child_projects() and load_project()
// call each other in mutual recursion.
static rt_ret load_project(
    jg_t * jg,
    jg_obj_get_t * obj,
    struct rt_project * project
);

// Same situation as above for store_child_projects() and store_project()
static rt_ret store_project(
    jg_t * jg,
    jg_obj_set_t * obj,
    struct rt_project * project
);

static rt_ret load_child_projects(
    jg_t * jg,
    jg_obj_get_t * obj,
    struct rt_project * project,
    char const * key
) {
    jg_arr_get_t * arr = NULL;
    size_t elem_c = 0;
    RT_GUARD_JG(jg_obj_get_arr(jg, obj, key, NULL, &arr, &elem_c));
    if (!elem_c) {
        return RT_OK;
    }
    project = project->child = calloc(1, sizeof(struct rt_project));
    for (size_t i = 0;;) {
        jg_obj_get_t * child_obj = NULL;
        RT_GUARD_JG(jg_arr_get_obj(jg, arr, i, NULL, &child_obj));
        RT_GUARD(load_project(jg, child_obj, project));
        if (++i >= elem_c) {
            return RT_OK;
        }
        project = project->next = calloc(1, sizeof(struct rt_project));
    }
}

static rt_ret load_project(
    jg_t * jg,
    jg_obj_get_t * obj,
    struct rt_project * project
) {
    RT_GUARD_JG(jg_obj_get_uint32(jg, obj, "id", NULL, &project->id));
    RT_GUARD_JG(jg_obj_get_str(jg, obj, "title", NULL, &project->title));
    RT_GUARD_JG(jg_obj_get_str(jg, obj, "description", NULL,
        &project->description));
    bool is_collapsed = false;
    RT_GUARD_JG(jg_obj_get_bool(jg, obj, "is_collapsed", &(bool){false},
        &is_collapsed));
    project->is_collapsed = is_collapsed; // from bool type to uint8_t
    return load_child_projects(jg, obj, project, "children");
}

rt_ret load_projects(
    jg_t * jg,
    jg_obj_get_t * obj
) {
    return load_child_projects(jg, obj, root_project, "projects");
}

static rt_ret store_child_projects(
    jg_t * jg,
    jg_obj_set_t * obj,
    struct rt_project * project,
    char const * key
) {
    project = project->child;
    size_t child_c = 0;
    for (struct rt_project * p = project; p; p = p->next) {
        child_c++;
    }
    if (!child_c) {
        return RT_OK;
    }
    jg_arr_set_t * arr = NULL;
    RT_GUARD_JG(jg_obj_set_arr(jg, obj, key, &arr));
    for (size_t i = 0; i < child_c; i++) {
        jg_obj_set_t * child_obj = NULL;
        RT_GUARD_JG(jg_arr_set_obj(jg, arr, &child_obj));
        RT_GUARD(store_project(jg, child_obj, project));
        project = project->next;
    }
    return RT_OK;
}

static rt_ret store_project(
    jg_t * jg,
    jg_obj_set_t * obj,
    struct rt_project * project
) {
    RT_GUARD_JG(jg_obj_set_uint32(jg, obj, "id", project->id));
    if (project->title) {
        RT_GUARD_JG(jg_obj_set_str(jg, obj, "title", project->title));
    }
    if (project->description) {
        RT_GUARD_JG(jg_obj_set_str(jg, obj, "description",
            project->description));
    }
    if (project->is_collapsed) {
        RT_GUARD_JG(jg_obj_set_uint8(jg, obj, "is_collapsed",
            project->is_collapsed));
    }
    return store_child_projects(jg, obj, project, "children");
}

rt_ret store_projects(
    jg_t * jg,
    jg_obj_set_t * obj
) {
    return store_child_projects(jg, obj, root_project, "projects");
}

static void send_child_projects(
    rs_t * rs,
    struct rt_project const * p
) {
    uint16_t child_c = 0;
    for (struct rt_project const * q = p; q; q = q->next) {
        child_c++;
    }
    rs_w_uint16_hton(rs, child_c);
    while (p) {
        rs_w_uint32_hton(rs, p->id);
        if (p->title) {
            rs_w_p(rs, p->title, strlen(p->title) + 1);
        } else {
            rs_w_uint8(rs, '\0');
        }
        if (p->description) {
            rs_w_p(rs, p->description, strlen(p->description) + 1);
        } else {
            rs_w_uint8(rs, '\0');
        }
        rs_w_uint8(rs, p->is_collapsed);
        send_child_projects(rs, p->child);
        p = p->next;
    }
}

void send_projects(
    rs_t * rs
) {
    send_child_projects(rs, root_project->child);
}

static struct rt_project * get_child_project(
    struct rt_project * project,
    uint32_t id
) {
    for (struct rt_project * p = project->child; p; p = p->next) {
        if (p->id == id) {
            return p;
        }
        struct rt_project * q = get_child_project(p, id);
        if (q) {
            return q;
        }
    }
    return NULL;
}

static struct rt_project * get_project(
    uint32_t id
) {
    if (root_project->id == id) {
        return root_project;
    }
    return get_child_project(root_project, id);
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
    struct rt_project * p = get_project(parent_id);
    if (!p) {
        return RT_PROJECT_ID_NOT_FOUND;
    }
    if (p->child) {
        p = p->child;
        while (p->next) {
            p = p->next;
        }
        p = p->next = calloc(1, sizeof(struct rt_project));
    } else {
        p = p->child = calloc(1, sizeof(struct rt_project));
    }
    p->id = id;
    rt_has_changed = true;
    return RT_OK;
}

static struct rt_project * cut_project(
    uint32_t parent_id,
    int i
) {
    struct rt_project * p = get_project(parent_id);
    if (!p) {
        return NULL;
    }
    struct rt_project * q = p->child;
    if (!q) {
        return NULL;
    }
    if (i == 0) {
        p->child = q->next;
        return q;
    }
    p = q->next;
    while (p) {
        if (--i < 1) {
            q->next = p->next;
            return p;
        }
        q = p;
        p = p->next;
    }
    return NULL;
}

static rt_ret paste_project(
    uint32_t parent_id,
    int i,
    struct rt_project * project
) {
    struct rt_project * p = get_project(parent_id);
    if (!p) {
        return RT_PROJECT_INVALID_TARGET;
    }
    if (!i) {
        project->next = p->child;
        p->child = project;
        return RT_OK;
    }
    p = p->child;
    while (p) {
        if (--i < 1) {
            project->next = p->next;
            p->next = project;
            return RT_OK;
        }
        p = p->next;
    }
    return RT_PROJECT_INVALID_TARGET;
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
    struct rt_project * p = cut_project(old_parent_id, old_i);
    if (!p) {
        return RT_PROJECT_INVALID_SOURCE;
    }
    RT_GUARD(paste_project(new_parent_id, new_i, p));
    rt_has_changed = true;
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
    struct rt_project * project = get_project(id);
    if (!project) {
        return RT_PROJECT_ID_NOT_FOUND;
    }
    project->is_collapsed = is_collapsed;
    rt_has_changed = true;
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
    struct rt_project * project = get_project(id);
    if (!project) {
        return RT_PROJECT_ID_NOT_FOUND;
    }
    if (project->title) {
        free(project->title);
    }
    project->title = title;
    rt_has_changed = true;
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
    // todo
    //rt_has_changed = true;
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
    // todo
    //rt_has_changed = true;
    return RT_OK;
}
