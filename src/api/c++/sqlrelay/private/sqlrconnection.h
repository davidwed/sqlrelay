// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

	private:
		void	init(const char *server,
				uint16_t port,
				const char *socket,
				const char *user,
				const char *password,
				int32_t retrytime,
				int32_t tries,
				bool copyreferences);
		void	setTimeoutFromEnv(const char *var,
					int32_t *timeoutsec,
					int32_t *timeoutusec);
		bool	openSession();
		bool	reConfigureSockets();
		bool	validateCertificate();
		void	setConnectFailedError();
		void	closeConnection();

		void	protocol();
		void	auth();
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

		void		clearError();
		void		setError(const char *err);
		uint16_t	getError();
		bool		gotError();

		void	flushWriteBuffer();

		socketclient	*cs();
		bool		endsessionsent();
		bool		suspendsessionsent();
		bool		connected();
		int32_t		responsetimeoutsec();
		int32_t		responsetimeoutusec();
		int64_t		errorno();
		char		*error();
		char		*clientinfo();
		uint64_t	clientinfolen();
		bool		debug();
		void		firstcursor(sqlrcursor *cur);
		void		lastcursor(sqlrcursor *cur);
		sqlrcursor	*lastcursor();

		sqlrconnectionprivate	*pvt;

	public:
		sqlrconnection(const char *server, uint16_t port,
					const char *socket,
					const char *user, const char *password, 
					int32_t retrytime, int32_t tries,
					bool copyreferences);
		static	bool	isYes(const char *str);
		static	bool	isNo(const char *str);

		const	char	*getCurrentSchema();

	friend class sqlrcursor;
