// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrroute_default : public sqlrroute {
	public:
			sqlrroute_default(xmldomnode *parameters);
			~sqlrroute_default();

		bool	init(sqlrserverconnection *sqlrcon);
		bool	route(sqlrserverconnection *sqlrcon);
	private:
		bool	enabled;
};

sqlrroute_default::sqlrroute_default(xmldomnode *parameters) :
					sqlrroute(parameters) {
	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
}

sqlrroute_default::~sqlrroute_default() {
}

bool sqlrroute_default::init(sqlrserverconnection *sqlrcon) {
	if (!enabled) {
		return true;
	}
	return true;
}

bool sqlrroute_default::route(sqlrserverconnection *sqlrcon) {
	if (!enabled) {
		return true;
	}
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrroute *new_sqlrroute_default(
						xmldomnode *parameters) {
		return new sqlrroute_default(parameters);
	}
}
