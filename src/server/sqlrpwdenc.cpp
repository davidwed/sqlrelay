// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

class sqlrpwdencprivate {
	friend class sqlrpwdenc;
	private:
		domnode	*_parameters;
		bool		_debug;
};

sqlrpwdenc::sqlrpwdenc(domnode *parameters, bool debug) {
	pvt=new sqlrpwdencprivate;
	pvt->_parameters=parameters;
	pvt->_debug=debug;
}

sqlrpwdenc::~sqlrpwdenc() {
	delete pvt;
}

const char *sqlrpwdenc::getId() {
	return pvt->_parameters->getAttributeValue("id");
}

bool sqlrpwdenc::oneWay() {
	return false;
}

char *sqlrpwdenc::encrypt(const char *value) {
	return NULL;
}

char *sqlrpwdenc::decrypt(const char *value) {
	return NULL;
}

domnode *sqlrpwdenc::getParameters() {
	return pvt->_parameters;
}

bool sqlrpwdenc::getDebug() {
	return pvt->_debug;
}
