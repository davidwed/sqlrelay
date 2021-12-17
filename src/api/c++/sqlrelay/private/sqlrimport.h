// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

		void	setShutdownFlag(bool *shutdownflag);

	protected:
		sqlrconnection	*sqlrcon;
		sqlrcursor	*sqlrcur;
		char		*dbtype;
		char		*objectname;
		bool		ignorecolumns;
		uint64_t	commitcount;
		logger		*lg;
		uint8_t		coarseloglevel;
		uint8_t		fineloglevel;
		uint32_t	logindent;
		bool		*shutdownflag;
		bool		logerrors;

		bool		lowercasecolumnnames;
		bool		uppercasecolumnnames;
		dictionary<const char *, const char *>	columnmap;
