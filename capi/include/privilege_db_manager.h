/*
 * Copyright(c)2013-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0(the License);
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

#include <glib.h>

typedef struct {
	int profile_id;
	char* profile;
	int package_type_id;
	char* package_type;
	char* privilege_name;
	char* privilege_display;
	char* privilege_description;
	int privilege_level_id;
	char* privilege_level;
	char* issued_version;
	char* expired_version;
	char* changed_to;
} privilege_info_db_row_s;

typedef enum {
	PRIVILEGE_DB_MANAGER_ERR_NONE                = 0,
	PRIVILEGE_DB_NO_EXIST_RESULT                 = 1,
	PRIVILEGE_DB_MANAGER_ERR_CONNECTION_FAIL     = 2,
	PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY       = 3,
	PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY       = 4,
	PRIVILEGE_DB_MANAGER_ERR_INVALID_TYPE        = 5,
	PRIVILEGE_DB_MANAGER_ERR_DB_BUSY_FAIL        = 6,
	PRIVILEGE_DB_MANAGER_ERR_DB_CONSTRAINT_FAIL  = 7,
	PRIVILEGE_DB_MANAGER_ERR_DB_FULL_FAIL        = 8,
	PRIVILEGE_DB_MANAGER_ERR_DB_UPDATE_FAIL      = 9,
} privilege_db_manager_error_e;

typedef enum {
	PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PUBLIC    =   0,
	PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PARTNER   =   1,
	PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM  =   2
} privilege_db_manager_privilege_level_e;

typedef enum {
	PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT         =   0,
	PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE        =   1
} privilege_db_manager_package_type_e;

#ifdef __cplusplus
	extern "C" {
#endif

int privilege_db_manager_get_privilege_list(const char* api_version, privilege_db_manager_package_type_e package_type, GList** privilege_list);

int privilege_db_manager_get_mapped_privilege_list(const char* api_version, privilege_db_manager_package_type_e package_type, GList* privilege_list, GList** mapped_privilege_list);

int privilege_db_manager_get_privilege_display(privilege_db_manager_package_type_e package_type, const char* privilege_name, const char* api_version, char** privilege_display);

int privilege_db_manager_get_privilege_description(privilege_db_manager_package_type_e package_type, const char* privilege_name, const char* api_version, char** privilege_description);

int privilege_db_manager_get_privilege_group_display(privilege_db_manager_package_type_e package_type, const char* privilege_name, const char* api_version, int* privilege_group_number);

int privilege_db_manager_get_privacy_display(const char *privacy, char **privacy_display);

int privilege_db_manager_is(char type, const char* privilege);
int __privilege_db_manager_get_privacy_list(GList** privacy_list);
int __privilege_db_manager_get_privilege_list_by_privacy(const char* privacy, GList **privilege_list);

int privilege_db_manager_get_black_list(int uid, privilege_db_manager_package_type_e package_type, GList** privilege_list);
int privilege_db_manager_set_black_list(int uid, privilege_db_manager_package_type_e pacakge_type, GList *privilege_list);
int privilege_db_manager_unset_black_list(int uid, privilege_db_manager_package_type_e package_type, GList *privilege_list);

/* Return the number of black-listed privileges in given privilege list */
int privilege_db_manager_check_black_list(int uid, privilege_db_manager_package_type_e pacakge_type, GList *privilege_list);

#ifdef __cplusplus
	}
#endif

#endif /* __PRIVILEGE_DB_MANAGER_H */
