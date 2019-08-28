// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

#include "rt_project.h"
#include "rt_storage.h"
#include "rt_task.h"

#include <dirent.h> // opendir(), readdir(), closedir()

#define RT_STORAGE_DIR "/realitree_storage/"
#define RT_FILENAME_PREFIX "Realitree"
#define RT_FILENAME_FORMAT "00001122-0011"
#define RT_FILENAME_SUFFIX ".json"

static char storage_path[] =
    RT_STORAGE_DIR RT_FILENAME_PREFIX RT_FILENAME_FORMAT RT_FILENAME_SUFFIX;

static rt_ret set_newest_storage_path(
    void
) {
    DIR * dir = opendir(RT_STORAGE_DIR);
    if (!dir) {
        RS_LOG_ERRNO(LOG_ERR, "Unsuccessful opendir()");
        return RT_FATAL;
    }
    struct dirent * entry = readdir(dir);
    if (entry) {
        char * newest_filename = entry->d_name;
        while ((entry = readdir(dir))) {
            if (strcmp(entry->d_name, newest_filename) > 0) {
                newest_filename = entry->d_name;
            }
        }
        if (newest_filename[0] != '.' || (newest_filename[1] != '\0' &&
            (newest_filename[1] != '.' || newest_filename[2] != '\0'))) {
            strncpy(
                storage_path + RS_CONST_STRLEN(RT_STORAGE_DIR),
                newest_filename,
                RS_CONST_STRLEN(storage_path) - RS_CONST_STRLEN(RT_STORAGE_DIR)
            );
            if (closedir(dir) == -1) {
                RS_LOG_ERRNO(LOG_ERR, "Unsuccessful closedir()");
                return RT_FATAL;
            }
            return RT_OK;
        }
    }
    RS_LOG(LOG_INFO, "No stored Realitree data found.");
    if (closedir(dir) == -1) {
        RS_LOG_ERRNO(LOG_ERR, "Unsuccessful closedir()");
        return RT_FATAL;
    }
    return RT_OK;
}

rt_ret load_from_file(
    uint32_t * client_offset
) {
    RT_GUARD(set_newest_storage_path());
    jg_t * jg = jg_init();
    RT_GUARD_JG(jg_parse_file(jg, storage_path));
    jg_obj_get_t * root_obj = NULL;
    RT_GUARD_JG(jg_root_get_obj(jg, NULL, &root_obj));
    RT_GUARD_JG(jg_obj_get_uint32(jg, root_obj, "client_offset", NULL,
        client_offset));
    RT_GUARD(load_projects(jg, root_obj));
    RT_GUARD(load_tasks(jg, root_obj));
    jg_free(jg);
    return RT_OK;
}

rt_ret store_as_file(
    uint32_t client_offset
) {
    jg_t * jg = jg_init();
    jg_obj_set_t * root_obj = NULL;
    RT_GUARD_JG(jg_root_set_obj(jg, &root_obj));
    RT_GUARD_JG(jg_obj_set_uint32(jg, root_obj, "client_offset",
        client_offset));
    RT_GUARD(store_projects(jg, root_obj));
    RT_GUARD(store_tasks(jg, root_obj));
    {
        time_t t = time(NULL);
        struct tm * tlocal = localtime(&t);
        strftime(
            storage_path +
                RS_CONST_STRLEN(RT_STORAGE_DIR) +
                RS_CONST_STRLEN(RT_FILENAME_PREFIX),
            // The strfime() max string size argument here boneheadedly also
            // counts the \0 byte, so us sizeof rather than RT_CONST_STRLEN:
            sizeof(RT_FILENAME_FORMAT),
            "%Y%m%d-%H%M",
            tlocal
        );
        // Overwrite '\0' written by strftime() to put '.' of ".ckv" back in.
        storage_path[RS_CONST_STRLEN(RT_STORAGE_DIR) +
                     RS_CONST_STRLEN(RT_FILENAME_PREFIX) +
                     RS_CONST_STRLEN(RT_FILENAME_FORMAT)] = '.';
    }
    RT_GUARD_JG(jg_generate_file(jg, NULL, storage_path));
    jg_free(jg);
    return RT_OK;
}
