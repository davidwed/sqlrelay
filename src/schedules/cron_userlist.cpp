// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class SQLRSERVER_DLLSPEC sqlrschedule_cron_userlist : public sqlrschedule {
	public:
		sqlrschedule_cron_userlist(sqlrservercontroller *cont,
							domnode *parameters);

		bool	allowed(sqlrserverconnection *sqlrcon,
						const char *user);
};

sqlrschedule_cron_userlist::sqlrschedule_cron_userlist(
						sqlrservercontroller *cont,
						domnode *parameters) :
					sqlrschedule(cont,parameters) {
}

bool sqlrschedule_cron_userlist::allowed(sqlrserverconnection *sqlrcon,
							const char *user) {

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
	return sqlrschedule::allowed(sqlrcon,user);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrschedule *new_sqlrschedule_cron_userlist(
						sqlrservercontroller *cont,
						domnode *parameters) {
		return new sqlrschedule_cron_userlist(cont,parameters);
	}
}
