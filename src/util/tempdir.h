// Copyright (c) 1999-2014 David Muse
// See the file COPYING for more information

#ifndef SQLRUTIL_TEMPDIR_H
#define SQLRUTIL_TEMPDIR_H

#include <sqlrutildll.h>

#include <cmdline.h>

class SQLRUTIL_DLLSPEC tempdir {
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
