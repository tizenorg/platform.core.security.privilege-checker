#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <privilege_manager.h>
#include <privilege_info.h>
#include <privilege_db_manager.h>

#define BRIGHTNESS 0
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define WHITE 37
#define BG_BLACK 40
#define ShowErrorMsg(condition, error_message, result, ...) \
	__print_result(__get_result_string(result)); \
	if (condition) { \
		__print_error_message(error_message); \
		free(error_message); \
		error_message = NULL; \
	}

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

static void __change_color_to_blue()
{
	printf("%c[%d;%dm", 0x1B, BRIGHTNESS, BLUE);
}

static void __change_color_to_origin()
{
	printf("%c[%dm", 0x1B, 0);
}

static void __change_to_bold_red()
{
	printf("%c[%dm%c[%dm", 0x1B, 1, 0x1B, RED);
}

static void __change_to_bold_yellow()
{
	printf("%c[%dm%c[%dm", 0x1B, 1, 0x1B, YELLOW);
}

static int fail_cnt = 0;
static int success_cnt = 0;
GList *privilege_list = NULL;

static const char *__get_result_string(privilege_manager_error_e ret)
{
	if (ret == PRVMGR_ERR_NONE)
		return "PRVMGR_ERR_NONE";
	else if (ret == PRVMGR_ERR_INVALID_PRIVILEGE)
		return "PRVMGR_ERR_INVALID_PRIVILEGE";
	else if (ret == PRVMGR_ERR_INVALID_PARAMETER)
		return "PRVMGR_ERR_INVALID_PARAMETER";
	else if (ret == PRVMGR_ERR_INTERNAL_ERROR)
		return "PRVMGR_ERR_INTERNAL_ERROR";
	else if (ret == PRVMGR_ERR_USING_BANNED_PRIVILEGE)
		return "PRVMGR_ERR_USING_BANNED_PRIVILEGE";

	return "FAIL";
}

static void __print_error_message(char *error_message)
{
	char *token = NULL;
	char *temp = strdup(error_message);
	char *save = NULL;

	char err_type[256] = { 0, };

	if (strstr(error_message, "DEPRECATED") != NULL)
		strncat(err_type, "PRVMGR_ERR_DEPRECATED_PRIVILEGE ", strlen("PRVMGR_ERR_DEPRECATED_PRIVILEGE "));

	if (strstr(error_message, "NO_EXIST") != NULL)
		strncat(err_type, "PRVMGR_ERR_NO_EXIST_PRIVILEGE ", strlen("PRVMGR_ERR_NO_EXIST_PRIVILEGE "));

	if (strstr(error_message, "MISMATCHED") != NULL)
		strncat(err_type, "PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL ", strlen("PRVMGR_ERR_MISMACHED_PRIVILEGE_LEVEL "));

	if (strstr(error_message, "INVALID_PARAMETER") != NULL)
		strncat(err_type, "PRVMGR_ERR_INVALID_PARAMETER ", strlen("PRVMGR_ERR_INVALID_PARAMETER "));

	if (strstr(error_message, "BANNED") != NULL)
		strncat(err_type, "PRVMGR_ERR_USING_BANNED_PRIVILEGE ", strlen("PRVMGR_ERR_USING_BANNED_PRIVILEGE "));

	if (strlen(err_type) == 0) {
		fail_cnt++;
		strncat(err_type, "test failed - unknown error type", strlen("test failed - unknown error type "));
		__change_color_to_red();
	} else {
		__change_color_to_blue();
	}
	printf("Error Message : %s\n", err_type);
	__change_color_to_origin();

	token = strtok_r(temp, "|", &save);
	while (token) {
		printf("%s\n", token);
		token = strtok_r(NULL, "|", &save);
	}
	printf("\n");
	free(temp);
}

typedef enum {
	goal = 1,
	pkg_type = 4,
	expect = 5
} tcinfo_type_e;

static void __tcinfo(tcinfo_type_e type, char *input_string, ...)
{
	switch (type) {
	case goal:
		__change_color_to_yellow();
		printf("TEST >> %s\n\n", input_string);
		__change_color_to_origin();
		break;
	case pkg_type:
		printf("PACKAGE TYPE : %s\n", input_string);
		break;
	case expect:
		printf("- expected result : %s\n", input_string);
		break;
	default:
		printf("no matching enum for input\n");
	}
}

static void __privinfo(char *name)
{
	printf("%s", name);
	privilege_list = g_list_append(privilege_list, name);
	printf("\n");
}

static void __print_result(const char *input_string)
{
	printf("- achieved result : %s\n\n", input_string);
}

static void __print_line()
{
	printf("-------------------------------------------------------------------\n");
}

static void __print_dline()
{
	printf("===================================================================\n");
}

static void __print_privilege_list(GList * privilege_list)
{
	GList *l;
	printf("Print privilege list: \n");
	for (l = privilege_list; l != NULL; l = l->next) {
		char *privilege_name = (char *)l->data;
		printf("%s\n", privilege_name);
	}
}

static void __check_verify_result(privilege_manager_error_e expected_result, privilege_manager_error_e result)
{
	if (expected_result != result) {
		__change_to_bold_red();
		printf(">> test fail\n");
		fail_cnt++;
	} else {
		__change_color_to_green();
		printf(">> test success\n");
		success_cnt++;
	}
	__change_color_to_origin();
}


static void __test_black_list_set_unset()
{
	int ret = 0;
	GList *black_list;

	__change_to_bold_yellow();
	printf("\nTest black list set/unset/get\n\n");
	__change_color_to_origin();

	__tcinfo(goal, "uid = 0, Get wrt black list");
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(black_list);
	__print_line();
	g_list_free(black_list);
	black_list = NULL;

	__tcinfo(goal, "uid = 0, Add wrt privileges");
	__privinfo("http://tizen.org/privilege/filesystem.read");
	__privinfo("http://tizen.org/privilege/account.read");
	__privinfo("http://tizen.org/privilege/internet");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_set_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();
	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(black_list);
	__print_line();
	g_list_free(black_list);
	black_list = NULL;

	__tcinfo(goal, "uid = 0, Add wrt privileges -- Add duplicated privileges");
	__privinfo("http://tizen.org/privilege/filesystem.read");
	__privinfo("http://tizen.org/privilege/account.read");
	__privinfo("http://tizen.org/privilege/internet");
	__privinfo("http://tizen.org/privilege/mediacapture");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_set_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();
	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(black_list);
	__print_line();
	g_list_free(black_list);
	black_list = NULL;

	__tcinfo(goal, "uid = 0, Remove wrt privileges");
	__privinfo("http://tizen.org/privilege/filesystem.read");
	__privinfo("http://tizen.org/privilege/account.read");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_unset_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();
	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(black_list);
	__print_line();
	g_list_free(black_list);
	black_list = NULL;

	__tcinfo(goal, "uid = 0, Remove wrt privileges -- Try to remove already removed privileges");
	__privinfo("http://tizen.org/privilege/filesystem.read");
	__privinfo("http://tizen.org/privilege/account.read");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_unset_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();
	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(black_list);
	__print_line();
	g_list_free(black_list);
	black_list = NULL;

}


static void __test_black_list_check()
{
	GList *black_list = NULL;
	int ret = 0;

	__change_to_bold_yellow();
	printf("\nTest privilege list with banned privilege\n\n");
	__change_color_to_origin();

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(black_list);
	__print_line();
	g_list_free(black_list);
	black_list = NULL;

	__tcinfo(goal, "uid = 0, contains banned privilege");
	__privinfo("http://tizen.org/privilege/mediacapture");
	__privinfo("http://tizen.org/privilege/internet");
	__privinfo("http://tizen.org/privilege/telephony");
	__tcinfo(expect, "PRVMGR_ERR_USING_BANNED_PRIVILEGE");
	ret = privilege_db_manager_check_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	if (ret > 0)
		ret = PRVMGR_ERR_USING_BANNED_PRIVILEGE;
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_USING_BANNED_PRIVILEGE, ret);
	__print_line();
	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "uid = 0, Not contains banned privilege");
	__privinfo("http://tizen.org/privilege/account.write");
	__privinfo("http://tizen.org/privilege/mediacontroller.client");
	__privinfo("http://tizen.org/privilege/telephony");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_db_manager_check_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	if (ret > 0)
		ret = PRVMGR_ERR_USING_BANNED_PRIVILEGE;
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();
	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "uid = 1, Contains banned privilege but the UID is different");
	__privinfo("http://tizen.org/privilege/account.read");
	__privinfo("http://tizen.org/privilege/internet");
	__privinfo("http://tizen.org/privilege/telephony");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_db_manager_check_black_list(1, PRVMGR_PACKAGE_TYPE_WRT, privilege_list);
	if (ret > 0)
		ret = PRVMGR_ERR_USING_BANNED_PRIVILEGE;
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();
	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "uid = 0, Contains banned privilege but the package type is different");
	__privinfo("http://tizen.org/privilege/account.read");
	__privinfo("http://tizen.org/privilege/internet");
	__privinfo("http://tizen.org/privilege/telephony");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_db_manager_check_black_list(0, PRVMGR_PACKAGE_TYPE_CORE, privilege_list);
	if (ret > 0)
		ret = PRVMGR_ERR_USING_BANNED_PRIVILEGE;
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();
	g_list_free(privilege_list);
	privilege_list = NULL;
}

static void __test_black_list_verify_privilege()
{
	GList *black_list = NULL;
	char *error_message = NULL;
	int ret = 0;

	__change_to_bold_yellow();
	printf("\nTest privilege_manager_verify_privilege with banned privilege\n\n");
	__change_color_to_origin();

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(black_list);
	__print_line();

	__tcinfo(goal, "2.3.1, wrt, Name=OK, Version=OK, Level=OK -- Public ");
	__privinfo("http://tizen.org/privilege/bluetooth.gap");
	__privinfo("http://tizen.org/privilege/bluetooth.health");
	__privinfo("http://tizen.org/privilege/alarm");
	__privinfo("http://tizen.org/privilege/application.info");
	__privinfo("http://tizen.org/privilege/calendar.read");
	__privinfo("http://tizen.org/privilege/call");
	__privinfo("http://tizen.org/privilege/callhistory.read");
	__privinfo("http://tizen.org/privilege/contact.write");
	__privinfo("http://tizen.org/privilege/content.read");
	__privinfo("http://tizen.org/privilege/datacontrol.consumer");
	__privinfo("http://tizen.org/privilege/datasync");
	__privinfo("http://tizen.org/privilege/download");
	__privinfo("http://tizen.org/privilege/filesystem.read");
	__privinfo("http://tizen.org/privilege/fullscreen");
	__privinfo("http://tizen.org/privilege/healthinfo");
	__privinfo("http://tizen.org/privilege/internet");
	__privinfo("http://tizen.org/privilege/location");
	__privinfo("http://tizen.org/privilege/mediacapture");
	__privinfo("http://tizen.org/privilege/messaging.write");
	__privinfo("http://tizen.org/privilege/nfc.admin");
	__privinfo("http://tizen.org/privilege/notification");
	__privinfo("http://tizen.org/privilege/package.info");
	__privinfo("http://tizen.org/privilege/power");
	__privinfo("http://tizen.org/privilege/telephony");
	__privinfo("http://tizen.org/privilege/websetting");
	__tcinfo(expect, "PRVMGR_ERR_USING_BANNED_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_USING_BANNED_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "uid = 0, Remove all banned wrt privileges");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_unset_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, black_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();
	g_list_free(black_list);
	black_list = NULL;

	__tcinfo(goal, "uid = 0, Get wrt black list");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_info_get_black_list(0, PRVMGR_PACKAGE_TYPE_WRT, &black_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(black_list);
	__print_line();


	__tcinfo(goal, "2.3.1, wrt, Name=OK, Version=OK, Level=OK -- Public ");
	__privinfo("http://tizen.org/privilege/bluetooth.gap");
	__privinfo("http://tizen.org/privilege/bluetooth.health");
	__privinfo("http://tizen.org/privilege/alarm");
	__privinfo("http://tizen.org/privilege/application.info");
	__privinfo("http://tizen.org/privilege/calendar.read");
	__privinfo("http://tizen.org/privilege/call");
	__privinfo("http://tizen.org/privilege/callhistory.read");
	__privinfo("http://tizen.org/privilege/contact.write");
	__privinfo("http://tizen.org/privilege/content.read");
	__privinfo("http://tizen.org/privilege/datacontrol.consumer");
	__privinfo("http://tizen.org/privilege/datasync");
	__privinfo("http://tizen.org/privilege/download");
	__privinfo("http://tizen.org/privilege/filesystem.read");
	__privinfo("http://tizen.org/privilege/fullscreen");
	__privinfo("http://tizen.org/privilege/healthinfo");
	__privinfo("http://tizen.org/privilege/internet");
	__privinfo("http://tizen.org/privilege/location");
	__privinfo("http://tizen.org/privilege/mediacapture");
	__privinfo("http://tizen.org/privilege/messaging.write");
	__privinfo("http://tizen.org/privilege/nfc.admin");
	__privinfo("http://tizen.org/privilege/notification");
	__privinfo("http://tizen.org/privilege/package.info");
	__privinfo("http://tizen.org/privilege/power");
	__privinfo("http://tizen.org/privilege/telephony");
	__privinfo("http://tizen.org/privilege/websetting");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

}

int main()
{
	__print_dline();

	__change_color_to_yellow();
	printf("\n");
	printf("Test function : privilege_manager_set_black_list\n");
	printf("                privilege_manager_unset_black_list\n");
	printf("                privilege_info_get_black_list\n");
	printf("\n");
	__change_color_to_origin();
	__test_black_list_set_unset();

	__print_dline();

	__change_color_to_yellow();
	printf("Test function : privilege_db_manager_check_black_list\n");
	printf("                privilege_info_get_black_list\n");
	__change_color_to_origin();
	__test_black_list_check();

	__print_dline();


	__change_color_to_yellow();
	printf("Test function : privilege_manager_verify_privilege\n");
	__change_color_to_origin();
	__test_black_list_verify_privilege();

	__print_dline();

	__change_color_to_green();
	printf("Test Complete\n");
	printf("success : %d, ", success_cnt);
	__change_color_to_red();
	printf("fail : %d\n", fail_cnt);
	__change_color_to_origin();

	return 0;
}
