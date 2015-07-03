{
	"targets": [
		{
			"target_name": "sqlrelay",
			"sources": ["sqlrelay.cpp"],
			"include_dirs": [
				"@NODEJSINCLUDEDIRS@"
			],
			"cflags": [
				"@NODEJSCFLAGS@"
			],
			"xcode_settings": {
				"cflags": [
					"@NODEJSCFLAGS@"
				],
			},
			"libraries" : [
				"@NODEJSLIBS@"
			]
		}
	]
}
