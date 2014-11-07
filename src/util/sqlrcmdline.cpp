// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <defaults.h>
#include <sqlrcmdline.h>
#include <config.h>

sqlrcmdline::sqlrcmdline(int argc, const char **argv) : commandline(argc,argv) {
	setId();
	setConfig();
	setLocalStateDir();
}

void sqlrcmdline::setId() {

	// get the id from the command line
	id=getValue("-id");
	if (!id[0]) {
		id=DEFAULT_ID;
	}
}

void sqlrcmdline::setConfig() {

	// get the config file from the command line
	config=getValue("-config");
	if (!config[0]) {
		config=DEFAULT_CONFIG_FILE;
	}
}

void sqlrcmdline::setLocalStateDir() {

	// get the localstatedir from the command line
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
