// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/permissions.h>
#include <rudiments/logger.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

class SQLRSERVER_DLLSPEC sqlrlogger_debug : public sqlrlogger {
	public:
			sqlrlogger_debug(sqlrloggers *ls,
						domnode *parameters);
			~sqlrlogger_debug();

		bool	init(sqlrlistener *sqlrl, sqlrserverconnection *sqlrcon);
		bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrevent_t event,
					const char *info);
	private:
		bool	openDebugFile();
		void	closeDebugFile();

		filedestination		*dbgfile;
		logger			*debuglogger;
		char			*dbgfilename;
		mode_t			dbgfileperms;
		const char		*name;
		bool			enabled;
		bool			loglistener;
		bool			logconnection;
};

sqlrlogger_debug::sqlrlogger_debug(sqlrloggers *ls,
					domnode *parameters) :
					sqlrlogger(ls,parameters) {
	dbgfile=NULL;
	debuglogger=NULL;
	dbgfilename=NULL;
	const char	*permstring=parameters->getAttributeValue("perms");
	if (!charstring::length(permstring)) {
		permstring="rw-------";
	}
	dbgfileperms=permissions::evalPermString(permstring);
	name=NULL;
	enabled=!charstring::isNo(parameters->getAttributeValue("enabled"));
	loglistener=!charstring::isNo(
			parameters->getAttributeValue("listener"));
	logconnection=!charstring::isNo(
			parameters->getAttributeValue("connection"));
}

sqlrlogger_debug::~sqlrlogger_debug() {
	closeDebugFile();
	delete[] dbgfilename;
}

bool sqlrlogger_debug::init(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon) {

	if (!enabled) {
		return true;
	}

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
	const char	*debugdir=(sqlrcon)?sqlrcon->cont->getDebugDir():
							sqlrl->getDebugDir();
	charstring::printf(&dbgfilename,"%s/sqlr-%s.%ld",
				debugdir,name,(long)process::getProcessId());
	return true;
}

bool sqlrlogger_debug::run(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrlogger_loglevel_t level,
				sqlrevent_t event,
				const char *info) {
	if (!enabled) {
		return true;
	}
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

bool sqlrlogger_debug::openDebugFile() {

	// create the debug file
	dbgfile=new filedestination();

	// open the file
	bool	retval=false;
	if (dbgfile->open(dbgfilename,dbgfileperms)) {
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

void sqlrlogger_debug::closeDebugFile() {
	if (dbgfile) {
		dbgfile->close();
		delete dbgfile;
		dbgfile=NULL;
		delete debuglogger;
		debuglogger=NULL;
	}
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrlogger *new_sqlrlogger_debug(
						sqlrloggers *ls,
						domnode *parameters) {
		return new sqlrlogger_debug(ls,parameters);
	}
}
