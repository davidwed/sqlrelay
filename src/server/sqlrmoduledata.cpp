// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrmoduledataprivate {
	friend class sqlrmoduledata;
	private:
		domnode		*_parameters;
		const char	*_moduletype;
		const char	*_id;
};

sqlrmoduledata::sqlrmoduledata(domnode *parameters) {
	pvt=new sqlrmoduledataprivate;
	pvt->_parameters=parameters;
	pvt->_moduletype=parameters->getAttributeValue("module");
	if (charstring::isNullOrEmpty(pvt->_moduletype)) {
		pvt->_moduletype=parameters->getAttributeValue("file");
	}
	pvt->_id=parameters->getAttributeValue("id");
}

sqlrmoduledata::~sqlrmoduledata() {
	delete pvt;
}

const char *sqlrmoduledata::getModuleType() {
	return pvt->_moduletype;
}

const char *sqlrmoduledata::getId() {
	return pvt->_id;
}

domnode *sqlrmoduledata::getParameters() {
	return pvt->_parameters;
}
