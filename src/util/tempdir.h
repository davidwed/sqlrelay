#ifndef SQLRUTIL_TEMPDIR_H
#define SQLRUTIL_TEMPDIR_H

#include <rudiments/logger.h>
#include <cmdline.h>

class tempdir {
	public:
			tempdir(cmdline *cmdl);
			~tempdir();
		char	*getString();
		int	getLength();
	protected:
		char	*tmpdir;
		int	tmpdirlen;
};

#endif
