// Copyright (c) 2014-2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/stdio.h>

class SQLRSERVER_DLLSPEC sqlrauth_krb_userlist : public sqlrauth {
	public:
			sqlrauth_krb_userlist(xmldomnode *parameters,
						sqlrpwdencs *sqlrpe,
						bool debug);
		bool	auth(sqlrserverconnection *sqlrcon,
					const char *user, const char *password);
	private:
		const char	**users;
		uint64_t	usercount;
};

sqlrauth_krb_userlist::sqlrauth_krb_userlist(xmldomnode *parameters,
						sqlrpwdencs *sqlrpe,
						bool debug) :
					sqlrauth(parameters,sqlrpe,debug) {

	users=NULL;
	usercount=parameters->getChildCount();
	if (!usercount) {
		return;
	}

	// Create an array of users and store the users from the configuration
	// in them.  This is faster than running through the xml over and over.
	users=new const char *[usercount];

	xmldomnode *user=parameters->getFirstTagChild("user");
	for (uint64_t i=0; i<usercount; i++) {
		users[i]=user->getAttributeValue("user");
		user=user->getNextTagSibling("user");
	}
}

bool sqlrauth_krb_userlist::auth(sqlrserverconnection *sqlrcon,
						const char *user,
						const char *password) {

	// get the security context
	gsscontext	*ctx=sqlrcon->cont->getGSSContext();
	if (!ctx) {
		return false;
	}

	// get the initiating principal
	const char	*initiator=ctx->getInitiator();
	if (charstring::isNullOrEmpty(initiator)) {
		return false;
	}

	// run through the user/password arrays...
	for (uint32_t i=0; i<usercount; i++) {

		// if the initiator matches...
		if (!charstring::compare(initiator,users[i])) {
			return true;
		}
	}
	return false;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_krb_userlist(
						xmldomnode *users,
						sqlrpwdencs *sqlrpe,
						bool debug) {
		return new sqlrauth_krb_userlist(users,sqlrpe,debug);
	}
}
