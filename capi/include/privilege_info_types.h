/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
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

#ifndef __TIZEN_SECURITY_PRIVILEGE_INFO_TYPES_H
#define __TIZEN_SECURITY_PRIVIELGE_INFO_TYPES_H

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _privilegeGroup
{
	_PRV_GROUP_1,
	_PRV_GROUP_2,
	_PRV_GROUP_3,
	MAX_PRV_GROUP,
} privilegeGroup;

#define PRIVILEGE_NUM 83
#define EXTRA_GROUP MAX_PRV_GROUP - 1

typedef struct
{
	const char* privilege;
	const char* name_string_id;
	const char* description_string_id;
	privilegeGroup privilege_group_enum;

} privilege_info_s;

typedef struct
{
	const char* privilege_group;
	privilegeGroup privilege_group_enum;
	const char* name_string_id;

} privilege_group_info_s;

typedef struct
{
	int privilege_group;
	privilege_info_privileges_cb callback;
	void* user_data;
} privilege_list_cb_data;

const static privilege_group_info_s privilege_group_info_table[MAX_PRV_GROUP] =
{
	{"http://tizen.org/privilege/group1", _PRV_GROUP_1, "IDS_PRIVILEGE_GROUP_NAME1"},
	{"http://tizen.org/privilege/group2", _PRV_GROUP_2, "IDS_PRIVILEGE_GROUP_NAME2"},
	{"http://tizen.org/privilege/group3", _PRV_GROUP_3, "IDS_PRIVILEGE_GROUP_NAME3"},
};

const static privilege_info_s privilege_info_table[PRIVILEGE_NUM] =
{
	{"http://tizen.org/privilege/alarm", "IDS_PRIVILEGE_NAME_ALARM", "IDS_PRIVILEGE_DESC_ALARM", _PRV_GROUP_1},
	{"http://tizen.org/privilege/application.launch", "IDS_PRIVILEGE_NAME_APPLICATION_LAUNCH", "IDS_PRIVILEGE_DESC_APPLICATION_LAUNCH", _PRV_GROUP_1},
	{"http://tizen.org/privilege/application.kill", "IDS_PRIVILEGE_NAME_APPLICATION_KILL", "IDS_PRIVILEGE_DESC_APPLICATION_KILL", _PRV_GROUP_1},
	{"http://tizen.org/privilege/appmanager.launch", "IDS_PRIVILEGE_NAME_APPMANAGER_LAUNCH", "IDS_PRIVILEGE_DESC_APPMANAGER_LAUNCH", _PRV_GROUP_1},
	{"http://tizen.org/privilege/appsetting", "IDS_PRIVILEGE_NAME_APPSETTING", "IDS_PRIVILEGE_DESC_APPSETTING", _PRV_GROUP_1},
	{"http://tizen.org/privilege/appusage", "IDS_PRIVILEGE_NAME_APPUSAGE", "IDS_PRIVILEGE_DESC_APPUSAGE", _PRV_GROUP_1},
	{"http://tizen.org/privilege/audiomanager.route", "IDS_PRIVILEGE_NAME_AUDIOMANAGER_ROUTE", "IDS_PRIVILEGE_DESC_AUDIOMANAGER_ROUTE", _PRV_GROUP_1},
	{"http://tizen.org/privilege/audiomanager.session", "IDS_PRIVILEGE_NAME_AUDIOMANAGER_SESSION", "IDS_PRIVILEGE_DESC_AUDIOMANAGER_SESSION", _PRV_GROUP_1},
	{"http://tizen.org/privilege/audiomanager.voipsession", "IDS_PRIVILEGE_NAME_AUDIOMANAGER_VOIPSESSION", "IDS_PRIVILEGE_DESC_AUDIOMANAGER_VOIPSESSION", _PRV_GROUP_1},
	{"http://tizen.org/privilege/audiorecorder", "IDS_PRIVILEGE_NAME_AUDIORECORDER", "IDS_PRIVILEGE_DESC_AUDIORECORDER", _PRV_GROUP_1},
	{"http://tizen.org/privilege/bluetooth.admin", "IDS_PRIVILEGE_NAME_BLUETOOTH_ADMIN", "IDS_PRIVILEGE_DESC_BLUETOOTH_ADMIN", _PRV_GROUP_1},
	{"http://tizen.org/privilege/bluetooth.gap", "IDS_PRIVILEGE_NAME_BLUETOOTH_GAP", "IDS_PRIVILEGE_DESC_BLUETOOTH_GAP", _PRV_GROUP_1},
	{"http://tizen.org/privilege/bluetooth.health", "IDS_PRIVILEGE_NAME_BLUETOOTH_HEALTH", "IDS_PRIVILEGE_DESC_BLUETOOTH_HEALTH", _PRV_GROUP_1},
	{"http://tizen.org/privilege/bluetooth.opp", "IDS_PRIVILEGE_NAME_BLUETOOTH_OPP", "IDS_PRIVILEGE_DESC_BLUETOOTH_OPP", _PRV_GROUP_1},
	{"http://tizen.org/privilege/bluetooth.spp", "IDS_PRIVILEGE_NAME_BLUETOOTH_SPP", "IDS_PRIVILEGE_DESC_BLUETOOTH_SPP", _PRV_GROUP_1},
	{"http://tizen.org/privilege/bluetoothmanager", "IDS_PRIVILEGE_NAME_BLUETOOTHMANAGER", "IDS_PRIVILEGE_DESC_BLUETOOTHMANAGER", _PRV_GROUP_1},
	{"http://tizen.org/privilege/calendar.read", "IDS_PRIVILEGE_NAME_CALENDAR_READ", "IDS_PRIVILEGE_DESC_CALENDAR_READ", _PRV_GROUP_1},
	{"http://tizen.org/privilege/calendar.write", "IDS_PRIVILEGE_NAME_CALENDAR_WRITE", "IDS_PRIVILEGE_DESC_CALENDAR_WRITE", _PRV_GROUP_1},
	{"http://tizen.org/privilege/callforward", "IDS_PRIVILEGE_NAME_CALLFORWARD", "IDS_PRIVILEGE_DESC_CALLFORWARD", _PRV_GROUP_1},
	{"http://tizen.org/privilege/callhistory.read", "IDS_PRIVILEGE_NAME_CALLHISTORY_READ", "IDS_PRIVILEGE_DESC_CALLHISTORY_READ", _PRV_GROUP_1},
	{"http://tizen.org/privilege/callhistory.write", "IDS_PRIVILEGE_NAME_CALLHISTORY_WRITE", "IDS_PRIVILEGE_DESC_CALLHISTORY_WRITE", _PRV_GROUP_1},
	{"http://tizen.org/privilege/camera", "IDS_PRIVILEGE_NAME_CAMERA", "IDS_PRIVILEGE_DESC_CAMERA", _PRV_GROUP_1},
	{"http://tizen.org/privilege/cellbroadcast", "IDS_PRIVILEGE_NAME_CELLBROADCAST", "IDS_PRIVILEGE_DESC_CELLBROADCAST", _PRV_GROUP_1},
	{"http://tizen.org/privilege/certificate.read", "IDS_PRIVILEGE_NAME_CERTIFICATE_READ", "IDS_PRIVILEGE_DESC_CERTIFICATE_READ", _PRV_GROUP_1},
	{"http://tizen.org/privilege/certificate.write", "IDS_PRIVILEGE_NAME_CERTIFICATE_WRITE", "IDS_PRIVILEGE_DESC_CERTIFICATE_WRITE", _PRV_GROUP_1},
	{"http://tizen.org/privilege/contact.read", "IDS_PRIVILEGE_NAME_CONTACT_READ", "IDS_PRIVILEGE_DESC_CONTACT_READ", _PRV_GROUP_1},
	{"http://tizen.org/privilege/contact.write", "IDS_PRIVILEGE_NAME_CONTACT_WRITE", "IDS_PRIVILEGE_DESC_CONTACT_WRITE", _PRV_GROUP_1},
	{"http://tizen.org/privilege/content.read", "IDS_PRIVILEGE_NAME_CONTENT_READ", "IDS_PRIVILEGE_DESC_CONTENT_READ", _PRV_GROUP_1},
	{"http://tizen.org/privilege/content.write", "IDS_PRIVILEGE_NAME_CONTENT_WRITE", "IDS_PRIVILEGE_DESC_CONTENT_WRITE", _PRV_GROUP_1},
	{"http://tizen.org/privilege/customnetaccount", "IDS_PRIVILEGE_NAME_CUSTOMNETACCOUNT", "IDS_PRIVILEGE_DESC_CUSTOMNETACCOUNT", _PRV_GROUP_1},
	{"http://tizen.org/privilege/dns", "IDS_PRIVILEGE_NAME_DNS", "IDS_PRIVILEGE_DESC_DNS", _PRV_GROUP_1},
	{"http://tizen.org/privilege/download", "IDS_PRIVILEGE_NAME_DOWNLOAD", "IDS_PRIVILEGE_DESC_DOWNLOAD", _PRV_GROUP_1},
	{"http://tizen.org/privilege/http", "IDS_PRIVILEGE_NAME_HTTP", "IDS_PRIVILEGE_DESC_HTTP", _PRV_GROUP_1},
	{"http://tizen.org/privilege/ime", "IDS_PRIVILEGE_NAME_IME", "IDS_PRIVILEGE_DESC_IME", _PRV_GROUP_1},
	{"http://tizen.org/privilege/inputmanager", "IDS_PRIVILEGE_NAME_INPUTMANAGER", "IDS_PRIVILEGE_DESC_INPUTMANAGER", _PRV_GROUP_1},
	{"http://tizen.org/privilege/location", "IDS_PRIVILEGE_NAME_LOCATION", "IDS_PRIVILEGE_DESC_LOCATION", _PRV_GROUP_1},
	{"http://tizen.org/privilege/messaging.email", "IDS_PRIVILEGE_NAME_MESSAGING_EMAIL", "IDS_PRIVILEGE_DESC_MESSAGING_EMAIL", _PRV_GROUP_1},
	{"http://tizen.org/privilege/messaging.mms", "IDS_PRIVILEGE_NAME_MESSAGING_MMS", "IDS_PRIVILEGE_DESC_MESSAGING_MMS", _PRV_GROUP_1},
	{"http://tizen.org/privilege/messaging.sms", "IDS_PRIVILEGE_NAME_MESSAGING_SMS", "IDS_PRIVILEGE_DESC_MESSAGING_SMS", _PRV_GROUP_1},
	{"http://tizen.org/privilege/network.account", "IDS_PRIVILEGE_NAME_NETWORK_ACCOUNT", "IDS_PRIVILEGE_DESC_NETWORK_ACCOUNT", _PRV_GROUP_1},
	{"http://tizen.org/privilege/network.connection", "IDS_PRIVILEGE_NAME_NETWORK_CONNECTION", "IDS_PRIVILEGE_DESC_NETWORK_CONNECTION", _PRV_GROUP_1},
	{"http://tizen.org/privilege/network.statistics.read", "IDS_PRIVILEGE_NAME_NETWORK_STATISTICS_READ", "IDS_PRIVILEGE_DESC_NETWORK_STATISTICS_READ", _PRV_GROUP_1},
	{"http://tizen.org/privilege/network.statistics.write", "IDS_PRIVILEGE_NAME_NETWORK_STATISTICS_WRITE", "IDS_PRIVILEGE_DESC_NETWORK_STATISTICS_WRITE", _PRV_GROUP_1},
	{"http://tizen.org/privilege/networkmanager", "IDS_PRIVILEGE_NAME_NETWORKMANAGER", "IDS_PRIVILEGE_DESC_NETWORKMANAGER", _PRV_GROUP_1},
	{"http://tizen.org/privilege/nfc.admin", "IDS_PRIVILEGE_NAME_NFC_ADMIN", "IDS_PRIVILEGE_DESC_NFC_ADMIN", _PRV_GROUP_1},
	{"http://tizen.org/privilege/nfc.cardemulation", "IDS_PRIVILEGE_NAME_NFC_CARDEMULATION", "IDS_PRIVILEGE_DESC_NFC_CARDEMULATION", _PRV_GROUP_1},
	{"http://tizen.org/privilege/nfc.common", "IDS_PRIVILEGE_NAME_NFC_COMMON", "IDS_PRIVILEGE_DESC_NFC_COMMON", _PRV_GROUP_1},
	{"http://tizen.org/privilege/nfc.p2p", "IDS_PRIVILEGE_NAME_NFC_P2P", "IDS_PRIVILEGE_DESC_NFC_P2P", _PRV_GROUP_1},
	{"http://tizen.org/privilege/nfc.tag", "IDS_PRIVILEGE_NAME_NFC_TAG", "IDS_PRIVILEGE_DESC_NFC_TAG", _PRV_GROUP_1},
	{"http://tizen.org/privilege/notification", "IDS_PRIVILEGE_NAME_NOTIFICATION", "IDS_PRIVILEGE_DESC_NOTIFICATION", _PRV_GROUP_1},
	{"http://tizen.org/privilege/notificationmanager", "IDS_PRIVILEGE_NAME_NOTIFICATIONMANAGER", "IDS_PRIVILEGE_DESC_NOTIFICATIONMANAGER", _PRV_GROUP_1},
	{"http://tizen.org/privilege/packageinfo", "IDS_PRIVILEGE_NAME_PACKAGEINFO", "IDS_PRIVILEGE_DESC_PACKAGEINFO", _PRV_GROUP_1},
	{"http://tizen.org/privilege/packagemanager.install", "IDS_PRIVILEGE_NAME_PACKAGEMANAGER_INSTALL", "IDS_PRIVILEGE_DESC_PACKAGEMANAGER_INSTALL", _PRV_GROUP_1},
	{"http://tizen.org/privilege/packagesetting", "IDS_PRIVILEGE_NAME_PACKAGESETTING", "IDS_PRIVILEGE_DESC_PACKAGESETTING", _PRV_GROUP_1},
	{"http://tizen.org/privilege/power", "IDS_PRIVILEGE_NAME_POWER", "IDS_PRIVILEGE_DESC_POWER", _PRV_GROUP_1},
	{"http://tizen.org/privilege/push", "IDS_PRIVILEGE_NAME_PUSH", "IDS_PRIVILEGE_DESC_PUSH", _PRV_GROUP_1},
	{"http://tizen.org/privilege/setting", "IDS_PRIVILEGE_NAME_SETTING", "IDS_PRIVILEGE_DESC_SETTING", _PRV_GROUP_1},
	{"http://tizen.org/privilege/smstrigger", "IDS_PRIVILEGE_NAME_SMSTRIGGER", "IDS_PRIVILEGE_DESC_SMSTRIGGER", _PRV_GROUP_1},
	{"http://tizen.org/privilege/socket", "IDS_PRIVILEGE_NAME_SOCKET", "IDS_PRIVILEGE_DESC_SOCKET", _PRV_GROUP_1},
	{"http://tizen.org/privilege/systeminfo", "IDS_PRIVILEGE_NAME_SYSTEMINFO", "IDS_PRIVILEGE_DESC_SYSTEMINFO", _PRV_GROUP_1},
	{"http://tizen.org/privilege/systemsetting.read", "IDS_PRIVILEGE_NAME_SYSTEMSETTING_READ", "IDS_PRIVILEGE_DESC_SYSTEMSETTING_READ", _PRV_GROUP_1},
	{"http://tizen.org/privilege/systemsetting.write", "IDS_PRIVILEGE_NAME_SYSTEMSETTING_WRITE", "IDS_PRIVILEGE_DESC_SYSTEMSETTING_WRITE", _PRV_GROUP_1},
	{"http://tizen.org/privilege/uimanager", "IDS_PRIVILEGE_NAME_UIMANAGER", "IDS_PRIVILEGE_DESC_UIMANAGER", _PRV_GROUP_1},
	{"http://tizen.org/privilege/useridentity", "IDS_PRIVILEGE_NAME_USERIDENTITY", "IDS_PRIVILEGE_DESC_USERIDENTITY", _PRV_GROUP_1},
	{"http://tizen.org/privilege/vibrator", "IDS_PRIVILEGE_NAME_VIBRATOR", "IDS_PRIVILEGE_DESC_VIBRATOR", _PRV_GROUP_1},
	{"http://tizen.org/privilege/videorecorder", "IDS_PRIVILEGE_NAME_VIDEORECORDER", "IDS_PRIVILEGE_DESC_VIDEORECORDER", _PRV_GROUP_1},
	{"http://tizen.org/privilege/wappush", "IDS_PRIVILEGE_NAME_WAPPUSH", "IDS_PRIVILEGE_DESC_WAPPUSH", _PRV_GROUP_1},
	{"http://tizen.org/privilege/web.privacy", "IDS_PRIVILEGE_NAME_WEB_PRIVACY", "IDS_PRIVILEGE_DESC_WEB_PRIVACY", _PRV_GROUP_1},
	{"http://tizen.org/privilege/web.service", "IDS_PRIVILEGE_NAME_WEB_SERVICE", "IDS_PRIVILEGE_DESC_WEB_SERVICE", _PRV_GROUP_1},
	{"http://tizen.org/privilege/wifi.admin", "IDS_PRIVILEGE_NAME_WIFI_ADMIN", "IDS_PRIVILEGE_DESC_WIFI_ADMIN", _PRV_GROUP_1},
	{"http://tizen.org/privilege/wifi.read", "IDS_PRIVILEGE_NAME_WIFI_READ", "IDS_PRIVILEGE_DESC_WIFI_READ", _PRV_GROUP_1},
	{"http://tizen.org/privilege/wifi.wifidirect.admin", "IDS_PRIVILEGE_NAME_WIFI_WIFIDIRECT_ADMIN", "IDS_PRIVILEGE_DESC_WIFI_WIFIDIRECT_ADMIN", _PRV_GROUP_1},
	{"http://tizen.org/privilege/wifi.wifidirect.read", "IDS_PRIVILEGE_NAME_WIFI_WIFIDIRECT_READ", "IDS_PRIVILEGE_DESC_WIFI_WIFIDIRECT_READ", _PRV_GROUP_1},
	{"http://tizen.org/privilege/drmservice", "IDS_PRIVILEGE_NAME_DRMSERVICE", "IDS_PRIVILEGE_DESC_DRMSERVICE", _PRV_GROUP_1},
	{"http://tizen.org/privilege/imemanager", "IDS_PRIVILEGE_NAME_IMEMANAGER", "IDS_PRIVILEGE_DESC_IMEMANAGER", _PRV_GROUP_1},
	{"http://tizen.org/privilege/telephonymanager", "IDS_PRIVILEGE_NAME_TELEPHONYMANAGER", "IDS_PRIVILEGE_DESC_TELEPHONYMANAGER", _PRV_GROUP_1},
	{"http://tizen.org/privilege/platforminfo", "IDS_PRIVILEGE_NAME_PLATFORMINFO", "IDS_PRIVILEGE_DESC_PLATFORMINFO", _PRV_GROUP_1},
	{"http://tizen.org/privilege/packagelicensemanager", "IDS_PRIVILEGE_NAME_PACKAGELICENSEMANAGER", "IDS_PRIVILEGE_DESC_PACKAGELICENSEMANAGER", _PRV_GROUP_1},
	{"http://tizen.org/privilege/wifimanager", "IDS_PRIVILEGE_NAME_WIFIMANAGER", "IDS_PRIVILEGE_DESC_WIFIMANAGER", _PRV_GROUP_1},
	{"http://tizen.org/privilege/account.read", "IDS_PRIVILEGE_NAME_ACCOUNT_READ", "IDS_PRIVILEGE_DESC_ACCOUNT_READ", _PRV_GROUP_1},
	{"http://tizen.org/privilege/account.write", "IDS_PRIVILEGE_NAME_ACCOUNT_WRITE", "IDS_PRIVILEGE_DESC_ACCOUNT_WRITE", _PRV_GROUP_1},
	{"http://tizen.org/privilege/userprofile.read", "IDS_PRIVILEGE_NAME_USERPROFILE_READ", "IDS_PRIVILEGE_DESC_USERPROFILE_READ", _PRV_GROUP_1},
	{"http://tizen.org/privilege/userprofile.write", "IDS_PRIVILEGE_NAME_USERPROFILE_WRITE", "IDS_PRIVILEGE_DESC_USERPROFILE_WRITE", _PRV_GROUP_1},
};

#ifdef __cplusplus
}
#endif

#endif // __TIZEN_SECURITY_PRIVACY_INFO_TYPES_H
