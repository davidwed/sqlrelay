// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information.

	private:
		int	openSession();
		void	closeConnection();

		int	authenticateWithListener();
		int	authenticateWithConnection();
		int	genericAuthentication();
		void	getReconnect();
		int	getNewPort();

		void	clearSessionFlags();

		void	debugPreStart();
		void	debugPreEnd();
		void	debugPrint(const char *string);
		void	debugPrint(long number);
		void	debugPrint(double number);
		void	debugPrint(char character);
		void	debugPrintBlob(const char *blob, unsigned long length);
		void	debugPrintClob(const char *clob, unsigned long length);

		int	autoCommit(unsigned short on);

		void	clearError();
		void	setError(const char *err);

		// session state
		unsigned short	endsessionsent;
		unsigned short	suspendsessionsent;
		int		connected;

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
		int		reconnect;

		// error
		char		*error;

		// identify
		char		*id;

		// debug
		int		debug;
		int		webdebug;
		int		(*printfunction)(const char *,...);

		// copy references flag
		int		copyrefs;

		// cursor list
		sqlrcursor	*firstcursor;
		sqlrcursor	*lastcursor;

	public:
		void		copyReferences();

	friend class sqlrcursor;
