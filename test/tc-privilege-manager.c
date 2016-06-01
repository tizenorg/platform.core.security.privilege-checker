#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <glib.h>
#include <privilege_manager.h>
#include "tc-common.h"

void __privilege_manager_get_mapped_privilege_list()
{
	int ret = 0;
	GList* mapped_privilege_list = NULL;
	ret = privilege_manager_get_mapped_privilege_list(api_version, pkg_type, privilege_list, &mapped_privilege_list);
	/* TBD : Add function to compare mapped list and correct list
	if (mapped_privilege_list != NULL)
		__check_mapping_n_count_result(ret, 1, mapped_privilege_list); */
	__print_result('m', ret);
	__print_line();

	gfree(privilege_list);
	gfree(mapped_privilege_list);
}

static void __test_privilege_manager_get_mapped_privilege_list()
{
	int ret = 0;
	GList *mapped_privilege_list;

	/* Test for invalid input parameter */
	__tcinfo(version_type, NULL, "core");
	__tcinfo(goal, "!!! WRONG PARAM !!! api_version = NULL");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_INVALID_PARAMETER);
	__privilege_manager_get_mapped_privilege_list();

	__tcinfo(version_type, "2.3.4.5.1", "core");
	__tcinfo(goal, "!!! WRONG PARAM !!! api_version == 2.3.4.5.1");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_INVALID_PARAMETER);
	__privilege_manager_get_mapped_privilege_list();

	__tcinfo(version_type, "2.3", "core");
	__tcinfo(goal, "api_version == 2.3, wrong privilege => returned mapping privilege list should be null");
	__privinfo("http://tizen.org/privilege/wrong-privilege-name", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list(api_version, pkg_type, privilege_list, &mapped_privilege_list);
	if (mapped_privilege_list == NULL) {
		printf("it's null\n");
		__print_result('m', ret);
	} else {
		printf("it's NOT null\n");
		ret = -1;
		__print_result('m', ret);
		__print_privilege_list(mapped_privilege_list);
		gfree(mapped_privilege_list);
	}
	__print_line();

#ifdef PROFILE_TYPE_MOBILE

	/* 2.3 core - mobile */
	__tcinfo(version_type, "2.3", "core");

	__tcinfo(goal, "TC1");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/camera", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, &mapped_privilege_list);
	__print_privilege_list(mapped_privilege_list);
	__print_line();
	gfree(privilege_list);

	__tcinfo(goal, "TC2");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/camera", NULL, NULL);
	__privinfo("http://tizen.org/privilege/vpnservice", NULL, "Valid from 3.0 so it should be ignored");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, &mapped_privilege_list);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	gfree(privilege_list);

	/* 2.4 core - mobile */
	__tcinfo(version_type, "2.4", "core");

	/* 3.0 core - mobile */
	__tcinfo(version_type, "3.0", "core");

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/message.read", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list("3.0", PRVMGR_PACKAGE_TYPE_CORE, privilege_list, &mapped_privilege_list);
	__print_result('m', ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	gfree(privilege_list);

	/* 2.2.1 wrt - mobile */
	__tcinfo(version_type, "2.2.1", "wrt");

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.write", NULL, NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list(api_version, PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result('m', ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	gfree(privilege_list);

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/platform", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.write", NULL, NULL);
	__privinfo("http://tizen.org/privilege/datasync", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list(api_version, PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result('m', ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	gfree(privilege_list);

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/content.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/nfc.p2p", NULL, NULL);
	__privinfo("http://tizen.org/privilege/call", NULL, NULL);
	__privinfo("http://tizen.org/privilege/internet", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list("2.2.1", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result('m', ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	gfree(privilege_list);

	/* 2.3 wrt - mobile */
	__tcinfo(version_type, "2.3", "wrt");

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.write", NULL, NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result('m', ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	gfree(privilege_list);

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/platform", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.write", NULL, NULL);
	__privinfo("http://tizen.org/privilege/datasync", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result('m', ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	gfree(privilege_list);

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, NULL);
	__privinfo("http://tizen.org/privilege/content.read", NULL, NULL);
	__privinfo("http://tizen.org/privilege/nfc.p2p", NULL, NULL);
	__privinfo("http://tizen.org/privilege/call", NULL, NULL);
	__privinfo("http://tizen.org/privilege/internet", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result('m', ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	gfree(privilege_list);

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/location", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result('m', ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	gfree(privilege_list);

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/platform", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list("2.3", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result('m', ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	gfree(privilege_list);

	/* 2.4 wrt - mobile */
	__tcinfo(version_type, "2.4", "wrt");

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/location", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result('m', ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	gfree(privilege_list);

	__tcinfo(goal, "normal mapping");
	__privinfo("http://tizen.org/privilege/internal/default/platform", NULL, NULL);
	__tcinfo(expect, PRVMGR_ERR_NONE);
	ret = privilege_manager_get_mapped_privilege_list("2.4", PRVMGR_PACKAGE_TYPE_WRT, privilege_list, &mapped_privilege_list);
	__print_result('m', ret);
	__print_privilege_list(mapped_privilege_list);
	__print_line();

	gfree(privilege_list);

#endif
}

void __privilege_manager_verify_privilege()
{
	char* error_message = NULL;
	int ret = 0;
	ret = privilege_manager_verify_privilege(api_version, pkg_type, privilege_list, visibility, &error_message);
	__print_result('m', ret);
	ShowErrorMsg(error_message != NULL, error_message, ret);
	__print_line();

	gfree(privilege_list);
}

static void __test_privilege_manager_verify_privilege()
{
	__tcinfo(version_type, NULL, "core");
	__tcinfo(goal, "api_version == NULL");
	__privinfo("http://tizen.org/privilege/account.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/account.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/alarm.get", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings", "Public", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PARAMETER);
	__privilege_manager_verify_privilege();

	__tcinfo(version_type, "2.3.4.2.1", "core");
	__tcinfo(goal, "invalid api_version == 2.3.4.2.1");
	__privinfo("http://tizen.org/privilege/account.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/account.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/alarm.get", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings", "Public", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PARAMETER);
	__privilege_manager_verify_privilege();

	__tcinfo(version_type, "2.", "core");
	__tcinfo(goal, "invalid api_version == 2.");
	__privinfo("http://tizen.org/privilege/account.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/account.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/alarm.get", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings", "Public", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PARAMETER);
	__privilege_manager_verify_privilege();

	__tcinfo(version_type, "a.b", "core");
	__tcinfo(goal, "invalid api_version == a.b");
	__privinfo("http://tizen.org/privilege/account.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/account.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/alarm.get", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings", "Public", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PARAMETER);
	__privilege_manager_verify_privilege();

#ifdef PROFILE_TYPE_MOBILE

	/* 2.3 core - mobile */
	__tcinfo(version_type, "2.3", "core");

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/account.readd", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/tizen/privilege/internet", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/healthinfo", NULL, "Valid from 2.3.1");
	__privinfo("http://tizen.org/privilege/keygrab", NULL, "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

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
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 2.3.1 core - mobile */
	__tcinfo(version_type, "2.3.1", "core");

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/account.readd", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/tizen/privilege/internet", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/ime", NULL, "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/keygrab", NULL, "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

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
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemsettings", NULL, "Deprecated from 2.3.1");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 2.4 core - mobile */
	__tcinfo(version_type, "2.4", "core");

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
	__privinfo("http://tizen.org/privilege/minicontrol.provider", "Public", "Valid from 2.4 and deprecated from 3.0");
	__privinfo("http://tizen.org/privilege/mediahistory.read", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/packagemanager.clearcache", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/systemmonitor", "Public", "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform");
	__privinfo("http://tizen.org/privilege/web-history.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/reboot", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/inputgenerator", "Platform", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/keygrab", "Platform", "Valid from 2.4");
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/account.readd", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/tizen/privilege/internet", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!!");
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/web-history.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/reboot", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/inputgenerator", "Platform", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/keygrab", "Platform", "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemsettings", NULL, "Deprecated from 2.3.1");
	__privinfo("http://tizen.org/privilege/keymanager.admin", NULL, "Deprecated from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 3.0 core - mobile */
	__tcinfo(version_type, "3.0", "core");

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Public");
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/healthinfo", "Public", NULL);
	__privinfo("http://tizen.org/privilege/widget.viewer", "Public", NULL);
	__privinfo("http://tizen.org/privilege/secureelement", "Public", NULL);
	__privinfo("http://tizen.org/privilege/apphistory.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill.bgapp", "Public", NULL);
	__privinfo("http://tizen.org/privilege/ime", "Public", NULL);
	__privinfo("http://tizen.org/privilege/imemanager", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mapservice", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacontroller.client", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacontroller.server", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediahistory.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.clearcache", "Public", NULL);
	__privinfo("http://tizen.org/privilege/systemmonitor", "Public", NULL);
	__privinfo("http://tizen.org/privilege/vpnservice", "Public", "Valid from 3.0, Mobile profile only");
	__privinfo("http://tizen.org/privilege/d2d.datasharing", "Public", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/location.coarse", "Public", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/use_ir", "Public", "Valid from 3.0");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform");
	__privinfo("http://tizen.org/privilege/web-history.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/reboot", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/inputgenerator", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/keygrab", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/vpnservice.admin", "Platform", "Valid from 3.0, Mobile profile only");
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/account.readd", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/tizen/privilege/internet", NULL, NULL);
	__privinfo("http://tizen.org/privilege/XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/internal/default/public", NULL, "Internal Privilege");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!!");
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/web-history.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/reboot", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/inputgenerator", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/keygrab", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/vpnservice.admin", "Platform", "Mobile profile only");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemsettings", NULL, "Deprecated from 2.3.1");
	__privinfo("http://tizen.org/privilege/keymanager.admin", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/keymanager", NULL, "Deprecated from 3.0");
	__privinfo("http://tizen.org/privilege/minicontrol.provider", NULL, "Deprecated from 3.0");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 2.2.1 Web - mobile */
	__tcinfo(version_type, "2.2.1", "wrt");

	__tcinfo(goal, "!!! NAME NOT EXIST !!! -- Under 2.3.1, Undefined privileges on web apps are ignored");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with public signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with partner signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 2.3 Web - mobile */
	__tcinfo(version_type, "2.3", "wrt");

	__tcinfo(goal, "!!! NAME NOT EXIST !!! -- Under 2.3.1, Undefined privileges on web apps are ignored");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with public signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with partner signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 2.3.1 Web - mobile */
	__tcinfo(version_type, "2.3.1", "wrt");

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__privinfo("http://tizen.org/privilege/ime", NULL, "Valid from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/calendar", NULL, "Deprecated from 2.0");
	__privinfo("http://tizen.org/privilege/mediacontent", NULL, "Deprecated from 2.0");
	__privinfo("http://tizen.org/privilege/systemmanager", NULL, "Deprecated from 2.3.1");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with public signatured certificate");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with partner signatured certificate");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 2.4 Web - mobile */
	__tcinfo(version_type, "2.4", "wrt");

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemmanager", NULL, "Deprecated from 2.3.1");
	__privinfo("http://tizen.org/privilege/bluetooth.admin", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/bluetooth.gap", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/bluetooth.spp", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/bluetooth.health", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/websetting", NULL, "Deprecated from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with public signatured certificate");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with partner signatured certificate");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 3.0 Web - mobile */
	__tcinfo(version_type, "3.0", "wrt");

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
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", NULL);
	__privinfo("http://tizen.org/privilege/ime", "Public", NULL);
	__privinfo("http://tizen.org/privilege/led", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacontroller.client", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mediacontroller.server", "Public", NULL);
	__privinfo("http://tizen.org/privilege/d2d.datasharing", "Public", "Valid from 3.0");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemmanager", NULL, "Deprecated from 2.3.1");
	__privinfo("http://tizen.org/privilege/bluetooth.admin", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/bluetooth.gap", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/bluetooth.spp", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/bluetooth.health", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/websetting", NULL, "Deprecated from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with public signatured certificate");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with partner signatured certificate");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* Web - mobile => Test for duplicated privilege history */

	__tcinfo(goal, "bluetooth exist at 1.0 and 2.4");

	__tcinfo(version_type, "1.0", "wrt");
	__privinfo("http://tizen.org/privilege/bluetooth", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(version_type, "2.3.1", "wrt");
	__privinfo("http://tizen.org/privilege/bluetooth", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(version_type, "2.4", "wrt");
	__privinfo("http://tizen.org/privilege/bluetooth", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

#endif
#ifdef PROFILE_TYPE_WEARABLE

	/* 2.3.1 core - wearable */
	__tcinfo(version_type, "2.3.1", "core");

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform");
	__privinfo("http://tizen.org/privilege/bluetooth.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/keymanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.read", NULL, "Mobile profile only until 3.0");
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", "Mobile profile only");
	__privinfo("http://tizen.org/privilege/calendar.write", "Public", "Mobile profile only");
	__privinfo("http://tizen.org/privilege/ime", NULL, "Valid from 3.0");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	gfree(privilege_list);
	__tcinfo(goal, "!!! LEVEL MISMATCHED !!!");
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/keymanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/reboot", "Platform", "Valid from 2.3.1");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 3.0 core - wearable */
	__tcinfo(version_type, "3.0", "core");

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Public");
	__privinfo("http://tizen.org/privilege/account.read", "Public", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/account.write", "Public", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/alarm.get", "Public", NULL);
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/callhistory.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/callhistory.write", "Public", NULL);
	__privinfo("http://tizen.org/privilege/contact.read", "Public", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/display", "Public", NULL);
	__privinfo("http://tizen.org/privilege/download", "Public", NULL);
	__privinfo("http://tizen.org/privilege/display", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__privinfo("http://tizen.org/privilege/mapservice", "Public", "Valid from 2.4");
	__privinfo("http://tizen.org/privilege/mediacontroller.client", "Public", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/nfc", "Public", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.clearcache", "Public", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/telephony", "Public", NULL);
	__privinfo("http://tizen.org/privilege/widget.viewer", "Public", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform");
	__privinfo("http://tizen.org/privilege/bluetooth.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/keygrab", "Platform", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/systemsettings.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__privinfo("http://tizen.org/privilege/calendar.read", "Public", "Mobile profile only");
	__privinfo("http://tizen.org/privilege/calendar.write", "Public", "Mobile profile only");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!!");
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/systemsettings.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/nfc.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/telephony.admin", "Platform", NULL);
	__privinfo("http://tizen.org/privielge/keygrab", "Platform", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/reboot", "Platform", "Valid from 2.3.1");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/keymanager.admin", "Platform", "Deprecated from 3.0");
	__privinfo("http://tizen.org/privilege/keymanager", "Public", "Deprecated from 3.0");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 2.3 Web - wearable */
	__tcinfo(version_type, "2.3", "wrt");

	__tcinfo(goal, "!!! NAME NOT EXIST !!! -- Under 2.3.1, Undefined privileges on web apps are ignored");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with public signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with partner signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 2.3.1 Web - wearable */
	__tcinfo(version_type, "2.3.1", "wrt");

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Partner ");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/content.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.read", NULL, "Mobile profile only");
	__privinfo("http://tizen.org/privilege/ime", NULL, "Valid from 3.0");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemmanager", NULL, "Deprecated from 2.3.1");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with public signatured certificate");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with partner signatured certificate");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 3.0 Web - wearable */
	__tcinfo(version_type, "3.0", "wrt");

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
	__privinfo("http://tizen.org/privilege/bluetooth", "Public", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/ime", "Public", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/led", "Public", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/mediacontroller.client", "Public", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/mediacontroller.server", "Public", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/d2d.datasharing", "Public", "Valid from 3.0");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Partner ");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/call", "Public", NULL);
	__privinfo("http://tizen.org/privilege/content.read", "Public", NULL);
	__privinfo("http://tizen.org/privilege/internet", "Public", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! NAME NOT EXIST !!!");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__privinfo("http://tizen.org/privilege/contact.read", NULL, "Mobile profile only");
	__privinfo("http://tizen.org/privilege/vpnservice", NULL, "Mobile profile only");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	gfree(privilege_list);
	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemmanager", NULL, "Deprecated from 2.3.1");
	__privinfo("http://tizen.org/privilege/bluetooth.gap", NULL, "Deprecated from 3.0");
	__privinfo("http://tizen.org/privilege/bluetooth.spp", NULL, "Deprecated from 3.0");
	__privinfo("http://tizen.org/privilege/bluetooth.health", NULL, "Deprecated from 3.0");
	__privinfo("http://tizen.org/privilege/bluetooth.admin", NULL, "Deprecated from 3.0");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with public signatured certificate");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with partner signatured certificate");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();
#endif
#ifdef PROFILE_TYPE_TV
	/* 2.3 Web - TV */

	__tcinfo(version_type, "2.3", "wrt");

	__tcinfo(goal, "!!! NAME NOT EXIST !!! -- Under 2.3.1, Undefined privileges on web apps are ignored");
	__privinfo("http://tizen.org/privilege/XXXXXXXX", NULL, NULL);
	__privinfo("http://tizen.org/privilege/account.writeeeeee", NULL, NULL);
	__privinfo("http://tizen.org/privilege/alarm.gettttttttt", NULL, NULL);
	__privinfo("http://tizen.org/privilege/bluetoothsdfwfvwfdwgwgwgeegw", NULL, NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with public signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/networkbearerselection", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/systemmanager", "Partner", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED -- with partner signatured certificate!!!");
	__privinfo("http://tizen.org/privilege/bluetoothmanager", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/bookmark.read", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 2.4 Web - TV */

	__tcinfo(version_type, "2.4", "wrt");

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
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Partner ");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform ");
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemmanager", NULL, "Deprecated from 2.3.1");
	__privinfo("http://tizen.org/privilege/websetting", NULL, "Deprecated from 2.4");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with public signatured certificate");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with partner signatured certificate");
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	/* 3.0 Web - TV */

	__tcinfo(version_type, "3.0", "wrt");

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
	__privinfo("http://tizen.org/privilege/d2d.datasharing", "Public", "Valid from 3.0");
	__privinfo("http://tizen.org/privilege/volume.set", "Public", "Valid from 3.0");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Partner ");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "Name=OK, Version=OK, Level=OK -- Platform ");
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__tcinfo(cert_level, "platform");
	__tcinfo(expect, PRVMGR_ERR_NONE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! DEPRECATED !!!");
	__privinfo("http://tizen.org/privilege/systemmanager", NULL, "Deprecated from 2.3.1");
	__privinfo("http://tizen.org/privilege/websetting", NULL, "Deprecated from 2.4");
	__privinfo("http://tizen.org/privilege/keymanager", NULL, "Deprecated from 3.0");
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with public signatured certificate");
	__privinfo("http://tizen.org/privilege/appmanager.certificate", "Partner", NULL);
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__privinfo("http://tizen.org/privilege/appmanager.kill", "Partner", NULL);
	__tcinfo(cert_level, "public");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

	__tcinfo(goal, "!!! LEVEL MISMATCHED !!! -- with partner signatured certificate");
	__privinfo("http://tizen.org/privilege/packagemanager.install", "Platform", NULL);
	__tcinfo(cert_level, "partner");
	__tcinfo(expect, PRVMGR_ERR_INVALID_PRIVILEGE);
	__privilege_manager_verify_privilege();

#endif
}

int main()
{
	__tcinfo(function, "privilege_manager_verify_privilege");
	__test_privilege_manager_verify_privilege();

	__tcinfo(function, "privilege_manager_get_mapped_privilege_list");
	__test_privilege_manager_get_mapped_privilege_list();

	__color_to_green();
	printf("Test Complete\n");
	printf("success : %d, ", success_cnt);
	__color_to_red();
	printf("fail : %d\n", fail_cnt);
	__color_to_origin();

	return 0;
}
