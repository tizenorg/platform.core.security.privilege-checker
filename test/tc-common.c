#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <glib.h>
#include <tc-common.h>

/*** Color section ***/
void __color_to_red()
{
	printf("%c[%d;%dm", 0x1B, BRIGHTNESS, RED);
}
void __color_to_green()
{
	printf("%c[%d;%dm", 0x1B, BRIGHTNESS, GREEN);
}
void __color_to_yellow()
{
	printf("%c[%d;%dm", 0x1B, BRIGHTNESS, YELLOW);
}
void __color_to_blue()
{
	printf("%c[%d;%dm", 0x1B, BRIGHTNESS, BLUE);
}
void __color_to_origin()
{
	printf("%c[%dm", 0x1B, 0);
}
void __color_to_bold_red()
{
	printf("%c[%dm%c[%dm", 0x1B, 1, 0x1B, RED);
}
void __color_to_bold_yellow()
{
	printf("%c[%dm%c[%dm", 0x1B, 1, 0x1B, YELLOW);
}

/* error enum to string */
char* __get_result_string(char type, int ret)
{
	switch (type) {
	case 'm':
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
		break;
	case 'd':
		if (ret == PRIVILEGE_DB_MANAGER_ERR_NONE)
			return "PRIVILEGE_DB_MANAGER_ERR_NONE";
		else if (ret == PRIVILEGE_DB_NO_EXIST_RESULT)
			return "PRIVILEGE_DB_NO_EXIST_RESULT";
		else if (ret == PRIVILEGE_DB_MANAGER_ERR_CONNECTION_FAIL)
			return "PRIVILEGE_DB_MANAGER_ERR_CONNECTION_FAIL";
		else if (ret == PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY)
			return "PRIVILEGE_DB_MANAGER_ERR_INVALID_QUERY";
		else if (ret == PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY)
			return "PRIVILEGE_DB_MANAGER_ERR_OUT_OF_MEMORY";
		else if (ret == PRIVILEGE_DB_MANAGER_ERR_INVALID_TYPE)
			return "PRIVILEGE_DB_MANAGER_ERR_INVALID_TYPE";
		else if (ret == PRIVILEGE_DB_MANAGER_ERR_DB_BUSY_FAIL)
            return "PRIVILEGE_DB_MANAGER_ERR_DB_BUSY_FAIL";
		else if (ret == PRIVILEGE_DB_MANAGER_ERR_DB_CONSTRAINT_FAIL)
            return "PRIVILEGE_DB_MANAGER_ERR_DB_CONSTRAINT_FAIL";
		else if (ret == PRIVILEGE_DB_MANAGER_ERR_DB_FULL_FAIL)
            return "PRIVILEGE_DB_MANAGER_ERR_DB_FULL_FAIL";
		else if (ret == PRIVILEGE_DB_MANAGER_ERR_DB_UPDATE_FAIL)
            return "PRIVILEGE_DB_MANAGER_ERR_DB_UPDATE_FAIL";
		break;
	default:
		break;
	}

	return "FAIL";
}

/* shows error_message returned by privilege_manager_verify_privilege() */
void __print_error_message(char *error_message)
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
		__color_to_red();
	} else {
		__color_to_blue();
	}
	printf("\nError Message : %s\n", err_type);
	__color_to_origin();

	token = strtok_r(temp, "|", &save);
	while (token) {
		printf("%s\n", token);
		token = strtok_r(NULL, "|", &save);
	}
	printf("\n");
	free(temp);
}

void __tcinfo(tcinfo_type_e type, ...)
{
	va_list ap;
	va_start(ap, type);
	switch (type) {
	case goal: /* shows test goal */
		__color_to_yellow();
		printf("TEST >> %s\n\n", va_arg(ap, char*));
		__color_to_origin();
		break;
	case cert_level: /* set certificate level */
		visibility_string = va_arg(ap, char*);
		if (strncmp(visibility_string, "public", strlen(visibility_string)) == 0)
			visibility = PRVMGR_PACKAGE_VISIBILITY_PUBLIC;
		else if (strncmp(visibility_string, "partner", strlen(visibility_string)) == 0)
			visibility = PRVMGR_PACKAGE_VISIBILITY_PARTNER;
		else if(strncmp(visibility_string, "platform", strlen(visibility_string)) == 0)
			visibility = PRVMGR_PACKAGE_VISIBILITY_PLATFORM;
		else
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!WRONG CERT LEVEL!!!!!!!!!!!!!!!!!!!!!");
		printf("\n- SIGNATURE LEVEL : %s\n", visibility_string);
		break;
	case version_type: /* set api_version and pkg_type */
		__print_dline();
		__color_to_bold_yellow();
		api_version = va_arg(ap, char*);
		char* pkg_type_string = va_arg(ap, char*);
		if(strncmp("core", pkg_type_string, strlen("core")) == 0 )
			pkg_type = PRVMGR_PACKAGE_TYPE_CORE;
		else if(strncmp("wrt", pkg_type_string, strlen("wrt")) == 0)
			pkg_type = PRVMGR_PACKAGE_TYPE_WRT;
		else
			printf("!!!!!!!!!!!!!!!!!!!!!!WRONG PACKAGE TYPE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		printf("- API VERSION : %s\n", api_version);
		printf("- PACKAGE TYPE : %s\n", pkg_type_string);
		__color_to_origin();
		__print_dline();
		__print_nline();
		break;
	case expect: /* set expected_result */
		expected_result = va_arg(ap, int);
		break;
	case results: /* shows expected result and achieved result */
		printf("\n- EXPECTED RESULT : %s\n", va_arg(ap, char*));
		printf("- ACHIEVED RESULT : %s\n\n", va_arg(ap, char*));
		break;
	case function: /* shows function to test */
		__color_to_yellow();
		__print_dline();
		printf("# TEST FUNCTION : %s()\n", va_arg(ap, char*));
		__print_dline();
		__color_to_origin();
		break;
	default:
		printf("no matching enum for input\n");
	}
	va_end(ap);
}

void __privinfo(char *name, char *level, char *comment)
{
	if (level != NULL)
		printf("|%s| ", level);
	printf("%s", name);
	privilege_list = g_list_append(privilege_list, name);
	if (comment != NULL)
		printf("   -- %s", comment);
	printf("\n");
}

void __print_line()
{
	printf("-------------------------------------------------------------------\n");
}

void __print_dline()
{
	printf("===================================================================\n");
}

void __print_nline()
{
	printf("\n");
}

void __print_privilege_list(GList * privilege_list)
{
	GList *l;
	printf("Print mapped privilege list: \n");
	for (l = privilege_list; l != NULL; l = l->next) {
		char *privilege_name = (char *)l->data;
		printf("%s\n", privilege_name);
	}
}

bool __check_result(int result)
{
	if (expected_result == result)
		return true;
	else
		return false;
}

bool __count_result(int result)
{
	bool success = __check_result(result);
	if (success)
		success_cnt++;
	else
		fail_cnt++;
	return success;
}

void __print_result(char type, int result)
{
	__tcinfo(results, __get_result_string(type, expected_result), __get_result_string(type, result));
	if (__count_result(result)) {
		__color_to_green();
		printf(">> test success\n");
	} else {
		__color_to_bold_red();
		printf(">> test fail\n");
	}
	__color_to_origin();
}

