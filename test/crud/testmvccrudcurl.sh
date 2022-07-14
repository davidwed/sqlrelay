sqlrsh -config ../sqlrelay.conf.d/oracle.conf -id oracletest -command "drop table testtable"
sqlrsh -config ../sqlrelay.conf.d/oracle.conf -id oracletest -command "create table testtable (col1 varchar2(128), col2 varchar2(128), col3 varchar2(128))"



# create test
echo "======================================================================"
echo "create:"
curl -X POST \
	-H "Content-type: application/json" \
	--data-binary @- \
	http://localhost/mvccrud.cgi/create << EOF
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
curl -X POST \
	-H "Content-type: application/json" \
	--data-binary @- \
	http://localhost/mvccrud.cgi/read << EOF
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
curl -X POST \
	-H "Content-type: application/json" \
	--data-binary @- \
	http://localhost/mvccrud.cgi/update << EOF
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
curl -X POST \
	-H "Content-type: application/json" \
	--data-binary @- \
	http://localhost/mvccrud.cgi/read << EOF
EOF
echo



# delete test
echo "======================================================================"
echo "delete:"
curl -X POST \
	-H "Content-type: application/json" \
	--data-binary @- \
	http://localhost/mvccrud.cgi/delete << EOF
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
curl -X POST \
	-H "Content-type: application/json" \
	--data-binary @- \
	http://localhost/mvccrud.cgi/delete << EOF
EOF
echo

echo "======================================================================"
sqlrsh -config ../sqlrelay.conf.d/oracle.conf -id oracletest -command "drop table testtable"
