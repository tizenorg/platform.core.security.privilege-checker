#include <stdio.h>
#include <privilege_info.h>

#define BRIGHTNESS 0
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BG_BLACK 40

static int fail_cnt = 0;
static int success_cnt = 0;

static void __change_color_to_red()
{
	printf("%c[%d;%dm", 0x1B, BRIGHTNESS, RED);
}

static void __change_color_to_green()
{
	printf("%c[%d;%dm", 0x1B, BRIGHTNESS, GREEN);
}

static void __change_color_to_yellow()
{
	printf("%c[%d;%dm", 0x1B, BRIGHTNESS, YELLOW);
}

static void __change_color_to_origin()
{
	printf("%c[%dm", 0x1B, 0);
}

static const char *__get_result_string(privilege_manager_error_e ret)
{
	if (ret == PRVMGR_ERR_NONE)
		return "PRVMGR_ERR_NONE";
	else if (ret == PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL)
		return "PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL";
	else if (ret == PRVMGR_ERR_NO_EXIST_PRIVILEGE)
		return "PRVMGR_ERR_NO_EXIST_PRIVILEGE";
	else if (ret == PRVMGR_ERR_INVALID_PARAMETER)
		return "PRVMGR_ERR_INVALID_PARAMETER";
	else if (ret == PRVMGR_ERR_INTERNAL_ERROR)
		return "PRVMGR_ERR_INTERNAL_ERROR";

	return "FAIL";
}

static void __check_get_privilege_display_name_result(privilege_manager_error_e expected_result, privilege_manager_error_e result, const char *display_name)
{
	printf("expected result = %s, result = %s\n", __get_result_string(expected_result), __get_result_string(result));

	if (expected_result != result) {
		printf("not matched\n");
		__change_color_to_red();
		printf("test fail\n");
		fail_cnt++;
	} else {
		printf("matched\n");
		if (result == PRVMGR_ERR_NONE) {
			printf("display_name = %s\n", display_name);
		} else {
			if (display_name != NULL) {
				printf("display_name = %s\n", display_name);
				printf("display_name must be NULL\n");
				__change_color_to_red();
				printf("test fail\n");
				fail_cnt++;
				__change_color_to_origin();
				return;
			}
		}
		__change_color_to_green();
		printf("test success\n");
		success_cnt++;
	}
	__change_color_to_origin();
}

static void __check_get_privilege_description_result(privilege_manager_error_e expected_result, privilege_manager_error_e result, const char *description)
{
	printf("expected result = %s, result = %s\n", __get_result_string(expected_result), __get_result_string(result));

	if (expected_result != result) {
		printf("not matched\n");
		__change_color_to_red();
		printf("test fail\n");
		fail_cnt++;
	} else {
		printf("matched\n");
		if (result == PRVMGR_ERR_NONE) {
			printf("description = %s\n", description);
		} else {
			if (description != NULL) {
				printf("description = %s\n", description);
				printf("description must be NULL\n");
				__change_color_to_red();
				printf("test fail\n");
				fail_cnt++;
				__change_color_to_origin();
				return;
			}
		}

		__change_color_to_green();
		printf("test success\n");
		success_cnt++;
	}
	__change_color_to_origin();
}

static void __check_get_privilege_group_display_name_result(privilege_manager_error_e expected_result, privilege_manager_error_e result, const char *group_name)
{
	printf("expected result = %s, result = %s\n", __get_result_string(expected_result), __get_result_string(result));

	if (expected_result != result) {
		printf("not matched\n");
		__change_color_to_red();
		printf("test fail\n");
		fail_cnt++;
	} else {
		printf("matched\n");

		if (result == PRVMGR_ERR_NONE) {
			if (group_name == NULL) {
				printf("group_name must not be NULL\n");
				__change_color_to_red();
				printf("test fail\n");
				fail_cnt++;
				__change_color_to_origin();
				return;
			} else {
				printf("group_name = %s\n", group_name);
			}
		} else {
			if (group_name != NULL) {
				printf("group_name = %s\n", group_name);
				printf("group_name must be NULL\n");
				__change_color_to_red();
				printf("test fail\n");
				fail_cnt++;
				__change_color_to_origin();
				return;
			}
		}
		__change_color_to_green();
		printf("test success\n");
		success_cnt++;
	}
	__change_color_to_origin();
}
static void __test_privilege_info_is_privacy()
{
	int ret;
	printf("-----------------------------------------------------------\n");
	printf("privilege : http://tizen.org/privilege/account.read\n");
	ret = privilege_info_is_privacy("http://tizen.org/privilege/account.read");
	if (ret == 1)
		printf("SUCCESS: http://tizen.org/privilege/account.read is PRIVACY\n");
	else if (ret == 0)
		printf("Test FAILED\n");

	printf("-----------------------------------------------------------\n");
	printf("privilege : http://tizen.org/privilege/internet\n");
	ret = privilege_info_is_privacy("http://tizen.org/privilege/internet");
	if (ret == 0)
		printf("SUCCESS: http://tizen.org/privilege/internet is NOT PRIVACY\n");
	else if (ret == 1)
		printf("Test FAILED\n");
}

static void __test_privilege_info_get_privacy_list()
{
	GList* privacy_list = NULL;
	GList* l;
	int ret = privilege_info_get_privacy_list(&privacy_list);
	if (ret == 0) {
		for (l = privacy_list; l != NULL; l = l->next) {
			char *privacy_name = (char*)l->data;
			printf("%s\n", privacy_name);
		}
	} else {
		printf("Test FAILED\n");
	}
	g_list_free(privacy_list);
}

static void __test_privilege_info_get_privilege_list_by_privacy()
{
	GList* privilege_list = NULL;
	GList* l;
	printf("-----------------------------------------------------------\n");
	printf("privacy : ACCOUNT\n");
	int ret = privilege_info_get_privilege_list_by_privacy("ACCOUNT", &privilege_list);
	if (ret == 0) {
		for (l = privilege_list; l != NULL; l = l->next) {
			char *privilege_name = (char*)l->data;
			printf("%s\n", privilege_name);
		}
	} else {
		printf("Test FAILED\n");
	}

	if (privilege_list != NULL) {
		g_list_free(privilege_list);
		privilege_list = NULL;
	}

	printf("-----------------------------------------------------------\n");
	printf("privacy : USERHISTORY\n");
	ret = privilege_info_get_privilege_list_by_privacy("USERHISTORY", &privilege_list);
	if (ret == 0) {
		for (l = privilege_list; l != NULL; l = l->next) {
			char *privilege_name = (char*)l->data;
			printf("%s\n", privilege_name);
		}
	} else {
		printf("Test FAILED\n");
	}

	g_list_free(privilege_list);
}

static void __test_privilege_info_get_privilege_display_name()
{
	int ret;
	char *display_name = NULL;

	printf("-----------------------------------------------------------\n");
	printf("privilege : http://tizen.org/privilege/account.read\n");
	printf("expected result : PRVMGR_ERR_NONE\n");
	display_name = NULL;
	ret = privilege_info_get_privilege_display_name("http://tizen.org/privilege/account.read", &display_name);
	__check_get_privilege_display_name_result(PRVMGR_ERR_NONE, ret, display_name);
	free(display_name);

	printf("-----------------------------------------------------------\n");
	printf("privilege : http://tizen.org/privilege/badge.admin\n");
	printf("expected result : PRVMGR_ERR_NONE\n");
	display_name = NULL;
	ret = privilege_info_get_privilege_display_name("http://tizen.org/privilege/badge.admin", &display_name);
	__check_get_privilege_display_name_result(PRVMGR_ERR_NONE, ret, display_name);
	free(display_name);

	printf("-----------------------------------------------------------\n");
	printf("privilege : http://tizen.org/privilege/accousdfsdnt.resdfsdfad\n");
	printf("expected result : PRVMGR_ERR_NONE\n");
	display_name = NULL;
	ret = privilege_info_get_privilege_display_name("http://tizen.org/privilege/accousdfsdnt.resdfsdfad", &display_name);
	__check_get_privilege_display_name_result(PRVMGR_ERR_NONE, ret, display_name);
	free(display_name);

	printf("-----------------------------------------------------------\n");
	printf("privilege : NULL\n");
	printf("expected result : PRVMGR_ERR_INVALID_PARAMETER\n");
	display_name = NULL;
	ret = privilege_info_get_privilege_display_name(NULL, &display_name);
	__check_get_privilege_display_name_result(PRVMGR_ERR_INVALID_PARAMETER, ret, display_name);
	printf("-----------------------------------------------------------\n");
	free(display_name);
}

static void __test_privilege_info_get_privilege_description()
{
	int ret;
	char *description = NULL;

	printf("-----------------------------------------------------------\n");
	printf("privilege : http://tizen.org/privilege/account.read\n");
	printf("expected result : PRVMGR_ERR_NONE\n");
	ret = privilege_info_get_privilege_description("http://tizen.org/privilege/account.read", &description);
	__check_get_privilege_description_result(PRVMGR_ERR_NONE, ret, description);

	printf("-----------------------------------------------------------\n");
	printf("privilege : http://tizen.org/privilege/badge.admin\n");
	printf("expected result : PRVMGR_ERR_NONE\n");
	if (description != NULL) {
		free(description);
		description = NULL;
	}
	ret = privilege_info_get_privilege_description("http://tizen.org/privilege/badge.admin", &description);
	__check_get_privilege_description_result(PRVMGR_ERR_NONE, ret, description);

	printf("-----------------------------------------------------------\n");
	printf("privilege : http://tizen.org/privilege/accousdfsdnt.resdfsdfad\n");
	printf("expected result : PRVMGR_ERR_NONE\n");
	if (description != NULL) {
		free(description);
		description = NULL;
	}
	ret = privilege_info_get_privilege_description("http://tizen.org/privilege/accousdfsdnt.resdfsdfad", &description);
	__check_get_privilege_description_result(PRVMGR_ERR_NONE, ret, description);

	printf("-----------------------------------------------------------\n");
	printf("privilege : NULL\n");
	printf("expected result : PRVMGR_ERR_INVALID_PARAMETER\n");
	if (description != NULL) {
		free(description);
		description = NULL;
	}
	ret = privilege_info_get_privilege_description(NULL, &description);
	__check_get_privilege_description_result(PRVMGR_ERR_INVALID_PARAMETER, ret, description);
	printf("-----------------------------------------------------------\n");
	free(description);
}

static void __test__privilege_info_get_privilege_group_display_name()
{
	int ret;
	char *group_name = NULL;

	printf("-----------------------------------------------------------\n");
	printf("privilege : http://tizen.org/privilege/group/group0\n");
	printf("expected result : PRVMGR_ERR_NONE\n");
	ret = privilege_info_get_privilege_group_display_name("http://tizen.org/privilege/group/group0", &group_name);
	__check_get_privilege_group_display_name_result(PRVMGR_ERR_NONE, ret, group_name);
	free(group_name);

	printf("-----------------------------------------------------------\n");
	printf("privilege : http://tizen.org/privilege/group/group01232\n");
	printf("expected result : PRVMGR_ERR_NONE\n");
	group_name = NULL;
	ret = privilege_info_get_privilege_group_display_name("http://tizen.org/privilege/group/group01232", &group_name);
	__check_get_privilege_group_display_name_result(PRVMGR_ERR_NONE, ret, group_name);
	free(group_name);

	printf("-----------------------------------------------------------\n");
	printf("privilege : NULL\n");
	printf("expected result : PRVMGR_ERR_INVALID_PARAMETER\n");
	group_name = NULL;
	ret = privilege_info_get_privilege_group_display_name(NULL, &group_name);
	__check_get_privilege_group_display_name_result(PRVMGR_ERR_INVALID_PARAMETER, ret, group_name);
	printf("-----------------------------------------------------------\n");
	free(group_name);

}

int main()
{
	__change_color_to_yellow();
	printf("Test function : privilege_info_is_privacy()\n");
	__change_color_to_origin();
	__test_privilege_info_is_privacy();

	__change_color_to_yellow();
	printf("Test function : privilege_info_get_privilege_display_name\n");
	__change_color_to_origin();
	__test_privilege_info_get_privilege_display_name();

	__change_color_to_yellow();
	printf("Test function : privilege_info_get_privilege_description\n");
	__change_color_to_origin();
	__test_privilege_info_get_privilege_description();

	__change_color_to_yellow();
	printf("Test function : privilege_info_get_privilege_group_display_name\n");
	__change_color_to_origin();
	__test__privilege_info_get_privilege_group_display_name();

	__change_color_to_yellow();
	printf("Test function : privilege_info_get_privacy_list\n");
	__change_color_to_origin();
	__test_privilege_info_get_privacy_list();

	__change_color_to_yellow();
	printf("Test function : privilege_info_get_privilege_list_by_privacy\n");
	__change_color_to_origin();
	__test_privilege_info_get_privilege_list_by_privacy();

	__change_color_to_green();
	printf("Test Complete\n");
	printf("success : %d, ", success_cnt);

	__change_color_to_red();
	printf("fail : %d\n", fail_cnt);
	__change_color_to_origin();
}
