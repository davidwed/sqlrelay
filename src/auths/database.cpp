// Copyright (c) 2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

class SQLRSERVER_DLLSPEC database : public sqlrauth {
	public:
			database(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe);
		bool	authenticate(sqlrserverconnection *sqlrcon,
					const char *user, const char *password);
};

database::database(xmldomnode *parameters,
				sqlrpwdencs *sqlrpe) :
				sqlrauth(parameters,sqlrpe) {
}

bool database::authenticate(sqlrserverconnection *sqlrcon,
				const char *user, const char *password) {
	return sqlrcon->cont->databaseBasedAuth(user,password);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_database(
						xmldomnode *users,
						sqlrpwdencs *sqlrpe) {
		return new database(users,sqlrpe);
	}
}
