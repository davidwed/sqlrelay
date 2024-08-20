// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrnotification_events : public sqlrnotification {
	public:
		sqlrnotification_events(domnode *parameters);

		bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info);
	private:
		domnode	*eventsnode;
		domnode	*recipientsnode;
};

sqlrnotification_events::sqlrnotification_events(domnode *parameters) :
					sqlrnotification(parameters) {
	eventsnode=parameters->getFirstTagChild("events");
	recipientsnode=parameters->getFirstTagChild("recipients");
}

bool sqlrnotification_events::run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info) {

	// for each event...
	for (domnode *enode=eventsnode->getFirstTagChild("event");
			!enode->isNullNode();
			enode=enode->getNextTagSibling("event")) {

		// get the event string (and fudge it if necessary)
		const char	*eventstr=enode->getAttributeValue("event");
		if (!charstring::compare(eventstr,"query")) {
			eventstr="query_executed";
		}

		// do we care about this event?
		if (event!=((sqlrl)?sqlrl->getEventType(eventstr):
				sqlrcon->cont->getEventType(eventstr))) {
			continue;
		}

		// do we care about this query
		if (event==SQLREVENT_QUERY_EXECUTED) {
			const char	*pattern=
					enode->getAttributeValue("pattern");
			if (!charstring::isNullOrEmpty(pattern)) {
				if (!regularexpression::match(
					sqlrcon->cont->getCurrentQuery(),
					pattern)) {
					continue;
				}
			}
		}
	

		// for each recipient...
		for (domnode *rnode=recipientsnode->
					getFirstTagChild("recipient");
			!rnode->isNullNode();
			rnode=rnode->getNextTagSibling("recipient")) {

			// send the notification
			// FIXME: this can fail...
			sendNotification(sqlrl,sqlrcon,sqlrcur,
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
							domnode *parameters) {
		return new sqlrnotification_events(parameters);
	}
}
