// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrrouter_default : public sqlrrouter {
	public:
			sqlrrouter_default(xmldomnode *parameters);
			~sqlrrouter_default();

		bool	init(sqlrserverconnection *sqlrcon);
		bool	route(sqlrserverconnection *sqlrcon);
	private:
		bool	enabled;
};

sqlrrouter_default::sqlrrouter_default(xmldomnode *parameters) :
					sqlrrouter(parameters) {
	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
}

sqlrrouter_default::~sqlrrouter_default() {
}

bool sqlrrouter_default::init(sqlrserverconnection *sqlrcon) {
	if (!enabled) {
		return true;
	}
	return true;
}

bool sqlrrouter_default::route(sqlrserverconnection *sqlrcon) {
	if (!enabled) {
		return true;
	}
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrrouter *new_sqlrrouter_default(
						xmldomnode *parameters) {
		return new sqlrrouter_default(parameters);
	}
}
