// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

	protected:
		regularexpression	createtemp;

		uint16_t	id;

		char			*querybuffer;
		uint32_t		querylength;
		sqlrquerystatus_t	querystatus;

		xmldom		*querytree;
		stringbuffer	translatedquery;

		uint16_t		inbindcount;
		sqlrserverbindvar	*inbindvars;
		uint16_t		outbindcount;
		sqlrserverbindvar	*outbindvars;

		uint64_t	totalrowsfetched;

		uint64_t	commandstartsec;
		uint64_t	commandstartusec;
		uint64_t	commandendsec;
		uint64_t	commandendusec;
		uint64_t	querystartsec;
		uint64_t	querystartusec;
		uint64_t	queryendsec;
		uint64_t	queryendusec;

		uint32_t	maxerrorlength;

		char		*error;
		uint32_t	errorlength;
		int64_t		errnum;
		bool		liveconnection;
		bool		errorsetmanually;

		sqlrcursorstate_t	state;

		sqlrquerycursor		*customquerycursor;

	public:
		// flags that are only useful to the sqlrservercontroller
		bool		prepared;
		bool		querywasintercepted;
		bool		bindswerefaked;
		bool		fakeinputbindsforthisquery;
		stringbuffer	querywithfakeinputbinds;
