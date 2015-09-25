// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>

class SQLRSERVER_DLLSPEC sqlrelay : public sqlrauth {
	public:
			sqlrelay(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe);
			~sqlrelay();
		bool	authenticate(sqlrserverconnection *sqlrcon,
					const char *user, const char *password);
	private:
		const char	*host;
		uint16_t	port;
		const char	*socket;
		const char	*user;
		const char	*password;
		const char	*table;
		const char	*usercolumn;
		const char	*passwordcolumn;
		const char	*passwordfunction;
		const char	*debug;

		stringbuffer	query;

		sqlrconnection	*sqlrcon;
		sqlrcursor	*sqlrcur;
};

sqlrelay::sqlrelay(xmldomnode *parameters,
				sqlrpwdencs *sqlrpe) :
				sqlrauth(parameters,sqlrpe) {

	host=parameters->getAttributeValue("host");
	port=charstring::toInteger(parameters->getAttributeValue("port"));
	socket=parameters->getAttributeValue("socket");
	user=parameters->getAttributeValue("user");
	password=parameters->getAttributeValue("password");
	table=parameters->getAttributeValue("table");
	usercolumn=parameters->getAttributeValue("usercolumn");
	passwordcolumn=parameters->getAttributeValue("passwordcolumn");
	passwordfunction=parameters->getAttributeValue("passwordfunction");
	debug=parameters->getAttributeValue("debug");

	sqlrcon=new sqlrconnection(host,port,socket,user,password,0,1);

	if (!charstring::compareIgnoringCase(debug,"on")) {
		sqlrcon->debugOn();
	} else if (debug && debug[0] &&
			charstring::compareIgnoringCase(debug,"off")) {
		sqlrcon->debugOn();
		sqlrcon->setDebugFile(debug);
	}

	sqlrcur=new sqlrcursor(sqlrcon);

	const char	*bind1=":1";
	const char	*bind2=":2";
	const char	*db=sqlrcon->identify();
	if (!charstring::compare(db,"db2") ||
			!charstring::compare(db,"informix") ||
			!charstring::compare(db,"firebird") ||
			!charstring::compare(db,"mysql")) {
		bind1="?";
		bind2="?";
	} else if (!charstring::compare(db,"freetds") ||
			!charstring::compare(db,"sybase")) {
		bind1="@1";
		bind2="@2";
	} else if (!charstring::compare(db,"postgresql")) {
		bind1="$1";
		bind2="$2";
	}

	query.append("select count(*) from ")->append(table);
	query.append(" where ");
	query.append(usercolumn)->append("=")->append(bind1);
	query.append(" and ");
	query.append(passwordcolumn)->append("=");
	if (passwordfunction && passwordfunction[0]) {
		query.append(passwordfunction)->append('(');
	}
	query.append(bind2);
	if (passwordfunction && passwordfunction[0]) {
		query.append(')');
	}

	sqlrcur->prepareQuery(query.getString());
}

sqlrelay::~sqlrelay() {
	delete sqlrcur;
	delete sqlrcon;
}

bool sqlrelay::authenticate(sqlrserverconnection *sqlrcon,
				const char *user, const char *password) {

	sqlrcur->inputBind("1",user);
	sqlrcur->inputBind("2",password);
	bool	retval=(sqlrcur->executeQuery() &&
			sqlrcur->rowCount() &&
			sqlrcur->getFieldAsInteger(0,(uint32_t)0));
	sqlrcon->endSession();
	return retval;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_sqlrelay(
						xmldomnode *users,
						sqlrpwdencs *sqlrpe) {
		return new sqlrelay(users,sqlrpe);
	}
}
