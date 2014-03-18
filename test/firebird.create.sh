#!/bin/sh

echo "drop table testtable;" > createscript.sql

isql-fb -u testuser -p testpassword '/opt/firebird/testdb.gdb' -i createscript.sql

echo "create table testtable (testinteger integer, testsmallint smallint, testdecimal decimal(10,2), testnumeric numeric(10,2), testfloat float, testdouble double precision, testdate date, testtime time, testchar char(50), testvarchar varchar(50), testtimestamp timestamp, testblob blob);" > createscript.sql

isql-fb -u testuser -p testpassword '/opt/firebird/testdb.gdb' -i createscript.sql

echo "drop procedure testproc;" > createscript.sql

isql-fb -u testuser -p testpassword '/opt/firebird/testdb.gdb' -i createscript.sql

echo "set term ^; create procedure testproc(in1 integer, in2 float, in3 varchar(20), in4 blob) returns (out1 integer, out2 float, out3 varchar(20), out4 blob) as begin out1 = in1; out2 = in2; out3 = in3; out4 = in4; suspend; end^" > createscript.sql

isql-fb -u testuser -p testpassword '/opt/firebird/testdb.gdb' -i createscript.sql

echo "drop table testtable1;" > createscript.sql

isql-fb -u testuser -p testpassword '/opt/firebird/testdb.gdb' -i createscript.sql

echo "create table testtable1 (testblob blob);" > createscript.sql

isql-fb -u testuser -p testpassword '/opt/firebird/testdb.gdb' -i createscript.sql

echo "drop procedure testproc1;" > createscript.sql

isql-fb -u testuser -p testpassword '/opt/firebird/testdb.gdb' -i createscript.sql

echo "set term ^; create procedure testproc1(in1 blob) returns (out1 blob) as begin out1 = in1; suspend; end^" > createscript.sql

isql-fb -u testuser -p testpassword '/opt/firebird/testdb.gdb' -i createscript.sql

rm createscript.sql
