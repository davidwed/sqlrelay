// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <defaults.h>
#include <sqlrelay/sqlrutil.h>
#include <config.h>

sqlrcmdline::sqlrcmdline(int argc, const char **argv) : commandline(argc,argv) {
	id=getValue("-id");
	if (!id[0]) {
		id=DEFAULT_ID;
	}
	config=getValue("-config");
	localstatedir=getValue("-localstatedir");
}

const char *sqlrcmdline::getConfig() const {
	return config;
}

const char *sqlrcmdline::getId() const {
	return id;
}

const char *sqlrcmdline::getLocalStateDir() const {
	return localstatedir;
}
