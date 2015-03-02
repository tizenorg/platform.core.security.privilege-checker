#!/bin/bash

DB_NAME="wrt_privilege_info.db"

rm $DB_NAME 2> /dev/null
echo "Creating $DB_NAME ..."
touch $DB_NAME

echo "Creating PRIVILEGE_INFO table ..."
sqlite3 $DB_NAME "CREATE TABLE PRIVILEGE_INFO (PROFILE_ID NUMERIC, PROFILE TEXT, PACKAGE_TYPE_ID NUMERIC, PACKAGE_TYPE TEXT, PRIVILEGE_LEVEL_ID NUMERIC, PRIVILEGE_LEVEL TEXT, API_VERSION_ISSUED TEXT, API_VERSION_EXPIRED TEXT, DOCUMENTED INTEGER, PRIVILEGE_NAME TEXT, PRIVILEGE_DISPLAY TEXT, PRIVILEGE_DESCRIPTION TEXT, PRIVILEGE_GROUP_ID NUMERIC, PRIVLEGE_GROUP TEXT, CHANGED_TO TEXT);"

echo "Inserting data ..."
LIST=`cat wrt_privilege_info.csv | sed 's/ //g'`
for i in $LIST
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

	elif [ "$PROFILE" = "mobile" ]
	then
		PROFILE_ID=1

	elif [ "$PROFILE" = "wearable" ]
	then
		PROFILE_ID=2

	else
		echo "Fail to create table : PROFILE must be common, mobile or wearable"
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
		echo "Fail to create table : PACKAGE_TYPE must be osp, wrt or core"
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

	PRIVILEGE_DISPLAY=`echo $i | cut -d "," -f 8`

	PRIVILEGE_DESCRIPTION=`echo $i | cut -d "," -f 9`

	PRIVILEGE_GROUP=`echo $i | cut -d "," -f 10`

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

    CHANGED_TO=`echo $i | cut -d "," -f 11`

	echo "Inserting $PRIVILEGE_NAME ..."

	sqlite3 $DB_NAME "insert into privilege_info values ( $PROFILE_ID, '$PROFILE', $PACKAGE_TYPE_ID, '$PACKAGE_TYPE', $PRIVILEGE_LEVEL_ID, '$PRIVILEGE_LEVEL', '$API_VERSION_ISSUED', '$API_VERSION_EXPIRED', '$DOCUMENTED', '$PRIVILEGE_NAME', '$PRIVILEGE_DISPLAY', '$PRIVILEGE_DESCRIPTION', $PRIVILEGE_GROUP_ID, '$PRIVILEGE_GROUP', '$CHANGED_TO')"
done

echo "Check inserted data"
sqlite3 $DB_NAME "select * from privilege_info"
