// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrrouter_regex : public sqlrrouter {
	public:
			sqlrrouter_regex(xmldomnode *parameters);
			~sqlrrouter_regex();

		bool	init(sqlrserverconnection *sqlrcon);
		bool	route(sqlrserverconnection *sqlrcon);
	private:
		bool	enabled;
};

sqlrrouter_regex::sqlrrouter_regex(xmldomnode *parameters) :
					sqlrrouter(parameters) {
	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
}

sqlrrouter_regex::~sqlrrouter_regex() {
}

bool sqlrrouter_regex::init(sqlrserverconnection *sqlrcon) {
	if (!enabled) {
		return true;
	}
	return true;
}

bool sqlrrouter_regex::route(sqlrserverconnection *sqlrcon) {
	if (!enabled) {
		return true;
	}
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrrouter *new_sqlrrouter_regex(
						xmldomnode *parameters) {
		return new sqlrrouter_regex(parameters);
	}
}
