// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>

#include <rudiments/charstring.h>
#include <rudiments/sys.h>
#include <rudiments/stringbuffer.h>

#ifdef _WIN32
	#define LOCALSTATEDIR "C:\\Program Files\\Firstworks\\var\\"
	#define SYSCONFDIR "C:\\Program Files\\Firstworks\\etc\\"
	#define LIBEXECDIR "C:\\Program Files\\Firstworks\\libexec\\sqlrelay\\"
#endif

sqlrpaths::sqlrpaths(sqlrcmdline *cmdl) {

	char	slash=sys::getDirectorySeparator();

	localstatedir=cmdl->getLocalStateDir();
	if (!cmdl->getLocalStateDir()[0]) {
		localstatedir=LOCALSTATEDIR;
	}

	stringbuffer	scratch;
	scratch.append(localstatedir)->append("sqlrelay")->append(slash);
	char	*lsdir=scratch.detachString();

	scratch.append(lsdir)->append("tmp")->append(slash);
	tmpdirlen=scratch.getStringLength();
	tmpdir=scratch.detachString();

	scratch.append(tmpdir)->append("sockseq");
	sockseqfile=scratch.detachString();

	scratch.append(tmpdir)->append("sockets")->append(slash);
	socketsdir=scratch.detachString();

	scratch.append(tmpdir)->append("ipc")->append(slash);
	ipcdir=scratch.detachString();

	scratch.append(tmpdir)->append("pids")->append(slash);
	piddir=scratch.detachString();

	scratch.append(lsdir)->append("log")->append(slash);
	logdir=scratch.detachString();

	scratch.append(lsdir)->append("debug")->append(slash);
	debugdir=scratch.detachString();

	scratch.append(lsdir)->append("cache")->append(slash);
	cachedir=scratch.detachString();

	const char	*sysconfdir=SYSCONFDIR;

	scratch.append(sysconfdir)->append("sqlrelay.conf");
	defaultconfigfile=scratch.detachString();

	scratch.append(sysconfdir)->append("sqlrelay.conf.d")->append(slash);
	defaultconfigdir=scratch.detachString();

	if (cmdl->getConfig()) {
		configfile=cmdl->getConfig();
	} else {
		configfile=defaultconfigfile;
	}

	libexecdir=LIBEXECDIR;
}

sqlrpaths::~sqlrpaths() {
	delete[] tmpdir;
	delete[] sockseqfile;
	delete[] socketsdir;
	delete[] ipcdir;
	delete[] piddir;
	delete[] logdir;
	delete[] debugdir;
	delete[] cachedir;
	delete[] defaultconfigfile;
	delete[] defaultconfigdir;
}

const char *sqlrpaths::getLocalStateDir() {
	return localstatedir;
}

const char *sqlrpaths::getSockSeqFile() {
	return sockseqfile;
}

const char *sqlrpaths::getSocketsDir() {
	return socketsdir;
}

const char *sqlrpaths::getIpcDir() {
	return ipcdir;
}

const char *sqlrpaths::getPidDir() {
	return piddir;
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

const char *sqlrpaths::getLibExecDir() {
	return libexecdir;
}
