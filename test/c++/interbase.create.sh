#!/bin/sh

echo "create table testtable (testinteger integer, testsmallint smallint, testdecimal decimal(10,2), testnumeric numeric(10,2), testfloat float, testdouble double precision, testdate date, testtime time, testchar char(50), testvarchar varchar(50), testtimestamp timestamp);" > createscript.sql

/opt/interbase/bin/isql -u testuser -p testpassword '/opt/interbase/testdb.gdb' -i createscript.sql

rm createscript.sql
