// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <config.h>
#include <defaults.h>
#include <rudiments/daemonprocess.h>

#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#if defined(HAVE_DIRENT_H)
	#include <dirent.h>
#elif defined(HAVE_DIR_H)
	#include <dir.h>
#elif defined(HAVE_NDIR_H)
	#include <ndir.h>
#elif defined(HAVE_SYS_NDIR_H)
	#include <sys/ndir.h>
#endif
#include <string.h>

class dirnode {
	friend class cachemanager;
	private:
			dirnode(const char *dirname);
			dirnode(const char *start, const char *end);
			~dirnode();
		char	*dirname;
		dirnode	*next;
};

class cachemanager : public daemonprocess {
	public:
			cachemanager(int argc, const char **argv);
			~cachemanager();
		void	scan();
	private:
		void	erase(const char *dirname, const char *filename);
		void	parseCacheDirs(const char *cachedirs);

		int	scaninterval;
		dirnode	*firstdir;
		dirnode	*currentdir;
};

#endif
