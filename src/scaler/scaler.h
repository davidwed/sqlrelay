// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#ifndef MONITOR_H
#define MONITOR_H

#include <config.h>
#include <defaults.h>

#include <sqlrconfigfile.h>

#include <rudiments/daemonprocess.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/sharedmemory.h>
#include <rudiments/randomnumber.h>


class scaler : public daemonprocess {

	public:
			scaler();
			~scaler();
		int	initScaler(int argc, const char **argv);
		void	loop();

	private:
		void	cleanUp();

		void	openMoreConnections();
		void	getRandomConnectionId();
		int	availableDatabase();
		int	countSessions();
		int	countConnections();

		int		tmpdirlen;

		char		*pidfile;

		char		*id;
		char		*config;
		char		*dbase;

		int		connections;
		int		maxconnections;
		int		maxqueuelength;
		int		growby;
		int		ttl;

		char		*idfilename;

		semaphoreset	*semset;

		sharedmemory	*idmemory;

		sqlrconfigfile		*cfgfile;

		connectstringnode	*connectstringlist;
		char			*connectionid;
		int			metrictotal;

		randomnumber	*randnum;
		int		currentseed;

		int		init;

		int		debug;
};

#endif
