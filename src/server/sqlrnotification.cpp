// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrnotificationprivate {
	friend class sqlrnotification;
	private:
		sqlrnotifications	*_ns;
		xmldomnode		*_parameters;
};

sqlrnotification::sqlrnotification(sqlrnotifications *ns,
					xmldomnode *parameters) {
	pvt=new sqlrnotificationprivate;
	pvt->_ns=ns;
	pvt->_parameters=parameters;
}

sqlrnotification::~sqlrnotification() {
	delete pvt;
}

bool sqlrnotification::run(sqlrlistener *sqlrl,
			sqlrserverconnection *sqlrcon,
			sqlrservercursor *sqlrcur,
			sqlrevent_t event,
			const char *info) {
	return true;
}

sqlrnotifications *sqlrnotification::getNotifications() {
	return pvt->_ns;
}

xmldomnode *sqlrnotification::getParameters() {
	return pvt->_parameters;
}
