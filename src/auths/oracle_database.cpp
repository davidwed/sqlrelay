// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/stringbuffer.h>

class SQLRSERVER_DLLSPEC sqlrauth_oracle_database : public sqlrauth {
	public:
		sqlrauth_oracle_database(sqlrservercontroller *cont,
							domnode *parameters);
		const char	*auth(sqlrcredentials *cred);
	private:
		bool		first;
		stringbuffer	lastuser;
		stringbuffer	lastpassword;
};

sqlrauth_oracle_database::sqlrauth_oracle_database(
					sqlrservercontroller *cont,
					domnode *parameters) :
					sqlrauth(cont,parameters) {
	first=true;
}

const char *sqlrauth_oracle_database::auth(sqlrcredentials *cred) {

	// this module only supports oracle credentials
	if (charstring::compare(cred->getType(),"oracle")) {
		return NULL;
	}

	const char	*user=((sqlroraclecredentials *)cred)->getUser();
	const char	*password=((sqlroraclecredentials *)cred)->getPassword();
	uint64_t	passwordsize=((sqlroraclecredentials *)cred)->
							getPasswordSize();
	const char	*method=((sqlroraclecredentials *)cred)->getMethod();
	const char	*extra=((sqlroraclecredentials *)cred)->getExtra();

	if (getDebug()) {
		debugStart("auth %s",method);
		debugWrite("user: \"%s\"",user);
		stringbuffer	b;
		b.append("password: \"");
		b.safePrint(password,passwordsize);
		b.append("\"");
		debugWrite(b.getString());
		debugWrite("method: \"%s\"",method);
		debugWrite("extra: \"%s\"",extra);
		debugEnd();
	}

	// sanity check on method
	if (charstring::compare(method,"oracle_clear_password")) {
		return NULL;
	}

	// if this is the first time, initialize the lastuser/lastpassword
	// from the user/password that was originally used to log in to the
	// database
	if (first) {
		lastuser.append(cont->getUser());
		lastpassword.append(cont->getPassword());
		first=false;
	}

	// if the user we want to change to is different from the user
	// that's currently logged in, then try to change to that user
	bool	success=true;
	if ((lastuser.getSize()==0 &&
		lastpassword.getSize()==0) ||
		charstring::compare(lastuser.getString(),user) ||
		charstring::compare(lastpassword.getString(),password)) {

		debugStart("auth");
		debugStart("changing user to %s",user);
		debugEnd();

		// change user
		success=cont->changeUser(user,password);

		// keep a record of which user we're changing to
		// and whether that user was successful in auth
		lastuser.clear();
		lastpassword.clear();
		if (success) {
			lastuser.append(user);
			lastpassword.append(password);
		}

	} else {

		debugStart("auth");
		debugWrite("already logged in as %s",user);
		debugEnd();
	}
	return (success)?user:NULL;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_oracle_database(
						sqlrservercontroller *cont,
						domnode *parameters) {
		return new sqlrauth_oracle_database(cont,parameters);
	}
}
