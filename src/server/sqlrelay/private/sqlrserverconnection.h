// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

	protected:

		bool		autocommit;
		bool		fakeautocommit;

		uint32_t	maxquerysize;
		uint32_t	maxerrorlength;

		char		*error;
		uint32_t	errorlength;
		int64_t		errnum;
		bool		liveconnection;
		bool		errorsetmanually;

		char		*dbhostname;
		char		*dbipaddress;
		uint32_t	dbhostiploop;
