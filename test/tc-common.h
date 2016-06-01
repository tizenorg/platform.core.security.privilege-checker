#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <glib.h>
#include <privilege_manager.h>
#include <privilege_db_manager.h>

#define BRIGHTNESS 0
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define WHITE 37
#define BG_BLACK 40
#define ShowErrorMsg(condition, error_message, result, ...) \
	if (condition) { \
		__print_error_message(error_message); \
		free(error_message); \
		error_message = NULL; \
	}
#define gfree(list) \
	g_list_free(list); \
	list = NULL;

int success_cnt;
int fail_cnt;
GList *privilege_list;
int expected_result;
privilege_manager_visibility_e visibility;
char* visibility_string;
privilege_manager_package_type_e pkg_type;
char* api_version;

void __color_to_red();
void __color_to_green();
void __color_to_yellow();
void __color_to_blue();
void __color_to_origin();
void __color_to_bold_red();
void __color_to_bold_yellow();

void __print_line();
void __print_dline();
void __print_nline();
void __print_test_func();

char* __get_result_string(char type, int ret);
void __print_error_message(char *error_message);

typedef enum {
	goal = 1,
	cert_level = 2,
	expect = 3,
	results = 5,
	function = 6,
	version_type = 7
} tcinfo_type_e;

void __tcinfo(tcinfo_type_e type, ...);
void __privinfo(char *name, char *level, char *comment);

void __print_privilege_list(GList * privilege_list);

bool __check_result(int result);
bool __count_result(int result);
void __print_result(char type, int result);


