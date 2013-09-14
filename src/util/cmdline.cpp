// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <defaults.h>
#include <cmdline.h>
#include <config.h>

cmdline::cmdline(int argc, const char **argv) : commandline(argc,argv) {
	setId();
	setConfig();
	setLocalStateDir();
}

void cmdline::setId() {

	// get the id from the command line
	id=getValue("-id");
	if (!id[0]) {
		id=DEFAULT_ID;
	}
}

void cmdline::setConfig() {

	// get the config file from the command line
	config=getValue("-config");
	if (!config[0]) {
		config=DEFAULT_CONFIG_FILE;
	}
}

void cmdline::setLocalStateDir() {

	// get the localstatedir from the command line
	localstatedir=getValue("-localstatedir");
}

const char *cmdline::getConfig() const {
	return config;
}

const char *cmdline::getId() const {
	return id;
}

const char *cmdline::getLocalStateDir() const {
	return localstatedir;
}
