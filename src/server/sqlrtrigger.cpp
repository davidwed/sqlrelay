// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrtriggerprivate {
	friend class sqlrtrigger;
	private:
};

sqlrtrigger::sqlrtrigger(sqlrservercontroller *cont,
				domnode *parameters) :
				sqlrservermodule(cont,parameters) {
	pvt=new sqlrtriggerprivate;
}

sqlrtrigger::~sqlrtrigger() {
	delete pvt;
}

bool sqlrtrigger::runBefore(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur) {
	return true;
}

bool sqlrtrigger::runAfter(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur) {
	return true;
}
