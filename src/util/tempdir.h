#ifndef SQLRUTIL_TEMPDIR_H
#define SQLRUTIL_TEMPDIR_H

#include <rudiments/logger.h>
#include <cmdline.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class tempdir {
	public:
			tempdir(cmdline *cmdl);
			~tempdir();
		char	*getString();
		int32_t	getLength();
	protected:
		char	*tmpdir;
		int32_t	tmpdirlen;
};

#endif
