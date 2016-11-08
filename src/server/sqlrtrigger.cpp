// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrtriggerprivate {
	friend class sqlrtrigger;
	private:
		xmldomnode	*_parameters;
		bool		_debug;
};

sqlrtrigger::sqlrtrigger(xmldomnode *parameters, bool debug) {
	pvt=new sqlrtriggerprivate;
	pvt->_parameters=parameters;
	pvt->_debug=debug;
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

xmldomnode *sqlrtrigger::getParameters() {
	return pvt->_parameters;
}

bool sqlrtrigger::getDebug() {
	return pvt->_debug;
}
