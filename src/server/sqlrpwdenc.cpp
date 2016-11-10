// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

class sqlrpwdencprivate {
	friend class sqlrpwdenc;
	private:
		xmldomnode	*_parameters;
		bool		_debug;
};

sqlrpwdenc::sqlrpwdenc(xmldomnode *parameters, bool debug) {
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

xmldomnode *sqlrpwdenc::getParameters() {
	return pvt->_parameters;
}

bool sqlrpwdenc::getDebug() {
	return pvt->_debug;
}
