#!/bin/sh

HOST=$1

if ( test -z "$HOST" )
then
	echo "usage: ./test.sh host"
	exit 1
fi

# standard sql commands
bteq << EOF
.logon $HOST/testuser,testpassword

.width 500;

bt;
select 1;
et;

begin transaction;
select 1;
end transaction;

bt;
select 1;
rollback;

drop table testtable;

create table testtable (
	col1 byteint,
	col2 smallint,
	col3 integer,
	col4 bigint,
	col5 decimal(10,3),
	col6 number(10,3),
	col7 float,
	col8 char(128),
	col9 varchar(128),
	col10 date,
	col11 time,
	col12 timestamp
	);

insert into testtable values (
	1,
	1,
	1,
	1,
	1.234,
	1.234,
	1.234,
	'hi',
	'hello',
	'2001-01-01',
	'01:01:01',
	'2001-01-01 01:01:01'
	);

insert into testtable values (
	2,
	2,
	2,
	2,
	2.345,
	2.345,
	2.345,
	'hi',
	'hello',
	'2002-02-02',
	'02:02:02',
	'2002-02-02 02:02:02'
	);

select * from testtable;

update testtable set col1=3 where col1=1;
update testtable set col1=4 where col1=2;

select * from testtable;

delete from testtable;

insert into testtable values (
	1,
	null,
	1,
	null,
	1.234,
	null,
	1.234,
	null,
	'hello',
	null,
	'01:01:01',
	null
	);

select * from testtable;

delete from testtable;

insert into testtable values (
	null,
	1,
	null,
	1,
	null,
	1.234,
	null,
	'hi',
	null,
	'2001-01-01',
	null,
	'2001-01-01 01:01:01'
	);

select * from testtable;

drop table testtable;


.quit
EOF
