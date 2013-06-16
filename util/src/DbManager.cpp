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

#include <DbManager.h>
#include <Util.h>
#include <dlog.h>

static const char PRIVACY_INFO_DB_PATH[] = "/opt/dbspace/.privacylist.db";
static const char PRIVILEGE_INFO_DB_PATH[] = "/opt/dbspace/.privilegelist.db";

int
DbManager::InsertPrivacy(const PrivacyInfo* pPrivacyInfo)
{
	LOGI("enter");

	int res = PRIV_CHECKER_SUCCESS;
	int dbRes = SQLITE_OK;

	char query[256] = {0,};
	sprintf(query, "INSERT INTO PrivacyInfo values(?, ?, ?, ?)");

	TryReturn(pPrivacyInfo != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] pPrivacyInfo is null");
	TryReturn(pPrivacyInfo->pId != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] PrivacyID is null");

	res = DeletePrivacy(pPrivacyInfo);
	TryReturn(res == PRIV_CHECKER_SUCCESS, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[PRIV_CHECKER_ERROR_DB_ERROR] Delete operation of privilege is failed.");

	LOGI("id: %s", pPrivacyInfo->pId);

	sqlite3* pDbHandlerTemp = NULL;
	dbRes = sqlite3_open_v2(PRIVACY_INFO_DB_PATH, &pDbHandlerTemp, SQLITE_OPEN_READWRITE , NULL);
	TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB open error. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	DbDeleter dbdel;
	std::unique_ptr <sqlite3, DbDeleter> pDbHandler (pDbHandlerTemp, dbdel);

	sqlite3_stmt* pStmtTemp = NULL;
	dbRes = sqlite3_prepare_v2(pDbHandler.get(), query, -1, & pStmtTemp, 0);
	TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB prepare failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	StmtDeleter deltr;
	std::unique_ptr <sqlite3_stmt, StmtDeleter> pStmt(pStmtTemp, deltr);

	dbRes = sqlite3_bind_text(pStmt.get(), 1, (char*)(pPrivacyInfo->pId), -1, SQLITE_TRANSIENT);
	TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] bind_text failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	if (pPrivacyInfo->pStringInfo != NULL)
	{
		TryReturn(pPrivacyInfo->pStringInfo->pModule != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] String module of Privacy is null");
		TryReturn(pPrivacyInfo->pStringInfo->pNameId != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] Name Id of Privacy is null");
		TryReturn(pPrivacyInfo->pStringInfo->pDescId != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] Description Id of Privacy is null");

		LOGI("str_module: %s", pPrivacyInfo->pStringInfo->pModule);
		LOGI("str_name_id: %s", pPrivacyInfo->pStringInfo->pNameId);
		LOGI("str_desc_id: %s", pPrivacyInfo->pStringInfo->pDescId);

		dbRes = sqlite3_bind_text(pStmt.get(), 2, (char*)(pPrivacyInfo->pStringInfo->pModule), -1, SQLITE_TRANSIENT);
		TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] bind_text failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

		dbRes = sqlite3_bind_text(pStmt.get(), 3, (char*)(pPrivacyInfo->pStringInfo->pNameId), -1, SQLITE_TRANSIENT);
		TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] bind_text failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

		dbRes = sqlite3_bind_text(pStmt.get(), 4, (char*)(pPrivacyInfo->pStringInfo->pDescId), -1, SQLITE_TRANSIENT);
		TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] bind_text failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);
	}
	else
	{
		LOGI("str_module does not exist.");
	}

	dbRes = sqlite3_step(pStmt.get());
	TryReturn(dbRes == SQLITE_DONE, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB step failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	LOGI("leave");
	return 0;
}

int
DbManager::InsertPrivilege(const PrivilegeInfo* pPrivilegeInfo)
{
	LOGI("enter");

	int res = PRIV_CHECKER_SUCCESS;
	int dbRes = SQLITE_OK;

	TryReturn(pPrivilegeInfo != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] pPrivilegeInfo is null");
	TryReturn(pPrivilegeInfo->pId != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] PrivilegeID is null");
	TryReturn(pPrivilegeInfo->pPrivilegeLevel != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] Privilege level is null");

	res = DeletePrivilege(pPrivilegeInfo);
	TryReturn(res == PRIV_CHECKER_SUCCESS, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[PRIV_CHECKER_ERROR_DB_ERROR] Delete operation of privilege is failed.");

	LOGI("id: %s", pPrivilegeInfo->pId);
	LOGI("privilege_level: %s", pPrivilegeInfo->pPrivilegeLevel);

	char query[256] = {0,};
	sprintf(query, "INSERT INTO PrivilegeInfo values(?, ?, ?, ?, ?)");

	sqlite3* pPrivilegeDbHandlerTemp = NULL;
	dbRes = sqlite3_open_v2(PRIVILEGE_INFO_DB_PATH, &pPrivilegeDbHandlerTemp, SQLITE_OPEN_READWRITE , NULL);
	TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB open error. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	DbDeleter dbdel;
	std::unique_ptr <sqlite3, DbDeleter> pPrivilegeDbHandler (pPrivilegeDbHandlerTemp, dbdel);

	sqlite3_stmt* pPrivilegeStmtTemp = NULL;
	dbRes = sqlite3_prepare_v2(pPrivilegeDbHandler.get(), query, -1, & pPrivilegeStmtTemp, 0);
	TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB prepare failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	StmtDeleter deltr;
	std::unique_ptr <sqlite3_stmt, StmtDeleter> pPrivilegeStmt(pPrivilegeStmtTemp, deltr);

	dbRes = sqlite3_bind_text(pPrivilegeStmt.get(), 1, (char*)(pPrivilegeInfo->pId), -1, SQLITE_TRANSIENT);
	TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] bind_text failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	dbRes = sqlite3_bind_text(pPrivilegeStmt.get(), 2, (char*)(pPrivilegeInfo->pPrivilegeLevel), -1, SQLITE_TRANSIENT);
	TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] bind_text failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	if (pPrivilegeInfo->pStringInfo != NULL)
	{
		TryReturn(pPrivilegeInfo->pStringInfo->pModule != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] String module of Privilege is null");
		TryReturn(pPrivilegeInfo->pStringInfo->pNameId != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] Name Id of Privilege is null");
		TryReturn(pPrivilegeInfo->pStringInfo->pDescId != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] Description Id of Privilege is null");

		LOGI("str_module: %s", pPrivilegeInfo->pStringInfo->pModule);
		LOGI("str_name_id: %s", pPrivilegeInfo->pStringInfo->pNameId);
		LOGI("str_desc_id: %s", pPrivilegeInfo->pStringInfo->pDescId);

		dbRes = sqlite3_bind_text(pPrivilegeStmt.get(), 3, (char*)(pPrivilegeInfo->pStringInfo->pModule), -1, SQLITE_TRANSIENT);
		TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] bind_text failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

		dbRes = sqlite3_bind_text(pPrivilegeStmt.get(), 4, (char*)(pPrivilegeInfo->pStringInfo->pNameId), -1, SQLITE_TRANSIENT);
		TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] bind_text failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

		dbRes = sqlite3_bind_text(pPrivilegeStmt.get(), 5, (char*)(pPrivilegeInfo->pStringInfo->pDescId), -1, SQLITE_TRANSIENT);
		TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] bind_text failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);
	}
	else
	{
		LOGI("str_module does not exist.");
	}

	dbRes = sqlite3_step(pPrivilegeStmt.get());
	TryReturn(dbRes == SQLITE_DONE, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB step failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	if (pPrivilegeInfo->pPrivacy != NULL)
	{
		char queryPrivacy[256] = {0,};
		sprintf(queryPrivacy, "INSERT INTO PrivilegeToPrivacyTable values(?, ?)");

		LOGI("privacy: %s", pPrivilegeInfo->pPrivacy);

		sqlite3* pDbHandlerTemp = NULL;
		dbRes = sqlite3_open_v2(PRIVACY_INFO_DB_PATH, &pDbHandlerTemp, SQLITE_OPEN_READWRITE , NULL);
		TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB open error. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

		DbDeleter dbdel;
		std::unique_ptr <sqlite3, DbDeleter> pDbHandler (pDbHandlerTemp, dbdel);

		sqlite3_stmt* pStmtTemp = NULL;
		dbRes = sqlite3_prepare_v2(pDbHandler.get(), queryPrivacy, -1, & pStmtTemp, 0);
		TryReturn(res == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB prepare failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

		StmtDeleter deltr;
		std::unique_ptr <sqlite3_stmt, StmtDeleter> pStmt(pStmtTemp, deltr);

		dbRes = sqlite3_bind_text(pStmt.get(), 1, (char*)(pPrivilegeInfo->pId), -1, SQLITE_TRANSIENT);
		TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] bind_text failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

		dbRes = sqlite3_bind_text(pStmt.get(), 2, (char*)(pPrivilegeInfo->pPrivacy), -1, SQLITE_TRANSIENT);
		TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] bind_text failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

		dbRes = sqlite3_step(pStmt.get());
		TryReturn(dbRes == SQLITE_DONE, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB step failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);
	}

	LOGI("leave");

	return res;
}


int
DbManager::DeletePrivacy(const PrivacyInfo* pPrivacyInfo)
{
	LOGI("enter");

	int res = PRIV_CHECKER_SUCCESS;
	int dbRes = SQLITE_OK;

	TryReturn(pPrivacyInfo != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] pPrivacyInfo is null");
	TryReturn(pPrivacyInfo->pId != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] PrivacyID is null");

	LOGI("id: %s", pPrivacyInfo->pId);

	char query[256] = {0,};
	sprintf(query, "DELETE FROM PrivacyInfo WHERE PRIVACY_ID = '%s'", (char*)(pPrivacyInfo->pId));

	sqlite3* pDbHandlerTemp = NULL;
	dbRes = sqlite3_open_v2(PRIVACY_INFO_DB_PATH, &pDbHandlerTemp, SQLITE_OPEN_READWRITE , NULL);
	TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB open error. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	DbDeleter dbdel;
	std::unique_ptr <sqlite3, DbDeleter> pDbHandler (pDbHandlerTemp, dbdel);

	sqlite3_stmt* pStmtTemp = NULL;
	dbRes = sqlite3_prepare_v2(pDbHandler.get(), query, -1, & pStmtTemp, 0);
	TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB prepare failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	StmtDeleter deltr;
	std::unique_ptr <sqlite3_stmt, StmtDeleter> pStmt(pStmtTemp, deltr);

	dbRes = sqlite3_step(pStmt.get());
	TryReturn(dbRes == SQLITE_DONE, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB step failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	LOGI("leave");
	return 0;
}

int
DbManager::DeletePrivilege(const PrivilegeInfo* pPrivilegeInfo)
{
	LOGI("enter");

	int res = PRIV_CHECKER_SUCCESS;
	int dbRes = SQLITE_OK;

	TryReturn(pPrivilegeInfo != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] pPrivilegeInfo is null");
	TryReturn(pPrivilegeInfo->pId != NULL, , PRIV_CHECKER_ERROR_INVALID_ARG, "[PRIV_CHECKER_ERROR_INVALID_ARG] PrivilegeID is null");

	LOGI("id: %s", pPrivilegeInfo->pId);

	char query[256] = {0,};
	sprintf(query, "DELETE FROM PrivilegeInfo WHERE PRIVILEGE_ID = '%s'", (char*)(pPrivilegeInfo->pId));

	sqlite3* pPrivilegeDbHandlerTemp = NULL;
	dbRes = sqlite3_open_v2(PRIVILEGE_INFO_DB_PATH, &pPrivilegeDbHandlerTemp, SQLITE_OPEN_READWRITE , NULL);
	TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB open error. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	DbDeleter dbdel;
	std::unique_ptr <sqlite3, DbDeleter> pPrivilegeDbHandler (pPrivilegeDbHandlerTemp, dbdel);

	sqlite3_stmt* pPrivilegeStmtTemp = NULL;
	dbRes = sqlite3_prepare_v2(pPrivilegeDbHandler.get(), query, -1, & pPrivilegeStmtTemp, 0);
	TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB prepare failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	StmtDeleter deltr;
	std::unique_ptr <sqlite3_stmt, StmtDeleter> pPrivilegeStmt(pPrivilegeStmtTemp, deltr);

	dbRes = sqlite3_step(pPrivilegeStmt.get());
	TryReturn(dbRes == SQLITE_DONE, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB step failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

	if (pPrivilegeInfo->pPrivacy != NULL)
	{

		LOGI("privacy: %s", pPrivilegeInfo->pPrivacy);

		char queryPrivacy[256] = {0,};
		sprintf(queryPrivacy, "DELETE FROM PrivilegeToPrivacyTable WHERE PRIVILEGE_ID = '%s'", (char*)(pPrivilegeInfo->pId));

		sqlite3* pDbHandlerTemp = NULL;
		dbRes = sqlite3_open_v2(PRIVACY_INFO_DB_PATH, &pDbHandlerTemp, SQLITE_OPEN_READWRITE , NULL);
		TryReturn(dbRes == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB open error. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

		DbDeleter dbdel;
		std::unique_ptr <sqlite3, DbDeleter> pDbHandler (pDbHandlerTemp, dbdel);

		sqlite3_stmt* pStmtTemp = NULL;
		dbRes = sqlite3_prepare_v2(pDbHandler.get(), queryPrivacy, -1, & pStmtTemp, 0);
		TryReturn(res == SQLITE_OK, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB prepare failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);

		StmtDeleter deltr;
		std::unique_ptr <sqlite3_stmt, StmtDeleter> pStmt(pStmtTemp, deltr);

		dbRes = sqlite3_step(pStmt.get());
		TryReturn(dbRes == SQLITE_DONE, res = PRIV_CHECKER_ERROR_DB_ERROR, PRIV_CHECKER_ERROR_DB_ERROR, "[%s] DB step failed. (%d)", Util::GetErrorMessage(PRIV_CHECKER_ERROR_DB_ERROR), dbRes);
	}

	LOGI("leave");

	return res;
}
