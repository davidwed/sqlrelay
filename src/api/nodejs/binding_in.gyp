{
	"targets": [
		{
			"target_name": "sqlrelay",
			"sources": ["sqlrelay.cpp"],
			"include_dirs": [
				"@TOP_BUILDDIR_ABS@/src/api/c++",
				"/usr/local/firstworks/include"
			],
			"ldflags" : [
				"-L@TOP_BUILDDIR_ABS@/src/api/c++/.libs",
				"-lsqlrclient",
				"-L/usr/local/firstworks/lib",
				"-lrudiments"
			]
		}
	]
}
