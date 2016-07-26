#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "privilege_db_manager.h"
#include "privilege_manager.h"

#ifdef __TIZEN__
#include <dlog.h>
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "PRIVILEGE_MANAGER"
#define _LOGE(fmt, arg...) LOGE(fmt, ##arg)
#define _LOGD(fmt, arg...) LOGD(fmt, ##arg)
#define _LOGW(fmt, arg...) LOGW(fmt, ##arg)
#define _LOGI(fmt, arg...) LOGI(fmt, ##arg)
#endif
#else
#define _LOGE(fmt, arg...)
#define _LOGD(fmt, arg...)
#define _LOGW(fmt, arg...)
#define _LOGI(fmt, arg...)
#endif

#define MESSAGE_SIZE	512

#define API_VERSION_PADDING ".0.0"
#define API_VERSION_PADDING_LEN strlen(API_VERSION_PADDING)
#define MAX_API_VERSION_LEN 5

#define TryReturn(condition, expr, returnValue, ...)\
	if (!(condition)) { \
		_LOGE(__VA_ARGS__); \
		expr; \
		return returnValue; \
	}

static void __free_privilege_list(GList * privilege_list)
{
	GList *l = NULL;
	for (l = privilege_list; l != NULL; l = l->next) {
		privilege_info_db_row_s *privilege_info_db_row = (privilege_info_db_row_s *) l->data;
		if (privilege_info_db_row->profile != NULL)
			free(privilege_info_db_row->profile);
		if (privilege_info_db_row->package_type != NULL)
			free(privilege_info_db_row->package_type);
		if (privilege_info_db_row->privilege_name != NULL)
			free(privilege_info_db_row->privilege_name);
		if (privilege_info_db_row->privilege_display != NULL)
			free(privilege_info_db_row->privilege_display);
		if (privilege_info_db_row->privilege_description != NULL)
			free(privilege_info_db_row->privilege_description);
		if (privilege_info_db_row->privilege_level != NULL)
			free(privilege_info_db_row->privilege_level);
		if (privilege_info_db_row->issued_version != NULL)
			free(privilege_info_db_row->issued_version);
		if (privilege_info_db_row->expired_version != NULL)
			free(privilege_info_db_row->expired_version);
		if (privilege_info_db_row->changed_to != NULL)
			free(privilege_info_db_row->changed_to);
	}
}

static int __privilege_manager_check_privilege_list(const char *api_version, const char *privilege, GList * valid_privilege_list, int *privilege_level, char **changed_to, char **valid_api_version)
{
	TryReturn(privilege != NULL, , PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] privilege is NULL");
	int i, is_valid_version = 0;
	int ret_val = PRVMGR_ERR_NO_EXIST_PRIVILEGE;
	char *tmp_api_version = NULL;
	char *tmp_expired_version = NULL;
	char *tmp_issued_version = NULL;
	GList *l = NULL;
	for (l = valid_privilege_list; l != NULL; l = l->next) {
		privilege_info_db_row_s *privilege_info_db_row = (privilege_info_db_row_s *)l->data;
		if (strcmp(privilege_info_db_row->privilege_name, privilege) == 0) {
			_LOGD("Matched privilege name exist");
			_LOGD("Check api version");

			if (tmp_api_version != NULL) {
				free(tmp_api_version);
				tmp_api_version = NULL;
			}
			if (tmp_issued_version != NULL) {
				free(tmp_issued_version);
				tmp_issued_version = NULL;
			}
			if (tmp_expired_version != NULL) {
				free(tmp_expired_version);
				tmp_expired_version = NULL;
			}
			is_valid_version = 0;

			tmp_api_version = strdup(api_version);
			TryReturn(tmp_api_version != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] tmp_api_version's strdup is failed.");
			size_t new_size = snprintf(0, 0, "%s%s", tmp_api_version, API_VERSION_PADDING) + 1;
			tmp_api_version = realloc(tmp_api_version, new_size * sizeof(char));
			TryReturn(tmp_api_version != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] tmp_api_version's realloc is failed.");
			strncat(tmp_api_version, API_VERSION_PADDING, API_VERSION_PADDING_LEN);

			tmp_expired_version = strdup(privilege_info_db_row->expired_version);
			TryReturn(tmp_expired_version != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] tmp_expired_version's strdup is failed.");
			new_size = snprintf(0, 0, "%s%s", tmp_expired_version, API_VERSION_PADDING) + 1;
			tmp_expired_version = realloc(tmp_expired_version, new_size * sizeof(char));
			TryReturn(tmp_expired_version != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] tmp_expired_version's realloc is failed.");
			strncat(tmp_expired_version, API_VERSION_PADDING, API_VERSION_PADDING_LEN);

			tmp_issued_version = strdup(privilege_info_db_row->issued_version);
			TryReturn(tmp_issued_version != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] tmp_issued_version's strdup is failed.");
			new_size = snprintf(0, 0, "%s%s", tmp_issued_version, API_VERSION_PADDING) + 1;
			tmp_issued_version = realloc(tmp_issued_version, new_size * sizeof(char));
			TryReturn(tmp_issued_version != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] tmp_issued_version's realloc is failed.");
			strncat(tmp_issued_version, API_VERSION_PADDING, API_VERSION_PADDING_LEN);

			if (strncmp(tmp_api_version, tmp_expired_version, MAX_API_VERSION_LEN) == 0) {
				is_valid_version = 1;
			} else {
				for (i = 0; is_valid_version == 0 && i < MAX_API_VERSION_LEN; i++) {
					if (tmp_api_version[i] > tmp_expired_version[i])
						is_valid_version = 1;
					else if (tmp_api_version[i] < tmp_expired_version[i])
						break;
				}
			}

			for (i = 0; is_valid_version == 0 && i < MAX_API_VERSION_LEN; i++) {
				if (tmp_api_version[i] < tmp_issued_version[i])
					is_valid_version = 2;
				else if (tmp_api_version[i] > tmp_issued_version[i])
					break;
			}

			if (is_valid_version == 0) {
				*privilege_level = privilege_info_db_row->privilege_level_id;
				ret_val = PRVMGR_ERR_NONE;
				goto FINISH;
			} else if (is_valid_version == 1) {
				_LOGD("%s is invalid in tizen version: %s", privilege_info_db_row->privilege_name, api_version);
				_LOGD("privilege deprecated version is equal to or lower than api version");
				if (*valid_api_version != NULL) {
					free(*valid_api_version);
					*valid_api_version = NULL;
				}
				*valid_api_version = strdup(privilege_info_db_row->expired_version);
				TryReturn(valid_api_version != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] valid_api_version's strdup is failed.");

				if (privilege_info_db_row->changed_to != NULL && strcmp(privilege_info_db_row->changed_to, "") != 0) {
					if (*changed_to != NULL) {
						free(*changed_to);
						*changed_to = NULL;
					}
					_LOGD("%s was changed to %s.", privilege, privilege_info_db_row->changed_to);
					*changed_to = strdup(privilege_info_db_row->changed_to);
					TryReturn(changed_to != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] changed_to's strdup is failed.");
				}

				ret_val = PRVMGR_ERR_DEPRECATED_PRIVILEGE;
			} else if (is_valid_version == 2) {
				_LOGD("%s is invalid in tizen version: %s", privilege_info_db_row->privilege_name, api_version);
				_LOGD("privilege issued version is higher than api version");
				if (*valid_api_version != NULL) {
					free(*valid_api_version);
					*valid_api_version = NULL;
				}
				*valid_api_version = strdup(privilege_info_db_row->issued_version);
				TryReturn(valid_api_version != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] valid_api_version's strdup is failed.");

				ret_val = PRVMGR_ERR_NO_EXIST_PRIVILEGE;
			}
		}
	}

 FINISH:
	free(tmp_issued_version);
	free(tmp_expired_version);
	free(tmp_api_version);
	return ret_val;
}

const char *__get_privilege_level_string(privilege_db_manager_privilege_level_e privilege_db_manager_privilege_level)
{
	if (privilege_db_manager_privilege_level == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PUBLIC)
		return "public";
	else if (privilege_db_manager_privilege_level == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PARTNER)
		return "partner";
	else if (privilege_db_manager_privilege_level == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM)
		return "platform";
	else
		return "not defined privilege";
}

int __check_api_version_validity(const char *api_version)
{

	int i;
	int is_valid_version_type = 1;
	int api_version_size = strlen(api_version);
	if (api_version_size % 2 == 1 && (3 <= api_version_size && api_version_size <= 7)) {
		for (i = 0; i < api_version_size; i++) {
			if (i % 2 == 0) {
				if (!('0' <= api_version[i] && api_version[i] <= '9'))
					is_valid_version_type = 0;
			} else {
				if (api_version[i] != '.')
					is_valid_version_type = 0;
			}
		}
	} else {
		is_valid_version_type = 0;
	}

	if (is_valid_version_type == 0)
		return PRVMGR_ERR_INVALID_PARAMETER;

	return PRVMGR_ERR_NONE;
}

int privilege_manager_verify_privilege(const char *api_version, privilege_manager_package_type_e package_type, GList * privilege_list, privilege_manager_visibility_e visibility, char **error_message)
{
	_LOGD("privilege_manager_verify_privilege called");
	GList *l;
	int ret;
	int ret_val = PRVMGR_ERR_NONE;
	char *message_list = NULL;
	char *noexist_message = NULL;
	char *deprecated_message = NULL;
	char *mismatched_message = NULL;
	char message[MESSAGE_SIZE] = { 0, };
	char guide_message[MESSAGE_SIZE] = { 0, };
	char *changed_to = NULL;
	char *valid_api_version = NULL;
	GList *valid_privilege_list;
	char *wrt_active_version = "2.3.1";
	int is_valid_wrt_version = 1;
	char *pkg_type = NULL;
	int i = 0;

	/* Check invalid parameters */
	if (api_version == NULL) {
		_LOGE("[PRVMGR_ERR_INVALID_PARAMETER] api_version is NULL");
		*error_message = strdup("[PRVMGR_ERR_INVALID_PARAMETER] api_version is NULL");
		TryReturn(error_message != NULL, , PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");

		return PRVMGR_ERR_INVALID_PARAMETER;
	} else {
		ret = __check_api_version_validity(api_version);
		if (ret != PRVMGR_ERR_NONE) {
			_LOGE("[PRVMGR_ERR_INVALID_PARAMETER] %s is in invalid form. api_version form should be X.X, X.X.X or X.X.X.X(X=integer)", api_version);
			*error_message = strdup("[PRVMGR_ERR_INVALID_PARAMETER] api_version form should be a X.X, X.X.X or X.X.X.X(X=integer)");
			TryReturn(error_message != NULL, , PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed");
			return ret;
		}
	}

	if (package_type != PRVMGR_PACKAGE_TYPE_WRT && package_type != PRVMGR_PACKAGE_TYPE_CORE) {
		_LOGD("checking package type = %d", package_type);
		_LOGE("[PRVMGR_ERR_INVALID_PARAMETER] package_type is not a PRVMGR_PACKAGE_TYPE_WRT or PRVMGR_PACKAGE_TYPE_CORE");
		*error_message = strdup("[PRVMGR_ERR_INVALID_PARAMETER] package_type is a unknown type. package_type must be a PRVMGR_PACKAGE_TYPE_WRT or PRVMGR_PACKAGE_TYPE_CORE");
		TryReturn(error_message != NULL, , PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");

		return PRVMGR_ERR_INVALID_PARAMETER;
	}
	if (package_type == PRVMGR_PACKAGE_TYPE_WRT) {
		char *tmp_api_version = strdup(api_version);
		TryReturn(tmp_api_version != NULL, , PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] tmp_api_version's strdup is failed.");
		size_t new_size = snprintf(0, 0, "%s%s", tmp_api_version, API_VERSION_PADDING) + 1;
		tmp_api_version = realloc(tmp_api_version, new_size * sizeof(char));
		TryReturn(tmp_api_version != NULL, free(tmp_api_version), PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] tmp_api_version's realloc is failed.");
		strncat(tmp_api_version, API_VERSION_PADDING, API_VERSION_PADDING_LEN);
		for (i = 0; is_valid_wrt_version == 1 && i < MAX_API_VERSION_LEN; i++) {
			if (tmp_api_version[i] < wrt_active_version[i])
				is_valid_wrt_version = 0;
			else if (tmp_api_version[i] > wrt_active_version[i])
				break;
		}
		pkg_type = strdup("WRT");
		TryReturn(pkg_type != NULL, free(tmp_api_version), PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] pkg_type's strdup is failed.");
		_LOGD("package type = %s, api version %s, is valid wrt version %d", pkg_type, api_version, is_valid_wrt_version);
		snprintf(guide_message, MESSAGE_SIZE, "Check config.xml| - Current required_version(=api version) = %s, |   ", api_version);
		free(tmp_api_version);
	} else if (package_type == PRVMGR_PACKAGE_TYPE_CORE) {
		pkg_type = strdup("Native");
		TryReturn(pkg_type != NULL, , PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] pkg_type's strdup is failed.");
		snprintf(guide_message, MESSAGE_SIZE, "Check tizen-manifest.xml| - Current api-version = %s, |   ", api_version);
	}

	if ((visibility & PRVMGR_PACKAGE_VISIBILITY_PUBLIC) != PRVMGR_PACKAGE_VISIBILITY_PUBLIC && (visibility & PRVMGR_PACKAGE_VISIBILITY_PARTNER) != PRVMGR_PACKAGE_VISIBILITY_PARTNER && (visibility & PRVMGR_PACKAGE_VISIBILITY_PLATFORM) != PRVMGR_PACKAGE_VISIBILITY_PLATFORM) {

		_LOGE("[PRVMGR_ERR_INVALID_PARAMETER] visibility don't include any public, partner, platform");
		*error_message = strdup("[INVALID_PARAMETER] Signature Level is invalid. Signature Level must be a public, partner or platform");
		TryReturn(error_message != NULL, free(pkg_type), PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");
		free(pkg_type);
		return PRVMGR_ERR_INVALID_PARAMETER;
	}

	if ((visibility & PRVMGR_PACKAGE_VISIBILITY_PUBLIC) == PRVMGR_PACKAGE_VISIBILITY_PUBLIC)
		strncat(guide_message, "certificate signature level = public||", strlen("certificate signature level = public||"));
	else if ((visibility & PRVMGR_PACKAGE_VISIBILITY_PARTNER) == PRVMGR_PACKAGE_VISIBILITY_PARTNER)
		strncat(guide_message, "certificate signature level = partner||", strlen("certificate signature level = partner||"));
	else
		strncat(guide_message, "certificate signature level = platform||", strlen("certificate signature level = platform||"));

	if (privilege_list == NULL) {
		_LOGE("[PRVMGR_ERR_INVALID_PARAMETER] privilege_list is NULL");
		*error_message = strdup("[PRVMGR_ERR_INVALID_PARAMETER] privilege_list is NULL");
		TryReturn(error_message != NULL, free(pkg_type), PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");

		free(pkg_type);
		return PRVMGR_ERR_INVALID_PARAMETER;
	}

	/* Check black list */
	int uid = getuid();
	ret = privilege_db_manager_check_black_list(uid, package_type, privilege_list);
	if (ret > 0) {
		_LOGE("[PRVMGR_ERR_USING_BANNED_PRIVILEGE] Application manifest contains banned privilege(s) declared by DPM");
		*error_message = strdup("[PRVMGR_ERR_USING_BANNED_PRIVILEGE] Application manifest contains banned privilege(s) declared by the DPM");
		return PRVMGR_ERR_USING_BANNED_PRIVILEGE;
	}

	/* Get valid privilege list */
	ret = privilege_db_manager_get_privilege_list(api_version, package_type, &valid_privilege_list);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE) {
		_LOGE("[FAIL TO CALL FUNCTION] privilege_db_manager_get_privilege_list()");
		*error_message = strdup("[PRVMGR_ERR_INTERNAL_ERROR] failed to get privilege list from DB");
		free(pkg_type);
		return PRVMGR_ERR_INTERNAL_ERROR;
	}
	/* Compare received privilege with valid privilege list */
	for (l = privilege_list; l != NULL; l = l->next) {
		int privilege_level_id = PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PUBLIC;
		char *privilege_name = (char *)l->data;

		_LOGD("Checking privilege = %s", privilege_name);
		if (valid_api_version != NULL) {
			free(valid_api_version);
			valid_api_version = NULL;
		}
		if (changed_to != NULL) {
			free(changed_to);
			changed_to = NULL;
		}
		ret = __privilege_manager_check_privilege_list(api_version, privilege_name, valid_privilege_list, &privilege_level_id, &changed_to, &valid_api_version);

		if (is_valid_wrt_version == 0)
			ret = PRVMGR_ERR_NONE;

		if (ret == PRVMGR_ERR_NO_EXIST_PRIVILEGE) {
			memset(message, 0, MESSAGE_SIZE);
			if (valid_api_version != NULL && strcmp(valid_api_version, "") != 0) {
				_LOGE("[PRVMGR_ERR_NO_EXIST_PRIVILEGE]%s %s privilege is valid from Tizen version %s and your api version is %s. Use at least api version %s or remove the privilege.", pkg_type, privilege_name, valid_api_version, api_version, valid_api_version);
				snprintf(message, MESSAGE_SIZE, " - %s|   >> Use at least api version %s or remove the privilege.|", privilege_name, valid_api_version);
			} else {
				_LOGE("[PRVMGR_ERR_NO_EXIST_PRIVILEGE]%s %s is an invalid privilege. Check spelling or remove the privilege.", pkg_type, privilege_name);
				snprintf(message, MESSAGE_SIZE, " - %s|   >> Check spelling or remove the privilege.|", privilege_name);
			}

			if (noexist_message == NULL) {
				noexist_message = strdup("");
				TryReturn(noexist_message != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] noexist_message's strdup is failed.");
			}
			size_t new_size = snprintf(0, 0, "%s%s", noexist_message, message) + 1;
			noexist_message = realloc(noexist_message, new_size * sizeof(char));
			TryReturn(noexist_message != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] noexist_message's realloc is failed.");

			strncat(noexist_message, message, strlen(message));

			ret_val = PRVMGR_ERR_INVALID_PRIVILEGE;

		} else if (ret == PRVMGR_ERR_DEPRECATED_PRIVILEGE) {

			memset(message, 0, MESSAGE_SIZE);
			if (changed_to != NULL && strcmp(changed_to, "") != 0) {
				_LOGE("[PRVMGR_ERR_DEPRECATED_PRIVILEGE]%s %s is a deprecated since Tizen version %s and your api version is %s. Use %s instead or use api version lower than %s.", pkg_type, privilege_name, valid_api_version, api_version, changed_to, valid_api_version);
				snprintf(message, MESSAGE_SIZE, " - %s|   >> Use %s instead of it or use api version lower than %s.|", privilege_name, changed_to, valid_api_version);
			} else {
				_LOGE("[PRVMGR_ERR_DEPRECATED_PRIVILEGE]%s %s is deprecated since Tizen version %s and your api version is %s. Remove the privilege.", pkg_type, privilege_name, valid_api_version, api_version);
				snprintf(message, MESSAGE_SIZE, " - %s|   >> Remove the privilege.|", privilege_name);
			}

			if (deprecated_message == NULL) {
				deprecated_message = strdup("");
				TryReturn(deprecated_message != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] deprecated_message's strdup is failed.");
			}
			size_t new_size = snprintf(0, 0, "%s%s", deprecated_message, message) + 1;
			deprecated_message = realloc(deprecated_message, new_size * sizeof(char));
			TryReturn(deprecated_message != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] deprecated_message's realloc is failed.");

			strncat(deprecated_message, message, strlen(message));

			ret_val = PRVMGR_ERR_INVALID_PRIVILEGE;

		} else if (ret == PRVMGR_ERR_NONE) {
			_LOGD("visibility = %d", visibility);
			_LOGD("privilege level = %d", privilege_level_id);

			if ((visibility & PRVMGR_PACKAGE_VISIBILITY_PUBLIC) == PRVMGR_PACKAGE_VISIBILITY_PUBLIC) {
				if (privilege_level_id == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PARTNER || privilege_level_id == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM) {
					_LOGE("[PRVMGR_ERR_MISMATCHED_PRIVILEGE_LEVEL] Visibility and Privilege level are mismatched");
					_LOGE("[PRVMGR_ERR_MISMATCHED_PRIVILEGE_LEVEL] Visibility = public, Privilege Level = %s", __get_privilege_level_string(privilege_level_id));

					memset(message, 0, MESSAGE_SIZE);
					snprintf(message, MESSAGE_SIZE, " - %s|   >> Use at least %s signatured certificate.|", privilege_name, __get_privilege_level_string(privilege_level_id));

					if (mismatched_message == NULL) {
						mismatched_message = strdup("");
						TryReturn(mismatched_message != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] mismatched_message's strdup is failed.");
					}
					size_t new_size = snprintf(0, 0, "%s%s", mismatched_message, message) + 1;
					mismatched_message = realloc(mismatched_message, new_size * sizeof(char));
					TryReturn(mismatched_message != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] mismatched_message's realloc is failed.");
					strncat(mismatched_message, message, strlen(message));

					ret_val = PRVMGR_ERR_INVALID_PRIVILEGE;
				}
			} else if ((visibility & PRVMGR_PACKAGE_VISIBILITY_PARTNER) == PRVMGR_PACKAGE_VISIBILITY_PARTNER) {
				if (privilege_level_id == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM) {
					_LOGE("[PRVMGR_ERR_MISMATCHED_PRIVILEGE_LEVEL] Visibility and Privilege level are mismatched");
					_LOGE("[PRVMGR_ERR_MISMATCHED_PRIVILEGE_LEVEL] Visibility = partner, Privilege Level = %s", __get_privilege_level_string(privilege_level_id));

					memset(message, 0, MESSAGE_SIZE);
					snprintf(message, MESSAGE_SIZE, " - %s|   >> Use at least %s signatured certificate.|", privilege_name, __get_privilege_level_string(privilege_level_id));

					if (mismatched_message == NULL) {
						mismatched_message = strdup("");
						TryReturn(mismatched_message != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] mismatched_message's strdup is failed.");
					}
					size_t new_size = snprintf(0, 0, "%s%s", mismatched_message, message) + 1;
					mismatched_message = realloc(mismatched_message, new_size * sizeof(char));
					TryReturn(mismatched_message, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] mismatched_message's realloc is failed.");
					strncat(mismatched_message, message, strlen(message));
					ret_val = PRVMGR_ERR_INVALID_PRIVILEGE;
				}
			}
		} else if (ret == PRVMGR_ERR_INVALID_PARAMETER) {
			_LOGE("[PRVMGR_ERR_INVALID_PARAMETER] privilege_name is NULL");
			*error_message = strdup("[INVALID_PARAMETER] Invalid parameter was passed.|");
			TryReturn(error_message != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");

			ret_val = PRVMGR_ERR_INVALID_PARAMETER;
			goto FINISH;
		} else if (ret == PRVMGR_ERR_INTERNAL_ERROR) {
			_LOGE("[PRVMGR_ERR_INVALID_PARAMETER] Unknown Error occured.");
			*error_message = strdup("[INTERNAL_ERROR] Unknown Error occured.|");
			TryReturn(error_message != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");

			ret_val = PRVMGR_ERR_INTERNAL_ERROR;
			goto FINISH;
		}
	}
	char *newline = "|";

	if (ret_val != PRVMGR_ERR_NONE) {
		message_list = strdup(guide_message);
		TryReturn(message_list != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] message_list's strdup is failed.");
		if (noexist_message != NULL) {
			size_t new_size = snprintf(0, 0, "%s[NO_EXIST_PRIVILEGE]|%s", message_list, noexist_message) + 1;
			message_list = realloc(message_list, new_size);
			TryReturn(message_list != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] message_list's realloc is failed.");
			strncat(message_list, "[NO_EXIST_PRIVILEGE]|", strlen("[NO_EXIST_PRIVILEGE]|"));
			strncat(message_list, noexist_message, strlen(noexist_message));
		}
		if (deprecated_message != NULL) {
			size_t new_size = snprintf(0, 0, "%s[DEPRECATED_PRIVILEGE]|%s", message_list, deprecated_message) + 1;
			message_list = realloc(message_list, new_size);
			TryReturn(message_list != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] message_list's realloc is failed.");
			strncat(message_list, "[DEPRECATED_PRIVILEGE]|", strlen("[DEPRECATED_PRIVILEGE]|"));
			strncat(message_list, deprecated_message, strlen(deprecated_message));
		}
		if (mismatched_message != NULL) {
			size_t new_size = snprintf(0, 0, "%s[MISMATCHED_PRIVILEGE_LEVEL]|%s", message_list, mismatched_message) + 1;
			message_list = realloc(message_list, new_size);
			TryReturn(message_list != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] message_list's realloc is failed.");
			strncat(message_list, "[MISMATCHED_PRIVILEGE_LEVEL]|", strlen("[MISMATCHED_PRIVILEGE_LEVEL]|"));
			strncat(message_list, mismatched_message, strlen(mismatched_message));
		}
		size_t total_size = snprintf(0, 0, "%s%s", message_list, newline) + 1;
		message_list = realloc(message_list, total_size);
		TryReturn(message_list != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] message_list's realloc is failed.");
		strncat(message_list, newline, strlen(newline));
		*error_message = strdup(message_list);
		TryReturn(error_message != NULL, ret_val = PRVMGR_ERR_OUT_OF_MEMORY; goto FINISH, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");
	}

 FINISH:
	free(message_list);
	free(deprecated_message);
	free(mismatched_message);
	free(noexist_message);
	free(changed_to);
	free(valid_api_version);
	free(pkg_type);
	__free_privilege_list(valid_privilege_list);
	g_list_free(valid_privilege_list);
	return ret_val;
}

int privilege_manager_get_mapped_privilege_list(const char *api_version, privilege_manager_package_type_e package_type, GList * privilege_list, GList ** mapped_privilege_list)
{
	int ret;
	TryReturn(api_version != NULL, , PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] api_version is NULL.");
	ret = __check_api_version_validity(api_version);
	TryReturn(ret == PRVMGR_ERR_NONE, , PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] %s is in invalid form. api_version form should be X.X, X.X.X or X.X.X.X(X=integer)", api_version);

	if (package_type != PRVMGR_PACKAGE_TYPE_WRT && package_type != PRVMGR_PACKAGE_TYPE_CORE) {
		_LOGE("[PRVMGR_ERR_INVALID_PARAMETER] package_type is not a PRVMGR_PACKAGE_TYPE_WRT or PRVMGR_PACKAGE_TYPE_CORE");
		return PRVMGR_ERR_INVALID_PARAMETER;
	}

	TryReturn(privilege_list != NULL, , PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] privilege_list is NULL.");
	ret = privilege_db_manager_get_mapped_privilege_list(api_version, package_type, privilege_list, mapped_privilege_list);
	TryReturn(ret == PRVMGR_ERR_NONE, , PRVMGR_ERR_INTERNAL_ERROR, "[PRVMGR_ERR_INTERNAL_ERROR] privilege_db_manager_get_mapped_privilege_list failed, ret = %d", ret);
	return ret;
}

int privilege_manager_set_black_list(int uid, privilege_manager_package_type_e package_type, GList *privilege_list)
{
	TryReturn(privilege_list != NULL, , PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] privilege_list is NULL.");
	TryReturn(package_type == PRVMGR_PACKAGE_TYPE_WRT || package_type == PRVMGR_PACKAGE_TYPE_CORE, , PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] Invalid package_type = %d.", package_type);
	int ret = privilege_db_manager_set_black_list(uid, package_type, privilege_list);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		ret = PRVMGR_ERR_INTERNAL_ERROR;
	return ret;
}

int privilege_manager_unset_black_list(int uid, privilege_manager_package_type_e package_type, GList *privilege_list)
{
	TryReturn(privilege_list != NULL, , PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] privilege_list is NULL.");
	TryReturn(package_type == PRVMGR_PACKAGE_TYPE_WRT || package_type == PRVMGR_PACKAGE_TYPE_CORE, , PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] Invalid package_type = %d.", package_type);
	int ret = privilege_db_manager_unset_black_list(uid, package_type, privilege_list);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		ret = PRVMGR_ERR_INTERNAL_ERROR;
	return ret;
}
