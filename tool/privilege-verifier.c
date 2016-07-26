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
#include "privilege_manager.h"


#define TOOL_VERSION "1.0.0"
#define DATE "2016.07.26"

static const char usage[] =
	"Usage: %s [options] <path>\n"
	"options:\n"
	" -v --version          output version information and exit\n"
	" -h --help             output usage information and exit\n"
	" --api-version         set api-version\n"
	" --package-type        set package type, wgt/tpk, You need to call this tool twice seperately for hybrid application case\n"
	" --certificate-level   set certificate level, public/partner/platform\n"
	" --numprivileges       number of privilege list\n"
	" --privileges          privilege list\n"
;

static int __get_package_type_value(char* package_type_name)
{
	if (!strcmp(package_type_name, "wgt") || !strcmp(package_type_name, "WGT")) {
		return PRVMGR_PACKAGE_TYPE_WRT;
	}
	else if (!strcmp(package_type_name, "tpk") || !strcmp(package_type_name, "TPK")) {
		return PRVMGR_PACKAGE_TYPE_CORE;
	}
	return -1;
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
	static const char shortoptions[] = "vha::k::c::n::p::";
	static struct option options[] = {
		{"version", no_argument, 0, 'v'},
		{"help", no_argument, 0, 'h'},
		{"api-version", required_argument, 0, 'a'},
		{"package-type", required_argument, 0, 'k'},
		{"certificate-level", required_argument, 0, 'c'},
		{"numprivileges", required_argument, 0, 'n'},
		{"privilege", required_argument, 0, 'p'},
		{NULL, 0, 0, 0}
	};

	char* api_version = NULL;
	char* package_type_name = NULL;
	char* visibility_name = NULL;
	int privilege_count = 0;
	GList* privilege_list = NULL;
	int c;

	while ((c = getopt_long(argc, argv, shortoptions, options, NULL)) != -1) {
		switch (c) {
			case 'a': api_version = optarg; break;
			case 'k': package_type_name = optarg; break;
			case 'c': visibility_name = optarg; break;
			case 'n': privilege_count = *(int*)optarg; break;
			case 'p': break;
			case 'v':
				printf("%s version %s (%s) ", basename(argv[0]), TOOL_VERSION, DATE);
				exit(0);
			case 'h':
				printf(usage, basename(argv[0]));
				exit(0);
			default:
				printf(usage, basename(argv[0]));
				exit(1);
		}
	}

	while ((c = getopt_long(argc, argv, shortoptions, options, NULL)) != -1) {
		switch (c) {
			case 'p':
			{
				int i;
				for (i=0; i<privilege_count; i++) {
					privilege_list = g_list_append(privilege_list, optarg);
				}
			}
			break;
		}
	}

	char* error_message = NULL;

	privilege_manager_package_type_e package_type = __get_package_type_value(package_type_name);
	privilege_manager_visibility_e visibility = __get_visibility_value(visibility_name);

	privilege_manager_verify_privilege(api_version, package_type, privilege_list, visibility, &error_message);

	return 0;
}
