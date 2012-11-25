// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef STATUSCONNECTION_H
#define STATUSCONNECTION_H

#include <sqlrcontroller.h>

class status : public sqlrcontroller_svr {
	public:
		status();
		shmdata 		*getStatistics();
		int			getConnectionCount();
		int			getSessionCount();
		bool			init(int argc, const char **argv);
		semaphoreset		*getSemset();
	private:
		bool	createSharedMemoryAndSemaphores(const char *tmpdir,
								const char *id);

		bool	connected;

		const char	*connectionid;
		tempdir		*tmpdir;

		semaphoreset	*statussemset;

		shmdata		privateshm;
};

#endif
