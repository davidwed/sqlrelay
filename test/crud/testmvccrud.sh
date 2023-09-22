sqlrsh -host localhost -user testuser -password testpassword -command "drop table testtable"
sqlrsh -host localhost -user testuser -password testpassword -command "create table testtable (colstr varchar(128), colint int, coltrue boolean, colfalse boolean, colnull varchar(128))"



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
		"colstr": "val1",
		"colint": 1,
		"coltrue": true,
		"colfalse": false,
		"colnull": null
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
				{ "var": "colstr" },
				"val1"
			] },
			{ "=" : [
				{ "var": "colint" },
				1
			] },
			{ "=" : [
				{ "var": "coltrue" },
				true
			] },
			{ "=" : [
				{ "var": "colfalse" },
				false
			] },
			{ "isnull" : [
				{ "var": "colnull" }
			] }
		]
	},
	"sort": {
		"colstr" : "asc",
		"colint" : "asc",
		"coltrue" : "asc"
		"colfalse" : "asc"
		"colnull" : "asc"
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
			{ "var": "colstr" },
			"val1"
		]
	},
	"data": {
		"colstr": "val2",
		"colint": 2,
		"coltrue": false,
		"colfalse": true,
		"colnull": "not-null"
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
			{ "var": "colstr" },
			"val2"
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
sqlrsh -host localhost -user testuser -password testpassword -command "drop table testtable"
