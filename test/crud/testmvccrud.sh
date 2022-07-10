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
#QUERY_STRING="col1=val1&col2=val2&col3=val3"
#export QUERY_STRING
REQUEST_METHOD="post"
export REQUEST_METHOD
CONTENT_TYPE="application/json"
export CONTENT_TYPE
./mvccrud.cgi << EOF
{
	"data": {
		"col1": "val1",
		"col2": "val2",
		"col3": "val3"
	}
}
EOF
echo


# read test
echo "read:"
unset PATH_INFO
unset QUERY_STRING
PATH_INFO="/read.html"
export PATH_INFO
REQUEST_METHOD="post"
export REQUEST_METHOD
CONTENT_TYPE="application/json"
export CONTENT_TYPE
./mvccrud.cgi << EOF
{
	"criteria" : {
		"and" : [
			{ "=" : [
				{ "var": "col1" },
				"val1"
			] },
			{ "=" : [
				{ "var": "col2" },
				"val2"
			] },
			{ "=" : [
				{ "var": "col3" },
				"val3"
			] }
		]
	},
	"sort": {
		"col1" : "asc",
		"col2" : "asc",
		"col3" : "asc"
	}
}
EOF
echo


# update test
echo "update:"
unset PATH_INFO
unset QUERY_STRING
PATH_INFO="/update.html"
export PATH_INFO
#QUERY_STRING="col1=newval1&col2=newval2&col3=newval3"
#export QUERY_STRING
REQUEST_METHOD="post"
export REQUEST_METHOD
CONTENT_TYPE="application/json"
export CONTENT_TYPE
./mvccrud.cgi << EOF
{
	"criteria" : {
		"=" : [
			{ "var": "col1" },
			"val1"
		]
	},
	"data": {
		"col1": "newval1",
		"col2": "newval2",
		"col3": "newval3"
	}
}
EOF
echo


# delete test
echo "delete:"
unset PATH_INFO
unset QUERY_STRING
PATH_INFO="/delete.html"
export PATH_INFO
#QUERY_STRING="col1=newval1"
#export QUERY_STRING
REQUEST_METHOD="post"
export REQUEST_METHOD
CONTENT_TYPE="application/json"
export CONTENT_TYPE
./mvccrud.cgi << EOF
{
	"criteria" : {
		"=" : [
			{ "var": "col1" },
			"newval1"
		]
	},
}
EOF
echo

sqlrsh -config ../sqlrelay.conf.d/oracle.conf -id oracletest -command "drop table testtable"
