#include <sqlrelay/sqlrserver.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>

class SQLRSERVER_DLLSPEC hardcoded : public sqlrauth {
	public:
			hardcoded(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe);
		bool	authenticate(const char *user, const char *password);
};

hardcoded::hardcoded(xmldomnode *parameters,
				sqlrpwdencs *sqlrpe) :
				sqlrauth(parameters,sqlrpe) {
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

bool hardcoded::authenticate(const char *user, const char *password) {
	for (const cred_t *c=credentials; c->user; c++) {
		if (!charstring::compare(user,c->user) &&
			!charstring::compare(password,c->password)) {
			return true;
		}
	}
	return false;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_hardcoded(
						xmldomnode *users,
						sqlrpwdencs *sqlrpe) {
		return new hardcoded(users,sqlrpe);
	}
}
