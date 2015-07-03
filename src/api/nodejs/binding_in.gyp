{
	"targets": [
		{
			"target_name": "sqlrelay",
			"sources": ["sqlrelay.cpp"],
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
