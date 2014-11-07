// Copyright (c) 1999-2014 David Muse
// See the file COPYING for more information

#ifndef SQLRUTIL_SQLRTEMPDIR_H
#define SQLRUTIL_SQLRTEMPDIR_H

#include <sqlrutildll.h>

#include <sqlrcmdline.h>

class SQLRUTIL_DLLSPEC sqlrtempdir {
	public:
			sqlrtempdir(sqlrcmdline *cmdl);
			~sqlrtempdir();
		char	*getString();
		int32_t	getLength();
	protected:
		char	*tmpdir;
		int32_t	tmpdirlen;
};

#endif
