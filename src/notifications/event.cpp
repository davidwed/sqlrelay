// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrnotification_event : public sqlrnotification {
	public:
			sqlrnotification_event(xmldomnode *parameters);
			~sqlrnotification_event();

		bool	init(sqlrlistener *sqlrl, sqlrserverconnection *sqlrcon);
		bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info);
	private:
		bool	enabled;
};

sqlrnotification_event::sqlrnotification_event(xmldomnode *parameters) :
						sqlrnotification(parameters) {

	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");


}

sqlrnotification_event::~sqlrnotification_event() {
}

bool sqlrnotification_event::init(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon) {
	if (!enabled) {
		return true;
	}
	return true;
}

bool sqlrnotification_event::run(sqlrlistener *sqlrl,
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
	SQLRSERVER_DLLSPEC sqlrnotification *new_sqlrnotification_event(
						xmldomnode *parameters) {
		return new sqlrnotification_event(parameters);
	}
}
