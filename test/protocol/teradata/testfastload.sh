#!/bin/sh

HOST=$1

if ( test -z "$HOST" )
then
	echo "usage: ./testfastloas.sh host"
	exit 1
fi


# standard sql commands
bteq << EOF
.logon $HOST/testuser,testpassword
drop table testtable;
drop table error_1;
drop table error_2;
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
.quit
EOF

# create fastload input
rm -f test.input
a=0
a=0
while true
do
	printf "|blahh |%05d |%05d |%05d |-%05d.123 |-%05d.456 |-%05d.789 |hello%05d |hello%05d |2000-01-02 |03:04:05 |2000-01-02 03:04:05.000006 EST |\n" $b $a $a $a $a $a $a $a $a >> test.input
	let a=$a+1
	let b=$b+1
	if ( test "$b" -gt "127" )
	then
		b=127
	fi
	#if ( test "$a" = "20000" )
	#if ( test "$a" = "5000" )
	if ( test "$a" = "500" )
	#if ( test "$a" = "100" )
	#if ( test "$a" = "1" )
	then
		break;
	fi
done

# run fastload
#export COPANOMLOG=`pwd`/fastload.log
#export NETRACE=3
time fastload << EOF
sessions 2;
errlimit 25;
logon $HOST/testuser,testpassword;
set record unformatted;
define
	delim0(char(1)),
	val1(char(6)),
	delim1(char(1)),
	val2(char(6)),
	delim2(char(1)),
	val3(char(6)),
	delim3(char(1)),
	val4(char(6)),
	delim4(char(1)),
	val5(char(11)),
	delim5(char(1)),
	val6(char(11)),
	delim6(char(1)),
	val7(char(11)),
	delim7(char(1)),
	val8(char(11)),
	delim8(char(1)),
	val9(char(11)),
	delim9(char(1)),
	val10(char(11)),
	delim10(char(1)),
	val11(char(9)),
	delim11(char(1)),
	val12(char(31)),
	delim12(char(1)),
	newlinechar(char(1))
file=test.input;
show;
begin loading testtable errorfiles error_1, error_2 nodrop;
insert into testtable (col1,col2,col3,col4,col5,col6,col7,col8,col9,col10,col11,col12) values (:val1,:val2,:val3,:val4,:val5,:val6,:val7,:val8,:val9,:val10,:val11,:val12);
end loading;
logoff;
EOF
#unset COPANOMLOG
#unset NETRACE

# verify fastload
bteq << EOF
.logon $HOST/testuser,testpassword
.width 512;
select * from error_1;
select * from testtable order by col1, col2;
drop table testtable;
.quit
EOF
