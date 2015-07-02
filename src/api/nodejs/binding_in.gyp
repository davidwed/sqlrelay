{
	"targets": [
		{
			"target_name": "sqlrelay",
			"sources": ["sqlrelay.cpp"],
			"include_dirs": [
				"@top_builddir@/src/api/c++",
				"/usr/local/firstworks/include"
			],
			"ldflags" : [
				"-L@top_builddir@/src/c++/.libs",
				"-lsqlrclient",
				"-L/usr/local/firstworks/lib",
				"-lrudiments"
			]
		}
	]
}
