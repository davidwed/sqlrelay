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
		bool				init(int argc, const char **argv);
	private:
		uint16_t	getNumberOfConnectStringVars();
		void		handleConnectString();
		bool		logIn(bool printerrors);
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
		bool		isTransactional();
		const char	*identify();
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();

		bool		createSharedMemoryAndSemaphores(const char *tmpdir, const char *id);

		bool	connected;

		const char	*db;
		const char	*host;
		const char	*port;
		const char	*socket;

		const char	*connectionid;
		tempdir		*tmpdir;
		
};

#endif
