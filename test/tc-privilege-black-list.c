#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <privilege_manager.h>
#include <privilege_info.h>
#include <privilege_db_manager.h>
#include "tc-common.h"

static void __test_black_list_set_unset()
{
	int ret = 0;
	GList *black_list;

	__color_to_bold_yellow();
	printf("\nTest black list set/unset/get\n\n");
	__color_to_origin();

	__tcinfo(goal, "uid = 0, Get wrt black list");
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__print_result('m', ret);
	__print_privilege_list(black_list);
	__print_line();
	gfree(black_list);

	__tcinfo(goal, "uid = 0, Add wrt privileges");
	__privinfo("http://tizen.org/privilege/filesystem.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/internet", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_set_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	__print_result('m', ret);
	__print_line();
	gfree(privilege_list);

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result('m', ret);
	__print_privilege_list(black_list);
	__print_line();
	gfree(black_list);
	black_list = NULL;

	__tcinfo(goal, "uid = 0, Add wrt privileges -- Add duplicated privileges");
	__privinfo("http://tizen.org/privilege/filesystem.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/internet", NULL, NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_set_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	__print_result('m', ret);
	__print_line();
	gfree(privilege_list);

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result('m', ret);
	__print_privilege_list(black_list);
	__print_line();
	gfree(black_list);
	black_list = NULL;

	__tcinfo(goal, "uid = 0, Remove wrt privileges");
	__privinfo("http://tizen.org/privilege/filesystem.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_unset_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	__print_result('m', ret);
	__print_line();
	gfree(privilege_list);

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result('m', ret);
	__print_privilege_list(black_list);
	__print_line();
	gfree(black_list);
	black_list = NULL;

	__tcinfo(goal, "uid = 0, Remove wrt privileges -- Try to remove already removed privileges");
	__privinfo("http://tizen.org/privilege/filesystem.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_unset_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	__print_result('m', ret);
	__print_line();
	gfree(privilege_list);

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result('m', ret);
	__print_privilege_list(black_list);
	__print_line();
	gfree(black_list);
	black_list = NULL;

}


static void __test_black_list_check()
{
	GList *black_list = NULL;
	int ret = 0;

	__color_to_bold_yellow();
	printf("\nTest privilege list with banned privilege\n\n");
	__color_to_origin();

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result('m', ret);
	__print_privilege_list(black_list);
	__print_line();
	gfree(black_list);

	__tcinfo(goal, "uid = 0, contains banned privilege");
	__privinfo("http://tizen.org/privilege/mediacapture", NULL, NULL);
	__privinfo("http://tizen.org/privilege/internet", NULL, NULL);
	__privinfo("http://tizen.org/privilege/telephony", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_USING_BANNED_PRIVILEGE);
	ret = privilege_db_manager_check_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	if (ret > 0)
		ret = PRVMGR_ERR_USING_BANNED_PRIVILEGE;
	__print_result('m', ret);
	__print_line();
	gfree(privilege_list);

	__tcinfo(goal, "uid = 0, Not contains banned privilege");
	__privinfo("http://tizen.org/privilege/account.write", NULL, NULL);
	__privinfo("http://tizen.org/privilege/mediacontroller.client", NULL, NULL);
	__privinfo("http://tizen.org/privilege/telephony", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_db_manager_check_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	if (ret > 0)
		ret = PRVMGR_ERR_USING_BANNED_PRIVILEGE;
	__print_result('m', ret);
	__print_line();
	gfree(privilege_list);

	__tcinfo(goal, "uid = 1, Contains banned privilege but the UID is different");
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/internet", NULL, NULL);
	__privinfo("http://tizen.org/privilege/telephony", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_db_manager_check_black_list(1, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	if (ret > 0)
		ret = PRVMGR_ERR_USING_BANNED_PRIVILEGE;
	__print_result('m', ret);
	__print_line();
	gfree(privilege_list);

	__tcinfo(goal, "uid = 0, Contains banned privilege but the package type is different");
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/internet", NULL, NULL);
	__privinfo("http://tizen.org/privilege/telephony", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_db_manager_check_black_list(0, PRVMGR_PACKAGE_TYPE_CORE, privilege_list);
	if (ret > 0)
		ret = PRVMGR_ERR_USING_BANNED_PRIVILEGE;
	__print_result('m', ret);
	__print_line();
	gfree(privilege_list);
}

static void __test_black_list_verify_privilege()
{
	GList *black_list = NULL;
	char *error_message = NULL;
	int ret = 0;

	__color_to_bold_yellow();
	printf("\nTest privilege_manager_verify_privilege with banned privilege\n\n");
	__color_to_origin();

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result('m', ret);
	__print_privilege_list(black_list);
	__print_line();

	__tcinfo(goal, "2.4, wrt, Name=OK, Version=OK, Level=OK -- Public => Using Banned Privilege");
	__privinfo("http://tizen.org/privilege/alarm", NULL, NULL);
	__privinfo("http://tizen.org/privilege/application.info", NULL, NULL);
	__privinfo("http://tizen.org/privilege/content.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/download", NULL, NULL);
	__privinfo("http://tizen.org/privilege/filesystem.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/internet", NULL, NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", NULL, NULL);
	__privinfo("http://tizen.org/privilege/package.info", NULL, NULL);
	__privinfo("http://tizen.org/privilege/telephony", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_USING_BANNED_PRIVILEGE);
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__print_result('m', ret);
	__print_line();

	gfree(privilege_list);

	__tcinfo(goal, "uid = 0, Remove all banned wrt privileges");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_unset_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, black_list);
	__print_result('m', ret);
	__print_line();
	gfree(black_list);

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result('m', ret);
	__print_privilege_list(black_list);
	__print_line();

	__tcinfo(goal, "2.4, wrt, Name=OK, Version=OK, Level=OK -- Public ");
	__privinfo("http://tizen.org/privilege/alarm", NULL, NULL);
	__privinfo("http://tizen.org/privilege/application.info", NULL, NULL);
	__privinfo("http://tizen.org/privilege/content.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/download", NULL, NULL);
	__privinfo("http://tizen.org/privilege/filesystem.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/internet", NULL, NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", NULL, NULL);
	__privinfo("http://tizen.org/privilege/package.info", NULL, NULL);
	__privinfo("http://tizen.org/privilege/telephony", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__print_result('m', ret);
	__print_line();

	gfree(privilege_list);

}

int main()
{
	__print_dline();

	__tcinfo(function, "privilege_manager_set_black_list");
	__tcinfo(function, "privilege_manager_unset_black_list");
	__tcinfo(function, "privilege_info_get_black_list");
	__test_black_list_set_unset();

	__tcinfo(function, "privilege_db_manager_check_black_list");
	__tcinfo(function, "privilege_info_get_black_list");
	__test_black_list_check();

	__tcinfo(function, "privilege_manager_verify_privilege");
	__test_black_list_verify_privilege();

	__print_dline();

	__color_to_green();
	printf("Test Complete\n");
	printf("success : %d, ", success_cnt);
	__color_to_red();
	printf("fail : %d\n", fail_cnt);
	__color_to_origin();

	return 0;
}
