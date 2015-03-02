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
    _PRV_GROUP_0,   // Hardware controls
    _PRV_GROUP_1,   // Location
    _PRV_GROUP_2,   // Network connections
    _PRV_GROUP_3,   // Paid services
    _PRV_GROUP_4,   // Personal information
    _PRV_GROUP_5,   // System settings
    _PRV_GROUP_6,   // Other privileges
    MAX_PRV_GROUP,
} privilegeGroup;

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
    const char* privilege;
    const char* privilege_level;
} core_privilege_info_s;


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

const static privilege_group_info_s const privilege_group_info_table[MAX_PRV_GROUP] =
{
    {"http://tizen.org/privilege/group/group0", _PRV_GROUP_0, "IDS_TPLATFORM_BODY_HARDWARE_CONTROLS_ABB"},
    {"http://tizen.org/privilege/group/group1", _PRV_GROUP_1, "IDS_TPLATFORM_OPT_LOCATION_T_LBS"},
    {"http://tizen.org/privilege/group/group2", _PRV_GROUP_2, "IDS_TPLATFORM_BODY_NETWORK_CONNECTIONS_ABB"},
    {"http://tizen.org/privilege/group/group3", _PRV_GROUP_3, "IDS_TPLATFORM_BODY_PAID_SERVICES_ABB"},
    {"http://tizen.org/privilege/group/group4", _PRV_GROUP_4, "IDS_TPLATFORM_BODY_PERSONAL_INFORMATION_ABB"},
    {"http://tizen.org/privilege/group/group5", _PRV_GROUP_5, "IDS_TPLATFORM_BODY_SYSTEM_SETTINGS_ABB"},
    {"http://tizen.org/privilege/group/group6", _PRV_GROUP_6, "IDS_TPLATFORM_BODY_OTHER_PRIVILEGES_ABB"},
};

#ifdef __cplusplus
}
#endif

#endif // __TIZEN_SECURITY_PRIVACY_INFO_TYPES_H
