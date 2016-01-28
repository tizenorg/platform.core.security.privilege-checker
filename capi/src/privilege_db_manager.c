#include "privilege_db_manager.h"
#include <sqlite3.h>
#include <dlog.h>
#include <stdio.h>
#include <stdlib.h>
#define PRIVILEGE_INFO_CORE_DB_PATH "/usr/share/privilege-manager/.core_privilege_info.db"
#define PRIVILEGE_INFO_WRT_DB_PATH "/usr/share/privilege-manager/.wrt_privilege_info.db"
#define PRIVILEGE_MAPPING_CORE_DB_PATH "/usr/share/privilege-manager/.core_privilege_mapping.db"
#define PRIVILEGE_MAPPING_WRT_DB_PATH "/usr/share/privilege-manager/.wrt_privilege_mapping.db"

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "PRIVILEGE_DB_MANAGER"
#endif

typedef enum {
	PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON = 0,
	PRIVILEGE_DB_MANAGER_PROFILE_TYPE_MOBILE = 1,
	PRIVILEGE_DB_MANAGER_PROFILE_TYPE_WEARABLE = 2,
	PRIVILEGE_DB_MANAGER_PROFILE_TYPE_TV = 3
} privilege_db_manager_profile_type_e;

#ifdef PROFILE_TYPE_MOBILE
static privilege_db_manager_profile_type_e g_privilege_db_manager_profile_type = PRIVILEGE_DB_MANAGER_PROFILE_TYPE_MOBILE;
#elif PROFILE_TYPE_WEARABLE
static privilege_db_manager_profile_type_e g_privilege_db_manager_profile_type = PRIVILEGE_DB_MANAGER_PROFILE_TYPE_WEARABLE;
#elif PROFILE_TYPE_TV
static privilege_db_manager_profile_type_e g_privilege_db_manager_profile_type = PRIVILEGE_DB_MANAGER_PROFILE_TYPE_TV;
#else
static privilege_db_manager_profile_type_e g_privilege_db_manager_profile_type = PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON;
#endif

#define TryReturn(condition, expr, returnValue, ...)	\
	if (!(condition)) { \
		LOGE(__VA_ARGS__); \
		expr; \
		return returnValue; \
	}

int __initialize_db(sqlite3 ** db, privilege_db_manager_package_type_e package_type)
{
	char *db_path = NULL;

	if (package_type == PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE)
		db_path = PRIVILEGE_INFO_CORE_DB_PATH;
	else
		db_path = PRIVILEGE_INFO_WRT_DB_PATH;

	int ret = sqlite3_open_v2(db_path, db, SQLITE_OPEN_READONLY, NULL);
	if (ret != SQLITE_OK) {
		LOGE("[DB_FAIL] Can't open database %s : %s", db_path, sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return PRIVILEGE_DB_MANAGER_ERR_CONNECTION_FAIL;
	}
	return PRIVILEGE_DB_MANAGER_ERR_NONE;
}

int __initialize_mapping_db(sqlite3 ** db, privilege_db_manager_package_type_e package_type)
{
	char *db_path = NULL;

	if (package_type == PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE)
		db_path = PRIVILEGE_MAPPING_CORE_DB_PATH;
	else
		db_path = PRIVILEGE_MAPPING_WRT_DB_PATH;

	int ret = sqlite3_open_v2(db_path, db, SQLITE_OPEN_READONLY, NULL);
	if (ret != SQLITE_OK) {
		LOGE("[DB_FAIL] Can't open database %s : %s", db_path, sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return PRIVILEGE_DB_MANAGER_ERR_CONNECTION_FAIL;
	}
	return PRIVILEGE_DB_MANAGER_ERR_NONE;
}

void __finalize_db(sqlite3 * db, sqlite3_stmt * stmt)
{
	if (stmt != NULL)
		sqlite3_finalize(stmt);

	if (db != NULL)
		sqlite3_close(db);
}

int privilege_db_manager_get_privilege_list(const char *api_version, privilege_db_manager_package_type_e package_type, GList ** privilege_list)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret;
	char *changed_to_version = NULL;

	if (g_privilege_db_manager_profile_type == PRIVILEGE_DB_MANAGER_PROFILE_TYPE_TV) {
		changed_to_version = strdup("CHANGED_TO_2_4_0");
	} else {
		if (strncmp(api_version, "2.3.1", strlen("2.3.1")) == 0)
			changed_to_version = strdup("CHANGED_TO_2_3_1");
		else if (strncmp(api_version, "2.4", strlen("2.4")) == 0)
			changed_to_version = strdup("CHANGED_TO_2_4_0");
		else if (strcmp(api_version, "3.0") == 0 || strcmp(api_version, "3") == 0)
			changed_to_version = strdup("CHANGED_TO_2_4_0");	/* it should be changed to CHANGED_TO_3_0_0 */
		else
			changed_to_version = strdup("CHANGED_TO_2_4_0");
	}
	TryReturn(changed_to_version != NULL, , PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] privilege_name's strdup is failed.");

	ret = __initialize_db(&db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	GList *temp_privilege_list = NULL;

	char *sql = sqlite3_mprintf("select privilege_name, privilege_level_id, %s, api_version_issued, api_version_expired from privilege_info where(profile_id=%d or profile_id=%d)and package_type_id=%d", changed_to_version, PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, package_type);
	free(changed_to_version);
	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		sqlite3_close(db);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	do {
		ret = sqlite3_step(stmt);
		if (ret == SQLITE_ROW) {
			privilege_info_db_row_s *privilege_info_db_row = (privilege_info_db_row_s *)malloc(sizeof(privilege_info_db_row_s));
			TryReturn(privilege_info_db_row != NULL, free(privilege_info_db_row), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] privilege_info_db_row's malloc is failed.");

			privilege_info_db_row->profile = NULL;
			privilege_info_db_row->package_type = NULL;
			privilege_info_db_row->privilege_name = NULL;
			privilege_info_db_row->privilege_display = NULL;
			privilege_info_db_row->privilege_description = NULL;
			privilege_info_db_row->privilege_level = NULL;
			privilege_info_db_row->issued_version = NULL;
			privilege_info_db_row->expired_version = NULL;
			privilege_info_db_row->package_type = NULL;
			privilege_info_db_row->changed_to = NULL;
			privilege_info_db_row->profile_id = 0;
			privilege_info_db_row->package_type_id = 0;
			privilege_info_db_row->privilege_level_id = 0;

			privilege_info_db_row->privilege_name = strdup((char *)sqlite3_column_text(stmt, 0));
			TryReturn(privilege_info_db_row->privilege_name != NULL, free(privilege_info_db_row->privilege_name);
					  free(privilege_info_db_row), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] privilege_name's strdup is failed.");
			privilege_info_db_row->privilege_level_id = sqlite3_column_int(stmt, 1);
			privilege_info_db_row->changed_to = strdup((char *)sqlite3_column_text(stmt, 2));
			TryReturn(privilege_info_db_row->changed_to != NULL, free(privilege_info_db_row->privilege_name);
					  free(privilege_info_db_row->changed_to);
					  free(privilege_info_db_row), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] changed_to's strdup is failed.");

			privilege_info_db_row->issued_version = strdup((char *)sqlite3_column_text(stmt, 3));
			TryReturn(privilege_info_db_row->issued_version != NULL, free(privilege_info_db_row->privilege_name);
					  free(privilege_info_db_row->changed_to);
					  free(privilege_info_db_row->issued_version);
					  free(privilege_info_db_row), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] issued_version's strdup is failed.");

			privilege_info_db_row->expired_version = strdup((char *)sqlite3_column_text(stmt, 4));
			TryReturn(privilege_info_db_row->expired_version != NULL, free(privilege_info_db_row->privilege_name);
					  free(privilege_info_db_row->changed_to);
					  free(privilege_info_db_row->issued_version);
					  free(privilege_info_db_row->expired_version);
					  free(privilege_info_db_row), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] expired_version's strdup is failed.");

			temp_privilege_list = g_list_append(temp_privilege_list, privilege_info_db_row);
		}
	} while (ret == SQLITE_ROW);

	*privilege_list = temp_privilege_list;

	__finalize_db(db, stmt);

	return PRIVILEGE_DB_MANAGER_ERR_NONE;
}

int privilege_db_manager_get_mapped_privilege_list(const char *api_version, privilege_db_manager_package_type_e package_type, GList * privilege_list, GList ** mapped_privilege_list)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret;

	ret = __initialize_mapping_db(&db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	GList *temp_privilege_list = NULL;

	char *privilege_list_str = NULL;
	GList *l;
	for (l = privilege_list; l != NULL; l = l->next) {
		char *privilege_name = (char *)l->data;
		if (privilege_list_str == NULL) {
			size_t size = snprintf(0, 0, "'%s'", privilege_name) + 1;
			privilege_list_str = (char *)malloc(size * sizeof(char));
			TryReturn(privilege_list_str != NULL, , PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] privilege_list_str's malloc is failed.");
			snprintf(privilege_list_str, size, "'%s'", privilege_name);
			LOGD("privilege_list_str = %s", privilege_list_str);
		} else {
			size_t new_size = snprintf(0, 0, "%s, '%s'", privilege_list_str, privilege_name) + 1;
			privilege_list_str = realloc(privilege_list_str, new_size * sizeof(char));
			TryReturn(privilege_list_str != NULL, free(privilege_list_str), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] privilege_list_str's realloc is failed.");
			strncat(privilege_list_str, ", '", strlen(", '"));
			strncat(privilege_list_str, privilege_name, strlen(privilege_name));
			strncat(privilege_list_str, "'", strlen("'"));
			LOGD("privilege_list_str = %s", privilege_list_str);
		}
	}

	char *sql = sqlite3_mprintf("select distinct mapped_privilege_name from privilege_mapping where privilege_name in(%s)and(profile_id=%d or profile_id=%d)and from_api_version<=%Q and to_api_version>%Q", privilege_list_str, PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, api_version, api_version);
	free(privilege_list_str);

	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		sqlite3_close(db);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	do {
		ret = sqlite3_step(stmt);
		if (ret == SQLITE_ROW) {
			char *privilege_name = strdup((char *)sqlite3_column_text(stmt, 0));
			temp_privilege_list = g_list_append(temp_privilege_list, privilege_name);
		}
	} while (ret == SQLITE_ROW);

	*mapped_privilege_list = temp_privilege_list;

	__finalize_db(db, stmt);

	return PRIVILEGE_DB_MANAGER_ERR_NONE;
}

int privilege_db_manager_get_privilege_display(privilege_db_manager_package_type_e package_type, const char *privilege_name, const char *api_version, char **privilege_display)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret;
	ret = __initialize_db(&db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	char *sql = NULL;

	if (api_version == NULL) {
		/* api_version == NULL then get display name regardless of api version */
		sql = sqlite3_mprintf("select privilege_display from privilege_info where(profile_id=%d or profile_id=%d)and package_type_id=%d and privilege_name=%Q", PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, package_type, privilege_name);
	} else {
		sql = sqlite3_mprintf("select privilege_display from privilege_info where(profile_id=%d or profile_id=%d)and package_type_id=%d and privilege_name=%Q and api_version_issued<=%Q and api_version_expired>=%Q", PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, package_type, privilege_name, api_version, api_version);
	}

	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		sqlite3_close(db);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW) {
		LOGD("privilege_display = %s", (char *)sqlite3_column_text(stmt, 0));

		*privilege_display = strdup((char *)sqlite3_column_text(stmt, 0));
		TryReturn(*privilege_display != NULL, , PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] privilege_name's strdup is failed.");
		__finalize_db(db, stmt);
		return PRIVILEGE_DB_MANAGER_ERR_NONE;
	}

	__finalize_db(db, stmt);
	return PRIVILEGE_DB_NO_EXIST_RESULT;
}

int privilege_db_manager_get_privilege_description(privilege_db_manager_package_type_e package_type, const char *privilege_name, const char *api_version, char **privilege_description)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret;
	ret = __initialize_db(&db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	char *sql = NULL;

	if (api_version == NULL)
		sql = sqlite3_mprintf("select privilege_description from privilege_info where(profile_id=%d or profile_id=%d)and package_type_id=%d and privilege_name=%Q", PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, package_type, privilege_name);
	else
		sql = sqlite3_mprintf("select privilege_description from privilege_info where(profile_id=%d or profile_id=%d)and package_type_id=%d and privilege_name=%Q and api_version_issued<=%Q and api_version_expired>=%Q", PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, package_type, privilege_name, api_version, api_version);

	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		sqlite3_close(db);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW) {
		LOGD("privilege_description = %s", (char *)sqlite3_column_text(stmt, 0));

		*privilege_description = strdup((char *)sqlite3_column_text(stmt, 0));
		TryReturn(*privilege_description != NULL, , PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] privilege_name's strdup is failed.");

		__finalize_db(db, stmt);
		return PRIVILEGE_DB_MANAGER_ERR_NONE;
	}

	__finalize_db(db, stmt);
	return PRIVILEGE_DB_NO_EXIST_RESULT;
}

int privilege_db_manager_get_privilege_group_display(privilege_db_manager_package_type_e package_type, const char *privilege_name, const char *api_version, int *privilege_group_number)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret;
	ret = __initialize_db(&db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	char *sql = sqlite3_mprintf("select privilege_group_id from privilege_info where(profile_id=%d or profile_id=%d)and package_type_id=%d and privilege_name=%Q and api_version_issued<=%Q and api_version_expired>=%Q",
								PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, package_type, privilege_name, api_version, api_version);

	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		sqlite3_close(db);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW) {
		*privilege_group_number = (int)sqlite3_column_int(stmt, 0);
		LOGD("privilege_group_number = %d", *privilege_group_number);

		__finalize_db(db, stmt);
		return PRIVILEGE_DB_MANAGER_ERR_NONE;
	}

	__finalize_db(db, stmt);
	return PRIVILEGE_DB_NO_EXIST_RESULT;
}

int __privilege_db_manager_is_privacy(const char* privilege)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret = __initialize_db(&db, PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return -1;

	char *sql = sqlite3_mprintf("select is_privacy from privilege_info where(profile_id=%d or profile_id=%d)and package_type_id=%d and privilege_name=%Q",
								PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, privilege);
	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		sqlite3_close(db);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW)
		ret = sqlite3_column_int(stmt, 0);
	else
		ret = -1;

	__finalize_db(db, stmt);
	return ret;
}

int __privilege_db_manager_get_privacy_list(GList **privacy_list)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret = __initialize_db(&db, PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	char *sql = sqlite3_mprintf("select DISTINCT privacy_group from privilege_info where is_privacy=1 order by privacy_group");
	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		sqlite3_close(db);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	GList* temp_privacy_list = NULL;
	do {
		ret = sqlite3_step(stmt);
		if (ret == SQLITE_ROW) {
			char *privacy_name = strdup((char *)sqlite3_column_text(stmt, 0));
			temp_privacy_list = g_list_append(temp_privacy_list, privacy_name);
		}
	} while (ret == SQLITE_ROW);

	*privacy_list = temp_privacy_list;

	__finalize_db(db, stmt);

	return PRIVILEGE_DB_MANAGER_ERR_NONE;

}

int __privilege_db_manager_get_privilege_list_by_privacy(const char* privacy, GList **privilege_list)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret = __initialize_db(&db, PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	char *sql = sqlite3_mprintf("select distinct privilege_name from privilege_info where is_privacy=1 and privacy_group=%Q", privacy);
	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		sqlite3_close(db);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	GList* temp_privilege_list = NULL;
	do {
		ret = sqlite3_step(stmt);
		if (ret == SQLITE_ROW) {
			char *privilege_name = strdup((char *)sqlite3_column_text(stmt, 0));
			temp_privilege_list = g_list_append(temp_privilege_list, privilege_name);
		}
	} while (ret == SQLITE_ROW);

	*privilege_list = temp_privilege_list;

	__finalize_db(db, stmt);

	return PRIVILEGE_DB_MANAGER_ERR_NONE;

}
