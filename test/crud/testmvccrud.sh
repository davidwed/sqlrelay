sqlrsh -host localhost -user test -password test -command "drop table testtable"
sqlrsh -host localhost -user test -password test -command "create table testtable (col1 varchar(128), col2 varchar(128), col3 varchar(128))"



# create test
echo "======================================================================"
echo "create:"
unset PATH_INFO
unset REQUEST_METHOD
unset CONTENT_TYPE
PATH_INFO="/create"
export PATH_INFO
REQUEST_METHOD="POST"
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
echo "======================================================================"
echo "read:"
unset PATH_INFO
unset REQUEST_METHOD
unset CONTENT_TYPE
PATH_INFO="/read"
export PATH_INFO
REQUEST_METHOD="POST"
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
echo "======================================================================"
echo "update:"
unset PATH_INFO
unset REQUEST_METHOD
unset CONTENT_TYPE
PATH_INFO="/update"
export PATH_INFO
REQUEST_METHOD="POST"
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


# read-after-update test
echo "======================================================================"
echo "read-after-update:"
unset PATH_INFO
unset REQUEST_METHOD
unset CONTENT_TYPE
PATH_INFO="/read"
export PATH_INFO
./mvccrud.cgi << EOF
EOF
echo



# delete test
echo "======================================================================"
echo "delete:"
unset PATH_INFO
unset REQUEST_METHOD
unset CONTENT_TYPE
PATH_INFO="/delete"
export PATH_INFO
REQUEST_METHOD="POST"
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


# read-after-delete test
echo "======================================================================"
echo "read-after-delete:"
unset PATH_INFO
unset REQUEST_METHOD
unset CONTENT_TYPE
PATH_INFO="/read"
export PATH_INFO
./mvccrud.cgi << EOF
EOF
echo

echo "======================================================================"
sqlrsh -host localhost -user test -password test -command "drop table testtable"
