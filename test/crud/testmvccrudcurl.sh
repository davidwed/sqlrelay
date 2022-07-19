#!/bin/sh

# use the provided extension or fall back to cgi
EXT=cgi
if ( test -n "$1" )
then
	EXT=$1
fi
URL=http://localhost/mvccrud.$EXT


sqlrsh -config ../sqlrelay.conf.d/postgresql.conf -id postgresqltest -command "drop table testtable"
sqlrsh -config ../sqlrelay.conf.d/postgresql.conf -id postgresqltest -command "create table testtable (col1 varchar(128), col2 varchar(128), col3 varchar(128))"


# create test
echo "======================================================================"
echo "create:"
echo $URL/create
curl -X POST \
	-H "Content-type: application/json" \
	--data-binary @- \
	$URL/create << EOF
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
echo $URL/read
curl -X POST \
	-H "Content-type: application/json" \
	--data-binary @- \
	$URL/read << EOF
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
echo $URL/update
curl -X POST \
	-H "Content-type: application/json" \
	--data-binary @- \
	$URL/update << EOF
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
echo $URL/read
curl -X POST \
	-H "Content-type: application/json" \
	--data-binary @- \
	$URL/read << EOF
EOF
echo



# delete test
echo "======================================================================"
echo "delete:"
echo $URL/delete
curl -X POST \
	-H "Content-type: application/json" \
	--data-binary @- \
	$URL/delete << EOF
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
echo $URL/read
curl -X POST \
	-H "Content-type: application/json" \
	--data-binary @- \
	$URL/delete << EOF
EOF
echo

echo "======================================================================"
sqlrsh -config ../sqlrelay.conf.d/postgresql.conf -id postgresqltest -command "drop table testtable"
