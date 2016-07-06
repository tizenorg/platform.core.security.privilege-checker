#!/bin/bash

DB_NAME=".core_privilege_info.db"

rm $DB_NAME 2> /dev/null
echo "Creating $DB_NAME ..."
touch $DB_NAME

target_profile=$1

echo "Creating PRIVILEGE_INFO table ..."
sqlite3 $DB_NAME "CREATE TABLE PRIVILEGE_INFO (PROFILE_ID NUMERIC, PROFILE TEXT, PACKAGE_TYPE_ID NUMERIC, PACKAGE_TYPE TEXT, PRIVILEGE_LEVEL_ID NUMERIC, PRIVILEGE_LEVEL TEXT, API_VERSION_ISSUED TEXT, API_VERSION_EXPIRED TEXT, DOCUMENTED INTEGER, PRIVILEGE_NAME TEXT, IS_PRIVACY NUMERIC, PRIVACY_GROUP TEXT, PRIVILEGE_DISPLAY TEXT, PRIVILEGE_DESCRIPTION TEXT, PRIVILEGE_GROUP_ID NUMERIC, PRIVLEGE_GROUP TEXT, CHANGED_TO_2_3_1 TEXT, CHANGED_TO_2_4_0 TEXT);"

echo "Create VALID_PRIVILEGE_INFO Table..."
sqlite3 $DB_NAME "CREATE TABLE VALID_PRIVILEGE_INFO (PRIVILEGE_NAME TEXT UNIQUE, IS_PRIVACY NUMERIC, IS_INTERNAL NUMERIC);"

echo "Create PRIVACY_INFO table..."
sqlite3 $DB_NAME "CREATE TABLE PRIVACY_INFO (PRIVACY_GROUP TEXT UNIQUE, PRIVACY_DISPLAY TEXT);"

echo "Inserting data ..."
IFS=$'\n'
for i in `cat core_privilege_info.csv`
do
	temp=`echo $i | awk '/^#/'`
	if [ ! "$temp" = "" ]
	then
		continue
	fi

	PROFILE=`echo $i | cut -d "," -f 1`
	#echo PROFILE = $PROFILE

	if [ "$PROFILE" = "common" ]
	then
		PROFILE_ID=0
	elif [ ! "$PROFILE" = "$target_profile" ]
	then
		continue
	elif [ "$PROFILE" = "mobile" ]
	then
		PROFILE_ID=1

	elif [ "$PROFILE" = "wearable" ]
	then
		PROFILE_ID=2
	elif [ "$PROFILE" = "tv" ]
	then
		PROFILE_ID=3
	else
		echo "Fail to create table : PROFILE must be common, mobile, wearable or tv"
		exit
	fi

	PACKAGE_TYPE=`echo $i | cut -d "," -f 2`

	if [ "$PACKAGE_TYPE" = "wrt" ]
	then
		PACKAGE_TYPE_ID=0

	elif [ "$PACKAGE_TYPE" = "core" ]
	then
		PACKAGE_TYPE_ID=1

	else
		echo "Fail to create table : PACKAGE_TYPE must be wrt or core"
		exit
	fi

	PRIVILEGE_LEVEL=`echo $i | cut -d "," -f 3`

	if [ "$PRIVILEGE_LEVEL" = "public" ]
	then
		PRIVILEGE_LEVEL_ID=0

	elif [ "$PRIVILEGE_LEVEL" = "partner" ]
	then
		PRIVILEGE_LEVEL_ID=1

	elif [ "$PRIVILEGE_LEVEL" = "platform" ]
	then
		PRIVILEGE_LEVEL_ID=2

	else
		echo "Fail to create table : PRIVILEGE_LEVEL must be public, partner or platform"
		exit
	fi

	API_VERSION_ISSUED=`echo $i | cut -d "," -f 4`
	API_VERSION_EXPIRED=`echo $i | cut -d "," -f 5`
	DOCUMENTED=`echo $i | cut -d "," -f 6`

	PRIVILEGE_NAME=`echo $i | cut -d "," -f 7`

	IS_PRIVACY_TEXT=`echo $i | cut -d "," -f 8`
	if [ "$IS_PRIVACY_TEXT" = "yes" ]
	then
		IS_PRIVACY=1
	elif [ "$IS_PRIVACY_TEXT" = "no" ]
	then
		IS_PRIVACY=0
	else
		echo "Fail to create table : IS_PRIVACY must be yes or no"
		exit
	fi

	PRIVACY_GROUP=`echo $i | cut -d "," -f 9`

	PRIVILEGE_DISPLAY=`echo $i | cut -d "," -f 11`

	PRIVILEGE_DESCRIPTION=`echo $i | cut -d "," -f 12`

	PRIVILEGE_GROUP=`echo $i | cut -d "," -f 13`

	if [ "$PRIVILEGE_GROUP" = "IDS_TPLATFORM_BODY_HARDWARE_CONTROLS_ABB" ]
	then
		PRIVILEGE_GROUP_ID=0

	elif [ "$PRIVILEGE_GROUP" = "IDS_TPLATFORM_OPT_LOCATION_T_LBS" ]
	then
		PRIVILEGE_GROUP_ID=1

	elif [ "$PRIVILEGE_GROUP" = "IDS_TPLATFORM_BODY_NETWORK_CONNECTIONS_ABB" ]
	then
		PRIVILEGE_GROUP_ID=2

	elif [ "$PRIVILEGE_GROUP" = "IDS_TPLATFORM_BODY_PAID_SERVICES_ABB" ]
	then
		PRIVILEGE_GROUP_ID=3

	elif [ "$PRIVILEGE_GROUP" = "IDS_TPLATFORM_BODY_PERSONAL_INFORMATION_ABB" ]
	then
		PRIVILEGE_GROUP_ID=4

	elif [ "$PRIVILEGE_GROUP" = "IDS_TPLATFORM_BODY_SYSTEM_SETTINGS_ABB" ]
	then
		PRIVILEGE_GROUP_ID=5

	elif [ "$PRIVILEGE_GROUP" = "IDS_TPLATFORM_BODY_OTHER_PRIVILEGES_ABB" ]
	then
		PRIVILEGE_GROUP_ID=6

	else
		echo "Fail to create table : PRIVILEGE_GROUP must be one of them below"
		echo "IDS_TPLATFORM_BODY_HARDWARE_CONTROLS_ABB"
		echo "IDS_TPLATFORM_OPT_LOCATION_T_LBS"
		echo "IDS_TPLATFORM_BODY_NETWORK_CONNECTIONS_ABB"
		echo "IDS_TPLATFORM_BODY_PAID_SERVICES_ABB"
		echo "IDS_TPLATFORM_BODY_PERSONAL_INFORMATION_ABB"
		echo "IDS_TPLATFORM_BODY_SYSTEM_SETTINGS_ABB"
		echo "IDS_TPLATFORM_BODY_OTHER_PRIVILEGES_ABB"
		exit
	fi

	CHANGED_TO_2_3_1=`echo $i | cut -d "," -f 14`
	CHANGED_TO_2_4_0=`echo $i | cut -d "," -f 15`

	echo "Inserting $PRIVILEGE_NAME ..."

	sqlite3 $DB_NAME "insert into privilege_info values ( $PROFILE_ID, '$PROFILE', $PACKAGE_TYPE_ID, '$PACKAGE_TYPE', $PRIVILEGE_LEVEL_ID, '$PRIVILEGE_LEVEL', '$API_VERSION_ISSUED', '$API_VERSION_EXPIRED', '$DOCUMENTED', '$PRIVILEGE_NAME', '$IS_PRIVACY', '$PRIVACY_GROUP', '$PRIVILEGE_DISPLAY', '$PRIVILEGE_DESCRIPTION', $PRIVILEGE_GROUP_ID, '$PRIVILEGE_GROUP', '$CHANGED_TO_2_3_1', '$CHANGED_TO_2_4_0')"
	sqlite3 $DB_NAME "insert into valid_privilege_info (privilege_name, is_privacy, is_internal) values ('$PRIVILEGE_NAME', '$IS_PRIVACY', 0)"
done

for internal_privilege in `cat internal_only.list`
do
	echo "Inserting $internal_privilege ..."
	sqlite3 $DB_NAME "insert into valid_privilege_info (privilege_name, is_privacy, is_internal) values ('$internal_privilege', 0, 1)"
done

echo "Insert into PRIVACY_INFO"
echo "ACCOUNT"
sqlite3 $DB_NAME "insert into privacy_info (privacy_group, privacy_display) values ('ACCOUNT','IDS_ST_BODY_ACCOUNTS')"
echo "BOOKMARK"
sqlite3 $DB_NAME "insert into privacy_info (privacy_group, privacy_display) values ('BOOKMARK','IDS_BR_OPT_BOOKMARK')"
echo "CALENDAR"
sqlite3 $DB_NAME "insert into privacy_info (privacy_group, privacy_display) values ('CALENDAR','IDS_TPLATFORM_OPT_CALENDAR_ABB')"
echo "CALL"
sqlite3 $DB_NAME "insert into privacy_info (privacy_group, privacy_display) values ('CALL','IDS_ST_BODY_CALL')"
echo "CAMERA"
sqlite3 $DB_NAME "insert into privacy_info (privacy_group, privacy_display) values ('CAMERA','IDS_TPLATFORM_BODY_CAMERA')"
echo "CONTACTS"
sqlite3 $DB_NAME "insert into privacy_info (privacy_group, privacy_display) values ('CONTACTS','IDS_TPLATFORM_OPT_CONTACTS_ABB')"
echo "LOCATION"
sqlite3 $DB_NAME "insert into privacy_info (privacy_group, privacy_display) values ('LOCATION','IDS_TPLATFORM_OPT_LOCATION_T_LBS')"
echo "MESSAGE"
sqlite3 $DB_NAME "insert into privacy_info (privacy_group, privacy_display) values ('MESSAGE','IDS_ST_BODY_MESSAGE')"
echo "MICROPHONE"
sqlite3 $DB_NAME "insert into privacy_info (privacy_group, privacy_display) values ('MICROPHONE','Microphone')"
echo "SENSOR"
sqlite3 $DB_NAME "insert into privacy_info (privacy_group, privacy_display) values ('SENSOR','Sensor')"
echo "USERHISTORY"
sqlite3 $DB_NAME "insert into privacy_info (privacy_group, privacy_display) values ('USERHISTORY','User History')"
