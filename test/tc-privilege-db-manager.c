#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <privilege_db_manager.h>
#include <tc-common.h>

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


void __test_privilege_db_manager_get_privilege_list()
{
	GList *privilege_list = NULL;
	int ret = privilege_db_manager_get_privilege_list("2.4", PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, &privilege_list);
	if (ret != PRIVILEGE_DB_MANAGER_ERR_NONE) {
		printf("failed to call privilege_db_manager_get_privilege_list()\n");
		printf("error message = %s\n", __get_result_string('d', ret));
		__color_to_red();
		printf("test fail\n");
		__color_to_origin();
		fail_cnt++;
		return;
	}

	GList *l = NULL;
	for (l = privilege_list; l != NULL; l = l->next) {
		privilege_info_db_row_s *privilege_info_db_row = (privilege_info_db_row_s *) l->data;
		printf("privilege_name = %s\n", privilege_info_db_row->privilege_name);
		printf("privilege_level_id = %d\n", privilege_info_db_row->privilege_level_id);
	}
	__free_privilege_list(privilege_list);

	__color_to_green();
	printf("test success\n");
	__color_to_origin();
	success_cnt++;
}

void __print_mapped_privilege_list(int ret, GList* mapped_privilege_list)
{
	GList *l = NULL;
	if (ret == PRIVILEGE_DB_MANAGER_ERR_NONE && mapped_privilege_list != NULL) {
		for (l = mapped_privilege_list; l != NULL; l = l->next) {
			char *privilege_name = (char *)l->data;
			printf("mapped_privilege_name = %s\n", privilege_name);
		}
		success_cnt++;
	} else
		fail_cnt++;

	printf("test result = %s\n", __get_result_string('d', ret));
}

void __test_privilege_db_manager_get_mapped_privilege_list()
{
	GList *privilege_list = NULL;
	GList *mapped_privilege_list = NULL;
	int ret;

	__print_line();
	printf("privilege : http://tizen.org/privilege/internal/default/public\n");
	printf("api_version : 2.4\n");
	printf("package type : wrt\n");
	privilege_list = g_list_append(privilege_list, "http://tizen.org/privilege/internal/default/public");
	ret = privilege_db_manager_get_mapped_privilege_list("2.4", PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_mapped_privilege_list(ret, mapped_privilege_list);

	g_list_free(privilege_list);
	privilege_list = NULL;
	g_list_free(mapped_privilege_list);
	mapped_privilege_list = NULL;

	__print_line();
	printf("privilege : http://tizen.org/privilege/mediacapture\n");
	printf("api_version : 2.4\n");
	printf("package type : wrt\n");
	privilege_list = g_list_append(privilege_list, "http://tizen.org/privilege/mediacapture");
	ret = privilege_db_manager_get_mapped_privilege_list("2.4", PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_mapped_privilege_list(ret, mapped_privilege_list);

	g_list_free(privilege_list);
	privilege_list = NULL;
	g_list_free(mapped_privilege_list);
	mapped_privilege_list = NULL;

	__print_line();
	printf("privilege : http://tizen.org/privilege/internal/default/public\n");
	printf("privilege : http://tizen.org/privilege/mediacapture\n");
	printf("api_version : 2.4\n");
	printf("package type : wrt\n");
	privilege_list = g_list_append(privilege_list, "http://tizen.org/privilege/internal/default/public");
	privilege_list = g_list_append(privilege_list, "http://tizen.org/privilege/mediacapture");
	ret = privilege_db_manager_get_mapped_privilege_list("2.4", PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_mapped_privilege_list(ret, mapped_privilege_list);

	g_list_free(privilege_list);
	privilege_list = NULL;
	g_list_free(mapped_privilege_list);
	mapped_privilege_list = NULL;

	__print_line();
	printf("privilege : http://tizen.org/privilege/internal/default/public\n");
	printf("api_version : 2.2.1\n");
	printf("package type : wrt\n");
	privilege_list = g_list_append(privilege_list, "http://tizen.org/privilege/internal/default/public");
	ret = privilege_db_manager_get_mapped_privilege_list("2.2.1", PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_mapped_privilege_list(ret, mapped_privilege_list);

	g_list_free(privilege_list);
	privilege_list = NULL;
	g_list_free(mapped_privilege_list);
	mapped_privilege_list = NULL;

	__print_line();
	printf("privilege : http://tizen.org/privilege/mediacapture\n");
	printf("api_version : 2.2.1\n");
	printf("package type : wrt\n");
	privilege_list = g_list_append(privilege_list, "http://tizen.org/privilege/mediacapture");
	ret = privilege_db_manager_get_mapped_privilege_list("2.2.1", PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_mapped_privilege_list(ret, mapped_privilege_list);

	g_list_free(privilege_list);
	privilege_list = NULL;
	g_list_free(mapped_privilege_list);
	mapped_privilege_list = NULL;

	__print_line();
	printf("privilege : http://tizen.org/privilege/content.read\n");
	printf("api_version : 2.2.1\n");
	printf("package type : wrt\n");
	privilege_list = g_list_append(privilege_list, "http://tizen.org/privilege/content.read");
	ret = privilege_db_manager_get_mapped_privilege_list("2.2.1", PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_mapped_privilege_list(ret, mapped_privilege_list);

	g_list_free(privilege_list);
	privilege_list = NULL;
	g_list_free(mapped_privilege_list);
	mapped_privilege_list = NULL;

	__print_line();
	printf("privilege : http://tizen.org/privilege/internal/webappdefault\n");
	printf("privilege : http://tizen.org/privilege/internal/default/public\n");
	printf("privilege : http://tizen.org/privilege/mediacapture\n");
	printf("privilege : http://tizen.org/privilege/content.read\n");
	printf("api_version : 2.2.1\n");
	printf("package type : wrt\n");
	privilege_list = g_list_append(privilege_list, "http://tizen.org/privilege/internal/webappdefault");
	privilege_list = g_list_append(privilege_list, "http://tizen.org/privilege/internal/default/public");
	privilege_list = g_list_append(privilege_list, "http://tizen.org/privilege/mediacapture");
	privilege_list = g_list_append(privilege_list, "http://tizen.org/privilege/content.read");
	ret = privilege_db_manager_get_mapped_privilege_list("2.2.1", PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_mapped_privilege_list(ret, mapped_privilege_list);

	g_list_free(privilege_list);
	g_list_free(mapped_privilege_list);

	__color_to_green();
	printf("test success\n");
	__color_to_origin();
	success_cnt++;
}

void __check_get_privilege_display_result(privilege_db_manager_error_e expected_result, privilege_db_manager_error_e result, char *privilege_display)
{
	printf("expected result = %s, result = %s\n", __get_result_string('d', expected_result), __get_result_string('d', result));

	if (expected_result != result) {
		printf("not matched\n");
		__color_to_red();
		printf("test fail\n");
		fail_cnt++;
	} else {
		printf("matched\n");

		if (privilege_display == NULL)
			printf("privilege_display = NULL\n");
		else
			printf("privilege_display = %s\n", privilege_display);

		__color_to_green();
		printf("test success\n");
		success_cnt++;
	}
	__color_to_origin();
}

void __test_privilege_db_manager_get_privilege_display()
{
	int ret;

	char *privilege_display = NULL;
#ifdef PROFILE_TYPE_MOBILE
	__print_line();
	printf("privilege : http://tizen.org/privilege/location\n");
	printf("privilege_type : core\n");
	printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
	ret = privilege_db_manager_get_privilege_display(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/location", "2.3", &privilege_display);
	__check_get_privilege_display_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_display);
	free(privilege_display);
#endif
	__print_line();
	printf("privilege : http://tizen.org/privilege/internet\n");
	printf("privilege_type : wrt\n");
	printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
	privilege_display = NULL;
	ret = privilege_db_manager_get_privilege_display(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, "http://tizen.org/privilege/internet", "2.3", &privilege_display);
	__check_get_privilege_display_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_display);
	free(privilege_display);

	__print_line();
	printf("privilege : http://tizen.org/privilege/messasdfsfsdfsdfad\n");
	printf("expected result : PRIVILEGE_DB_NO_EXIST_RESULT\n");
	privilege_display = NULL;
	ret = privilege_db_manager_get_privilege_display(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/messasdfsfsdfsdfad", "2.3", &privilege_display);
	__check_get_privilege_display_result(PRIVILEGE_DB_NO_EXIST_RESULT, ret, privilege_display);
	free(privilege_display);

	__print_line();
	printf("api_version is NULL\n");
	printf("privilege : http://tizen.org/privilege/internet\n");
	printf("privilege_type : wrt\n");
	printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
	privilege_display = NULL;
	ret = privilege_db_manager_get_privilege_display(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, "http://tizen.org/privilege/internet", NULL, &privilege_display);
	__check_get_privilege_display_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_display);
	free(privilege_display);

	__print_line();
	printf("api_version is NULL\n");
	printf("privilege : http://tizen.org/privilege/messasdfsfsdfsdfad\n");
	printf("expected result : PRIVILEGE_DB_NO_EXIST_RESULT\n");
	privilege_display = NULL;
	ret = privilege_db_manager_get_privilege_display(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/messasdfsfsdfsdfad", NULL, &privilege_display);
	__check_get_privilege_display_result(PRIVILEGE_DB_NO_EXIST_RESULT, ret, privilege_display);
	free(privilege_display);

	__print_line();

}

void __check_get_privilege_description_result(privilege_db_manager_error_e expected_result, privilege_db_manager_error_e result, char *privilege_description)
{
	printf("expected result = %s, result = %s\n", __get_result_string('d', expected_result), __get_result_string('d', result));

	if (expected_result != result) {
		printf("not matched\n");
		__color_to_red();
		printf("test fail\n");
		fail_cnt++;
	} else {
		printf("matched\n");

		if (privilege_description == NULL)
			printf("privilege_description = NULL\n");
		else
			printf("privilege_description = %s\n", privilege_description);

		__color_to_green();
		printf("test success\n");
		success_cnt++;
	}
	__color_to_origin();
}

void __test_privilege_db_manager_get_privilege_description()
{
	int ret;
	char *privilege_description = NULL;
#ifdef PROFILE_TYPE_MOBILE
	__print_line();
	printf("privilege : http://tizen.org/privilege/location\n");
	printf("privilege_type : core\n");
	printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
	ret = privilege_db_manager_get_privilege_description(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/location", "2.3", &privilege_description);
	__check_get_privilege_description_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_description);

	if (privilege_description != NULL) {
		free(privilege_description);
		privilege_description = NULL;
	}
#endif
	__print_line();
	printf("privilege : http://tizen.org/privilege/internet\n");
	printf("privilege_type : wrt\n");
	printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
	ret = privilege_db_manager_get_privilege_description(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, "http://tizen.org/privilege/internet", "2.3", &privilege_description);
	__check_get_privilege_description_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_description);

	if (privilege_description != NULL) {
		free(privilege_description);
		privilege_description = NULL;
	}

	__print_line();
	printf("privilege : http://tizen.org/privilege/messasdfsfsdfsdfad\n");
	printf("expected result : PRIVILEGE_DB_NO_EXIST_RESULT\n");
	ret = privilege_db_manager_get_privilege_description(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/messasdfsfsdfsdfad", "2.3", &privilege_description);
	__check_get_privilege_description_result(PRIVILEGE_DB_NO_EXIST_RESULT, ret, privilege_description);

	if (privilege_description != NULL) {
		free(privilege_description);
		privilege_description = NULL;
	}

	__print_line();
	printf("api_version is NULL\n");
	printf("privilege : http://tizen.org/privilege/internet\n");
	printf("privilege_type : wrt\n");
	printf("expected result : PRIVILEGE_DB_MANAGER_ERR_NONE\n");
	ret = privilege_db_manager_get_privilege_description(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_WRT, "http://tizen.org/privilege/internet", NULL, &privilege_description);
	__check_get_privilege_description_result(PRIVILEGE_DB_MANAGER_ERR_NONE, ret, privilege_description);

	if (privilege_description != NULL) {
		free(privilege_description);
		privilege_description = NULL;
	}

	__print_line();
	printf("api_version is NULL\n");
	printf("privilege : http://tizen.org/privilege/messasdfsfsdfsdfad\n");
	printf("expected result : PRIVILEGE_DB_NO_EXIST_RESULT\n");
	ret = privilege_db_manager_get_privilege_description(PRIVILEGE_DB_MANAGER_PACKAGE_TYPE_CORE, "http://tizen.org/privilege/messasdfsfsdfsdfad", NULL, &privilege_description);
	__check_get_privilege_description_result(PRIVILEGE_DB_NO_EXIST_RESULT, ret, privilege_description);
	__print_line();

	free(privilege_description);

}

int main()
{
	__tcinfo(function, "privilege_db_manager_get_privilege_list");
	__test_privilege_db_manager_get_privilege_list();

	__tcinfo(function, "privilege_db_manager_get_mapped_privilege_list");
	__test_privilege_db_manager_get_mapped_privilege_list();

	__tcinfo(function, "privilege_db_manager_get_privilege_display");
	__test_privilege_db_manager_get_privilege_display();

	__tcinfo(function, "privilege_db_manager_get_privilege_description");
	__test_privilege_db_manager_get_privilege_description();

	__color_to_green();
	printf("Test Complete\n");
	printf("success : %d, ", success_cnt);
	__color_to_red();
	printf("fail : %d\n", fail_cnt);
	__color_to_origin();

	return 0;
}
