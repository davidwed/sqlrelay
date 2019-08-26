// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class SQLRSERVER_DLLSPEC sqlrschedule_cron_userlist : public sqlrschedule {
	public:
			sqlrschedule_cron_userlist(sqlrservercontroller *cont,
							sqlrschedules *ss,
							domnode *parameters);

		bool	allowed(sqlrserverconnection *sqlrcon,
						const char *user);
	private:
		bool	enabled;
		bool	defaultallow;
};

sqlrschedule_cron_userlist::sqlrschedule_cron_userlist(
						sqlrservercontroller *cont,
						sqlrschedules *ss,
						domnode *parameters) :
					sqlrschedule(cont,ss,parameters) {

	enabled=!charstring::isNo(parameters->getAttributeValue("enabled"));
	defaultallow=charstring::compareIgnoringCase(
			parameters->getAttributeValue("default"),"deny");

	// parse the rules
	for (domnode *r=parameters->
				getFirstTagChild("rules")->
				getFirstTagChild();
			!r->isNullNode();
			r=r->getNextTagSibling()) {

		if (!charstring::compare(r->getName(),"allow") ||
			!charstring::compare(r->getName(),"deny")) {

			addRule(charstring::compareIgnoringCase(
						r->getName(),"deny"),
						r->getAttributeValue("when"));
		}
	}
}

bool sqlrschedule_cron_userlist::allowed(sqlrserverconnection *sqlrcon,
							const char *user) {

	if (!enabled) {
		debugPrintf("module disabled\n");
		return true;
	}

	// do we care about this user?
	debugPrintf("user...\n");

	bool	found=false;
	for (domnode *u=getParameters()->
				getFirstTagChild("users")->
				getFirstTagChild("user");
			!u->isNullNode();
			u=u->getNextTagSibling("user")) {

		const char	*userattr=u->getAttributeValue("user");

		debugPrintf("	%s=%s - ",user,userattr);

		if (!charstring::compare(user,userattr) ||
			!charstring::compare(userattr,"*")) {
			found=true;
			debugPrintf("yes\n");
			break;
		}

		debugPrintf("no\n");
	}

	if (!found) {
		debugPrintf("	user not found, not applying any schedule\n\n");
		return true;
	}

	// compare date/time to schedule rules
	datetime	dt;
	dt.getSystemDateAndTime();
	return rulesAllow(&dt,defaultallow);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrschedule *new_sqlrschedule_cron_userlist(
						sqlrservercontroller *cont,
						sqlrschedules *ss,
						domnode *parameters) {
		return new sqlrschedule_cron_userlist(cont,ss,parameters);
	}
}
