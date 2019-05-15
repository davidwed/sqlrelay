// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <defaults.h>
#include <sqlrelay/sqlrutil.h>
#include <config.h>

sqlrcmdline::sqlrcmdline(int argc, const char **argv) : commandline(argc,argv) {
	id=getValue("-id");
	if (charstring::isNullOrEmpty(id)) {
		id=DEFAULT_ID;
	}
}

const char *sqlrcmdline::getId() const {
	return id;
}
