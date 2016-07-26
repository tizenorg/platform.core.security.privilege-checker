#include "privilege_db_manager.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __TIZEN__
#include <dlog.h>
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "PRIVILEGE_DB_MANAGER"
#define _LOGE(fmt, arg...) LOGE(fmt, ##arg)
#define _LOGD(fmt, arg...) LOGD(fmt, ##arg)
#define _LOGW(fmt, arg...) LOGW(fmt, ##arg)
#define _LOGI(fmt, arg...) LOGI(fmt, ##arg)
#endif
#else
#define _LOGE
#define _LOGD
#define _LOGW
#define _LOGI
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
		_LOGE(__VA_ARGS__); \
		expr; \
		return returnValue; \
	}

int __initialize_db(char type, sqlite3 ** db, privilege_db_manager_package_type_e package_type)
{
	char *db_path = NULL;
	int db_mode = SQLITE_OPEN_READONLY;

	switch (type) {
	case 'i':
		if (package_type == PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE)
			db_path = PRIVILEGE_INFO_CORE_DB_PATH;
		else
			db_path = PRIVILEGE_INFO_WRT_DB_PATH;
		break;
	case 'm':
		if (package_type == PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE)
			db_path = PRIVILEGE_MAPPING_CORE_DB_PATH;
		else
			db_path = PRIVILEGE_MAPPING_WRT_DB_PATH;
		break;
	case 'u':
		db_mode = SQLITE_OPEN_READWRITE;
	case 'p':
		db_path = PRIVILEGE_POLICY_DB_PATH;
		break;
	default:
		_LOGE("Undefined db initialize mode!");
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_TYPE;
	}
	_LOGD("DB PATH = %s", db_path);
	int ret = sqlite3_open_v2(db_path, db, db_mode, NULL);
	if (ret != SQLITE_OK) {
		_LOGE("[DB_FAIL] Can't open database %s : %s", db_path, sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return PRIVILEGE_DB_MANAGER_ERR_CONNECTION_FAIL;
	}
	return PRIVILEGE_DB_MANAGER_ERR_NONE;
}

void __finalize_db(sqlite3 * db, sqlite3_stmt * stmt, char* sql)
{
	if (stmt != NULL)
		sqlite3_finalize(stmt);

	if (db != NULL)
		sqlite3_close(db);

	if (sql != NULL)
		sqlite3_free(sql);
}

int __make_privilege_list_str(GList *privilege_list, char** privilege_list_str)
{
	GList *l;
	char* temp_privilege_list_str = NULL;
	for (l = privilege_list; l != NULL; l = l->next) {
		char *privilege_name = (char *)l->data;
		if (temp_privilege_list_str == NULL)
			temp_privilege_list_str = sqlite3_mprintf("'%q'", privilege_name);
		else
			temp_privilege_list_str = sqlite3_mprintf("%s, '%q'", temp_privilege_list_str, privilege_name);
	}
	*privilege_list_str = temp_privilege_list_str;
	return 0;
}

int __get_db_error(int ret)
{
	_LOGE("[PRIVILEGE_DB_MANAGER_ERR_DB_FAIL] %s", sqlite3_errstr(ret));
	switch (ret) {
	case SQLITE_BUSY:
		ret = PRIVILEGE_DB_MANAGER_ERR_DB_BUSY_FAIL;
		break;
	case SQLITE_CONSTRAINT:
		ret = PRIVILEGE_DB_MANAGER_ERR_DB_CONSTRAINT_FAIL;
		break;
	case SQLITE_FULL:
		ret = PRIVILEGE_DB_MANAGER_ERR_DB_FULL_FAIL;
		break;
	default:
		ret = PRIVILEGE_DB_MANAGER_ERR_DB_UPDATE_FAIL;
	}

	return ret;
}

int privilege_db_manager_check_black_list(int uid, privilege_db_manager_package_type_e package_type, GList* privilege_list)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret = 0;
	int count = 0;

	ret = __initialize_db('p', &db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	char *privilege_list_str = NULL;
	ret = __make_privilege_list_str(privilege_list, &privilege_list_str);
	_LOGD("check black list with uid = %d, package_type = %d, privilege_list = %s", uid, package_type, privilege_list_str);
	char *sql = sqlite3_mprintf("select distinct privilege_name from black_list where privilege_name in(%s)and uid=%d and package_type=%d", privilege_list_str, uid, package_type);
	sqlite3_free(privilege_list_str);
	TryReturn(sql != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] sqlite3_mprintf failed");

	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		_LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		__finalize_db(db, stmt, sql);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	do {
		ret = sqlite3_step(stmt);
		if (ret == SQLITE_ROW)
			++count;
	} while (ret == SQLITE_ROW);

	if (count > 0)
		_LOGE("Privilege list contains banned privileges!");

	__finalize_db(db, stmt, sql);

	return count;
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

	ret = __initialize_db('i', &db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	GList *temp_privilege_list = NULL;

	char *sql = sqlite3_mprintf("select privilege_name, privilege_level_id, %s, api_version_issued, api_version_expired from privilege_info where(profile_id=%d or profile_id=%d)and package_type_id=%d", changed_to_version, PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, package_type);
	free(changed_to_version);
	TryReturn(sql != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] sqlite3_mprintf failed");

	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		_LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		__finalize_db(db, stmt, sql);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	do {
		ret = sqlite3_step(stmt);
		if (ret == SQLITE_ROW) {
			privilege_info_db_row_s *privilege_info_db_row = (privilege_info_db_row_s *)malloc(sizeof(privilege_info_db_row_s));
			TryReturn(privilege_info_db_row != NULL, free(privilege_info_db_row); __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] privilege_info_db_row's malloc is failed.");

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
					  free(privilege_info_db_row);
					  __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] privilege_name's strdup is failed.");
			privilege_info_db_row->privilege_level_id = sqlite3_column_int(stmt, 1);
			privilege_info_db_row->changed_to = strdup((char *)sqlite3_column_text(stmt, 2));
			TryReturn(privilege_info_db_row->changed_to != NULL, free(privilege_info_db_row->privilege_name);
					  free(privilege_info_db_row->changed_to);
					  free(privilege_info_db_row);
					  __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] changed_to's strdup is failed.");

			privilege_info_db_row->issued_version = strdup((char *)sqlite3_column_text(stmt, 3));
			TryReturn(privilege_info_db_row->issued_version != NULL, free(privilege_info_db_row->privilege_name);
					  free(privilege_info_db_row->changed_to);
					  free(privilege_info_db_row->issued_version);
					  free(privilege_info_db_row);
					  __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] issued_version's strdup is failed.");

			privilege_info_db_row->expired_version = strdup((char *)sqlite3_column_text(stmt, 4));
			TryReturn(privilege_info_db_row->expired_version != NULL, free(privilege_info_db_row->privilege_name);
					  free(privilege_info_db_row->changed_to);
					  free(privilege_info_db_row->issued_version);
					  free(privilege_info_db_row->expired_version);
					  free(privilege_info_db_row);
					  __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] expired_version's strdup is failed.");

			temp_privilege_list = g_list_append(temp_privilege_list, privilege_info_db_row);
		}
	} while (ret == SQLITE_ROW);

	*privilege_list = temp_privilege_list;

	__finalize_db(db, stmt, sql);

	return PRIVILEGE_DB_MANAGER_ERR_NONE;
}

int privilege_db_manager_get_mapped_privilege_list(const char *api_version, privilege_db_manager_package_type_e package_type, GList * privilege_list, GList ** mapped_privilege_list)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret;

	ret = __initialize_db('m', &db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	GList *temp_privilege_list = NULL;

	char *privilege_list_str = NULL;
	ret = __make_privilege_list_str(privilege_list, &privilege_list_str);
	TryReturn(ret == 0 && privilege_list_str != NULL, sqlite3_close(db), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] making privilege_list_str for where in query is failed.");

	char *sql = sqlite3_mprintf("select distinct mapped_privilege_name from privilege_mapping where privilege_name in(%s)and(profile_id=%d or profile_id=%d)and from_api_version<=%Q and to_api_version>%Q", privilege_list_str, PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, api_version, api_version);
	sqlite3_free(privilege_list_str);
	TryReturn(sql != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] sqlite3_mprintf failed");

	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		_LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		__finalize_db(db, stmt, sql);
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

	__finalize_db(db, stmt, sql);

	return PRIVILEGE_DB_MANAGER_ERR_NONE;
}

int privilege_db_manager_get_privacy_display(const char *privacy, char **privacy_display)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret = __initialize_db('i', &db, PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;
	char *sql = sqlite3_mprintf("select privacy_display from privacy_info where privacy_group=%Q", privacy);
	TryReturn(sql != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] sqlite3_mprintf failed");
	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		_LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
        __finalize_db(db, stmt, sql);
        return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
    }

	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW) {
		_LOGD("privacy display = %s", (char*) sqlite3_column_text(stmt, 0));
		*privacy_display = strdup((char*)sqlite3_column_text(stmt, 0));
		TryReturn(*privacy_display != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] privilege_name's strdup is failed.");
        __finalize_db(db, stmt, sql);
        return PRIVILEGE_DB_MANAGER_ERR_NONE;
    }

	__finalize_db(db, stmt, sql);

	return PRIVILEGE_DB_NO_EXIST_RESULT;
}

int privilege_db_manager_get_privilege_display(privilege_db_manager_package_type_e package_type, const char *privilege_name, const char *api_version, char **privilege_display)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret;
	ret = __initialize_db('i', &db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	char *sql = NULL;

	if (api_version == NULL) {
		/* api_version == NULL then get display name regardless of api version */
		sql = sqlite3_mprintf("select privilege_display from privilege_info where(profile_id=%d or profile_id=%d)and package_type_id=%d and privilege_name=%Q", PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, package_type, privilege_name);
	} else {
		sql = sqlite3_mprintf("select privilege_display from privilege_info where(profile_id=%d or profile_id=%d)and package_type_id=%d and privilege_name=%Q and api_version_issued<=%Q and api_version_expired>%Q", PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, package_type, privilege_name, api_version, api_version);
	}
	TryReturn(sql != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] sqlite3_mprintf failed");

	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		_LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		__finalize_db(db, stmt, sql);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW) {
		_LOGD("privilege_display = %s", (char *)sqlite3_column_text(stmt, 0));

		*privilege_display = strdup((char *)sqlite3_column_text(stmt, 0));
		TryReturn(*privilege_display != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] privilege_name's strdup is failed.");
		__finalize_db(db, stmt, sql);
		return PRIVILEGE_DB_MANAGER_ERR_NONE;
	}

	__finalize_db(db, stmt, sql);

	return PRIVILEGE_DB_NO_EXIST_RESULT;
}

int privilege_db_manager_get_privilege_description(privilege_db_manager_package_type_e package_type, const char *privilege_name, const char *api_version, char **privilege_description)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret;
	ret = __initialize_db('i', &db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	char *sql = NULL;

	if (api_version == NULL)
		sql = sqlite3_mprintf("select privilege_description from privilege_info where(profile_id=%d or profile_id=%d)and package_type_id=%d and privilege_name=%Q", PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, package_type, privilege_name);
	else
		sql = sqlite3_mprintf("select privilege_description from privilege_info where(profile_id=%d or profile_id=%d)and package_type_id=%d and privilege_name=%Q and api_version_issued<=%Q and api_version_expired>%Q", PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, package_type, privilege_name, api_version, api_version);
	TryReturn(sql != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] sqlite3_mprintf failed");

	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		_LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		__finalize_db(db, stmt, sql);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW) {
		_LOGD("privilege_description = %s", (char *)sqlite3_column_text(stmt, 0));

		*privilege_description = strdup((char *)sqlite3_column_text(stmt, 0));
		TryReturn(*privilege_description != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY, "[PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY] privilege_name's strdup is failed.");

		__finalize_db(db, stmt, sql);
		return PRIVILEGE_DB_MANAGER_ERR_NONE;
	}

	__finalize_db(db, stmt, sql);

	return PRIVILEGE_DB_NO_EXIST_RESULT;
}

int privilege_db_manager_get_privilege_group_display(privilege_db_manager_package_type_e package_type, const char *privilege_name, const char *api_version, int *privilege_group_number)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret;
	ret = __initialize_db('i', &db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	char *sql = sqlite3_mprintf("select privilege_group_id from privilege_info where(profile_id=%d or profile_id=%d)and package_type_id=%d and privilege_name=%Q and api_version_issued<=%Q",
								PRIVILEGE_DB_MANAGER_PROFILE_TYPE_COMMON, g_privilege_db_manager_profile_type, package_type, privilege_name, api_version, api_version);

	TryReturn(sql != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] sqlite3_mprintf failed");
	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		_LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		__finalize_db(db, stmt, sql);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW) {
		*privilege_group_number = (int)sqlite3_column_int(stmt, 0);
		_LOGD("privilege_group_number = %d", *privilege_group_number);

		__finalize_db(db, stmt, sql);
		return PRIVILEGE_DB_MANAGER_ERR_NONE;
	}

	__finalize_db(db, stmt, sql);

	return PRIVILEGE_DB_NO_EXIST_RESULT;
}

int privilege_db_manager_is(char type, const char* privilege)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int res = 0;
	int ret = __initialize_db('i', &db, PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE);
	TryReturn(ret == PRIVILEGE_DB_MANAGER_ERR_NONE, , ret, "[PRIVILEGE_DB_MANAGER] DB INITIALIZE FAIL");

	char *sql = sqlite3_mprintf("select * from valid_privilege_info where privilege_name=%Q", privilege);
	TryReturn(sql != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] sqlite3_mprintf failed");
	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	TryReturn(ret == SQLITE_OK, __finalize_db(db, stmt, sql), -PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));

	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW) {
		_LOGD("privilege: %s, is-privacy : %d, is-internal : %d", (char*)sqlite3_column_text(stmt, 0), sqlite3_column_int(stmt, 1), sqlite3_column_int(stmt, 2));
		switch (type) {
		case 'v':
			res = 1;
			break;
		case 'p':
			res = sqlite3_column_int(stmt, 1);
			break;
		case 'i':
			res = sqlite3_column_int(stmt, 2);
			break;
		default:
			_LOGE("Undefined type for privilege_db_manager_is()");
			return PRIVILEGE_DB_MANAGER_ERR_INVALID_TYPE;
		}
	} else if (ret == SQLITE_DONE) {
		_LOGD("[PRIVILEGE_DB_MANAGER] NO DATA TO READ. %s is invalid privilege. ret = %d", privilege, ret);
	} else {
		res = -1;
	}

	__finalize_db(db, stmt, sql);

	if (res < 0)
		return -ret;
	else
		return res;
}

int __privilege_db_manager_get_privacy_list(GList **privacy_list)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret = __initialize_db('i', &db, PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	char *sql = sqlite3_mprintf("select DISTINCT privacy_group from privilege_info where is_privacy=1 order by privacy_group");
	TryReturn(sql != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] sqlite3_mprintf failed");
	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		_LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		__finalize_db(db, stmt, sql);
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

	__finalize_db(db, stmt, sql);

	return PRIVILEGE_DB_MANAGER_ERR_NONE;

}

int __privilege_db_manager_get_privilege_list_by_privacy(const char* privacy, GList **privilege_list)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret = __initialize_db('i', &db, PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	char *sql = sqlite3_mprintf("select distinct privilege_name from privilege_info where is_privacy=1 and privacy_group=%Q", privacy);
	TryReturn(sql != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] sqlite3_mprintf failed");
	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		_LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		__finalize_db(db, stmt, sql);
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

	__finalize_db(db, stmt, sql);

	return PRIVILEGE_DB_MANAGER_ERR_NONE;

}

int privilege_db_manager_get_black_list(int uid, privilege_db_manager_package_type_e package_type, GList **privilege_list)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret = __initialize_db('p', &db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;
	_LOGD("Get privilege_name from black_list where uid = %d, package_type = %d", uid, package_type);
	char* sql = sqlite3_mprintf("select privilege_name from black_list where uid=%d and package_type=%d", uid, package_type);
	TryReturn(sql != NULL, __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] sqlite3_mprintf failed");
	ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (ret != SQLITE_OK) {
		_LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
		__finalize_db(db, stmt, sql);
		return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
	}

	GList *temp_privilege_list = NULL;
	do {
		ret = sqlite3_step(stmt);
		if (ret == SQLITE_ROW) {
			char *privilege_name = strdup((char *)sqlite3_column_text(stmt, 0));
			_LOGE("privilege name: %s", privilege_name);
			temp_privilege_list = g_list_append(temp_privilege_list, privilege_name);
		} else if (ret != SQLITE_DONE) {
			_LOGE("ret = %d", ret);
		}
	} while (ret == SQLITE_ROW);

	*privilege_list = temp_privilege_list;

	__finalize_db(db, stmt, sql);

	return PRIVILEGE_DB_MANAGER_ERR_NONE;
}

int privilege_db_manager_set_black_list(int uid, privilege_db_manager_package_type_e package_type, GList *privilege_list)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret = __initialize_db('u', &db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION", NULL, NULL, NULL);
	GList *l = NULL;
	for (l = privilege_list; l != NULL; l = l->next) {
		char *privilege_name = (char *)l->data;
		_LOGD("insert uid = %d, package_type = %d, privilege_name = %s", uid, package_type, privilege_name);
		char* sql = sqlite3_mprintf("insert or ignore into black_list (uid, package_type, privilege_name) values (%d, %d, %Q)", uid, package_type, privilege_name);
		TryReturn(sql != NULL, sqlite3_exec(db, "ROLLBACK TRANSACTION", NULL, NULL, NULL); __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] sqlite3_mprintf failed");
		ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
		if (ret != SQLITE_OK) {
			_LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
			sqlite3_exec(db, "ROLLBACK TRANSACTION", NULL, NULL, NULL);
			__finalize_db(db, stmt, sql);
			return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
		}
		ret = sqlite3_step(stmt);
		if (ret != SQLITE_DONE) {
			__get_db_error(ret);
			sqlite3_exec(db, "ROLLBACK TRANSACTION", NULL, NULL, NULL);
			__finalize_db(db, stmt, sql);
			return ret;
		}
		sqlite3_free(sql);
	}

	sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL);
	__finalize_db(db, stmt, NULL);
	return PRIVILEGE_DB_MANAGER_ERR_NONE;
}

int privilege_db_manager_unset_black_list(int uid, privilege_db_manager_package_type_e package_type, GList *privilege_list)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int ret = __initialize_db('u', &db, package_type);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE)
		return ret;

	sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION", NULL, NULL, NULL);
	GList *l = NULL;
	for (l = privilege_list; l != NULL; l = l->next) {
		char *privilege_name = (char *)l->data;
		_LOGD("delete from black_list where uid = %d, package_type = %d, privilege_name = %s", uid, package_type, privilege_name);
		char* sql = sqlite3_mprintf("delete from black_list where uid=%d and package_type=%d and privilege_name=%Q", uid, package_type, privilege_name);
		TryReturn(sql != NULL, sqlite3_exec(db, "ROLLBACK TRANSACTION", NULL, NULL, NULL); __finalize_db(db, stmt, sql), PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY, "[DB_FAIL] sqlite3_mprintf failed");
		ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
		if (ret != SQLITE_OK) {
			_LOGE("[DB_FAIL] fail to prepare database : %s", sqlite3_errmsg(db));
			sqlite3_exec(db, "ROLLBACK TRANSACTION", NULL, NULL, NULL);
			__finalize_db(db, stmt, sql);
			return PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY;
		}
		ret = sqlite3_step(stmt);
		if (ret != SQLITE_DONE) {
			__get_db_error(ret);
			sqlite3_exec(db, "ROLLBACK TRANSACTION", NULL, NULL, NULL);
			__finalize_db(db, stmt, sql);
			return ret;
		}
		sqlite3_free(sql);
	}

	sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL);
	__finalize_db(db, stmt, NULL);
	return PRIVILEGE_DB_MANAGER_ERR_NONE;
}
