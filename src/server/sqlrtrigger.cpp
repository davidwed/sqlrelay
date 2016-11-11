// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrtriggerprivate {
	friend class sqlrtrigger;
	private:
		sqlrtriggers	*_ts;
		xmldomnode	*_parameters;
};

sqlrtrigger::sqlrtrigger(sqlrservercontroller *cont,
				sqlrtriggers *ts,
				xmldomnode *parameters) {
	pvt=new sqlrtriggerprivate;
	pvt->_ts=ts;
	pvt->_parameters=parameters;
}

sqlrtrigger::~sqlrtrigger() {
	delete pvt;
}

bool sqlrtrigger::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				xmldom *querytree,
				bool before,
				bool success) {
	return true;
}

sqlrtriggers *sqlrtrigger::getTriggers() {
	return pvt->_ts;
}

xmldomnode *sqlrtrigger::getParameters() {
	return pvt->_parameters;
}
