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

#ifndef __TIZEN_SECURITY_PRIVILEGE_INFO_H
#define __TIZEN_SECURITY_PRIVILEGE_INFO_H

#include <glib.h>
#include "privilege_manager.h"

#ifndef EXPORT_API
#define EXPORT_API __attribute__((__visibility__("default")))
#endif
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Called to get a privilege group once for specific package.
 * @param[in] privilege_group The privilege group
 * @param[in] user_data The user data passed from the foreach function
 * @return 0 if success, nonzero value(>0) if fail. Callback is not called if return value is nonzero.
 * @pre privilege_info_foreach_privilege_group_list_by_pkgid() will invoke this callback.
 * @see privilege_info_foreach_privilege_group_list_by_pkgid()
 */
typedef int (*privilege_info_privilege_groups_cb) (const char *privilege_group, void* user_data);

/**
 * @brief Called to get a privilege in the privilege group once for specific package.
 * @param[in] privilege The privilege
 * @param[in] user_data The user data passed from the foreach function
 * @return 0 if success, nonzero value(>0) if fail. Callback is not called if return value is nonzero.
 * @pre privilege_info_foreach_privilege_list_by_pkgid_and_privilege_group() will invoke this callback.
 * @see privilege_info_foreach_privilege_list_by_pkgid_and_privilege_group()
 */
typedef int (*privilege_info_privileges_cb) (const char *privilege, void* user_data);

/**
 * @brief Retrieves privilege group list of the package.
 * @param [in] package_id The package id
 * @param [in] callback The callback function to invoke
 * @param [in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a nonzero error value.
 * @retval #PRVMGR_ERR_NONE Successful
 * @retval #PRVMGR_ERR_INTERNAL_ERROR Internal error
 * @retval #PRVMGR_ERR_INVALID_PARAMETER Invalid parameter
 * @post	This function invokes privilege_info_privilege_groups_cb() repeatedly for each application context.
 * @see privilege_info_privilege_groups_cb()
 */
EXPORT_API int privilege_info_foreach_privilege_group_list_by_pkgid(const char *package_id, privilege_info_privilege_groups_cb callback, void* user_data);

/**
 * @brief Retrieves privilege list in specific group of the package.
 * @param [in] package_id The package id
 * @param [in] callback The callback function to invoke
 * @param [in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a nonzero error value.
 * @retval #PRVMGR_ERR_NONE Successful
 * @retval #PRVMGR_ERR_INTERNAL_ERROR Internal error
 * @retval #PRVMGR_ERR_INVALID_PARAMETER Invalid parameter
 * @see privilege_info_privileges_cb()
 */
EXPORT_API int privilege_info_foreach_privilege_list_by_pkgid_and_privilege_group(const char *package_id, const char* privilege_group, privilege_info_privileges_cb callback, void* user_data);

/**
 * @brief Gets the display name of privilege group with the given privilege.
 * @remarks @a name must be released with free() by you.
 * @param [in] privilege_group The privilege group
 * @param [out] name The display name of the privilege group
 * @return 0 on success, otherwise a nonzero error value.
 * @retval #PRVMGR_ERR_NONE Successful
 * @retval #PRVMGR_ERR_OUT_OF_MEMORY Out of memory
 * @retval #PRVMGR_ERR_INVALID_PARAMETER Invalid parameter
 */
EXPORT_API int privilege_info_get_privilege_group_display_name(const char *privilege_group, char **name);

/**
 * @brief Gets the display name of privilege with the given privilege.
 * @remarks @a name must be released with free() by you.
 * @param [in] privilege The privilege
 * @param [out] name The display name of the privilege
 * @return 0 on success, otherwise a nonzero error value.
 * @retval #PRVMGR_ERR_NONE Successful
 * @retval #PRVMGR_ERR_OUT_OF_MEMORY Out of memory
 * @retval #PRVMGR_ERR_INVALID_PARAMETER Invalid parameter
 */
EXPORT_API int privilege_info_get_privilege_display_name(const char *privilege, char **name);

/**
 * @brief Gets the description of privilege with the given privilege.
 * @remarks @a description must be released with free() by you.
 * @param [in]  privilege The privilege
 * @param [out] description The description of the privilege
 * @return 0 on success, otherwise a nonzero error value.
 * @retval #PRVMGR_ERR_NONE Successful
 * @retval #PRVMGR_ERR_OUT_OF_MEMORY Out of memory
 * @retval #PRVMGR_ERR_INVALID_PARAMETER Invalid parameter
 */
EXPORT_API int privilege_info_get_privilege_description(const char *privilege, char **description);

/**
 * @brief Determines whether the given privilege is valid.
 * @param [in]  privilege The privilege
 * @return 1 if true(=is valid privilege. the privilege can be granted by security-manager and checked by cynara), 0 if false, and -1 on error
 */
EXPORT_API int privilege_info_is_valid(const char *privilege);

/**
 * @brief Determines whether the given privilege is internal privilege.
 * @param [in]  privilege The privilege
 * @return 1 if true(=is internal privilege), 0 if false, and -1 on error
 */
EXPORT_API int privilege_info_is_internal(const char *privilege);

/**
 * @brief Determines whether the given privilege is privacy related or not.
 * @param [in]  privilege The privilege
 * @return 1 if true(=is privacy related privilege), 0 if false, and -1 on error
 */
EXPORT_API int privilege_info_is_privacy(const char *privilege);

/**
 * @brief Gets all privacy list.
 * @remarks @a privacy_list must be released with g_list_free() by you.
 * @param [out]  privacy_list The privacy list
 * @return 0 on success, otherwise a nonzero error value.
 * @retval #PRVMGR_ERR_NONE Successful
 * @retval #PRVMGR_ERR_INTERNAL_ERROR Internal error
 */
EXPORT_API int privilege_info_get_privacy_list(GList **privacy_list);

/**
 * @brief Gets privacy display.
 * @remarks @a privacy_display must be released with g_list_free() by you.
 * @param [in] privacy The privacy
 * @param [out] privacy_display The privacy display
 * @return 0 on success, otherwise a nonzero error value.
 * @retval #PRVMGR_ERR_NONE Successful
 * @retval #PRVMGR_ERR_INTERNAL_ERROR Internal error
 */
EXPORT_API int privilege_info_get_privacy_display(const char* privacy, char** privacy_display);

/**
 * @brief Gets privilege list with the given privacy.
 * @remarks @a privilege_list must be released with g_list_free() by you.
 * @param [in]  privacy The privacy
 * @param [out] privilege_list The list of privileges mapped to the given privacy
 * @return 0 on success, otherwise a nonzero error value.
 * @retval #PRVMGR_ERR_NONE Successful
 * @retval #PRVMGR_ERR_INTERNAL_ERROR Internal error
 * @retval #PRVMGR_ERR_INVALID_PARAMETER Invalid parameter
 */
EXPORT_API int privilege_info_get_privilege_list_by_privacy(const char *privacy, GList **privilege_list);

/**
 * @brief Gets the list of privilges marked as black list.
 * @remarks @a privilege_list must be released with g_list_free() by you.
 * @param [in] uid The UID
 * @param [in] package_type The privilege_manager_package_type_e
 * @param [out] privilege_list The list for privileges marked as black list
 * @return 0 on success, otherwise a nonzero error value.
 * @retval #PRVMGR_ERR_NONE Successful
 * @retval #PRVMGR_ERR_INTERNAL_ERROR Internal error
 * @retval #PRVMGR_ERR_INVALID_PARAMETER Invalid parameter
 */
EXPORT_API int privilege_info_get_black_list(int uid, privilege_manager_package_type_e package_type, GList **privilege_list);

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_SECURITY_PRIVILEGE_INFO_H */
