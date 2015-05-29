// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>

#include <rudiments/charstring.h>
#include <rudiments/sys.h>
#include <rudiments/stringbuffer.h>

#ifdef _WIN32
	#include <windows.h>
#endif

sqlrpaths::sqlrpaths(sqlrcmdline *cmdl) {

	stringbuffer	scratch;

	char	*defaultlocalstatedir;
	char	*sysconfdir;

#ifdef _WIN32

	// get prefix from HKEY_LOCAL_MACHINE\SOFTWARE\SQLRelay\prefix
	HKEY	hkey;
	char	prefix[512];
	DWORD	prefixsize=sizeof(prefix);
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				"SOFTWARE\\SQLRelay",
				0,KEY_READ,&hkey)!=ERROR_SUCCESS ||
		RegQueryValueEx(hkey,"prefix",0,NULL,
				prefix,&prefixsize)!=ERROR_SUCCESS) {

		// fall back to defaults
		if (sizeof(long)==8) {
			charstring::copy(prefix,
					"C:\\Program Files\\Firstworks");
		} else {
			charstring::copy(prefix,
					"C:\\Program Files (x86)\\Firstworks");
		}
	}

	// build default localstatedir
	scratch.append(prefix)->append("\\var\\");
	defaultlocalstatedir=scratch.detachString();

	// sysconfdir
	scratch.append(prefix)->append("\\etc\\");
	sysconfdir=scratch.detachString();

	// libexecdir
	scratch.append(prefix)->append("\\libexec\\");
	libexecdir=scratch.detachString();

#else
	defaultlocalstatedir=charstring::duplicate(LOCALSTATEDIR);
	sysconfdir=charstring::duplicate(SYSCONFDIR);
	libexecdir=charstring::duplicate(LIBEXECDIR);
#endif

	char	slash=sys::getDirectorySeparator();

	localstatedir=charstring::duplicate(cmdl->getLocalStateDir());
	if (!cmdl->getLocalStateDir()[0]) {
		localstatedir=defaultlocalstatedir;
	} else {
		delete[] defaultlocalstatedir;
	}

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

	scratch.append(sysconfdir)->append("sqlrelay.conf");
	defaultconfigfile=scratch.detachString();

	scratch.append(sysconfdir)->append("sqlrelay.conf.d")->append(slash);
	defaultconfigdir=scratch.detachString();

	if (cmdl->getConfig()) {
		configfile=cmdl->getConfig();
	} else {
		configfile=defaultconfigfile;
	}

	delete[] sysconfdir;
}

sqlrpaths::~sqlrpaths() {
	delete[] localstatedir;
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
	delete[] libexecdir;
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
