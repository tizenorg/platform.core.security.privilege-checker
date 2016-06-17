#include <stdio.h>
#include <stdlib.h>
#include <privilege_info.h>
#include <tc-common.h>

static void __check_get_privilege_display_name_result(privilege_manager_error_e expected_result, privilege_manager_error_e result, const char *display_name)
{
	printf("expected result = %s, result = %s\n", __get_result_string('m', expected_result), __get_result_string('m', result));

	if (expected_result != result) {
		printf("not matched\n");
		__color_to_red();
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
				__color_to_red();
				printf("test fail\n");
				fail_cnt++;
				__color_to_origin();
				return;
			}
		}
		__color_to_green();
		printf("test success\n");
		success_cnt++;
	}
	__color_to_origin();
}

static void __check_get_privilege_description_result(privilege_manager_error_e expected_result, privilege_manager_error_e result, const char *description)
{
	printf("expected result = %s, result = %s\n", __get_result_string('m', expected_result), __get_result_string('m', result));

	if (expected_result != result) {
		printf("not matched\n");
		__color_to_red();
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
				__color_to_red();
				printf("test fail\n");
				fail_cnt++;
				__color_to_origin();
				return;
			}
		}

		__color_to_green();
		printf("test success\n");
		success_cnt++;
	}
	__color_to_origin();
}

static void __check_get_privilege_group_display_name_result(privilege_manager_error_e expected_result, privilege_manager_error_e result, const char *group_name)
{
	printf("expected result = %s, result = %s\n", __get_result_string('m', expected_result), __get_result_string('m', result));

	if (expected_result != result) {
		printf("not matched\n");
		__color_to_red();
		printf("test fail\n");
		fail_cnt++;
	} else {
		printf("matched\n");

		if (result == PRVMGR_ERR_NONE) {
			if (group_name == NULL) {
				printf("group_name must not be NULL\n");
				__color_to_red();
				printf("test fail\n");
				fail_cnt++;
				__color_to_origin();
				return;
			} else {
				printf("group_name = %s\n", group_name);
			}
		} else {
			if (group_name != NULL) {
				printf("group_name = %s\n", group_name);
				printf("group_name must be NULL\n");
				__color_to_red();
				printf("test fail\n");
				fail_cnt++;
				__color_to_origin();
				return;
			}
		}
		__color_to_green();
		printf("test success\n");
		success_cnt++;
	}
	__color_to_origin();
}
static void __test_privilege_info_is_valid()
{
	int ret;
	printf("privilege : http://tizen.org/privilege/account.read\n");
	ret = privilege_info_is_valid("http://tizen.org/privilege/account.read");
	if (ret == 1) {
		success_cnt++;
		printf("SUCCESS: http://tizen.org/privilege/account.read is valid\n");
	} else {
		fail_cnt++;
		printf("Test FAILED. ret =  %d\n", ret);
	}
	__print_line();

	printf("privilege : http://tizen.org/privilege/internal/default/public\n");
    ret = privilege_info_is_valid("http://tizen.org/privilege/internal/default/public");
    if (ret == 1) {
        success_cnt++;
        printf("SUCCESS: http://tizen.org/privilege/internal/default/public is valid\n");
    } else {
        fail_cnt++;
        printf("Test FAILED. ret =  %d\n", ret);
    }
    __print_line();

	printf("privilege : http://tizen.org/privilege/internettttttt\n");
	ret = privilege_info_is_valid("http://tizen.org/privilege/internettttttt");
	if (ret == 0) {
		success_cnt++;
		printf("SUCCESS: http://tizen.org/privilege/internettttttt is invalid\n");
	} else {
		fail_cnt++;
		printf("Test FAILED. ret = %d\n", ret);
	}
	__print_line();

}

static void __test_privilege_info_is_internal()
{
    int ret;
    printf("privilege : http://tizen.org/privilege/internal/default/public\n");
    ret = privilege_info_is_internal("http://tizen.org/privilege/internal/default/public");
    if (ret == 1) {
        success_cnt++;
        printf("SUCCESS: http://tizen.org/privilege/internal/default/public is internal\n");
    } else {
        fail_cnt++;
        printf("Test FAILED. ret =  %d\n", ret);
    }
    __print_line();

    printf("privilege : http://tizen.org/privilege/internal/dbus\n");
    ret = privilege_info_is_internal("http://tizen.org/privilege/internal/dbus");
    if (ret == 1) {
        success_cnt++;
        printf("SUCCESS: http://tizen.org/privilege/internal/dbus is internal\n");
    } else {
        fail_cnt++;
        printf("Test FAILED. ret = %d\n", ret);
    }
    __print_line();

	printf("privilege : http://tizen.org/privilege/internet\n");
    ret = privilege_info_is_internal("http://tizen.org/privilege/internet");
    if (ret == 0) {
        success_cnt++;
        printf("SUCCESS: http://tizen.org/privilege/internet is NOT internal\n");
    } else {
        fail_cnt++;
        printf("Test FAILED. ret = %d\n", ret);
    }
    __print_line();
    printf("privilege : http://tizen.org/privilege/internettttt\n");
    ret = privilege_info_is_internal("http://tizen.org/privilege/internettttt");
    if (ret == 0) {
        success_cnt++;
        printf("SUCCESS: http://tizen.org/privilege/internettttt is NOT internal\n");
    } else {
        fail_cnt++;
        printf("Test FAILED. ret = %d\n", ret);
    }
    __print_line();
}
static void __test_privilege_info_is_privacy()
{
    int ret;
    printf("privilege : http://tizen.org/privilege/account.read\n");
    ret = privilege_info_is_privacy("http://tizen.org/privilege/account.read");
    if (ret == 1) {
        success_cnt++;
        printf("SUCCESS: http://tizen.org/privilege/account.read is PRIVACY\n");
    } else {
        fail_cnt++;
        printf("Test FAILED. ret =  %d\n", ret);
    }
    __print_line();

    printf("privilege : http://tizen.org/privilege/internet\n");
    ret = privilege_info_is_privacy("http://tizen.org/privilege/internet");
    if (ret == 0) {
        success_cnt++;
        printf("SUCCESS: http://tizen.org/privilege/internet is NOT PRIVACY\n");
    } else {
        fail_cnt++;
        printf("Test FAILED. ret = %d\n", ret);
    }
    __print_line();

    printf("privilege : http://tizen.org/privilege/internettttt\n");
    ret = privilege_info_is_privacy("http://tizen.org/privilege/internettttt");
    if (ret == 0) {
        success_cnt++;
        printf("SUCCESS: http://tizen.org/privilege/internettttt is NOT PRIVACY\n");
    } else {
        fail_cnt++;
        printf("Test FAILED. ret = %d\n", ret);
    }
    __print_line();
}

static void __test_privilege_info_get_privacy_list()
{
	GList* privacy_list = NULL;
	GList* l;
	int ret = privilege_info_get_privacy_list(&privacy_list);
	if (ret == 0) {
		success_cnt++;
		for (l = privacy_list; l != NULL; l = l->next) {
			char *privacy_name = (char*)l->data;
			printf("%s\n", privacy_name);
		}
	} else {
		fail_cnt++;
		printf("Test FAILED\n");
	}
	gfree(privacy_list);
	__print_line();
}

static void __test_privilege_info_get_privilege_list_by_privacy()
{
	GList* privilege_list = NULL;
	GList* l;
	printf("privacy : ACCOUNT\n");
	int ret = privilege_info_get_privilege_list_by_privacy("ACCOUNT", &privilege_list);
	if (ret == 0) {
		success_cnt++;
		for (l = privilege_list; l != NULL; l = l->next) {
			char *privilege_name = (char*)l->data;
			printf("%s\n", privilege_name);
		}
	} else {
		fail_cnt++;
		printf("Test FAILED\n");
	}

	if (privilege_list != NULL)
		gfree(privilege_list);

	__print_line();

	printf("privacy : USERHISTORY\n");
	ret = privilege_info_get_privilege_list_by_privacy("USERHISTORY", &privilege_list);
	if (ret == 0) {
		success_cnt++;
		for (l = privilege_list; l != NULL; l = l->next) {
			char *privilege_name = (char*)l->data;
			printf("%s\n", privilege_name);
		}
	} else {
		fail_cnt++;
		printf("Test FAILED\n");
	}

	gfree(privilege_list);

	__print_line();
}

static void __test_privilege_info_get_privilege_display_name()
{
	int ret;
	char *display_name = NULL;

	printf("privilege : http://tizen.org/privilege/account.read\n");
	display_name = NULL;
	ret = privilege_info_get_privilege_display_name("http://tizen.org/privilege/account.read", &display_name);
	__check_get_privilege_display_name_result(PRVMGR_ERR_NONE, ret, display_name);
	free(display_name);
	__print_line();

	printf("privilege : http://tizen.org/privilege/badge.admin\n");
	display_name = NULL;
	ret = privilege_info_get_privilege_display_name("http://tizen.org/privilege/badge.admin", &display_name);
	__check_get_privilege_display_name_result(PRVMGR_ERR_NONE, ret, display_name);
	free(display_name);
	__print_line();

	printf("privilege : http://tizen.org/privilege/accousdfsdnt.resdfsdfad\n");
	display_name = NULL;
	ret = privilege_info_get_privilege_display_name("http://tizen.org/privilege/accousdfsdnt.resdfsdfad", &display_name);
	__check_get_privilege_display_name_result(PRVMGR_ERR_NONE, ret, display_name);
	free(display_name);
	__print_line();

	printf("privilege : NULL\n");
	display_name = NULL;
	ret = privilege_info_get_privilege_display_name(NULL, &display_name);
	__check_get_privilege_display_name_result(PRVMGR_ERR_INVALID_PARAMETER, ret, display_name);
	free(display_name);
	__print_line();
}

static void __test_privilege_info_get_privilege_description()
{
	int ret;
	char *description = NULL;

	printf("privilege : http://tizen.org/privilege/account.read\n");
	ret = privilege_info_get_privilege_description("http://tizen.org/privilege/account.read", &description);
	__check_get_privilege_description_result(PRVMGR_ERR_NONE, ret, description);
	__print_line();

	printf("privilege : http://tizen.org/privilege/badge.admin\n");
	if (description != NULL) {
		free(description);
		description = NULL;
	}
	ret = privilege_info_get_privilege_description("http://tizen.org/privilege/badge.admin", &description);
	__check_get_privilege_description_result(PRVMGR_ERR_NONE, ret, description);
	__print_line();

	printf("privilege : http://tizen.org/privilege/accousdfsdnt.resdfsdfad\n");
	if (description != NULL) {
		free(description);
		description = NULL;
	}
	ret = privilege_info_get_privilege_description("http://tizen.org/privilege/accousdfsdnt.resdfsdfad", &description);
	__check_get_privilege_description_result(PRVMGR_ERR_NONE, ret, description);
	__print_line();

	printf("privilege : NULL\n");
	if (description != NULL) {
		free(description);
		description = NULL;
	}
	ret = privilege_info_get_privilege_description(NULL, &description);
	__check_get_privilege_description_result(PRVMGR_ERR_INVALID_PARAMETER, ret, description);
	free(description);
	__print_line();
}

static void __test__privilege_info_get_privilege_group_display_name()
{
	int ret;
	char *group_name = NULL;

	printf("privilege : http://tizen.org/privilege/group/group0\n");
	ret = privilege_info_get_privilege_group_display_name("http://tizen.org/privilege/group/group0", &group_name);
	__check_get_privilege_group_display_name_result(PRVMGR_ERR_NONE, ret, group_name);
	free(group_name);

	printf("privilege : http://tizen.org/privilege/group/group01232\n");
	group_name = NULL;
	ret = privilege_info_get_privilege_group_display_name("http://tizen.org/privilege/group/group01232", &group_name);
	__check_get_privilege_group_display_name_result(PRVMGR_ERR_NONE, ret, group_name);
	free(group_name);

	printf("privilege : NULL\n");
	group_name = NULL;
	ret = privilege_info_get_privilege_group_display_name(NULL, &group_name);
	__check_get_privilege_group_display_name_result(PRVMGR_ERR_INVALID_PARAMETER, ret, group_name);
	free(group_name);

}

int main()
{
	__tcinfo(function, "privilege_info_is_privacy");
	__test_privilege_info_is_privacy();

	__tcinfo(function, "privilege_info_is_internal");
    __test_privilege_info_is_internal();

	__tcinfo(function, "privilege_info_is_valid");
    __test_privilege_info_is_valid();

	__tcinfo(function, "privilege_info_get_privilege_display_name");
	__test_privilege_info_get_privilege_display_name();

	__tcinfo(function, "privilege_info_get_privilege_description");
	__test_privilege_info_get_privilege_description();

	__tcinfo(function, "privilege_info_get_privilege_group_display_name");
	__test__privilege_info_get_privilege_group_display_name();

	__tcinfo(function, "privilege_info_get_privacy_list");
	__test_privilege_info_get_privacy_list();

	__tcinfo(function, "privilege_info_get_privilege_list_by_privacy");
	__test_privilege_info_get_privilege_list_by_privacy();

	__color_to_green();
	printf("Test Complete\n");
	printf("success : %d, ", success_cnt);

	__color_to_red();
	printf("fail : %d\n", fail_cnt);
	__color_to_origin();

	return 0;
}
