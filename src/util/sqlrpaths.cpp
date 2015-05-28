// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <rudiments/charstring.h>
#include <sqlrelay/sqlrutil.h>

sqlrpaths::sqlrpaths(sqlrcmdline *cmdl) {

	if (cmdl->getLocalStateDir()[0]) {

		tmpdirlen=charstring::length(cmdl->getLocalStateDir())+13;
		tmpdir=new char[tmpdirlen+1];
		charstring::copy(tmpdir,cmdl->getLocalStateDir());
		charstring::append(tmpdir,"/sqlrelay/tmp");

		logdirlen=charstring::length(cmdl->getLocalStateDir())+13;
		logdir=new char[logdirlen+1];
		charstring::copy(logdir,cmdl->getLocalStateDir());
		charstring::append(logdir,"/sqlrelay/log");

		debugdirlen=charstring::length(cmdl->getLocalStateDir())+15;
		debugdir=new char[debugdirlen+1];
		charstring::copy(debugdir,cmdl->getLocalStateDir());
		charstring::append(debugdir,"/sqlrelay/debug");

	} else {
		tmpdir=charstring::duplicate(TMP_DIR);
		tmpdirlen=charstring::length(tmpdir);

		logdir=charstring::duplicate(LOG_DIR);
		logdirlen=charstring::length(logdir);

		debugdir=charstring::duplicate(DEBUG_DIR);
		debugdirlen=charstring::length(debugdir);
	}
}

sqlrpaths::~sqlrpaths() {
	delete[] tmpdir;
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

uint32_t sqlrpaths::getLogDirLength() {
	return logdirlen;
}

const char *sqlrpaths::getDebugDir() {
	return debugdir;
}

uint32_t sqlrpaths::getDebugDirLength() {
	return debugdirlen;
}
