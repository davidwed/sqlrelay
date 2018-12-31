// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrloggerprivate {
	friend class sqlrlogger;
	private:
		sqlrloggers	*_ls;
		domnode	*_parameters;
};

sqlrlogger::sqlrlogger(sqlrloggers *ls, domnode *parameters) {
	pvt=new sqlrloggerprivate;
	pvt->_ls=ls;
	pvt->_parameters=parameters;
}

sqlrlogger::~sqlrlogger() {
	delete pvt;
}

bool sqlrlogger::init(sqlrlistener *sqlrl, sqlrserverconnection *sqlrcon) {
	return true;
}

bool sqlrlogger::run(sqlrlistener *sqlrl,
			sqlrserverconnection *sqlrcon,
			sqlrservercursor *sqlrcur,
			sqlrlogger_loglevel_t level,
			sqlrevent_t event,
			const char *info) {
	return true;
}

sqlrloggers *sqlrlogger::getLoggers() {
	return pvt->_ls;
}

domnode *sqlrlogger::getParameters() {
	return pvt->_parameters;
}

void sqlrlogger::endTransaction(bool commit) {
}

void sqlrlogger::endSession() {
}
