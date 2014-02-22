#ifndef SQLRUTIL_TEMPDIR_H
#define SQLRUTIL_TEMPDIR_H

#include <dll.h>

#include <cmdline.h>

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
