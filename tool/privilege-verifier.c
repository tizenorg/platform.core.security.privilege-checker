/*
 * Copyright(c)2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0(the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <libgen.h>
#include <limits.h>
#include "privilege_manager.h"

static int quiet_mode;
#define PRINT(msg, arg...) if (!quiet_mode) printf(msg, ##arg);
#define HELP_MSG PRINT(usage, basename(argv[0]));

#define TOOL_VERSION "1.0.0"
#define DATE "2016.07.26"

static const char usage[] =
	"Usage: %s [options] <path>\n"
	"options:\n"
	" -v --version             output version information and exit\n"
	" -h --help                output usage information and exit\n"
	" -a --api-version         set api-version\n"
	" -k --package-type        set package type, wgt/tpk, default is tpk, Must call twice seperately for hybrid case\n"
	" -c --certificate-level   set certificate level, public/partner/platform, default is public\n"
	" -p --privileges          privilege list\n"
	" -q --quiet               quiet mode\n"
;

static int __get_package_type_value(char* package_type_name)
{
	if (!strcmp(package_type_name, "wgt") || !strcmp(package_type_name, "WGT")) {
		return PRVMGR_PACKAGE_TYPE_WRT;
	}
	else if (!strcmp(package_type_name, "tpk") || !strcmp(package_type_name, "TPK")) {
		return PRVMGR_PACKAGE_TYPE_CORE;
	}
	return PRVMGR_PACKAGE_TYPE_MAX;
}

static int __get_visibility_value(char* visibility_name)
{
	if (!strcmp(visibility_name, "public") || !strcmp(visibility_name, "PUBLIC")) {
		return PRVMGR_PACKAGE_VISIBILITY_PUBLIC;
	}
	else if (!strcmp(visibility_name, "partner") || !strcmp(visibility_name, "PATNER")) {
		return PRVMGR_PACKAGE_VISIBILITY_PARTNER;
	}
	else if (!strcmp(visibility_name, "platform") || !strcmp(visibility_name, "PLATFORM")) {
		return PRVMGR_PACKAGE_VISIBILITY_PLATFORM;
	}
	return -1;	
}

int main(int argc, char* argv[])
{
	static const char shortoptions[] = "vha::k::c::n::p::q";
	static struct option options[] = {
		{"version", no_argument, 0, 'v'},
		{"help", no_argument, 0, 'h'},
		{"api-version", required_argument, 0, 'a'},
		{"package-type", required_argument, 0, 'k'},
		{"certificate-level", required_argument, 0, 'c'},
		{"privileges", required_argument, 0, 'p'},
		{"quiet", no_argument, 0, 'q'},
		{NULL, 0, 0, 0}
	};

	char* api_version = NULL;
	char* package_type_name = NULL;
	char* visibility_name = NULL;
	GList* privilege_list = NULL;
	int c;
	char* error_message = NULL;

	if (argc == 1) {
		HELP_MSG;
		exit(1);
	}

	while ((c = getopt_long(argc, argv, shortoptions, options, NULL)) != -1) {
		switch (c) {
			case 'a': api_version = optarg; break;
			case 'k': package_type_name = optarg; break;
			case 'c': visibility_name = optarg; break;
			case 'p':
			{
				for ( optind--; optind < argc && *argv[optind] != '-'; optind++) {
					privilege_list = g_list_append(privilege_list, argv[optind]);
				}
				break;
			}
			case 'v':
				printf("%s version %s (%s) \n", basename(argv[0]), TOOL_VERSION, DATE);
				exit(0);
			case 'h':
				HELP_MSG;
				exit(0);
			case 'q':
				quiet_mode = 1;
			default:
				HELP_MSG;
				exit(1);
		}
	}

	if ( api_version == NULL) {
		PRINT("api-version option is missed\n");
		HELP_MSG;
		exit(1);
	}
	else if (package_type_name == NULL) {
		PRINT("package-type option is missed.\nset default type : tpk\n");
		//package_type_name = "tpk";
	}
	else if (visibility_name == NULL) {
		PRINT("certificate-level option is missed.\nset default level : public\n");
		visibility_name = "public";
	}

	privilege_manager_package_type_e package_type = __get_package_type_value(package_type_name);
	if (package_type == PRVMGR_PACKAGE_TYPE_MAX) {
		PRINT("unrecognized package-type '%s'\n", package_type_name);
		HELP_MSG;
		exit(1);
	}

	privilege_manager_visibility_e visibility = __get_visibility_value(visibility_name);
	if (visibility == (unsigned int)-1) {
		PRINT("unrecognized certificate-level '%s'\n", visibility_name);
		HELP_MSG;
		exit(1);
	}

	privilege_manager_verify_privilege(api_version, package_type, privilege_list, visibility, &error_message);
	if (error_message)
		printf("%s\n", error_message);

	return 0;
}
