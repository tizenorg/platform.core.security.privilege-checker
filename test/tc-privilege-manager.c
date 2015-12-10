#include <stdio.h>
#include <privilege_manager.h>
#include <stdarg.h>

#define BRIGHTNESS 0
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36
#define WHITE 37
#define BG_BLACK 40

#define ShowErrorMsg(condition, error_message, result, ...) \
	__print_result(__get_result_string(result)); \
	if (condition) { \
		__print_error_message(error_message); \
		free(error_message); \
		error_message = NULL; \
	}

static int fail_cnt = 0;
static int success_cnt = 0;
GList *privilege_list = NULL;

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

static void __change_color_to_magenta()
{
	printf("%c[%d;%dm", 0x1B, BRIGHTNESS, MAGENTA);
}

static void __change_color_to_cyan()
{
	printf("%c[%d;%dm", 0x1B, BRIGHTNESS, CYAN);
}

static void __change_color_to_origin()
{
	printf("%c[%dm", 0x1B, 0);
}

static void __change_to_bold_white()
{
	printf("%c[%dm%c[%dm", 0x1B, 1, 0x1B, WHITE);
}

static void __change_to_bold_red()
{
	printf("%c[%dm%c[%dm", 0x1B, 1, 0x1B, RED);
}

static void __change_to_bold_green()
{
	printf("%c[%dm%c[%dm", 0x1B, 1, 0x1B, GREEN);
}

static void __change_to_bold_yellow()
{
	printf("%c[%dm%c[%dm", 0x1B, 1, 0x1B, YELLOW);
}

static void __change_to_bold_cyan()
{
	printf("%c[%dm%c[%dm", 0x1B, 1, 0x1B, CYAN);
}

static void __change_to_bold_blue()
{
	printf("%c[%dm%c[%dm", 0x1B, 1, 0x1B, BLUE);
}

static void __change_to_bold_magenta()
{
	printf("%c[%dm%c[%dm", 0x1B, 1, 0x1B, MAGENTA);
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

static const char *__get_result_string(privilege_manager_error_e ret)
{
	if (ret == PRVMGR_ERR_NONE)
		return "PRVMGR_ERR_NONE";
	else if (ret == PRVMGR_ERR_INTERNAL_ERROR)
		return "PRVMGR_ERR_INTERNAL_ERROR";
	else if (ret == PRVMGR_ERR_INVALID_PRIVILEGE)
		return "PRVMGR_ERR_INVALID_PRIVILEGE";
	else if (ret == PRVMGR_ERR_INVALID_PARAMETER)
		return "PRVMGR_ERR_INVALID_PARAMETER";
	else
		return "FAIL";

}

typedef enum {
	goal = 1,
	cert_level = 2,
	api_version = 3,
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
	case cert_level:
		printf("\n- signature level : %s\n\n", input_string);
		break;
	case api_version:
		printf("API VERSION : %s\n", input_string);
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

static void __privinfo(char *name, char *level, char *comment)
{
	if (level != NULL)
		printf("|%s| ", level);
	printf("%s", name);
	privilege_list = g_list_append(privilege_list, name);
	if (comment != NULL)
		printf("   -- %s", comment);
	printf("\n");
}

static void __print_result(char *input_string)
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
	printf("Print mapped privilege list: \n");
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

static void __test_privilege_manager_get_mapped_privilege_list()
{
	int ret = 0;
	GList *mapped_privilege_list;

	__tcinfo(goal, "api_version == NULL");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/camera", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PARAMETER");
	ret = privilege_manager_get_mapped_privilege_list(NULL, PRVMGR_PACKAGE_TYPE_CORE, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_INVALID_PARAMETER, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "api_version == 2.3.4.5.1");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/camera", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PARAMETER");
	ret = privilege_manager_get_mapped_privilege_list("2.3.4.5.1", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_INVALID_PARAMETER, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "api_version == 2.3");
	__privinfo("http://tizen.org/privilege/wrong-privilege-name", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	if (mapped_privilege_list == NULL)
		printf("it's null\n");
	else
		printf("it's NOT null\n");
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

#ifdef PROFILE_TYPE_MOBILE

	/* 2.3 core - mobile */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.3");
	__tcinfo(pkg_type, "core");
	__change_color_to_origin();
	__print_dline();

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/camera", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.write", NULL, NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	/* 2.2.1 wrt - mobile */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.2.1");
	__tcinfo(pkg_type, "wrt");
	__change_color_to_origin();
	__print_dline();

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.write", NULL, NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_get_mapped_privilege_list("2.2.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/platform", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.write", NULL, NULL);
	__privinfo("http://tizen.org/privilege/datasync", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_get_mapped_privilege_list("2.2.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/content.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/nfc.p2p", NULL, NULL);
	__privinfo("http://tizen.org/privilege/call", NULL, NULL);
	__privinfo("http://tizen.org/privilege/internet", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_get_mapped_privilege_list("2.2.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	/* 2.3 wrt - mobile */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.3");
	__tcinfo(pkg_type, "wrt");
	__change_color_to_origin();
	__print_dline();

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.write", NULL, NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/platform", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.write", NULL, NULL);
	__privinfo("http://tizen.org/privilege/datasync", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/content.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/nfc.p2p", NULL, NULL);
	__privinfo("http://tizen.org/privilege/call", NULL, NULL);
	__privinfo("http://tizen.org/privilege/internet", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/location", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/platform", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	/* 2.4 wrt - mobile */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.4");
	__tcinfo(pkg_type, "wrt");
	__change_color_to_origin();
	__print_dline();

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/location", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_get_mapped_privilege_list("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/platform", NULL, NULL);
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_get_mapped_privilege_list("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result(__get_result_string(ret));
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

#endif
}

static void __test_privilege_manager_verify_privilege()
{
	char *error_message = NULL;
	int ret = 0;

	__tcinfo(goal, "api_version == NULL");
	__privinfo("http://tizen.org/privilege/account.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/account.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/alarm.get", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings", "Public", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PARAMETER");
	ret = privilege_manager_verify_privilege(NULL, PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PARAMETER, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

	__tcinfo(goal, "invalid api_version == 2.3.4.2.1");
	__privinfo("http://tizen.org/privilege/account.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/account.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/alarm.get", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings", "Public", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PARAMETER");
	ret = privilege_manager_verify_privilege("2.3.4.2.1", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PARAMETER, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;

#ifdef PROFILE_TYPE_MOBILE

	/* 2.3 core - mobile */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.3");
	__tcinfo(pkg_type, "core");
	__change_color_to_origin();
	__print_dline();

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Public");
	__privinfo("http://tizen.org/privilege/account.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/account.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/alarm.get", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/calendar.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/callhistory.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/callhistory.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/datasharing", "Public", NULL);
	__privinfo("http://tizen.org/privilege/display", "Public", NULL);
	__privinfo("http://tizen.org/privilege/download", "Public", NULL);
	__privinfo("http://tizen.org/privilege/display", "Public", NULL);
	__privinfo("http://tizen.org/privilege/haptic", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/keymanager", "Public", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings", "Public", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform");
	__privinfo("http://tizen.org/privilege/bookmark.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/email.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/keymanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/web-history.admin", "Platform", NULL);
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PLATFORM, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/account.readd", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/tizen/privilege/internet", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/healthinfo", NULL, "Valid from 2.3.1");
	__privinfo("http://tizen.org/privilege/keygrab", NULL, "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, !!! Level=Mismatched !!!");
	__privinfo("http://tizen.org/privilege/bookmark.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/email.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/keymanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.admin", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	printf("\n");

	/* 2.3.1 core - mobile */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.3.1");
	__tcinfo(pkg_type, "core");
	__change_color_to_origin();
	__print_dline();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Public");
	__privinfo("http://tizen.org/privilege/account.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/account.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/alarm.get", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/calendar.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/callhistory.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/callhistory.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/datasharing", "Public", NULL);
	__privinfo("http://tizen.org/privilege/display", "Public", NULL);
	__privinfo("http://tizen.org/privilege/download", "Public", NULL);
	__privinfo("http://tizen.org/privilege/display", "Public", NULL);
	__privinfo("http://tizen.org/privilege/haptic", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/keymanager", "Public", NULL);
	__privinfo("http://tizen.org/privilege/healthinfo", "Public", "Valid from 2.3.1");
	__privinfo("http://tizen.org/privilege/widget.viewer", "Public", "Valid from 2.3.1");
	__privinfo("http://tizen.org/privilege/secureelement", "Public", "Valid from 2.3.1");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform");
	__privinfo("http://tizen.org/privilege/bookmark.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/email.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/keymanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/web-history.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/reboot", "Platform", "Valid from 2.3.1");
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PLATFORM, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/account.readd", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/tizen/privilege/internet", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/ime", NULL, "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/keygrab", NULL, "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!!");
	__privinfo("http://tizen.org/privilege/email.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/keymanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/web-history.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/reboot", "Platform", "Valid from 2.3.1");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemsettings", NULL, "Deprecated from 2.3.1");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	printf("\n");

	/* 2.4 core - mobile */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.4");
	__tcinfo(pkg_type, "core");
	__change_color_to_origin();
	__print_dline();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Public");
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/keymanager", "Public", NULL);
	__privinfo("http://tizen.org/privilege/healthinfo", "Public", NULL);
	__privinfo("http://tizen.org/privilege/widget.viewer", "Public", NULL);
	__privinfo("http://tizen.org/privilege/secureelement", "Public", NULL);
	__privinfo("http://tizen.org/privilege/apphistory.read", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/appmanager.kill.bgapp", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/ime", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/imemanager", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/mapservice", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/mediacontroller.client", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/mediacontroller.server", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/minicontrol.provider", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/mediahistory.read", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/packagemanager.clearcache", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/systemmonitor", "Public", "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform");
	__privinfo("http://tizen.org/privilege/web-history.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/reboot", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/inputgenerator", "Platform", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/keygrab", "Platform", "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PLATFORM, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/account.readd", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/tizen/privilege/internet", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!!");
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/web-history.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/reboot", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/inputgenerator", "Platform", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/keygrab", "Platform", "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemsettings", NULL, "Deprecated from 2.3.1");
	__privinfo("http://tizen.org/privilege/keymanager.admin", NULL, "Deprecated from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	printf("\n");

	/* 2.2.1 Web - mobile */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.2.1");
	__tcinfo(pkg_type, "wrt");
	__change_color_to_origin();
	__print_dline();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! NAME NOT EXIST !!! -- Under 2.3.1, Undefined privileges on web apps are ignored");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.2.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with public signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.2.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with partner signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.2.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	/* 2.3 Web - mobile */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.3");
	__tcinfo(pkg_type, "wrt");
	__change_color_to_origin();
	__print_dline();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! NAME NOT EXIST !!! -- Under 2.3.1, Undefined privileges on web apps are ignored");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with public signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with partner signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	/* 2.3.1 Web - mobile */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.3.1");
	__tcinfo(pkg_type, "wrt");
	__change_color_to_origin();
	__print_dline();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Public ");
	__privinfo("http://tizen.org/privilege/bluetooth.gap", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth.health", "Public", NULL);
	__privinfo("http://tizen.org/privilege/alarm", "Public", NULL);
	__privinfo("http://tizen.org/privilege/application.info", "Public", NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/callhistory.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/contact.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/content.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/datacontrol.consumer", "Public", NULL);
	__privinfo("http://tizen.org/privilege/datasync", "Public", NULL);
	__privinfo("http://tizen.org/privilege/download", "Public", NULL);
	__privinfo("http://tizen.org/privilege/filesystem.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/fullscreen", "Public", NULL);
	__privinfo("http://tizen.org/privilege/healthinfo", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/location", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", "Public", NULL);
	__privinfo("http://tizen.org/privilege/messaging.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Public", NULL);
	__privinfo("http://tizen.org/privilege/notification", "Public", NULL);
	__privinfo("http://tizen.org/privilege/package.info", "Public", NULL);
	__privinfo("http://tizen.org/privilege/power", "Public", NULL);
	__privinfo("http://tizen.org/privilege/telephony", "Public", NULL);
	__privinfo("http://tizen.org/privilege/websetting", "Public", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Partner ");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/content.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/datacontrol.consumer", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/location", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", "Public", NULL);
	__privinfo("http://tizen.org/privilege/messaging.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Public", NULL);
	__privinfo("http://tizen.org/privilege/notification", "Public", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform ");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/content.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/datacontrol.consumer", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/location", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", "Public", NULL);
	__privinfo("http://tizen.org/privilege/messaging.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Public", NULL);
	__privinfo("http://tizen.org/privilege/notification", "Public", NULL);
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PLATFORM, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__privinfo("http://tizen.org/privilege/ime", NULL, "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/calendar", NULL, "Deprecated from 2.0");
	__privinfo("http://tizen.org/privilege/mediacontent", NULL, "Deprecated from 2.0");
	__privinfo("http://tizen.org/privilege/systemmanager", NULL, "Deprecated from 2.3.1");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with public signatured certificate");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with partner signatured certificate");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	/* 2.4 Web - mobile */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.4");
	__tcinfo(pkg_type, "wrt");
	__change_color_to_origin();
	__print_dline();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Public ");
	__privinfo("http://tizen.org/privilege/alarm", "Public", NULL);
	__privinfo("http://tizen.org/privilege/application.info", "Public", NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/callhistory.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/contact.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/content.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/datacontrol.consumer", "Public", NULL);
	__privinfo("http://tizen.org/privilege/datasync", "Public", NULL);
	__privinfo("http://tizen.org/privilege/download", "Public", NULL);
	__privinfo("http://tizen.org/privilege/filesystem.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/fullscreen", "Public", NULL);
	__privinfo("http://tizen.org/privilege/healthinfo", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/location", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", "Public", NULL);
	__privinfo("http://tizen.org/privilege/messaging.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Public", NULL);
	__privinfo("http://tizen.org/privilege/notification", "Public", NULL);
	__privinfo("http://tizen.org/privilege/package.info", "Public", NULL);
	__privinfo("http://tizen.org/privilege/power", "Public", NULL);
	__privinfo("http://tizen.org/privilege/telephony", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/ime", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/led", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/mediacontroller.client", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/mediacontroller.server", "Public", "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Partner ");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/content.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/datacontrol.consumer", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/location", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", "Public", NULL);
	__privinfo("http://tizen.org/privilege/messaging.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Public", NULL);
	__privinfo("http://tizen.org/privilege/notification", "Public", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform ");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/content.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/datacontrol.consumer", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/location", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", "Public", NULL);
	__privinfo("http://tizen.org/privilege/messaging.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Public", NULL);
	__privinfo("http://tizen.org/privilege/notification", "Public", NULL);
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PLATFORM, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemmanager", NULL, "Deprecated from 2.3.1");
	__privinfo("http://tizen.org/privilege/bluetooth.admin", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/bluetooth.gap", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/bluetooth.spp", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/bluetooth.health", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/websetting", NULL, "Deprecated from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with public signatured certificate");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with partner signatured certificate");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

#endif
#ifdef PROFILE_TYPE_WEARABLE

	/* 2.3.1 core - wearable */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.3.1");
	__tcinfo(pkg_type, "core");
	__change_color_to_origin();
	__print_dline();

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Public");
	__privinfo("http://tizen.org/privilege/alarm.get", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/callhistory.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/callhistory.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/datasharing", "Public", NULL);
	__privinfo("http://tizen.org/privilege/display", "Public", NULL);
	__privinfo("http://tizen.org/privilege/download", "Public", NULL);
	__privinfo("http://tizen.org/privilege/display", "Public", NULL);
	__privinfo("http://tizen.org/privilege/haptic", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/keymanager", "Public", NULL);
	__privinfo("http://tizen.org/privilege/nfc", "Public", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.info", "Public", NULL);
	__privinfo("http://tizen.org/privilege/telephony", "Public", NULL);
	__privinfo("http://tizen.org/privilege/widget.viewer", "Public", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform");
	__privinfo("http://tizen.org/privilege/bluetooth.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/keymanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PLATFORM, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.read", NULL, "Mobile profile only until 2.4");
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", "Mobile profile only");
	__privinfo("http://tizen.org/privilege/calendar.write", "Public", "Mobile profile only");
	__privinfo("http://tizen.org/privilege/ime", NULL, "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!!");
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/keymanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/reboot", "Platform", "Valid from 2.3.1");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	printf("\n");

	/* 2.4 core - wearable */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.4");
	__tcinfo(pkg_type, "core");
	__change_color_to_origin();
	__print_dline();

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Public");
	__privinfo("http://tizen.org/privilege/account.read", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/account.write", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/alarm.get", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/callhistory.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/callhistory.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/contact.read", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/display", "Public", NULL);
	__privinfo("http://tizen.org/privilege/download", "Public", NULL);
	__privinfo("http://tizen.org/privilege/display", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mapservice", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/mediacontroller.client", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/minicontrol.viewer", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/nfc", "Public", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.clearcache", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/telephony", "Public", NULL);
	__privinfo("http://tizen.org/privilege/widget.viewer", "Public", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform");
	__privinfo("http://tizen.org/privilege/bluetooth.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Platform", NULL);
	__privinfo("http://tizen.org/privielge/keygrab", "Platform", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/systemsettings.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PLATFORM, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", "Mobile profile only");
	__privinfo("http://tizen.org/privilege/calendar.write", "Public", "Mobile profile only");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!!");
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privielge/keygrab", "Platform", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/reboot", "Platform", "Valid from 2.3.1");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/keymanager.admin", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	printf("\n");

	/* 2.3 Web - wearable */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.3");
	__tcinfo(pkg_type, "wrt");
	__change_color_to_origin();
	__print_dline();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! NAME NOT EXIST !!! -- Under 2.3.1, Undefined privileges on web apps are ignored");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with public signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with partner signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	/* 2.3.1 Web - wearable */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.3.1");
	__tcinfo(pkg_type, "wrt");
	__change_color_to_origin();
	__print_dline();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Public ");
	__privinfo("http://tizen.org/privilege/audiorecorder", "Public", "Wearable profile only");
	__privinfo("http://tizen.org/privilege/camera", "Public", "Wearable profile only");
	__privinfo("http://tizen.org/privilege/alarm", "Public", NULL);
	__privinfo("http://tizen.org/privilege/application.info", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth.admin", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth.spp", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth.health", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth.gap", "Public", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/content.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/download", "Public", NULL);
	__privinfo("http://tizen.org/privilege/filesystem.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/healthinfo", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/location", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", "Public", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Public", NULL);
	__privinfo("http://tizen.org/privilege/notification", "Public", NULL);
	__privinfo("http://tizen.org/privilege/package.info", "Public", NULL);
	__privinfo("http://tizen.org/privilege/power", "Public", NULL);
	__privinfo("http://tizen.org/privilege/telephony", "Public", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Partner ");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/content.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform ");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/content.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/location", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", "Public", NULL);
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PLATFORM, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.read", NULL, "Mobile profile only");
	__privinfo("http://tizen.org/privilege/ime", NULL, "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemmanager", NULL, "Deprecated from 2.3.1");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with public signatured certificate");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with partner signatured certificate");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	/* 2.4 Web - wearable */
	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.4");
	__tcinfo(pkg_type, "wrt");
	__change_color_to_origin();
	__print_dline();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Public ");
	__privinfo("http://tizen.org/privilege/audiorecorder", "Public", "Wearable profile only");
	__privinfo("http://tizen.org/privilege/camera", "Public", "Wearable profile only");
	__privinfo("http://tizen.org/privilege/alarm", "Public", NULL);
	__privinfo("http://tizen.org/privilege/application.info", "Public", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/location", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", "Public", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Public", NULL);
	__privinfo("http://tizen.org/privilege/notification", "Public", NULL);
	__privinfo("http://tizen.org/privilege/package.info", "Public", NULL);
	__privinfo("http://tizen.org/privilege/power", "Public", NULL);
	__privinfo("http://tizen.org/privilege/telephony", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/ime", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/mediacontroller.client", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/mediacontroller.server", "Public", "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Partner ");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/content.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform ");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/content.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/location", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", "Public", NULL);
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PLATFORM, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.read", NULL, "Mobile profile only");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemmanager", NULL, "Deprecated from 2.3.1");
	__privinfo("http://tizen.org/privilege/bluetooth.gap", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/bluetooth.spp", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/bluetooth.health", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/bluetooth.admin", NULL, "Deprecated from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with public signatured certificate");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with partner signatured certificate");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();
#endif
#ifdef PROFILE_TYPE_TV
	/* 2.3 Web - TV */

	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.3");
	__tcinfo(pkg_type, "wrt");
	__change_color_to_origin();
	__print_dline();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! NAME NOT EXIST !!! -- Under 2.3.1, Undefined privileges on web apps are ignored");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with public signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/systemmanager", "Partner", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with partner signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	/* 2.4 Web - TV */

	__print_dline();
	__change_to_bold_yellow();
	__tcinfo(api_version, "2.4");
	__tcinfo(pkg_type, "wrt");
	__change_color_to_origin();
	__print_dline();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Public ");
	__privinfo("http://tizen.org/privilege/alarm", "Public", NULL);
	__privinfo("http://tizen.org/privilege/application.info", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacapture", "Public", NULL);
	__privinfo("http://tizen.org/privilege/package.info", "Public", NULL);
	__privinfo("http://tizen.org/privilege/system", "Public", NULL);
	__privinfo("http://tizen.org/privilege/telephony", "Public", NULL);
	__privinfo("http://tizen.org/privilege/tv.audio", "Public", "TV profile only");
	__privinfo("http://tizen.org/privilege/tv.channel", "Public", "TV profile only");
	__privinfo("http://tizen.org/privilege/tv.display", "Public", "TV profile only");
	__privinfo("http://tizen.org/privilege/tv.window", "Public", "TV profile only");
	__privinfo("http://tizen.org/privilege/tv.inputdevice", "Public", "TV profile only");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Partner ");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform ");
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_NONE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PLATFORM, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_NONE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemmanager", NULL, "Deprecated from 2.3.1");
	__privinfo("http://tizen.org/privilege/websetting", NULL, "Deprecated from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with public signatured certificate");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PUBLIC, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();

	g_list_free(privilege_list);
	privilege_list = NULL;
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with partner signatured certificate");
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, "PRVMGR_ERR_INVALID_PRIVILEGE");
	ret = privilege_manager_verify_privilege("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, PRVMGR_PACKAGE_VISIBILITY_PARTNER, &error_message);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__check_verify_result(PRVMGR_ERR_INVALID_PRIVILEGE, ret);
	__print_line();
#endif
	g_list_free(privilege_list);
	privilege_list = NULL;
}

int main()
{
	__change_color_to_yellow();
	printf("Test function : privilege_manager_verify_privilege\n");
	__change_color_to_origin();
	__test_privilege_manager_verify_privilege();

	__change_color_to_yellow();
	printf("Test function : privilege_manager_get_mapped_privilege_list\n");
	__change_color_to_origin();
	__test_privilege_manager_get_mapped_privilege_list();

	__change_color_to_green();
	printf("Test Complete\n");
	printf("success : %d, ", success_cnt);
	__change_color_to_red();
	printf("fail : %d\n", fail_cnt);
	__change_color_to_origin();

	return 0;
}
