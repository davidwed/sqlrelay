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

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class scaler : public daemonprocess {

	public:
			scaler();
			~scaler();
		bool	initScaler(int argc, const char **argv);
		void	loop();

	private:
		void	cleanUp();

		bool	openMoreConnections();
		void	getRandomConnectionId();
		bool	availableDatabase();
		int	countSessions();
		int	countConnections();

		int		tmpdirlen;

		char		*pidfile;

		const char	*id;
		char		*config;
		const char	*dbase;

		sqlrconfigfile	*cfgfile;

		int		maxconnections;
		int		maxqueuelength;
		int		growby;
		int		ttl;

		char		*idfilename;

		semaphoreset	*semset;

		sharedmemory	*idmemory;

		linkedlist< connectstringcontainer * >	*connectstringlist;
		const char		*connectionid;
		int			metrictotal;

		int		currentseed;

		bool		init;

		int		debug;
};

#endif
