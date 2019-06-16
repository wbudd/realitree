// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

#include "rt_project.h"
#include "rt_storage.h"
#include "rt_task.h"

#include <dirent.h> // opendir(), readdir(), closedir()

#define RT_STORAGE_DIR "/realitree_storage/"
#define RT_FILENAME_PREFIX "Realitree"
#define RT_FILENAME_FORMAT "00001122-0011"
#define RT_FILENAME_SUFFIX ".ckv"

static char storage_path[] =
    RT_STORAGE_DIR RT_FILENAME_PREFIX RT_FILENAME_FORMAT RT_FILENAME_SUFFIX;

static rt_ret open_newest_file(
    FILE * * f
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
            *f = fopen(storage_path, "r");
            if (!*f) {
                RS_LOG_ERRNO(LOG_ERR, "Unsuccessful fopen(%s, r)",
                    storage_path);
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
    FILE * f = NULL;
    RT_GUARD(open_newest_file(&f));
    if (f) {
        ckv_t * ckv = ckv_init();
        if (!ckv) {
            RS_LOG(LOG_ERR, "%s", ckv_get_err_str(ckv));
            fclose(f);
            return RT_FATAL;
        }
        RT_GUARD_CKV(ckv_parse_file(ckv, f, storage_path));
        fclose(f);
        RT_GUARD_CKV(ckv_get_uint32(ckv, (ckv_arg_uint32){
            .key = "client_offset",
            .is_required = true,
            .dst = client_offset
        }));
        RT_GUARD(load_projects(ckv));
        RT_GUARD(load_tasks(ckv));
        ckv_free(ckv);
    }
    return RT_OK;
}

rt_ret store_as_file(
    uint32_t client_offset
) {
    ckv_t * ckv = ckv_init();
    if (!ckv) {
        RS_LOG(LOG_ERR, "%s", ckv_get_err_str(NULL));
        return RT_FATAL;
    }
    char const * root_keys[] = {"client_offset", "projects", "tasks"};
    RT_GUARD_CKV(ckv_set_root(ckv, root_keys, CKV_ELEM_C(root_keys))); 
    RT_GUARD_CKV(ckv_set_uint32(ckv, NULL, "client_offset", client_offset));
    RT_GUARD(store_projects(ckv));
    RT_GUARD(store_tasks(ckv));
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
    FILE * f = fopen(storage_path, "w");
    if (!f) {
        RS_LOG_ERRNO(LOG_ERR, "Unsuccessful fopen(%s, w)", storage_path);
        return RT_FATAL;
    }
    RT_GUARD_CKV(ckv_print_file(ckv, f, storage_path, true));
    fclose(f);
    ckv_free(ckv);
    return RT_OK;
}
