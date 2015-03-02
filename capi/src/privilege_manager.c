
#include <dlog.h>
#include <stdlib.h>
#include "privilege_db_manager.h"
#include "privilege_manager.h"



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

static int __privilege_manager_check_privilege_list(const char* privilege, int* privilege_level)
{
    TryReturn(privilege != NULL, PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] privilege is NULL");

    int ret;
    GList* privilege_list;
    ret = privilege_db_manager_get_privilege_list(&privilege_list);
    if(ret != PRIVILIEGE_DB_MANAGER_ERR_NONE){
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


int privilege_manager_verify_privilege_name(privilege_manager_package_type_e package_type, GList* privilege_list, GList **error_privilege_name_list)
{
    LOGD("privilege_manager_verify_privilege_name called");
    GList *l;
    int ret;
    int ret_val = PRVMGR_ERR_NONE;
    int i = 0;
    GList* temp_error_privilege_list = NULL;

    TryReturn(privilege_list != NULL, PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] privilege_list is NULL");

    for (l = privilege_list; l != NULL; l = l->next)
    {
        int privilege_level_id;
        char* privilege_name = (char*)l->data;

        if(privilege_name == NULL)
        {
            g_list_free(temp_error_privilege_list);
            LOGE("[PRVMGR_ERR_INVALID_PARAMETER] privilege_list[%d] is NULL", i);
            return PRVMGR_ERR_INVALID_PARAMETER;
        }
        i++;

        LOGD("Checking privilege : %s", privilege_name);

        ret = __privilege_manager_check_privilege_list(privilege_name, &privilege_level_id);
        if(ret == PRVMGR_ERR_NO_EXIST_PRIVILEGE)
        {
            LOGE("[PRVMGR_ERR_NO_EXIST_PRIVILEGE] There are no privilege %s in DB", privilege_name);

            char* error_privilege_name = strdup(privilege_name);
            TryReturn(privilege_name != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_privilege_name's strdup is failed.");

            temp_error_privilege_list = g_list_append(temp_error_privilege_list, error_privilege_name);
            ret_val = PRVMGR_ERR_NO_EXIST_PRIVILEGE;
        }
        else if(ret == PRVMGR_ERR_INTERNAL_ERROR || ret == PRVMGR_ERR_INVALID_PARAMETER)
        {
            return ret;
        }
    }

    *error_privilege_name_list = temp_error_privilege_list;

    return ret_val;
}


const char* __get_privilege_level_string(privilege_db_manager_privilege_level_e privilege_db_manager_privilege_level)
{
    if(privilege_db_manager_privilege_level == PRIVILIEGE_DB_MANAGER_PRIVILEGE_LEVEL_PUBLIC){
        return "public";
    }else if(privilege_db_manager_privilege_level == PRIVILIEGE_DB_MANAGER_PRIVILEGE_LEVEL_PARTNER){
        return "partner";
    }else if(privilege_db_manager_privilege_level == PRIVILIEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM){
        return "platform";
    }else{
        return "not defined privilege";
    }
}

int privilege_manager_verify_privilege_level(privilege_manager_package_type_e package_type, GList* privilege_list, privilege_manager_visibility_e visibility, GList **error_privilege_name_list, GList **error_privilege_level_list)
{
    LOGD("privilege_manager_verify_privilege_level called");
    GList *l;
    int ret;
    int ret_val = PRVMGR_ERR_NONE;
    int i = 0;
    GList* temp_error_privilege_name_list = NULL;
    GList* temp_error_privilege_level_list = NULL;

    TryReturn(privilege_list != NULL, PRVMGR_ERR_INVALID_PARAMETER, "[PRVMGR_ERR_INVALID_PARAMETER] privilege_list is NULL");

    for (l = privilege_list; l != NULL; l = l->next)
    {
        int privilege_level_id;
        char* privilege_name = (char*)l->data;

        if(privilege_name == NULL)
        {
            g_list_free(temp_error_privilege_level_list);
            g_list_free(temp_error_privilege_name_list);
            LOGE("[PRVMGR_ERR_INVALID_PARAMETER] privilege_list[%d] is NULL", i);
            return PRVMGR_ERR_INVALID_PARAMETER;
        }
        i++;

        LOGD("Checking privilege : %s", privilege_name);
        ret = __privilege_manager_check_privilege_list(privilege_name, &privilege_level_id);
        if(ret == PRVMGR_ERR_NONE)
        {
            LOGD("visibility : %d", visibility);
            LOGD("privilege level : %d", privilege_level_id);

            if((visibility & PRVMGR_PACKAGE_VISIBILITY_PUBLIC) == PRVMGR_PACKAGE_VISIBILITY_PUBLIC){
                if(privilege_level_id == PRIVILIEGE_DB_MANAGER_PRIVILEGE_LEVEL_PARTNER || privilege_level_id == PRIVILIEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM)
                {
                    LOGE("[PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL] Visibility and Privilege level are mismatched");
                    LOGE("[PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL] Visibility : public, Privilege Level : %s", __get_privilege_level_string(privilege_level_id));
                    char* error_privilege_name = strdup(privilege_name);
                    TryReturn(error_privilege_name != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_privilege_name's strdup is failed.");
                    char* error_privilege_level = strdup(__get_privilege_level_string(privilege_level_id));
                    if(error_privilege_level == NULL)
                    {
                        free(error_privilege_name);
                        LOGE("[PRVMGR_ERR_OUT_OF_MEMORY] error_privilege_level's strdup is failed.");
                        return PRVMGR_ERR_OUT_OF_MEMORY;
                    }
                    temp_error_privilege_name_list = g_list_append(temp_error_privilege_name_list, error_privilege_name);
                    temp_error_privilege_level_list = g_list_append(temp_error_privilege_level_list, error_privilege_level);
                    ret_val = PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL;
                }
            }
            else if((visibility & PRVMGR_PACKAGE_VISIBILITY_PARTNER) == PRVMGR_PACKAGE_VISIBILITY_PARTNER){
                if(privilege_level_id == PRIVILIEGE_DB_MANAGER_PRIVILEGE_LEVEL_PLATFORM)
                {
                    LOGE("[PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL] Visibility and Privilege level are mismatched");
                    LOGE("[PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL] Visibility : partner, Privilege Level : %s", __get_privilege_level_string(privilege_level_id));
                    char* error_privilege_name = strdup(privilege_name);
                    TryReturn(error_privilege_name != NULL, PRVMGR_ERR_OUT_OF_MEMORY, "[PRVMGR_ERR_OUT_OF_MEMORY] error_privilege_name's strdup is failed.");
                    char* error_privilege_level = strdup(__get_privilege_level_string(privilege_level_id));
                    if(error_privilege_level == NULL)
                    {
                        free(error_privilege_name);
                        LOGE("[PRVMGR_ERR_OUT_OF_MEMORY] error_privilege_level's strdup is failed.");
                        return PRVMGR_ERR_OUT_OF_MEMORY;
                    }
                    temp_error_privilege_name_list = g_list_append(temp_error_privilege_name_list, error_privilege_name);
                    temp_error_privilege_level_list = g_list_append(temp_error_privilege_level_list, error_privilege_level);
                    ret_val = PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL;
                }
            }
            else if((visibility & PRVMGR_PACKAGE_VISIBILITY_PLATFORM) == PRVMGR_PACKAGE_VISIBILITY_PLATFORM){

            }
            else{
                LOGE("[PRVMGR_ERR_INVALID_PARAMETER] visibility dont include any public, partner, platform");
                g_list_free(temp_error_privilege_name_list);
                g_list_free(temp_error_privilege_level_list);
                return PRVMGR_ERR_INVALID_PARAMETER;
            }
        }
        else if(ret == PRVMGR_ERR_INTERNAL_ERROR || ret == PRVMGR_ERR_INVALID_PARAMETER)
        {
            return ret;
        }
    }

    *error_privilege_name_list = temp_error_privilege_name_list;
    *error_privilege_level_list = temp_error_privilege_level_list;

    return ret_val;
}
