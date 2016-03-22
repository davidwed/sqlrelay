// Copyright (c) 2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

class SQLRSERVER_DLLSPEC sqlrauth_proxied : public sqlrauth {
	public:
			sqlrauth_proxied(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe);
		bool	auth(sqlrserverconnection *sqlrcon,
					const char *user, const char *password);
};

sqlrauth_proxied::sqlrauth_proxied(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe) :
					sqlrauth(parameters,sqlrpe) {
}

bool sqlrauth_proxied::auth(sqlrserverconnection *sqlrcon,
				const char *user, const char *password) {

	sqlrservercontroller	*cont=sqlrcon->cont;

	// if the user we want to change to is different from the user
	// that's currently beign proxied, try to change to that user
	bool	success=true;
	const char	*lastuser=cont->getLastUser();
	const char	*lastpassword=cont->getLastPassword();
	if ((charstring::isNullOrEmpty(lastuser) &&
		charstring::isNullOrEmpty(lastpassword)) || 
		charstring::compare(lastuser,user) ||
		charstring::compare(lastpassword,password)) {

		// change user
		success=sqlrcon->changeProxiedUser(user,password);

		// keep a record of which user we're changing to
		// and whether that user was successful in auth
		cont->setLastUser((success)?user:NULL);
		cont->setLastPassword((success)?password:NULL);
	}
	return success;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_proxied(
						xmldomnode *users,
						sqlrpwdencs *sqlrpe) {
		return new sqlrauth_proxied(users,sqlrpe);
	}
}
