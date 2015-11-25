#!/bin/bash

DB_NAME="tv_wrt_privilege_mapping.db"

rm $DB_NAME 2> /dev/null
echo "Creating $DB_NAME ..."
touch $DB_NAME

echo "Creating PRIVILEGE_MAPPING table ..."
sqlite3 $DB_NAME "CREATE TABLE PRIVILEGE_MAPPING (PROFILE_ID NUMERIC, PROFILE TEXT, PRIVILEGE_NAME TEXT, FROM_API_VERSION TEXT, TO_API_VERSION TEXT, MAPPED_PRIVILEGE_NAME TEXT);"

echo "Inserting data ..."
IFS=$'\n'
for i in `cat tv_wrt_privilege_mapping.csv`
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

	elif [ "$PROFILE" = "tv" ]
    then
        PROFILE_ID=3
	else
		echo "Fail to create table : PROFILE must be common, mobile, wearable or tv"
		exit
	fi

	PRIVILEGE_NAME=`echo $i | cut -d "," -f 2`
	FROM_API_VERSION=`echo $i | cut -d "," -f 3`
	TO_API_VERSION=`echo $i | cut -d "," -f 4`
	MAPPED_PRIVILEGE_NAME=`echo $i | cut -d "," -f 5`

	echo "Inserting $PRIVILEGE_NAME $MAPPED_PRIVILEGE_NAME..."

	sqlite3 $DB_NAME "insert into privilege_mapping values ( $PROFILE_ID, '$PROFILE', '$PRIVILEGE_NAME','$FROM_API_VERSION', '$TO_API_VERSION', '$MAPPED_PRIVILEGE_NAME')"
done

echo "Check inserted data"
sqlite3 $DB_NAME "select * from privilege_mapping"
