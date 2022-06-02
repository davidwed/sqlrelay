REQUEST_METHOD="GET"
export REQUEST_METHOD


# create test
unset PATH_INFO
unset QUERY_STRING
PATH_INFO="/create.html"
export PATH_INFO
QUERY_STRING="col1=val1&col2=val2&col3=val3"
export QUERY_STRING
./mvccrud.cgi


# read test
unset PATH_INFO
unset QUERY_STRING
PATH_INFO="/read.html"
export PATH_INFO
./mvccrud.cgi


# update test
unset PATH_INFO
unset QUERY_STRING
PATH_INFO="/update.html"
export PATH_INFO
QUERY_STRING="col1=newval1&col2=newval2&col3=newval3"
export QUERY_STRING
./mvccrud.cgi


# delete test
unset PATH_INFO
unset QUERY_STRING
PATH_INFO="/delete.html"
export PATH_INFO
QUERY_STRING="col1=newval1"
export QUERY_STRING
./mvccrud.cgi
