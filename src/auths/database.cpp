// Copyright (c) 2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

class SQLRSERVER_DLLSPEC sqlrauth_database : public sqlrauth {
	public:
			sqlrauth_database(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe);
		bool	auth(sqlrserverconnection *sqlrcon,
					const char *user, const char *password);
};

sqlrauth_database::sqlrauth_database(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe) :
					sqlrauth(parameters,sqlrpe) {
}

bool sqlrauth_database::auth(sqlrserverconnection *sqlrcon,
				const char *user, const char *password) {
	return sqlrcon->cont->databaseBasedAuth(user,password);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_database(
						xmldomnode *users,
						sqlrpwdencs *sqlrpe) {
		return new sqlrauth_database(users,sqlrpe);
	}
}
