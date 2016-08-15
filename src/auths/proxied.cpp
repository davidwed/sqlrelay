// Copyright (c) 2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/charstring.h>

class SQLRSERVER_DLLSPEC sqlrauth_proxied : public sqlrauth {
	public:
			sqlrauth_proxied(xmldomnode *parameters,
						sqlrpwdencs *sqlrpe,
						bool debug);
		const char	*auth(sqlrserverconnection *sqlrcon,
						sqlrcredentials *cred);
	private:
		stringbuffer	lastuser;
		stringbuffer	lastpassword;
};

sqlrauth_proxied::sqlrauth_proxied(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe,
					bool debug) :
					sqlrauth(parameters,sqlrpe,debug) {
}

const char *sqlrauth_proxied::auth(sqlrserverconnection *sqlrcon,
						sqlrcredentials *cred) {

	// this module only supports user/password credentials
	if (charstring::compare(cred->getType(),"userpassword")) {
		return NULL;
	}

	// get the user/password from the creds
	const char	*user=
			((sqlruserpasswordcredentials *)cred)->getUser();
	const char	*password=
			((sqlruserpasswordcredentials *)cred)->getPassword();

	// if the user we want to change to is different from the user
	// that's currently logged in, then try to change to that user
	bool	success=true;
	if ((lastuser.getStringLength()==0 &&
		lastpassword.getStringLength()==0) ||
		charstring::compare(lastuser.getString(),user) ||
		charstring::compare(lastpassword.getString(),password)) {

		// change user
		success=sqlrcon->changeProxiedUser(user,password);

		// keep a record of which user we're changing to
		// and whether that user was successful in auth
		lastuser.clear();
		lastpassword.clear();
		if (success) {
			lastuser.append(user);
			lastpassword.append(password);
		}
	}
	return (success)?user:NULL;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_proxied(
						xmldomnode *users,
						sqlrpwdencs *sqlrpe,
						bool debug) {
		return new sqlrauth_proxied(users,sqlrpe,debug);
	}
}
