// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information.

	private:
		bool	openSession();
		void	closeConnection();

		bool	authenticateWithListener();
		bool	authenticateWithConnection();
		bool	genericAuthentication();
		bool	getReconnect();
		bool	getNewPort();

		void	clearSessionFlags();

		void	debugPreStart();
		void	debugPreEnd();
		void	debugPrint(const char *string);
		void	debugPrint(int64_t number);
		void	debugPrint(double number);
		void	debugPrint(char character);
		void	debugPrintBlob(const char *blob, uint32_t length);
		void	debugPrintClob(const char *clob, uint32_t length);

		bool	autoCommit(bool on);

		void	clearError();
		void	setError(const char *err);
		bool	getError();

		void	flushWriteBuffer();

		// clients
		inetclientsocket	ics;
		unixclientsocket	ucs;
		clientsocket		*cs;

		// session state
		bool	endsessionsent;
		bool	suspendsessionsent;
		bool	connected;

		// connection
		const char		*server;
		uint16_t		listenerinetport;
		uint16_t		connectioninetport;
		char			*listenerunixport;
		const char		*connectionunixport;
		char			connectionunixportbuffer[MAXPATHLEN+1];
		int32_t			timeoutsec;
		int32_t			timeoutusec;
		int32_t			retrytime;
		int32_t			tries;

		// authentication
		const char	*user;
		uint32_t	userlen;
		const char	*password;
		uint32_t	passwordlen;
		bool		reconnect;

		// error
		char		*error;

		// identify
		char		*id;

		// db version
		char		*dbversion;

		// server version
		char		*serverversion;

		// current database name
		char		*currentdbname;

		// bind format
		char		*bindformat;

		// debug
		bool		debug;
		int32_t		webdebug;
		int		(*printfunction)(const char *,...);

		// copy references flag
		bool		copyrefs;

		// cursor list
		sqlrcursor	*firstcursor;
		sqlrcursor	*lastcursor;

	public:
		void		copyReferences();

	friend class sqlrcursor;
