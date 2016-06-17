#!/bin/bash

DB_NAME="/usr/share/privilege-manager/.core_privilege_info.db"

echo "Inserting data ..."

for i in `cat /var/cynara/db/_USER_TYPE_* | cut -d ";" -f 3 | sort | uniq | grep -v "*"`
do
	if [[ $i == "http://tizen.org/privilege/internal/"* ]]; then
		echo "Inserting $i ..."
		sqlite3 $DB_NAME "insert or ignore into valid_privilege_info (privilege_name, is_privacy, is_internal) values ('$i', 0, 1)"
	elif [ $i = "http://tizen.org/privilege/notexist" ]; then
        echo "Inserting $i ..."
		sqlite3 $DB_NAME "insert or ignore into valid_privilege_info (privilege_name, is_privacy, is_internal) values ('$i', 0, 1)"
	fi
done

echo "complete!"
