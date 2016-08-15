#include <sqlrelay/sqlrserver.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>

class SQLRSERVER_DLLSPEC hardcoded : public sqlrauth {
	public:
			hardcoded(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe,
					bool debug);
		const char	*authenticate(sqlrserverconnection *conn,
							sqlrcredentials *cred);
};

hardcoded::hardcoded(xmldomnode *parameters,
				sqlrpwdencs *sqlrpe,
				bool debug) :
				sqlrauth(parameters,sqlrpe,debug) {
}

struct cred_t {
	const char	*user;
	const char	*password;
};

static cred_t credentials[]={
	{"userone","passwordone"},
	{"usertwo","passwordtwo"},
	{"userthree","passwordthree"},
	{NULL,NULL}
};

const char *hardcoded::authenticate(sqlrserverconnection *sqlrcon,
						sqlrcredentials *cred) {

	// this module only supports user-password credentials
	if (charstring::compare(cred->getType(),"userpassword")) {
		return NULL;
	}

	// convert "cred" to user-password credentials
	sqlruserpasswordcredentials	*upcred=(sqlruserpasswordcredentials *)cred;

	// if the user matches one of the users in our list, then return the user
	for (const cred_t *c=credentials; c->user; c++) {
		if (!charstring::compare(upcred->getUser(),c->user) &&
			!charstring::compare(upcred->getPassword(),c->password)) {
			return upcred->getUser();
		}
	}

	// otherwise return NULL
	return NULL;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_hardcoded(
						xmldomnode *users,
						sqlrpwdencs *sqlrpe,
						bool debug) {
		return new hardcoded(users,sqlrpe,debug);
	}
}
