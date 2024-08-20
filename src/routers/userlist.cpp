// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/linkedlist.h>

class SQLRSERVER_DLLSPEC sqlrrouter_userlist : public sqlrrouter {
	public:
		sqlrrouter_userlist(sqlrservercontroller *cont,
						sqlrrouters *rs,
						domnode *parameters);
		~sqlrrouter_userlist();

		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn);
		bool	routeEntireSession();
	private:
		const char	*connid;

		const char	**users;
		uint64_t	usercount;
};

sqlrrouter_userlist::sqlrrouter_userlist(sqlrservercontroller *cont,
						sqlrrouters *rs,
						domnode *parameters) :
					sqlrrouter(cont,rs,parameters) {
	users=NULL;

	connid=parameters->getAttributeValue("connectionid");

	// this is faster than running through the xml over and over
	usercount=parameters->getChildCount();
	users=new const char *[usercount];
	domnode *user=parameters->getFirstTagChild("user");
	for (uint64_t i=0; i<usercount; i++) {
		users[i]=user->getAttributeValue("user");
		user=user->getNextTagSibling("user");
	}
}

sqlrrouter_userlist::~sqlrrouter_userlist() {
	delete[] users;
}

const char *sqlrrouter_userlist::route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn) {

	debugWrite("route");

	// get the user
	const char	*user=sqlrcon->cont->getCurrentUser();
	if (charstring::isNullOrEmpty(user)) {
		debugWrite("routing null/empty user");
		return NULL;
	}

	// run through the user array...
	for (uint64_t i=0; i<usercount; i++) {

		// if the user matches...
		if (!charstring::compare(user,users[i]) ||
			!charstring::compare(users[i],"*")) {
			debugWrite("routing user %s to %s",user,connid);
			return connid;
		}
	}

	debugEnd();

	return NULL;
}

bool sqlrrouter_userlist::routeEntireSession() {
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrrouter *new_sqlrrouter_userlist(
						sqlrservercontroller *cont,
						sqlrrouters *rs,
						domnode *parameters) {
		return new sqlrrouter_userlist(cont,rs,parameters);
	}
}
