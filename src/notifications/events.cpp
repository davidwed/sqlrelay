// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrnotification_events : public sqlrnotification {
	public:
			sqlrnotification_events(sqlrnotifications *ns,
						xmldomnode *parameters);

		bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info);
	private:
		bool		enabled;
		xmldomnode	*eventsnode;
		xmldomnode	*recipientsnode;
};

sqlrnotification_events::sqlrnotification_events(sqlrnotifications *ns,
						xmldomnode *parameters) :
					sqlrnotification(ns,parameters) {
	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
	eventsnode=parameters->getFirstTagChild("events");
	recipientsnode=parameters->getFirstTagChild("recipients");
}

bool sqlrnotification_events::run(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrevent_t event,
				const char *info) {
	if (!enabled) {
		return true;
	}

	// for each event...
	for (xmldomnode *enode=eventsnode->getFirstTagChild("event");
			!enode->isNullNode();
			enode=enode->getNextTagSibling("event")) {

		// do we care about this event?
		if (event!=ns->eventType(enode->getAttributeValue("event"))) {
			continue;
		}

		// for each recipient...
		for (xmldomnode *rnode=recipientsnode->
					getFirstTagChild("recipient");
			!rnode->isNullNode();
			rnode=rnode->getNextTagSibling("recipient")) {

			// send the notification
			// FIXME: this can fail...
			ns->sendNotification(
					rnode->getAttributeValue("address"),
					rnode->getAttributeValue("transportid"),
					enode->getAttributeValue("subject"),
					enode->getAttributeValue("template"),
					event,info);
		}
	}
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrnotification *new_sqlrnotification_events(
						sqlrnotifications *ns,
						xmldomnode *parameters) {
		return new sqlrnotification_events(ns,parameters);
	}
}
