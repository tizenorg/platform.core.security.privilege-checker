/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include <stdlib.h>
#include <dlog.h>
#include <privilege_checker.h>
#include <pkgmgr-info.h>
#include <package_manager.h>

#define COLOR_RED           "\033[0;40;31m"
#define COLOR_LIGHTBLUE     "\033[1;40;34m"
#define COLOR_YELLOW        "\033[1;40;33m"
#define COLOR_END           "\033[0;m"
#define COLOR_GRAY          "\033[1;40;30m"
#define COLOR_MAGENTA       "\033[1;35m"

#define LOG_PREFIX
#define FUNC_BEGIN() \
    do {\
        LOGD(COLOR_LIGHTBLUE""LOG_PREFIX" BEGIN >>>>"COLOR_END);\
    } while (0);

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "PRIVILEGE_CHECKER"
#endif

#define TryReturn(condition, returnValue, ...)  \
    if (!(condition)) { \
        LOGE(__VA_ARGS__); \
        return returnValue; \
    } \
    else {;}

#define CACHED_PRIVILEGE_MAX    256
#define SUB_HASH(str, len, value) ( (str[len-1] << 24) | (str[len-2] << 16) | value )

typedef struct _cached_privilege_s{
    char* privilege_name;
    int hash_value;
} cached_privilege_s;

static cached_privilege_s g_cached_privileges[CACHED_PRIVILEGE_MAX];
static int g_cached_privileges_size = 0;
static int g_isMached = false;

int __privilege_checker_hash(const char* privilege_name)
{
    TryReturn(privilege_name != NULL, -1, "[PRIV_CHECKER_ERR_INVALID_PARAMETER] privilege_name is NULL!");
    int len = strlen(privilege_name);
    TryReturn(len >= 2, -1, "[PRIV_CHECKER_ERR_INVALID_PARAMETER] privilege_name's size must be more than 2!");
    int hash_value = 0;
    int i=0;

    for(i=0; i<len; i++)
    {
        hash_value += privilege_name[i];
    }

    hash_value = SUB_HASH(privilege_name, len, hash_value);

    return hash_value;
}

void __privilege_checker_print_cache_table()
{
    int i=0;
    for(i=0; i<g_cached_privileges_size; i++){
        LOGD("privilege : %s, hash_value : %d", g_cached_privileges[i].privilege_name, g_cached_privileges[i].hash_value);
    }
}

int __privilege_checker_privilege_list_by_pkgid_cb(const char* package_manager_privilege, void* user_data)
{
    LOGD("Enter privile_name : %s", package_manager_privilege);
    TryReturn(package_manager_privilege != NULL, PRIV_CHECKER_ERR_INVALID_PARAMETER, "[PRIV_CHECKER_ERR_INVALID_PARAMETER] Callback privilege_id is NULL!");
    TryReturn(user_data != NULL, PRIV_CHECKER_ERR_INVALID_PARAMETER, "[PRIV_CHECKER_ERR_INVALID_PARAMETER] priv_info is NULL!");

    char *privilege_name = (char*)user_data;
    int len = strlen(package_manager_privilege);

    if(g_isMached == false && strncmp(package_manager_privilege, privilege_name, len) == 0)
    {
        LOGD("Found matched privilege");
        g_isMached = true;
    }

    LOGD("Add privilege and hash value to cache table");
    g_cached_privileges[g_cached_privileges_size].privilege_name = strdup(package_manager_privilege);
    TryReturn(g_cached_privileges[g_cached_privileges_size].privilege_name != NULL, PRIV_CHECKER_ERR_OUT_OF_MEMORY, "[PRIV_CHECKER_ERR_OUT_OF_MEMORY] privilege_name's strdup is failed.");
    g_cached_privileges[g_cached_privileges_size].hash_value = __privilege_checker_hash(package_manager_privilege);
    ++g_cached_privileges_size;

    return 0;
}

int privilege_checker_check_package_privilege(const char* package_id, const char* privilege_name)
{
    FUNC_BEGIN();

    TryReturn(privilege_name != NULL, PRIV_CHECKER_ERR_INVALID_PARAMETER, "[PRIV_CHECKER_ERR_INVALID_PARAMETER] privilege_id is NULL");
    TryReturn(package_id != NULL, PRIV_CHECKER_ERR_INVALID_PARAMETER, "[PRIV_CHECKER_ERR_INVALID_PARAMETER] package_id is NULL");

    LOGD("package_id = %s", package_id);
    LOGD("privilege_name = %s", privilege_name);

    int len = strlen(privilege_name);
    int hash_value = __privilege_checker_hash(privilege_name);
    int matched_index = 0;
    int matched_cnt = 0;
    int i = 0;
    char* user_data = NULL;

    //find privilege in cache
    for(i=0; i<g_cached_privileges_size; i++)
    {
        if(hash_value == g_cached_privileges[i].hash_value)
        {
            matched_index = i;
            ++matched_cnt;
        }
    }

    if(matched_cnt == 1){
        LOGD("Found cached hash privilege");
        return PRIV_CHECKER_ERR_NONE;

    }else if(matched_cnt > 1){
        LOGD("There are %d matched hash value", matched_cnt);
        LOGD("Check privilege string");

        LOGD("Privilege Name : %s", privilege_name);
        LOGD("Cached Privilege Name : %s", g_cached_privileges[matched_index].privilege_name);

        //check string privilege with last hash-matched one.
        if(strncmp(privilege_name, g_cached_privileges[matched_index].privilege_name, len) == 0)
        {
            LOGD("Found cached string privilege");
            return PRIV_CHECKER_ERR_NONE;
        }

        //check string privilege with rest of them.
        for(i=0; i<matched_index; i++)
        {
            if(strncmp(privilege_name, g_cached_privileges[g_cached_privileges_size].privilege_name, len) == 0)
            {
                LOGD("Found cached string privilege");
                return PRIV_CHECKER_ERR_NONE;
            }
        }
    }
    LOGD("Not Found cached privilege");

    //if there is no cached privilege, cache table size must be 0.
    TryReturn(g_cached_privileges_size == 0, PRIV_CHECKER_ERR_INVALID_PRIVILEGE, "[PRIV_CHECKER_ERR_INVALID_PRIVILEGE] %s privilege denied", privilege_name);

    LOGD("Check privilege from PackageManager");
    g_isMached = false;
    pkgmgrinfo_pkginfo_h handle;
    int res = pkgmgrinfo_pkginfo_get_pkginfo(package_id, &handle);
    TryReturn(res == PMINFO_R_OK, PRIV_CHECKER_ERR_INTERNAL_ERROR, "[PRIV_CHECKER_ERR_INTERNAL_ERROR] pkgmgrinfo_pkginfo_get_pkginfo is failed.");

    user_data = strdup(privilege_name);
    if(res != PMINFO_R_OK)
    {
        free(user_data);
        LOGE("[PRIV_CHECKER_ERR_OUT_OF_MEMORY] user_data's strdup is failed.");
        return PRIV_CHECKER_ERR_OUT_OF_MEMORY;
    }
    res = pkgmgrinfo_pkginfo_foreach_privilege(handle, __privilege_checker_privilege_list_by_pkgid_cb, user_data);
    pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
    if(user_data != NULL)
        free(user_data);

    TryReturn(res == PMINFO_R_OK, PRIV_CHECKER_ERR_INTERNAL_ERROR, "[PRIV_CHECKER_ERR_INTERNAL_ERROR] pkgmgrinfo_pkginfo_foreach_privilege is failed.");

    LOGD("Print cache table");
    __privilege_checker_print_cache_table();

    TryReturn(g_isMached == true, PRIV_CHECKER_ERR_INVALID_PRIVILEGE, "[PRIV_CHECKER_ERR_INVALID_PRIVILEGE] %s privilege denied", privilege_name);

    return PRIV_CHECKER_ERR_NONE;
}

