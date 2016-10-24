// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

		sqlrserverconnection	*conn;

	public:
		// flags that are only useful to the sqlrservercontroller
		bool		prepared;
		bool		querywasintercepted;
		bool		bindswerefaked;
		bool		fakeinputbindsforthisquery;
		stringbuffer	querywithfakeinputbinds;

	private:
		sqlrservercursorprivate	*pvt;
