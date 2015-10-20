/*
 * Copyright (c) 2013-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
	char* issued_version;
	char* expired_version;
	char* changed_to;
} privilege_info_db_row_s;

typedef enum
{
	PRIVILEGE_DB_MANAGER_ERR_NONE				= 0,
	PRIVILEGE_DB_NO_EXIST_RESULT				= 1,
	PRIVILEGE_DB_MANAGER_ERR_CONNECTION_FAIL	= 2,
	PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY		= 3,
	PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY		= 4,
} privilege_db_manager_error_e;

typedef enum
{
	PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PUBLIC		=   0,
	PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PARTNER	=   1,
	PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM	=   2
} privilege_db_manager_privilege_level_e;

typedef enum
{
	PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT	=   0,
	PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE	=   1,
	PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_OSP	=	2
} privilege_db_manager_package_type_e;

#ifdef __cplusplus
	extern "C" {
#endif

int privilege_db_manager_get_privilege_list(const char* api_version, privilege_db_manager_package_type_e package_type, GList** privilege_list);

int privilege_db_manager_get_privilege_display(privilege_db_manager_package_type_e package_type, const char* privilege_name, const char* api_version, char** privilege_display);

int privilege_db_manager_get_privilege_description(privilege_db_manager_package_type_e package_type, const char* privilege_name, const char* api_version, char** privilege_description);

int privilege_db_manager_get_privilege_display_by_name(privilege_db_manager_package_type_e package_type, const char* privilege_name, char** privilege_display);

int privilege_db_manager_get_privilege_description_by_name(privilege_db_manager_package_type_e package_type, const char* privilege_name, char** privilege_description);

int privilege_db_manager_get_privilege_group_display(privilege_db_manager_package_type_e package_type, const char* privilege_name, const char* api_version, int* privilege_group_number);

#ifdef __cplusplus
}
#endif

#endif // __PRIVILEGE_DB_MANAGER_H
