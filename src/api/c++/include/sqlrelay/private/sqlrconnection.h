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
		void	debugPrint(long number);
		void	debugPrint(double number);
		void	debugPrint(char character);
		void	debugPrintBlob(const char *blob, unsigned long length);
		void	debugPrintClob(const char *clob, unsigned long length);

		bool	autoCommit(bool on);

		void	clearError();
		void	setError(const char *err);

		// clients
		inetclientsocket	ics;
		unixclientsocket	ucs;
		clientsocket		*cs;

		// session state
		bool	endsessionsent;
		bool	suspendsessionsent;
		bool	connected;

		// connection
		char			*server;
		unsigned short		listenerinetport;
		unsigned short		connectioninetport;
		char			*listenerunixport;
		char			*connectionunixport;
		char			connectionunixportbuffer[MAXPATHLEN+1];
		int			retrytime;
		int			tries;

		// authentication
		char		*user;
		int		userlen;
		char		*password;
		int		passwordlen;
		bool		reconnect;

		// error
		char		*error;

		// identify
		char		*id;

		// debug
		bool		debug;
		int		webdebug;
		int		(*printfunction)(const char *,...);

		// copy references flag
		bool		copyrefs;

		// cursor list
		sqlrcursor	*firstcursor;
		sqlrcursor	*lastcursor;

	public:
		void		copyReferences();

	friend class sqlrcursor;
