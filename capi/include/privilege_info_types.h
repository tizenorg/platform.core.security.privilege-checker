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

#ifndef __TIZEN_SECURITY_PRIVILEGE_INFO_TYPES_H
#define __TIZEN_SECURITY_PRIVIELGE_INFO_TYPES_H

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _privilegeGroup
{
	_PRV_GROUP_1,
	_PRV_GROUP_2,
	_PRV_GROUP_3,
	MAX_PRV_GROUP,
} privilegeGroup;

#define PRIVILEGE_NUM 6
#define EXTRA_GROUP MAX_PRV_GROUP - 1

typedef struct
{
	const char* privilege;
	const char* name_string_id;
	const char* description_string_id;
	privilegeGroup privilege_group_enum;

} privilege_info_s;

typedef struct
{
	const char* privilege_group;
	privilegeGroup privilege_group_enum;
	const char* name_string_id;

} privilege_group_info_s;

typedef struct
{
	int privilege_group;
	privilege_info_privileges_cb callback;
	void* user_data;
} privilege_list_cb_data;

const static privilege_group_info_s privilege_group_info_table[MAX_PRV_GROUP] =
{
	{"http://tizen.org/privilege/group1", _PRV_GROUP_1, "IDS_PRIVILEGE_GROUP_NAME1"},
	{"http://tizen.org/privilege/group2", _PRV_GROUP_2, "IDS_PRIVILEGE_GROUP_NAME2"},
	{"http://tizen.org/privilege/group3", _PRV_GROUP_3, "IDS_PRIVILEGE_GROUP_NAME3"},
};

const static privilege_info_s privilege_info_table[PRIVILEGE_NUM] =
{
	{"http://tizen.org/privilege/alarm", "IDS_PRIVILEGE_NAME_ALARM", "IDS_PRIVILEGE_DESC_ALARM", _PRV_GROUP_1},
	{"http://tizen.org/privilege/application.launch", "IDS_PRIVILEGE_NAME_APPLICATION_LAUNCH", "IDS_PRIVILEGE_DESC_APPLICATION_LAUNCH", _PRV_GROUP_1},
	{"http://tizen.org/privilege/application.kill", "IDS_PRIVILEGE_NAME_APPLICATION_KILL", "IDS_PRIVILEGE_DESC_APPLICATION_KILL", _PRV_GROUP_1},
	{"http://tizen.org/privilege/appmanager.launch", "IDS_PRIVILEGE_NAME_APPMANAGER_LAUNCH", "IDS_PRIVILEGE_DESC_APPMANAGER_LAUNCH", _PRV_GROUP_2},
	{"http://tizen.org/privilege/appsetting", "IDS_PRIVILEGE_NAME_APPSETTING", "IDS_PRIVILEGE_DESC_APPSETTING", _PRV_GROUP_2},
	{"http://tizen.org/privilege/appusage", "IDS_PRIVILEGE_NAME_APPUSAGE", "IDS_PRIVILEGE_DESC_APPUSAGE", _PRV_GROUP_2},
};

#ifdef __cplusplus
}
#endif

#endif // __TIZEN_SECURITY_PRIVACY_INFO_TYPES_H
