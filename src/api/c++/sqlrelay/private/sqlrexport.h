// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	private:
		sqlrconnection		*sqlrcon;
		sqlrcursor		*sqlrcur;
		bool			ignorecolumns;
		const char * const	*fieldstoignore;
		filedescriptor		*fd;
		bool			exportrow;
		uint64_t		currentrow;
		uint32_t		currentcol;
		const char		*currentfield;
		logger			*lg;
		uint8_t			coarseloglevel;
		uint8_t			fineloglevel;
		uint32_t		logindent;
		bool			shutdownflag;
		dynamicarray<bool>	numbercolumns;
		uint64_t		exportedrowcount;
