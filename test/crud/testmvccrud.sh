sqlrsh -config ../sqlrelay.conf.d/oracle.conf -id oracletest -command "drop table testtable"
sqlrsh -config ../sqlrelay.conf.d/oracle.conf -id oracletest -command "create table testtable (col1 varchar2(128), col2 varchar2(128), col3 varchar2(128))"


REQUEST_METHOD="GET"
export REQUEST_METHOD


# create test
echo "create:"
unset PATH_INFO
unset QUERY_STRING
PATH_INFO="/create.html"
export PATH_INFO
QUERY_STRING="col1=val1&col2=val2&col3=val3"
export QUERY_STRING
./mvccrud.cgi
echo


# read test
echo "read:"
unset PATH_INFO
unset QUERY_STRING
PATH_INFO="/read.html"
export PATH_INFO
./mvccrud.cgi
echo


# update test
echo "update:"
unset PATH_INFO
unset QUERY_STRING
PATH_INFO="/update.html"
export PATH_INFO
QUERY_STRING="col1=newval1&col2=newval2&col3=newval3"
export QUERY_STRING
./mvccrud.cgi
echo


# delete test
echo "delete:"
unset PATH_INFO
unset QUERY_STRING
PATH_INFO="/delete.html"
export PATH_INFO
QUERY_STRING="col1=newval1"
export QUERY_STRING
./mvccrud.cgi
echo

sqlrsh -config ../sqlrelay.conf.d/oracle.conf -id oracletest -command "drop table testtable"
