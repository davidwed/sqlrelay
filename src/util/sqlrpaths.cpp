// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <rudiments/stringbuffer.h>
#include <sqlrelay/sqlrutil.h>

sqlrpaths::sqlrpaths(sqlrcmdline *cmdl) {

	localstatedir=cmdl->getLocalStateDir();
	if (!cmdl->getLocalStateDir()[0]) {
		localstatedir=LOCALSTATEDIR;
	}
	const char	*sysconfdir=SYSCONFDIR;

	stringbuffer	scratch;
	scratch.append(localstatedir)->append("/sqlrelay/tmp");
	tmpdirlen=scratch.getStringLength();
	tmpdir=scratch.detachString();

	scratch.append(localstatedir)->append("/sqlrelay/log");
	logdir=scratch.detachString();

	scratch.append(localstatedir)->append("/sqlrelay/debug");
	debugdir=scratch.detachString();

	scratch.append(localstatedir)->append("/sqlrelay/cache");
	cachedir=scratch.detachString();

	scratch.append(sysconfdir)->append("/sqlrelay.conf");
	defaultconfigfile=scratch.detachString();

	scratch.append(sysconfdir)->append("/sqlrelay.conf.d");
	defaultconfigdir=scratch.detachString();

	if (cmdl->getConfig()) {
		configfile=cmdl->getConfig();
	} else {
		configfile=defaultconfigfile;
	}
}

sqlrpaths::~sqlrpaths() {
	delete[] tmpdir;
	delete[] logdir;
	delete[] debugdir;
	delete[] cachedir;
	delete[] defaultconfigfile;
	delete[] defaultconfigdir;
}

const char *sqlrpaths::getLocalStateDir() {
	return localstatedir;
}

const char *sqlrpaths::getTmpDir() {
	return tmpdir;
}

uint32_t sqlrpaths::getTmpDirLength() {
	return tmpdirlen;
}

const char *sqlrpaths::getLogDir() {
	return logdir;
}

const char *sqlrpaths::getDebugDir() {
	return debugdir;
}

const char *sqlrpaths::getCacheDir() {
	return cachedir;
}

const char *sqlrpaths::getDefaultConfigFile() {
	return defaultconfigfile;
}

const char *sqlrpaths::getDefaultConfigDir() {
	return defaultconfigdir;
}

const char *sqlrpaths::getConfigFile() {
	return configfile;
}
