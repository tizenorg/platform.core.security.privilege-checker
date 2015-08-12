#include <dlog.h>
#include <stdlib.h>
#include <stdio.h>

#include "privilege_db_manager.h"
#include "privilege_manager.h"

#define MESSAGE_SIZE            512
#define MESSAGE_LIST_SIZE       10000

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "PRIVILEGE_MANAGER"
#endif

#define TryReturn(condition, returnValue, ...)  \
    if (!(condition)) { \
        LOGE(__VA_ARGS__); \
        return returnValue; \
    } \
    else {;}
/*
#ifdef CURRENT_TIZEN_VERSION
    static char* privilege_manager_current_tizen_version = (char*)CURRENT_TIZEN_VERSION;
#else
    static char* privilege_manager_current_tizen_version = "2.4"
#endif
*/


static void __free_privilege_list(GList* privilege_list)
{
    GList* l = NULL;
    for (l = privilege_list; l != NULL; l = l->next)
    {
        privilege_info_db_row_s* privilege_info_db_row = (privilege_info_db_row_s*)l->data;
        if(privilege_info_db_row->privilege_name != NULL)
            free(privilege_info_db_row->privilege_name);
    }
}


static int __privilege_manager_check_privilege_list(const char* api_version, const char* privilege, GList* vaild_privilege_list, int* privilege_level, char** changed_to, char** valid_api_version)
{
    TryReturn(privilege != NULL, PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] privilege is NULL");
    int i, is_valid_version = 0;
    GList* l = NULL;
    for (l = vaild_privilege_list; l != NULL; l = l->next)
    {
        privilege_info_db_row_s* privilege_info_db_row = (privilege_info_db_row_s*)l->data;
        if (strcmp(privilege_info_db_row->privilege_name, privilege) == 0)
        {
            LOGD("Matched privilege name exist");
            LOGD("Check api version");
            for(i=0; i<3; i++)
            {
                if( !(api_version[i] <= privilege_info_db_row->expired_version[i]) )
                {
                    is_valid_version = 1;
                }
                else if ( !(api_version[i] >= privilege_info_db_row->issued_version[i]))
                {
                        is_valid_version = 2;
                }
            }

            if(is_valid_version > 0)
            {
                LOGD("%s is invalid in tizen version: %s", privilege_info_db_row->privilege_name,  api_version);
            }

            if(is_valid_version == 0)
            {
                *privilege_level = privilege_info_db_row->privilege_level_id;
                return PRVMGR_ERR_NONE;
            }
            else if(is_valid_version == 1)
            {
                LOGD("privilege deprecated version is lower than api version");
                *valid_api_version = strdup(privilege_info_db_row->expired_version);
                TryReturn(valid_api_version != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] valid_api_version's strdup is failed.");
                if(privilege_info_db_row->changed_to != NULL && strcmp(privilege_info_db_row->changed_to, "") != 0 )
                {
                    LOGE("%s was changed to %s", privilege, privilege_info_db_row->changed_to);

                    *changed_to = strdup(privilege_info_db_row->changed_to);
                    TryReturn(changed_to != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] changed_to's strdup is failed.");
                }
                return PRVMGR_ERR_DEPRECATED_PRIVILEGE;
            }
            else if(is_valid_version == 2)
            {
                LOGD("privilege issued version is higher than api version");
                *valid_api_version = strdup(privilege_info_db_row->issued_version);
                TryReturn(valid_api_version != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] valid_api_version's strdup is failed.");
                return PRVMGR_ERR_NO_EXIST_PRIVILEGE;
            }
        }
    }

    return  PRVMGR_ERR_NO_EXIST_PRIVILEGE;
}

const char* __get_privilege_level_string(privilege_db_manager_privilege_level_e privilege_db_manager_privilege_level)
{
    if(privilege_db_manager_privilege_level == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PUBLIC){
        return "public";
    }else if(privilege_db_manager_privilege_level == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PARTNER){
        return "partner";
    }else if(privilege_db_manager_privilege_level == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM){
        return "platform";
    }else{
        return "not defined privilege";
    }
}


int privilege_manager_verify_privilege(const char* api_version, privilege_manager_package_type_e package_type, GList* privilege_list, privilege_manager_visibility_e visibility, char **error_message)
{
    LOGD("privilege_manager_verify_privilege called");
    GList *l;
    int ret;
    int ret_val = PRVMGR_ERR_NONE;
    char message_list[MESSAGE_LIST_SIZE] = {0,};
    char message[MESSAGE_SIZE] = {0,};
    char* changed_to = NULL;
    char* valid_api_version = NULL;
    GList* vaild_privilege_list;
    char* wrt_active_version = "2.4";
    int is_vaild_wrt_version;

    //Check invaild parameters
    if (api_version == NULL){
        LOGE("[PRVMGR_ERR_INVALID_PARAMETER] api_version is NULL");
        *error_message = strdup("[PRVMGR_ERR_INVALID_PARAMETER] api_version is NULL");
        TryReturn(error_message != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");

        return PRVMGR_ERR_INVALID_PARAMETER;
    }else{
        int i;
        int is_vaild_version_type = 1;
        int api_version_size = strlen(api_version);
        if( api_version_size % 2 == 1 && (3 <= api_version_size && api_version_size <= 7) ){
            for(i=0; i<api_version_size; i++){
                if(i % 2 == 0){
                    if( !('0' <= api_version[i] && api_version[i] <= '9') ){
                        is_vaild_version_type = 0;
                    }
                }else{
                    if(api_version[i] != '.'){
                        is_vaild_version_type = 0;
                    }
                }
            }
        }else{
            is_vaild_version_type = 0;
        }

        if(is_vaild_version_type == 0){
            LOGE("[PRVMGR_ERR_INVALID_PARAMETER] %s is in invaild form. api_version form should be X.X, X.X.X or X.X.X.X (X=integer)", api_version);
            *error_message = strdup("[PRVMGR_ERR_INVALID_PARAMETER] api_version form should be a X.X, X.X.X or X.X.X.X (X=integer)");
            TryReturn(error_message != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed");

            return PRVMGR_ERR_INVALID_PARAMETER;
        }
    }

    if(package_type != PRVMGR_PACKAGE_TYPE_WRT && package_type != PRVMGR_PACKAGE_TYPE_CORE){
        LOGD("checking package type = %d", package_type);
        LOGE("[PRVMGR_ERR_INVALID_PARAMETER] package_type is not a PRVMGR_PACKAGE_TYPE_WRT or PRVMGR_PACKAGE_TYPE_CORE");
        *error_message = strdup("[PRVMGR_ERR_INVALID_PARAMETER] package_type is a unknown type. package_type must be a PRVMGR_PACKAGE_TYPE_WRT or PRVMGR_PACKAGE_TYPE_CORE");
        TryReturn(error_message != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");

        return PRVMGR_ERR_INVALID_PARAMETER;
    }

    if( (visibility & PRVMGR_PACKAGE_VISIBILITY_PUBLIC) != PRVMGR_PACKAGE_VISIBILITY_PUBLIC
        && (visibility & PRVMGR_PACKAGE_VISIBILITY_PARTNER) != PRVMGR_PACKAGE_VISIBILITY_PARTNER
        && (visibility & PRVMGR_PACKAGE_VISIBILITY_PLATFORM) != PRVMGR_PACKAGE_VISIBILITY_PLATFORM ){

        LOGE("[PRVMGR_ERR_INVALID_PARAMETER] visibility don't include any public, partner, platform");
        *error_message = strdup("[INVALID_PARAMETER] Signature Level is invalid. Signature Level must be a public, partner or platform");
        TryReturn(error_message != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");

        return PRVMGR_ERR_INVALID_PARAMETER;
    }


    is_vaild_wrt_version = 1;
    int i=0;
    if(package_type == PRVMGR_PACKAGE_TYPE_WRT){
        for(i=0; i<3; i++){
            if( ! (api_version[i] >= wrt_active_version[i]) ){
                is_vaild_wrt_version = 0;
            }
        }
    }

    if(privilege_list == NULL){
        LOGE("[PRVMGR_ERR_INVALID_PARAMETER] privilege_list is NULL");
        *error_message = strdup("[PRVMGR_ERR_INVALID_PARAMETER] privilege_list is NULL");
        TryReturn(error_message != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");

        return PRVMGR_ERR_INVALID_PARAMETER;
    }


    //Get vaild privilege list
    ret = privilege_db_manager_get_privilege_list(api_version, package_type, &vaild_privilege_list);
    if(ret != PRIVILEGE_DB_MANAGER_ERR_NONE){
        LOGE("[FAIL TO CALL FUNCTION] privilege_db_manager_get_privilege_list()");
        *error_message = strdup("[PRVMGR_ERR_INTERNAL_ERROR] failed to get privilege list from DB");
        return PRVMGR_ERR_INTERNAL_ERROR;
    }

    //Compare received privilege with valid privilege list
    for (l = privilege_list; l != NULL; l = l->next)
    {
        int privilege_level_id = PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PUBLIC;
        char* privilege_name = (char*)l->data;

        LOGD("Checking privilege = %s", privilege_name);
        ret = __privilege_manager_check_privilege_list(api_version, privilege_name, vaild_privilege_list, &privilege_level_id, &changed_to, &valid_api_version);

        if( is_vaild_wrt_version == 0 )
        {
            ret = PRVMGR_ERR_NONE;
        }

        if(ret == PRVMGR_ERR_NO_EXIST_PRIVILEGE)
        {
            LOGE("[PRVMGR_ERR_NO_EXIST_PRIVILEGE] There are no privilege %s in Tizen version %s", privilege_name, api_version);

            memset(message, 0, MESSAGE_SIZE);
            if(valid_api_version != NULL && strcmp(valid_api_version, "") != 0)
            {
                sprintf(message, "[NO_EXIST_PRIVILEGE]%s is issued at Tizen version %s\n", privilege_name, valid_api_version);
            }
            else
            {
                sprintf(message, "[NO_EXIST_PRIVILEGE]%s is an invalid privilege\n", privilege_name);
            }

            strcat(message_list, message);

            ret_val = PRVMGR_ERR_INVALID_PRIVILEGE;

        }else if(ret == PRVMGR_ERR_DEPRECATED_PRIVILEGE){

            LOGE("[PRVMGR_ERR_DEPRECATED_PRIVILEGE]%s is deprecated before Tizen version %s", privilege_name, api_version);

            memset(message, 0, MESSAGE_SIZE);
            if(changed_to != NULL && strcmp(changed_to, "") != 0)
            {
                sprintf(message, "[DEPRECATED_PRIVILEGE]Use %s instead of %s\n", changed_to, privilege_name);
            }
            else
            {
                sprintf(message, "[DEPRECATED_PRIVIELGE]%s is deprecated after Tizen version %s\n", privilege_name, valid_api_version);
            }
            strcat(message_list, message);

            ret_val = PRVMGR_ERR_INVALID_PRIVILEGE;

        }else if(ret == PRVMGR_ERR_NONE){
            LOGD("visibility = %d", visibility);
            LOGD("privilege level = %d", privilege_level_id);

            if((visibility & PRVMGR_PACKAGE_VISIBILITY_PUBLIC) == PRVMGR_PACKAGE_VISIBILITY_PUBLIC){
                if(privilege_level_id == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PARTNER || privilege_level_id == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM)
                {
                    LOGE("[PRVMGR_ERR_MISMATCHED_PRIVILEGE_LEVEL] Visibility and Privilege level are mismatched");
                    LOGE("[PRVMGR_ERR_MISMATCHED_PRIVILEGE_LEVEL] Visibility = public, Privilege Level = %s", __get_privilege_level_string(privilege_level_id));

                    memset(message, 0, MESSAGE_SIZE);
                    sprintf(message, "[MISMATCHED_PRIVILEGE_LEVEL]Signature Level is too low to use %s - Signature Level = public, Privilege Level = %s\n", privilege_name, __get_privilege_level_string(privilege_level_id));

                    strcat(message_list, message);

                    ret_val = PRVMGR_ERR_INVALID_PRIVILEGE;
                }
            }
            else if((visibility & PRVMGR_PACKAGE_VISIBILITY_PARTNER) == PRVMGR_PACKAGE_VISIBILITY_PARTNER){
                if(privilege_level_id == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM)
                {
                    LOGE("[PRVMGR_ERR_MISMATCHED_PRIVILEGE_LEVEL] Visibility and Privilege level are mismatched");
                    LOGE("[PRVMGR_ERR_MISMATCHED_PRIVILEGE_LEVEL] Visibility = partner, Privilege Level = %s", __get_privilege_level_string(privilege_level_id));

                    memset(message, 0, MESSAGE_SIZE);
                    sprintf(message, "[MISMATCHED_PRIVILEGE_LEVEL]Signature Level is too low to use %s - Signature Level = partner, Privilege Level = %s\n", privilege_name, __get_privilege_level_string(privilege_level_id));

                    strcat(message_list, message);

                    ret_val = PRVMGR_ERR_INVALID_PRIVILEGE;
                }
            }
        }
        else if(ret == PRVMGR_ERR_INVALID_PARAMETER)
        {
            LOGE("[PRVMGR_ERR_INVALID_PARAMETER] privilege_name is NULL");
            *error_message = strdup("[INVALID_PARAMETER] Invaild parameter was passed.");
            TryReturn(error_message != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");

            ret_val = PRVMGR_ERR_INVALID_PARAMETER;
            goto FINISH;
        }
        else if(ret == PRVMGR_ERR_INTERNAL_ERROR)
        {
            LOGE("[PRVMGR_ERR_INVALID_PARAMETER] Unknown Error occured.");
            *error_message = strdup("[INTERNAL_ERROR] Unknown Error occured.");
            TryReturn(error_message != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");

            ret_val = PRVMGR_ERR_INTERNAL_ERROR;
            goto FINISH;
        }
    }

    if(ret_val != PRVMGR_ERR_NONE){
        *error_message = strdup(message_list);
        TryReturn(error_message != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_message's strdup is failed.");
    }

FINISH:
    g_list_free(vaild_privilege_list);
    return ret_val;
}


static int __privilege_manager_core_privilege_level(const char* privilege, int* privilege_level)
{
    TryReturn(privilege != NULL, PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] privilege is NULL");

    int ret;
    GList* privilege_list;
    ret = privilege_db_manager_get_privilege_list("2.4",PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, &privilege_list);
    if(ret != PRIVILEGE_DB_MANAGER_ERR_NONE){
        LOGE("[FAIL TO CALL FUNCTION] privilege_db_manager_get_privilege_list()");
        return PRVMGR_ERR_INTERNAL_ERROR;
    }

    GList* l = NULL;
    for (l = privilege_list; l != NULL; l = l->next)
    {
        privilege_info_db_row_s* privilege_info_db_row = (privilege_info_db_row_s*)l->data;
        if (strcmp(privilege_info_db_row->privilege_name, privilege) == 0)
        {
            *privilege_level = privilege_info_db_row->privilege_level_id;
            __free_privilege_list(privilege_list);
            return PRVMGR_ERR_NONE;
        }
    }
    __free_privilege_list(privilege_list);

    return  PRVMGR_ERR_NO_EXIST_PRIVILEGE;
}

int privilege_manager_verify_privilege_list(privilege_manager_package_type_e package_type, GList* privilege_list, privilege_manager_visibility_e visibility, char** error_privilege_name)
{
    LOGD("privilege_info_compare_privilege_level called");
    GList *l;
    int ret;
    int i = 0;
    *error_privilege_name = NULL;

    TryReturn(privilege_list != NULL, PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] privilege_list is NULL");

    for (l = privilege_list; l != NULL; l = l->next)
    {
        int privilege_level_id;
        char* privilege_name = (char*)l->data;

        TryReturn(privilege_name != NULL, PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] privilege_list[%d] is NULL", i);
        i++;

        LOGD("Checking privilege : %s", privilege_name);

        ret = __privilege_manager_core_privilege_level(privilege_name, &privilege_level_id);
        if(ret == PRVMGR_ERR_NO_EXIST_PRIVILEGE)
        {
            LOGE("[PRVMGR_ERR_NO_EXIST_PRIVILEGE] There are no privilege %s in DB", privilege_name);
            *error_privilege_name = (char*)calloc(strlen(privilege_name) + 1, sizeof(char));
            TryReturn(*error_privilege_name != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] Memory allocation failed.");
            memcpy(*error_privilege_name, privilege_name, strlen(privilege_name));

            return  PRVMGR_ERR_NO_EXIST_PRIVILEGE;
        }
        else if(ret == PRVMGR_ERR_NONE)
        {
            if(visibility == PRVMGR_PACKAGE_VISIBILITY_PARTNER){
                if(privilege_level_id == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM)
                {
                    LOGE("[PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL] Visibility and Privilege level are mismatched");
                    LOGE("[PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL] Visibility : partner, Privilege Level : %d", privilege_level_id);
                    *error_privilege_name = (char*)calloc(strlen(privilege_name) + 1, sizeof(char));
                    TryReturn(*error_privilege_name != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] Memory allocation failed.");
                    memcpy(*error_privilege_name, privilege_name, strlen(privilege_name));

                    return PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL;
                }
            }
            else if(visibility == PRVMGR_PACKAGE_VISIBILITY_PUBLIC){
                if(privilege_level_id == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PARTNER || privilege_level_id == PRIVILEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM)
                {
                    LOGE("[PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL] Visibility and Privilege level are mismatched");
                    LOGE("[PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL] Visibility : public, Privilege Level : %d", privilege_level_id);
                    *error_privilege_name = (char*)calloc(strlen(privilege_name) + 1, sizeof(char));
                    TryReturn(*error_privilege_name != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] Memory allocation failed.");
                    memcpy(*error_privilege_name, privilege_name, strlen(privilege_name));

                    return PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL;
                }
            }
        }
        else
        {
            return ret;
        }
    }

    return PRVMGR_ERR_NONE;
}

