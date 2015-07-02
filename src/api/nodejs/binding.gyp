{
	"targets": [
		{
			"target_name": "sqlrelay",
			"sources": ["sqlrelay.cpp"],
			"include_dirs": [
				"/home/dmuse/src/sqlrelay/src/api/c++",
				"/usr/local/firstworks/include"
			],
			"ldflags" : [
				"-L/home/dmuse/src/sqlrelay/src/api/c++/.libs",
				"-lsqlrclient",
				"-L/usr/local/firstworks/lib",
				"-lrudiments"
			]
		}
	]
}
