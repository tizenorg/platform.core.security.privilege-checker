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

#ifndef __TIZEN_SECURITY_PRIVILEGE_MANAGER_H
#define __TIZEN_SECURITY_PRIVILEGE_MANAGER_H

#include <glib.h>
#include "privilege_info.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    PRVMGR_PACKAGE_VISIBILITY_PUBLIC        =  1 << 1,
    PRVMGR_PACKAGE_VISIBILITY_PARTNER       =  1 << 2,
    PRVMGR_PACKAGE_VISIBILITY_PLATFORM      =  1 << 3,
} privilege_manager_visibility_e;

typedef enum
{
    PRVMGR_PACKAGE_TYPE_WRT     =  0,
    PRVMGR_PACKAGE_TYPE_CORE    =  1,
} privilege_manager_package_type_e;



/**
 * @brief verify privilege in the privilege list by checking its name and level at given api version.
 * @remarks @a privilege_list must be released with free() by you.
 * @remarks @a error_message must be released with free() by you.
 * @param [in] api_version The api version of the application
 * @param [in] privilege_manager_package_type_e The privilege_manager_package_type_e
 * @param [in] privilege_list The privilege_list
 * @param [in] privilege_manager_visibility_e The privilege_manager_visibility_e
 * @param [out] error_message The error_message's value is assigned when the return value isn't PRVMGR_ERR_NONE, otherwise null is assigned.
 * @return 0 on success, otherwise a negative error value.
 * @retval #PRVMGR_ERR_NONE Successful
 * @retval #PRVMGR_ERR_OUT_OF_MEMORY Out of memory
 * @retvak #PRVMGR_ERR_INVALID_PRIVILEGE The privilege_list contains invalid privilege(s).
 * @retval #PRVMGR_ERR_INVALID_PARAMETER Invalid parameter
 * @retval #PRVMGR_ERR_INTERNAL_ERROR Internal error
 */
EXPORT_API int privilege_manager_verify_privilege(const char* api_version, privilege_manager_package_type_e package_type, GList* privilege_list, privilege_manager_visibility_e visibility, char **error_message);

#ifdef __cplusplus
}
#endif

#endif // __TIZEN_SECURITY_PRIVILEGE_MANAGER_H

