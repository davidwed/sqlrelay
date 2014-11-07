// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#ifndef SQLRCONNECTION_H
#define SQLRCONNECTION_H

#include <sqlrelay/private/sqlrserverconnectionincludes.h>

class sqlrservercontroller;

class SQLRSERVER_DLLSPEC sqlrserverconnection {
	public:
			sqlrserverconnection(sqlrservercontroller *cont);
		virtual	~sqlrserverconnection();

		virtual bool	mustDetachBeforeLogIn();

		virtual bool	supportsAuthOnDatabase();
		virtual	void	handleConnectString()=0;

		virtual	bool	logIn(const char **error)=0;
		virtual	void	logOut()=0;

		virtual	bool	changeUser(const char *newuser,
						const char *newpassword);

		virtual bool	autoCommitOn();
		virtual bool	autoCommitOff();

		virtual bool	isTransactional();
		virtual bool	supportsTransactionBlocks();

		virtual bool		begin();
		virtual const char	*beginTransactionQuery();

		virtual bool	commit();
		virtual bool	rollback();

		virtual	void	errorMessage(char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection)=0;

		virtual bool		selectDatabase(const char *database);
		virtual const char	*selectDatabaseQuery();

		virtual char		*getCurrentDatabase();
		virtual const char	*getCurrentDatabaseQuery();

		virtual bool		getLastInsertId(uint64_t *id);
		virtual const char	*getLastInsertIdQuery();

		virtual bool		setIsolationLevel(const char *isolevel);
		virtual const char	*setIsolationLevelQuery();

		virtual bool		ping();
		virtual const char	*pingQuery();

		virtual const char	*identify()=0;

		virtual	const char	*dbVersion()=0;

		virtual const char	*dbHostNameQuery();
		virtual const char	*dbIpAddressQuery();
		virtual const char	*dbHostName();
		virtual const char	*dbIpAddress();

		virtual bool		getListsByApiCalls();
		virtual bool		getDatabaseList(
						sqlrservercursor *cursor,
						const char *wild);
		virtual bool		getTableList(
						sqlrservercursor *cursor,
						const char *wild);
		virtual bool		getColumnList(
						sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		virtual const char	*getDatabaseListQuery(bool wild);
		virtual const char	*getTableListQuery(bool wild);
		virtual const char	*getColumnListQuery(
						const char *table,
						bool wild);
		virtual bool		isSynonym(const char *table);
		virtual const char	*isSynonymQuery();

		virtual sqlrservercursor	*newCursor(uint16_t id)=0;
		virtual void		deleteCursor(sqlrservercursor *curs)=0;

		virtual	const char	*bindFormat();
		virtual	int16_t		nonNullBindValue();
		virtual	int16_t		nullBindValue();
		virtual char		bindVariablePrefix();
		virtual bool		bindValueIsNull(int16_t isnull);

		virtual const char	*tempTableDropPrefix();
		virtual bool		tempTableDropReLogIn();

		virtual void		endSession();

		bool	getAutoCommit();
		void	setAutoCommit(bool autocommit);
		bool	getFakeAutoCommit();

		void	clearError();
		void	setError(const char *err, int64_t errn, bool liveconn);

		char		*getErrorBuffer();
		uint32_t	getErrorLength();
		void		setErrorLength(uint32_t errorlength);
		uint32_t	getErrorNumber();
		void		setErrorNumber(uint32_t errnum);
		bool		getLiveConnection();
		void		setLiveConnection(bool liveconnection);

		sqlrservercontroller	*cont;

	#include <sqlrelay/private/sqlrserverconnection.h>
};


#endif
