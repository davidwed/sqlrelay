// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/private/sqlrserverdll.h>

#include <sqlrconfigfile.h>
#include <cmdline.h>
#include <tempdir.h>

#include <rudiments/semaphoreset.h>
#include <rudiments/sharedmemory.h>

#include <sqlrelay/private/sqlrshmdata.h>

// for pid_t
#include <sys/types.h>

class SQLRSERVER_DLLSPEC scaler {

	public:
			scaler();
			~scaler();
		bool	initScaler(int argc, const char **argv);
		void	loop();

		static	void	shutDown(int32_t signum);

	private:
		void	cleanUp();

		pid_t	openOneConnection();
		bool	connectionStarted();
		void	killConnection(pid_t connpid);
		bool	openMoreConnections();
		bool	reapChildren(pid_t connpid);
		void	getRandomConnectionId();
		bool	availableDatabase();

		uint32_t	getConnectedClientCount();
		uint32_t	getConnectionCount();
		void		incrementConnectionCount();
		void		decrementConnectionCount();

		char		*pidfile;

		char		*id;
		char		*config;
		char		*dbase;

		sqlrconfigfile	*cfgfile;

		uint32_t	maxconnections;
		uint32_t	maxqueuelength;
		uint32_t	growby;
		int32_t		ttl;

		semaphoreset	*semset;

		sharedmemory	*shmem;
		shmdata		*shm;

		linkedlist< connectstringcontainer * > *connectstringlist;
		const char	*connectionid;
		int32_t		metrictotal;

		uint32_t	currentseed;

		bool		init;

		bool		debug;

		tempdir		*tmpdir;
		cmdline		*cmdl;

		bool		iswindows;

		static	bool	shutdown;
};
