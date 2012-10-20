// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <config.h>
#include <defaults.h>
#include <rudiments/daemonprocess.h>
#include <cmdline.h>
#include <tempdir.h>

class dirnode {
	friend class cachemanager;
	private:
			dirnode(const char *dirname);
			dirnode(const char *start, const char *end);
			~dirnode();
		char	*dirname;
		dirnode	*next;
};

class cachemanager : public rudiments::daemonprocess {
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

		cmdline	*cmdl;
		tempdir	*tmpdir;

		char	*pidfile;
};

#endif
