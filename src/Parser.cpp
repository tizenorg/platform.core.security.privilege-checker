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

#include <Parser.h>
#include <DbManager.h>
#include <Util.h>
#include <dlog.h>

using namespace std;

int
Parser::SetPrivilegeDb(PrivilegeInfo* pPrivilegeInfo)
{
	LOGI("enter");
	int res = PRIV_CHECKER_SUCCESS;

	res = DbManager::InsertPrivilege(pPrivilegeInfo);
	TryReturn(res == PRIV_CHECKER_SUCCESS, , res, "[%s] Propagated.", Util::GetErrorMessage(res));

	LOGI("leave");
	return res;
}

int
Parser::SetPrivacyDb(PrivacyInfo* pPrivacyInfo)
{
	LOGI("enter");
	int res = PRIV_CHECKER_SUCCESS;

	res = DbManager::InsertPrivacy(pPrivacyInfo);
	TryReturn(res == PRIV_CHECKER_SUCCESS, , res, "[%s] Propagated.", Util::GetErrorMessage(res));

	LOGI("leave");
	return res;
}

int
Parser::ParseStringInfo (xmlDocPtr stringInfoDoc, xmlNodePtr currentDoc, StringInfo** ppStringInfo)
{
	LOGI("enter");
	int res = PRIV_CHECKER_SUCCESS;

	*ppStringInfo = (StringInfo*)calloc(1, sizeof(StringInfo));
	TryCatch(*ppStringInfo != NULL, res = PRIV_CHECKER_ERROR_OUT_OF_MEMORY, "[PRIV_CHECKER_ERROR_OUT_OF_MEMORY] The memory is insufficient.");

	currentDoc = currentDoc->xmlChildrenNode;
	while (currentDoc != NULL)
	{
		if (currentDoc->type == XML_ELEMENT_NODE)
		{
			if ((!xmlStrcmp(currentDoc->name, (const xmlChar *)"str_module")))
			{
				(*ppStringInfo)->pModule = xmlNodeListGetString(stringInfoDoc, currentDoc->xmlChildrenNode, 1);
			}
			else if ((!xmlStrcmp(currentDoc->name, (const xmlChar *)"str_name_id")))
			{
				(*ppStringInfo)->pNameId = xmlNodeListGetString(stringInfoDoc, currentDoc->xmlChildrenNode, 1);
			}
			else if ((!xmlStrcmp(currentDoc->name, (const xmlChar *)"str_desc_id")))
			{
				(*ppStringInfo)->pDescId = xmlNodeListGetString(stringInfoDoc, currentDoc->xmlChildrenNode, 1);
			}
			else
			{
				LOGE("[PRIV_CHECKER_ERROR_INVALID_ARG] Invalid policy type: %s", currentDoc->name);
				res = PRIV_CHECKER_ERROR_INVALID_ARG;
				goto CATCH;
			}
		}
		currentDoc = currentDoc->next;
	}


	LOGI("leave");
	return res;

CATCH:

	if (*ppStringInfo != NULL)
	{
		free(*ppStringInfo);
	}

	LOGI("error!");
	return res;
}

int
Parser::ParsePrivilege (xmlDocPtr privilegeDoc, xmlNodePtr currentDoc)
{
	LOGI("enter");
	int res = PRIV_CHECKER_SUCCESS;

	PrivilegeInfo* pPrivilegeInfo = (PrivilegeInfo*)calloc(1, sizeof(PrivilegeInfo));
	TryCatch(pPrivilegeInfo != NULL, res = PRIV_CHECKER_ERROR_OUT_OF_MEMORY, "[PRIV_CHECKER_ERROR_OUT_OF_MEMORY] The memory is insufficient.");

	currentDoc = currentDoc->xmlChildrenNode;
	while (currentDoc != NULL)
	{
		if (currentDoc->type == XML_ELEMENT_NODE)
		{
			if ((!xmlStrcmp(currentDoc->name, (const xmlChar *)"id")))
			{
				pPrivilegeInfo->pId = xmlNodeListGetString(privilegeDoc, currentDoc->xmlChildrenNode, 1);
			}
			else if ((!xmlStrcmp(currentDoc->name, (const xmlChar *)"privacy")))
			{
				pPrivilegeInfo->pPrivacy = xmlNodeListGetString(privilegeDoc, currentDoc->xmlChildrenNode, 1);
			}
			else if ((!xmlStrcmp(currentDoc->name, (const xmlChar *)"privilege_level")))
			{
				pPrivilegeInfo->pPrivilegeLevel = xmlNodeListGetString(privilegeDoc, currentDoc->xmlChildrenNode, 1);
			}
			else if ((!xmlStrcmp(currentDoc->name, (const xmlChar *)"string_info")))
			{
				res = ParseStringInfo(privilegeDoc, currentDoc, &(pPrivilegeInfo->pStringInfo));
				TryCatch(res == PRIV_CHECKER_SUCCESS, , "[%s] Propagated.", Util::GetErrorMessage(res));
			}
			else
			{
				LOGE("[PRIV_CHECKER_ERROR_INVALID_ARG] Invalid policy type: %s", currentDoc->name);
				res = PRIV_CHECKER_ERROR_INVALID_ARG;
				goto CATCH;
			}
		}
		currentDoc = currentDoc->next;
	}

	res = SetPrivilegeDb(pPrivilegeInfo);
	TryCatch(res == PRIV_CHECKER_SUCCESS, , "[%s] Propagated.", Util::GetErrorMessage(res));

	LOGI("leave");
	return res;

CATCH:

	if (pPrivilegeInfo != NULL)
	{
		free(pPrivilegeInfo);
	}

	LOGI("error!");
	return res;
}

int
Parser::ParsePrivilegeList (xmlDocPtr privilegeListDoc, xmlNodePtr currentDoc)
{
	LOGI("enter");
	int res = PRIV_CHECKER_SUCCESS;

	currentDoc = currentDoc->xmlChildrenNode;
	while (currentDoc != NULL)
	{
		if (currentDoc->type == XML_ELEMENT_NODE)
		{
			if ((!xmlStrcmp(currentDoc->name, (const xmlChar *)"privilege")))
			{
				res = ParsePrivilege(privilegeListDoc, currentDoc);
				TryReturn(res == PRIV_CHECKER_SUCCESS, , res, "[%s] Propagated.", Util::GetErrorMessage(res));
			}
			else
			{
				LOGE("[PRIV_CHECKER_ERROR_INVALID_ARG] Invalid policy type: %s", currentDoc->name);
				res = PRIV_CHECKER_ERROR_INVALID_ARG;
				return res;
			}
		}
		currentDoc = currentDoc->next;
	}

	LOGI("leave");
	return res;
}

int
Parser::ParsePrivacyList (xmlDocPtr privacyListDoc, xmlNodePtr currentDoc)
{
	LOGI("enter");
	int res = PRIV_CHECKER_SUCCESS;

	PrivacyInfo* pPrivacyInfo = (PrivacyInfo*)calloc(1, sizeof(PrivacyInfo));
	TryCatch(pPrivacyInfo != NULL, res = PRIV_CHECKER_ERROR_OUT_OF_MEMORY, "[PRIV_CHECKER_ERROR_OUT_OF_MEMORY] The memory is insufficient.");

	currentDoc = currentDoc->xmlChildrenNode;
	while (currentDoc != NULL)
	{
		if (currentDoc->type == XML_ELEMENT_NODE)
		{
			if ((!xmlStrcmp(currentDoc->name, (const xmlChar *)"id")))
			{
				pPrivacyInfo->pId = xmlNodeListGetString(privacyListDoc, currentDoc->xmlChildrenNode, 1);
			}
			else if ((!xmlStrcmp(currentDoc->name, (const xmlChar *)"string_info")))
			{
				res = ParseStringInfo(privacyListDoc, currentDoc, &(pPrivacyInfo->pStringInfo));
				TryCatch(res == PRIV_CHECKER_SUCCESS, , "[%s] Propagated.", Util::GetErrorMessage(res));
			}
			else
			{
				LOGE("[PRIV_CHECKER_ERROR_INVALID_ARG] Invalid policy type: %s", currentDoc->name);
				res = PRIV_CHECKER_ERROR_INVALID_ARG;
				goto CATCH;
			}
		}
		currentDoc = currentDoc->next;
	}

	res = SetPrivacyDb(pPrivacyInfo);
	TryCatch(res == PRIV_CHECKER_SUCCESS, , "[%s] Propagated.", Util::GetErrorMessage(res));

	LOGI("leave");
	return res;

CATCH:

	LOGI("error!");
	return res;

}


int
Parser::ParseExtendedPolicy(char *docName)
{
	LOGI("enter");
	int res = PRIV_CHECKER_SUCCESS;
	xmlDocPtr doc;
	xmlNodePtr currentDoc;

	doc = xmlParseFile(docName);
	TryReturn(doc != NULL, , PRIV_CHECKER_ERROR_XML_ERROR, "[PRIV_CHECKER_ERROR_XML_ERROR] XML parsing is failed.");

	currentDoc = xmlDocGetRootElement(doc);
	TryReturn(currentDoc != NULL, xmlFreeDoc(doc), PRIV_CHECKER_ERROR_XML_ERROR, "[PRIV_CHECKER_ERROR_XML_ERROR] Document is empty.");


	TryReturn(xmlStrcmp(currentDoc->name, (const xmlChar *)"extended_policy") == MATCHED, xmlFreeDoc(doc), PRIV_CHECKER_ERROR_INVALID_ARG,
										"[PRIV_CHECKER_ERROR_INVALID_ARG] Root node is not <extended_policy> but <%s>", currentDoc->name);


	currentDoc = currentDoc->xmlChildrenNode;
	while (currentDoc != NULL)
	{
		if (currentDoc->type == XML_ELEMENT_NODE)
		{
			if ((!xmlStrcmp(currentDoc->name, (const xmlChar *)"privilege_list")))
			{
				res = ParsePrivilegeList (doc, currentDoc);
				TryReturn(res == PRIV_CHECKER_SUCCESS, xmlFreeDoc(doc), res, "[%s] Propagated.", Util::GetErrorMessage(res));
			}
			else if ((!xmlStrcmp(currentDoc->name, (const xmlChar *)"privacy_list")))
			{
				res = ParsePrivacyList (doc, currentDoc);
				TryReturn(res == PRIV_CHECKER_SUCCESS, xmlFreeDoc(doc), res, "[%s] Propagated.", Util::GetErrorMessage(res));
			}
			else
			{
				LOGE("[PRIV_CHECKER_ERROR_INVALID_ARG] Invalid policy type: %s", currentDoc->name);
				xmlFreeDoc(doc);
				return PRIV_CHECKER_ERROR_INVALID_ARG;

			}
		}
		currentDoc = currentDoc->next;
	}

	xmlFreeDoc(doc);

	LOGI("leave");
	return res;
}

