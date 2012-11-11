// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef STATUSCONNECTION_H
#define STATUSCONNECTION_H

#define NUM_CONNECT_STRING_VARS 6

#include <sqlrconnection.h>


class statusconnection : public sqlrconnection_svr {
	public:
		statusconnection();
		sqlrstatistics 		*getStatistics();
		int			getConnectionCount();
		int			getSessionCount();
		bool			init(int argc, const char **argv);
		semaphoreset		*getSemset();
	private:
		uint16_t	getNumberOfConnectStringVars();
		void		handleConnectString();
		bool		logIn(bool printerrors);
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
		bool		isTransactional();
		const char	*identify();
		const char	*dbVersion();
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		bool		rollback();

		bool		createSharedMemoryAndSemaphores(const char *tmpdir, const char *id);

		bool	connected;

		const char	*db;
		const char	*host;
		const char	*port;
		const char	*socket;

		const char	*connectionid;
		tempdir		*tmpdir;

		semaphoreset	*statussemset;

		sqlrstatistics	privatestatistics;
};

#endif
