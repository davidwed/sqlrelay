// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	protected:
		sqlrconnection	*sqlrcon;
		sqlrcursor	*sqlrcur;
		char		*dbtype;
		char		*table;
		bool		ignorecolumns;
		uint64_t	commitcount;
		logger		*lg;
		uint8_t		coarseloglevel;
		uint8_t		fineloglevel;
		uint32_t	logindent;
		bool		shutdownflag;
		bool		logerrors;
