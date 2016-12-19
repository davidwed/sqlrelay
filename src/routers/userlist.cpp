// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/linkedlist.h>

class SQLRSERVER_DLLSPEC sqlrrouter_userlist : public sqlrrouter {
	public:
			sqlrrouter_userlist(sqlrservercontroller *cont,
						sqlrrouters *rs,
						xmldomnode *parameters);
			~sqlrrouter_userlist();

		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);
		bool		routeEntireSession();
	private:
		const char	*connectionid;

		const char	**users;
		uint64_t	usercount;

		bool	enabled;

		bool	debug;
};

sqlrrouter_userlist::sqlrrouter_userlist(sqlrservercontroller *cont,
						sqlrrouters *rs,
						xmldomnode *parameters) :
						sqlrrouter(cont,rs,parameters) {
	users=NULL;

	debug=cont->getConfig()->getDebugRouters();
	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
	if (!enabled && debug) {
		stdoutput.printf("	disabled\n");
		return;
	}

	connectionid=parameters->getAttributeValue("connectionid");

	// this is faster than running through the xml over and over
	usercount=parameters->getChildCount();
	users=new const char *[usercount];
	xmldomnode *user=parameters->getFirstTagChild("user");
	for (uint64_t i=0; i<usercount; i++) {
		users[i]=user->getAttributeValue("user");
		user=user->getNextTagSibling("user");
	}
}

sqlrrouter_userlist::~sqlrrouter_userlist() {
	delete[] users;
}

const char *sqlrrouter_userlist::route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {
	if (!enabled) {
		return NULL;
	}

	// get the user
	const char	*user=sqlrcon->cont->getCurrentUser();

	// run through the user array...
	for (uint64_t i=0; i<usercount; i++) {

		// if the user matches...
		if (!charstring::compare(user,users[i]) ||
			!charstring::compare(users[i],"*")) {
			if (debug) {
				stdoutput.printf("\nrouting user %s to %s\n",
							user,connectionid);
			}
			return connectionid;
		}
	}
	return NULL;
}

bool sqlrrouter_userlist::routeEntireSession() {
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrrouter *new_sqlrrouter_userlist(
						sqlrservercontroller *cont,
						sqlrrouters *rs,
						xmldomnode *parameters) {
		return new sqlrrouter_userlist(cont,rs,parameters);
	}
}
