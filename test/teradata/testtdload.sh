#FASTHOST=localhost
FASTHOST=192.168.123.245
#SQLRHOST=localhost
SQLRHOST=fedora

# standard sql commands
bteq << EOF
.logon $SQLRHOST/testuser,testpassword
drop table testtable;
drop table testtable_ET;
drop table testtable_Log;
drop table testtable_UV;
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
	printf "%03d,%05d,%05d,%05d,-%05d.123,-%05d.456,-%05d.789,hello%05d,hello%05d,2000-01-02,03:04:05,2000-01-02 03:04:05.000006\n" $b $a $a $a $a $a $a $a $a >> test.input
	let a=$a+1
	let b=$b+1
	if ( test "$b" -gt "127" )
	then
		b=127
	fi
	#if ( test "$a" = "20000" )
	#if ( test "$a" = "5000" )
	#if ( test "$a" = "500" )
	#if ( test "$a" = "100" )
	if ( test "$a" = "1" )
	then
		break;
	fi
done

# run fastload
#export COPANOMLOG=`pwd`/fastload.log
#export NETRACE=3
time tdload \
	-f test.input \
	-h $FASTHOST \
	-u testuser \
	-p testpassword \
	-t testtable \
	-d , \
	tdloadjob
#unset COPANOMLOG
#unset NETRACE

exit

# verify fastload
bteq << EOF
.logon $SQLRHOST/testuser,testpassword
.width 512;
select * from testtable_ET;
select * from testtable order by col1, col2;
drop table testtable;
.quit
EOF
