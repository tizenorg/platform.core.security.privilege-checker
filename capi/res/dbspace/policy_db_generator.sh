#!/bin/bash

DB_NAME=".policy.db"

rm $DB_NAME 2> /dev/null
echo "Creating $DB_NAME ..."
touch $DB_NAME

echo "Creating BLACK_LIST table ..."
sqlite3 $DB_NAME "CREATE TABLE BLACK_LIST (UID NUMERIC, PACKAGE_TYPE NUMERIC , PRIVILEGE_NAME TEXT not null, UNIQUE(UID, PACKAGE_TYPE, PRIVILEGE_NAME));"

# test
#sqlite3 $DB_NAME "insert into black_list values (5001, 1, 'http://tizen.org/privilege/account.write')"
#sqlite3 $DB_NAME "insert into black_list values (5001, 1, 'http://tizen.org/privilege/internet')"

echo "Check inserted data"
sqlite3 $DB_NAME "select * from black_list"
