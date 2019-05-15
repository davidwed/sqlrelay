// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/stringbuffer.h>

class SQLRSERVER_DLLSPEC sqlrauth_mysql_database : public sqlrauth {
	public:
			sqlrauth_mysql_database(sqlrservercontroller *cont,
							sqlrauths *auths,
							sqlrpwdencs *sqlrpe,
							domnode *parameters);
		const char	*auth(sqlrcredentials *cred);
	private:
		bool		first;
		stringbuffer	lastuser;
		stringbuffer	lastpassword;

		bool	debug;
};

sqlrauth_mysql_database::sqlrauth_mysql_database(
					sqlrservercontroller *cont,
					sqlrauths *auths,
					sqlrpwdencs *sqlrpe,
					domnode *parameters) :
					sqlrauth(cont,auths,sqlrpe,parameters) {
	debug=cont->getConfig()->getDebugAuths();
	first=true;
}

const char *sqlrauth_mysql_database::auth(sqlrcredentials *cred) {

	// this module only supports mysql credentials
	if (charstring::compare(cred->getType(),"mysql")) {
		return NULL;
	}

	const char	*user=((sqlrmysqlcredentials *)cred)->getUser();
	const char	*password=((sqlrmysqlcredentials *)cred)->getPassword();
	uint64_t	passwordlength=((sqlrmysqlcredentials *)cred)->
							getPasswordLength();
	const char	*method=((sqlrmysqlcredentials *)cred)->getMethod();
	const char	*extra=((sqlrmysqlcredentials *)cred)->getExtra();

	if (debug) {
		stdoutput.printf("auth %s {\n",method);
		stdoutput.printf("	user: \"%s\"\n",user);
		stdoutput.printf("	password: \"");
		stdoutput.safePrint(password,passwordlength);
		stdoutput.printf("\"\n");
		stdoutput.printf("	method: \"%s\"\n",method);
		stdoutput.printf("	extra: \"%s\"\n",extra);
		stdoutput.printf("}\n");
	}

	// sanity check on method
	if (charstring::compare(method,"mysql_clear_password")) {
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
	if ((lastuser.getStringLength()==0 &&
		lastpassword.getStringLength()==0) ||
		charstring::compare(lastuser.getString(),user) ||
		charstring::compare(lastpassword.getString(),password)) {

		if (debug) {
			stdoutput.printf("auth {\n");
			stdoutput.printf("	changing user to %s\n",user);
			stdoutput.printf("}\n");
		}

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

	} else if (debug) {

		stdoutput.printf("auth {\n");
		stdoutput.printf("	already logged in as %s\n",user);
		stdoutput.printf("}\n");
	}
	return (success)?user:NULL;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_mysql_database(
						sqlrservercontroller *cont,
						sqlrauths *auths,
						sqlrpwdencs *sqlrpe,
						domnode *parameters) {
		return new sqlrauth_mysql_database(cont,auths,
							sqlrpe,parameters);
	}
}
