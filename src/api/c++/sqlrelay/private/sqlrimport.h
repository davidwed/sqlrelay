
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
