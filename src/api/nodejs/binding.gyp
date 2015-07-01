{
	"targets": [
		{
			"target_name": "sqlrelay",
			"sources": ["sqlrelay.cpp"],
			"include_dirs": [
				"../c++",
				"/usr/local/firstworks/include"
			],
			"ldflags" : [
				"-L../c++/.libs",
				"-lsqlrclient",
				"-L/usr/local/firstworks/lib",
				"-lrudiments"
			]
		}
	]
}
