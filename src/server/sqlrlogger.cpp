// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrloggerprivate {
	friend class sqlrlogger;
	private:
};

sqlrlogger::sqlrlogger(domnode *parameters) : 
			sqlrservermodule(NULL,parameters) {
	pvt=new sqlrloggerprivate;
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
			sqlrloglevel_t level,
			sqlrevent_t event,
			const char *info) {
	return true;
}
