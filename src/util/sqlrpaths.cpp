// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>

#include <rudiments/charstring.h>
#include <rudiments/sys.h>
#include <rudiments/stringbuffer.h>

#include <config.h>

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
				(LPBYTE)prefix,&prefixsize)!=ERROR_SUCCESS) {

		// fall back to defaults
		charstring::copy(prefix,PREFIX);
	}

	// trim trailing slashes
	charstring::rightTrim(prefix,'\\');

	// build default bindir
	scratch.append(prefix)->append("\\bin\\");
	bindir=scratch.detachString();

	// build default localstatedir
	scratch.append(prefix)->append("\\var\\");
	defaultlocalstatedir=scratch.detachString();

	// sysconfdir
	scratch.append(prefix)->append("\\etc\\");
	sysconfdir=scratch.detachString();

	// libexecdir
	scratch.append(prefix)->append("\\libexec\\");
	scratch.append(SQLRELAY)->append("\\");
	libexecdir=scratch.detachString();

#else
	bindir=charstring::duplicate(BINDIR);
	defaultlocalstatedir=charstring::duplicate(LOCALSTATEDIR);
	sysconfdir=charstring::duplicate(SYSCONFDIR);
	libexecdir=charstring::duplicate(LIBEXECDIR);
#endif

	char	slash=sys::getDirectorySeparator();

	const char	*lsd=cmdl->getValue("-localstatedir");
	if (!charstring::isNullOrEmpty(lsd)) {
		localstatedir=charstring::duplicate(lsd);
		delete[] defaultlocalstatedir;
	} else {
		localstatedir=defaultlocalstatedir;
	}

	// we need to make sure that there's a slash after
	// localstatedir when building other paths off of it
	const char	*lsdterm=
#ifdef _WIN32
	(*(localstatedir+charstring::length(localstatedir)-1)=='\\')?"":"\\";
#else
	(*(localstatedir+charstring::length(localstatedir)-1)=='/')?"":"/";
#endif

	scratch.append(localstatedir)->append(lsdterm)->append("run")->
			append(slash)->append(SQLRELAY)->append(slash);
	tmpdirlen=scratch.getStringLength();
	tmpdir=scratch.detachString();

	scratch.append(tmpdir)->append("sockseq");
	sockseqfile=scratch.detachString();

	scratch.append(tmpdir);
	socketsdir=scratch.detachString();

	scratch.append(tmpdir);
	ipcdir=scratch.detachString();

	scratch.append(tmpdir);
	piddir=scratch.detachString();

	scratch.append(localstatedir)->append(lsdterm)->append("log")->
			append(slash)->append(SQLRELAY)->append(slash);
	logdir=scratch.detachString();

	scratch.append(logdir)->append("debug")->append(slash);
	debugdir=scratch.detachString();

	scratch.append(localstatedir)->append(lsdterm)->append("cache")->
			append(slash)->append(SQLRELAY)->append(slash);
	cachedir=scratch.detachString();

	scratch.append(sysconfdir)->append(SQLRELAY)->append(".conf");
	defaultconfigfile=scratch.detachString();

	scratch.append(sysconfdir)->append(SQLRELAY)->append(".conf.d");
	scratch.append(slash);
	defaultconfigdir=scratch.detachString();

	scratch.append(defaultconfigfile)->
			append(",dir://")->append(defaultconfigdir);
	defaultconfigurl=scratch.detachString();

	const char	*cfg=cmdl->getValue("-config");
	if (!charstring::isNullOrEmpty(cfg)) {
		configurl=cfg;
	} else {
		configurl=defaultconfigurl;
	}

	delete[] sysconfdir;
}

sqlrpaths::~sqlrpaths() {
	delete[] bindir;
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

const char *sqlrpaths::getBinDir() {
	return bindir;
}

const char *sqlrpaths::getLocalStateDir() {
	return localstatedir;
}

const char *sqlrpaths::getTmpDir() {
	return tmpdir;
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

const char *sqlrpaths::getDefaultConfigUrl() {
	return defaultconfigurl;
}

const char *sqlrpaths::getConfigUrl() {
	return configurl;
}

const char *sqlrpaths::getLibExecDir() {
	return libexecdir;
}
