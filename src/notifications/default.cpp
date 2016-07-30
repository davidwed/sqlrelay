// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrnotification_default : public sqlrnotification {
	public:
			sqlrnotification_default(xmldomnode *parameters);
			~sqlrnotification_default();

		bool	init(sqlrlistener *sqlrl, sqlrserverconnection *sqlrcon);
		bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info);
	private:
		bool	enabled;
};

sqlrnotification_default::sqlrnotification_default(xmldomnode *parameters) :
					sqlrnotification(parameters) {
	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
}

sqlrnotification_default::~sqlrnotification_default() {
}

bool sqlrnotification_default::init(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon) {
	if (!enabled) {
		return true;
	}
	return true;
}

bool sqlrnotification_default::run(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrevent_t event,
				const char *info) {
	if (!enabled) {
		return true;
	}
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrnotification *new_sqlrnotification_default(
						xmldomnode *parameters) {
		return new sqlrnotification_default(parameters);
	}
}
