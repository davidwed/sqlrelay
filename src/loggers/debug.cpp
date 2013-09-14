// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrlistener.h>
#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <sqlrlogger.h>
#include <cmdline.h>
#include <rudiments/charstring.h>
#include <rudiments/logger.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

class debug : public sqlrlogger {
	public:
			debug(xmldomnode *parameters);
			~debug();

		bool	init(sqlrlistener *sqlrl, sqlrconnection_svr *sqlrcon);
		bool	run(sqlrlistener *sqlrl,
					sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrlogger_eventtype_t event,
					const char *info);
	private:
		bool	openDebugFile();
		void	closeDebugFile();

		filedestination		*dbgfile;
		logger			*debuglogger;
		char			*dbgfilename;
		const char		*name;
		bool			loglistener;
		bool			logconnection;
};

debug::debug(xmldomnode *parameters) : sqlrlogger(parameters) {
	dbgfile=NULL;
	debuglogger=NULL;
	dbgfilename=NULL;
	name=NULL;
	loglistener=charstring::compareIgnoringCase(
			parameters->getAttributeValue("listener"),"no");
	logconnection=charstring::compareIgnoringCase(
			parameters->getAttributeValue("connection"),"no");
}

debug::~debug() {
	closeDebugFile();
	delete[] dbgfilename;
}

bool debug::init(sqlrlistener *sqlrl, sqlrconnection_svr *sqlrcon) {

	closeDebugFile();
	delete[] dbgfilename;

	// Log listener or connection.
	// Log both by default, but either can be disabled.
	if (sqlrl && !loglistener) {
		return true;
	}
	if (sqlrcon && !logconnection) {
		return true;
	}

	// set the debug file name
	name=(sqlrl)?"listener":"connection";
	cmdline		*cmdl=(sqlrcon)?sqlrcon->cont->cmdl:sqlrl->cmdl;
	const char	*localstatedir=cmdl->getLocalStateDir();
	size_t	dbgfilenamelen;
	if (localstatedir[0]) {
		dbgfilenamelen=charstring::length(localstatedir)+
					16+5+charstring::length(name)+20+1;
		dbgfilename=new char[dbgfilenamelen];
		charstring::printf(dbgfilename,dbgfilenamelen,
					"%s/sqlrelay/debug/sqlr-%s.%ld",
						localstatedir,name,
						(long)process::getProcessId());
	} else {
		dbgfilenamelen=charstring::length(DEBUG_DIR)+5+
					charstring::length(name)+20+1;
		dbgfilename=new char[dbgfilenamelen];
		charstring::printf(dbgfilename,dbgfilenamelen,
					"%s/sqlr-%s.%ld",DEBUG_DIR,name,
						(long)process::getProcessId());
	}

	return true;
}

bool debug::run(sqlrlistener *sqlrl,
				sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				sqlrlogger_loglevel_t level,
				sqlrlogger_eventtype_t event,
				const char *info) {
	if (sqlrl && !loglistener) {
		return true;
	}
	if (sqlrcon && !logconnection) {
		return true;
	}
	if (!debuglogger && !openDebugFile()) {
		return false;
	}
	char	*header=debuglogger->logHeader(name);
	debuglogger->write(header,0,info);
	delete[] header;
	return true;
}

bool debug::openDebugFile() {

	// create the debug file
	mode_t	oldumask=process::setFileCreationMask(066);
	dbgfile=new filedestination();
	process::setFileCreationMask(oldumask);

	// open the file
	bool	retval=false;
	if (dbgfile->open(dbgfilename)) {
		stdoutput.printf("Debugging to: %s\n",dbgfilename);
		debuglogger=new logger();
		debuglogger->addLogDestination(dbgfile);
		retval=true;
	} else {
		stderror.printf("Couldn't open debug file: %s\n",dbgfilename);
		if (dbgfile) {
			dbgfile->close();
			delete dbgfile;
			dbgfile=NULL;
		}
	}

	delete[] dbgfilename;
	dbgfilename=NULL;
	return retval;
}

void debug::closeDebugFile() {
	if (dbgfile) {
		dbgfile->close();
		delete dbgfile;
		dbgfile=NULL;
		delete debuglogger;
		debuglogger=NULL;
	}
}

extern "C" {
	sqlrlogger	*new_debug(xmldomnode *parameters) {
		return new debug(parameters);
	}
}
