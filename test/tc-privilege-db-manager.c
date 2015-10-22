#include <stdio.h>
#include <privilege_db_manager.h>

#define BRIGHTNESS 0
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BG_BLACK 40

static int fail_cnt=0;
static int success_cnt=0;

static void __change_color_to_red(){
    printf("%c[%d;%dm", 0x1B, BRIGHTNESS, RED);
}

static void __change_color_to_green(){
    printf("%c[%d;%dm", 0x1B, BRIGHTNESS, GREEN);
}

static void __change_color_to_yellow(){
    printf("%c[%d;%dm", 0x1B, BRIGHTNESS, YELLOW);
}

static void __change_color_to_origin(){
    printf("%c[%dm", 0x1B, 0);
}

static void __free_privilege_list(GList* privilege_list)
{
    GList* l = NULL;
    for (l = privilege_list; l != NULL; l = l->next)
    {
        privilege_info_db_row_s* privilege_info_db_row = (privilege_info_db_row_s*)l->data;
        if(privilege_info_db_row->profile != NULL)
            free(privilege_info_db_row->profile);
        if(privilege_info_db_row->package_type != NULL)
            free(privilege_info_db_row->package_type);
        if(privilege_info_db_row->privilege_name != NULL)
            free(privilege_info_db_row->privilege_name);
        if(privilege_info_db_row->privilege_display != NULL)
            free(privilege_info_db_row->privilege_display);
        if(privilege_info_db_row->privilege_description != NULL)
            free(privilege_info_db_row->privilege_description);
        if(privilege_info_db_row->privilege_level != NULL)
            free(privilege_info_db_row->privilege_level);
        if(privilege_info_db_row->issued_version != NULL)
            free(privilege_info_db_row->issued_version);
        if(privilege_info_db_row->expired_version != NULL)
            free(privilege_info_db_row->expired_version);
    }
}

static const char* __get_result_string(privilege_db_manager_error_e ret){
    if(ret == PRIVILEGE_DB_MANAGER_ERR_NONE){
        return "PRIVILEGE_DB_MANAGER_ERR_NONE";
    }
    else if(ret == PRIVILEGE_DB_NO_EXIST_RESULT){
        return "PRIVILEGE_DB_NO_EXIST_RESULT";
    }
    else if(ret == PRIVILEGE_DB_MANAGER_ERR_CONNECTION_FAIL){
        return "PRIVILEGE_DB_MANAGER_ERR_CONNECTION_FAIL";
    }
    else if(ret == PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY){
        return "PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY";
    }
    else if(ret == PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY){
        return "PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY";
    }
    else{
        return "FAIL";
    }
}

void __test_privilege_db_manager_get_privilege_list(){
    GList* privilege_list;
    int ret = privilege_db_manager_get_privilege_list("2.4", PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, &privilege_list);
    if(ret != PRIVILEGE_DB_MANAGER_ERR_NONE){
        printf("failed to call privilege_db_manager_get_privilege_list()\n");
        printf("error message = %s\n", __get_result_string(ret));
        __change_color_to_red();
        printf("test fail\n");
        __change_color_to_origin();
        fail_cnt++;
        return;
    }

    GList* l = NULL;
    for (l = privilege_list; l != NULL; l = l->next)
    {
        privilege_info_db_row_s* privilege_info_db_row = (privilege_info_db_row_s*)l->data;
        printf("privilege_name = %s\n", privilege_info_db_row->privilege_name);
        printf("privilege_level_id = %d\n", privilege_info_db_row->privilege_level_id);
    }
    __free_privilege_list(privilege_list);

    __change_color_to_green();
    printf("test success\n");
    __change_color_to_origin();
    success_cnt++;
}

void __check_get_privilege_display_result(privilege_db_manager_error_e expected_result, privilege_db_manager_error_e result, char* privilege_display)
{
    printf("expected result = %s, result = %s\n", __get_result_string(expected_result), __get_result_string(result));

    if(expected_result != result)
    {
        printf("not matched\n");
        __change_color_to_red();
        printf("test fail\n");
        fail_cnt++;
    }else{
        printf("matched\n");

        if(privilege_display == NULL){
            printf("privilege_display = NULL\n");
        }else{
            printf("privilege_display = %s\n", privilege_display);
        }
        __change_color_to_green();
        printf("test success\n");
        success_cnt++;
    }
    __change_color_to_origin();
}


void __test_privilege_db_manager_get_privilege_display(){
    int ret;

    char* privilege_display = NULL;
    printf("-----------------------------------------------------------\n");
    printf("privilege : http://tizen.org/privilege/location\n");
    printf("privilege_type : core\n");
    printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
    ret = privilege_db_manager_get_privilege_display(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/location", "2.3", &privilege_display);
    __check_get_privilege_display_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_display);
    free(privilege_display);

    printf("-----------------------------------------------------------\n");
    printf("privilege : http://tizen.org/privilege/power\n");
    printf("privilege_type : wrt\n");
    printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
    privilege_display = NULL;
    ret = privilege_db_manager_get_privilege_display(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, "http://tizen.org/privilege/power", "2.3", &privilege_display);
    __check_get_privilege_display_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_display);
    free(privilege_display);

    printf("-----------------------------------------------------------\n");
    printf("privilege : http://tizen.org/privilege/messasdfsfsdfsdfad\n");
    printf("expected result : PRIVILEGE_DB_NO_EXIST_RESULT\n");
    privilege_display = NULL;
    ret = privilege_db_manager_get_privilege_display(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/messasdfsfsdfsdfad", "2.3", &privilege_display);
    __check_get_privilege_display_result(PRIVILEGE_DB_NO_EXIST_RESULT, ret, privilege_display);
    printf("-----------------------------------------------------------\n");
    free(privilege_display);
}

void __test_privilege_db_manager_get_privilege_display_by_name(){
    int ret;

    char* privilege_display = NULL;
    printf("-----------------------------------------------------------\n");
    printf("privilege : http://tizen.org/privilege/location\n");
    printf("privilege_type : core\n");
    printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
    ret = privilege_db_manager_get_privilege_display_by_name(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/location", &privilege_display);
    __check_get_privilege_display_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_display);
    free(privilege_display);

    printf("-----------------------------------------------------------\n");
    printf("privilege : http://tizen.org/privilege/power\n");
    printf("privilege_type : wrt\n");
    printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
    privilege_display = NULL;
    ret = privilege_db_manager_get_privilege_display_by_name(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, "http://tizen.org/privilege/power", &privilege_display);
    __check_get_privilege_display_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_display);
    free(privilege_display);

    printf("-----------------------------------------------------------\n");
    printf("privilege : http://tizen.org/privilege/messasdfsfsdfsdfad\n");
    printf("expected result : PRIVILEGE_DB_NO_EXIST_RESULT\n");
    privilege_display = NULL;
    ret = privilege_db_manager_get_privilege_display_by_name(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/messasdfsfsdfsdfad", &privilege_display);
    __check_get_privilege_display_result(PRIVILEGE_DB_NO_EXIST_RESULT, ret, privilege_display);
    printf("-----------------------------------------------------------\n");
    free(privilege_display);
}

void __check_get_privilege_description_result(privilege_db_manager_error_e expected_result, privilege_db_manager_error_e result, char* privilege_description)
{
    printf("expected result = %s, result = %s\n", __get_result_string(expected_result), __get_result_string(result));

    if(expected_result != result)
    {
        printf("not matched\n");
        __change_color_to_red();
        printf("test fail\n");
        fail_cnt++;
    }else{
        printf("matched\n");

        if(privilege_description == NULL){
            printf("privilege_description = NULL\n");
        }else{
            printf("privilege_description = %s\n", privilege_description);
        }
        __change_color_to_green();
        printf("test success\n");
        success_cnt++;
    }
    __change_color_to_origin();
}

void __test_privilege_db_manager_get_privilege_description(){

    int ret;

    char* privilege_display = NULL;
    printf("-----------------------------------------------------------\n");
    printf("privilege : http://tizen.org/privilege/location\n");
    printf("privilege_type : core\n");
    printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
    ret = privilege_db_manager_get_privilege_description(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/location", "2.3", &privilege_display);
    __check_get_privilege_description_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_display);

    if(privilege_display != NULL){
        free(privilege_display);
        privilege_display = NULL;
    }

    printf("-----------------------------------------------------------\n");
    printf("privilege : http://tizen.org/privilege/power\n");
    printf("privilege_type : wrt\n");
    printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
    ret = privilege_db_manager_get_privilege_description(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, "http://tizen.org/privilege/power", "2.3", &privilege_display);
    __check_get_privilege_description_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_display);

    if(privilege_display != NULL){
        free(privilege_display);
        privilege_display = NULL;
    }

    printf("-----------------------------------------------------------\n");
    printf("privilege : http://tizen.org/privilege/messasdfsfsdfsdfad\n");
    printf("expected result : PRIVILEGE_DB_NO_EXIST_RESULT\n");
    ret = privilege_db_manager_get_privilege_description(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/messasdfsfsdfsdfad", "2.3", &privilege_display);
    __check_get_privilege_description_result(PRIVILEGE_DB_NO_EXIST_RESULT, ret, privilege_display);
    printf("-----------------------------------------------------------\n");

    free(privilege_display);

}
void __test_privilege_db_manager_get_privilege_description_by_name(){

    int ret;

    char* privilege_display = NULL;
    printf("-----------------------------------------------------------\n");
    printf("privilege : http://tizen.org/privilege/location\n");
    printf("privilege_type : core\n");
    printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
    ret = privilege_db_manager_get_privilege_description_by_name(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/location", &privilege_display);
    __check_get_privilege_description_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_display);

    if(privilege_display != NULL){
        free(privilege_display);
        privilege_display = NULL;
    }

    printf("-----------------------------------------------------------\n");
    printf("privilege : http://tizen.org/privilege/power\n");
    printf("privilege_type : wrt\n");
    printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
    ret = privilege_db_manager_get_privilege_description_by_name(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, "http://tizen.org/privilege/power", &privilege_display);
    __check_get_privilege_description_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_display);

    if(privilege_display != NULL){
        free(privilege_display);
        privilege_display = NULL;
    }

    printf("-----------------------------------------------------------\n");
    printf("privilege : http://tizen.org/privilege/messasdfsfsdfsdfad\n");
    printf("expected result : PRIVILEGE_DB_NO_EXIST_RESULT\n");
    ret = privilege_db_manager_get_privilege_description_by_name(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/messasdfsfsdfsdfad", &privilege_display);
    __check_get_privilege_description_result(PRIVILEGE_DB_NO_EXIST_RESULT, ret, privilege_display);
    printf("-----------------------------------------------------------\n");

    free(privilege_display);

}

int main()
{
    __change_color_to_yellow();
    printf("Test function : privilege_db_manager_get_privilege_list\n");
    __change_color_to_origin();
    __test_privilege_db_manager_get_privilege_list();

    __change_color_to_yellow();
    printf("Test function : privilege_db_manager_get_privilege_display\n");
    __change_color_to_origin();
    __test_privilege_db_manager_get_privilege_display();

	__change_color_to_yellow();
    printf("Test function : privilege_db_manager_get_privilege_display_by_name\n");
    __change_color_to_origin();
    __test_privilege_db_manager_get_privilege_display_by_name();

    __change_color_to_yellow();
    printf("Test function : privilege_db_manager_get_privilege_description\n");
    __change_color_to_origin();
    __test_privilege_db_manager_get_privilege_description();

    __change_color_to_yellow();
    printf("Test function : privilege_db_manager_get_privilege_description_by_name\n");
    __change_color_to_origin();
    __test_privilege_db_manager_get_privilege_description_by_name();

    __change_color_to_green();
    printf("Test Complete\n");
    printf("success : %d, ", success_cnt);
    __change_color_to_red();
    printf("fail : %d\n", fail_cnt);
    __change_color_to_origin();

    return 0;
}

