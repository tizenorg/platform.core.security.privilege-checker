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

#ifndef __TIZEN_SECURITY_PRIVILEGE_CHECKER_H
#define __TIZEN_SECURITY_PRIVILEGE_CHECKER_H

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerations of error code for Privilege Information.
 */
typedef enum
{
    PRIV_CHECKER_ERR_NONE               = TIZEN_ERROR_NONE,
    PRIV_CHECKER_ERR_INVALID_PARAMETER  = TIZEN_ERROR_INVALID_PARAMETER,
    PRIV_CHECKER_ERR_OUT_OF_MEMORY      = TIZEN_ERROR_OUT_OF_MEMORY,
    PRIV_CHECKER_ERR_INTERNAL_ERROR     = TIZEN_ERROR_UNKNOWN,
    PRIV_CHECKER_ERR_NO_DATA            = TIZEN_ERROR_NO_DATA,
    PRIV_CHECKER_ERR_INVALID_PRIVILEGE  = TIZEN_ERROR_PERMISSION_DENIED,
} privilege_checker_error_e;

EXPORT_API int privilege_checker_check_package_privilege(const char* package_id, const char *privilege);

#ifdef __cplusplus
}
#endif

#endif // __TIZEN_SECURITY_PRIVILEGE_INFO_H
