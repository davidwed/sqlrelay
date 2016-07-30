// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrschedule_default : public sqlrschedule {
	public:
			sqlrschedule_default(xmldomnode *parameters);
			~sqlrschedule_default();

		bool	init(sqlrserverconnection *sqlrcon);
		bool	allow(sqlrserverconnection *sqlrcon);
	private:
		bool	enabled;
};

sqlrschedule_default::sqlrschedule_default(xmldomnode *parameters) :
					sqlrschedule(parameters) {
	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
}

sqlrschedule_default::~sqlrschedule_default() {
}

bool sqlrschedule_default::init(sqlrserverconnection *sqlrcon) {
	if (!enabled) {
		return true;
	}
	return true;
}

bool sqlrschedule_default::allow(sqlrserverconnection *sqlrcon) {
	if (!enabled) {
		return true;
	}
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrschedule *new_sqlrschedule_default(
						xmldomnode *parameters) {
		return new sqlrschedule_default(parameters);
	}
}
