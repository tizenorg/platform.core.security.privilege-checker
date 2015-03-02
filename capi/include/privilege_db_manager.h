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

#ifndef __PRIVILEGE_DB_MANAGER_H
#define __PRIVILEGE_DB_MANAGER_H

#include <tizen.h>
#include <glib.h>

typedef struct
{
    int profile_id;
    char* profile;
    int package_type_id;
    char* package_type;
    char* privilege_name;
    char* privilege_display;
    char* privilege_description;
    int privilege_level_id;
    char* privilege_level;
    char* version;
} privilege_info_db_row_s;

typedef enum
{
    PRIVILIEGE_DB_MANAGER_ERR_NONE                  = TIZEN_ERROR_NONE,
    PRIVILIEGE_DB_NO_EXIST_RESULT                   = TIZEN_ERROR_NO_DATA,
    PRIVILIEGE_DB_MANAGER_ERR_CONNECTION_FAIL       = TIZEN_ERROR_UNKNOWN,
    PRIVILIEGE_DB_MANAGER_ERR_INVALID_QUERY         = TIZEN_ERROR_INVALID_PARAMETER,
    PRIVILIEGE_DB_MANAGER_ERR_OUT_OF_MEMORY         = TIZEN_ERROR_OUT_OF_MEMORY,
} privilege_db_manager_error_e;

typedef enum
{
    PRIVILIEGE_DB_MANAGER_PRIVILEGE_LEVEL_PUBLIC    =   0,
    PRIVILIEGE_DB_MANAGER_PRIVILEGE_LEVEL_PARTNER   =   1,
    PRIVILIEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM  =   2
} privilege_db_manager_privilege_level_e;

typedef enum
{
    PRIVILIEGE_DB_MANAGER_PACKAGE_TYPE_WRT          =   0,
    PRIVILIEGE_DB_MANAGER_PACKAGE_TYPE_CORE         =   1
} privilege_db_manager_package_type_e;

#ifdef __cplusplus
    extern "C" {
#endif

int privilege_db_manager_get_privilege_list(GList** privilege_list);

int privilege_db_manager_get_privilege_display(privilege_db_manager_package_type_e package_type, const char* privilege_name, const char* api_version, char** privilege_display);

int privilege_db_manager_get_privilege_description(privilege_db_manager_package_type_e package_type, const char* privilege_name, const char* api_version, char** privilege_description);

int privilege_db_manager_get_privilege_group_display(privilege_db_manager_package_type_e package_type, const char* privilege_name, const char* api_version, int* privilege_group_number);

#ifdef __cplusplus
    }
#endif

#endif // __PRIVILEGE_DB_MANAGER_H
