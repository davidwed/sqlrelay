// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>

class SQLRSERVER_DLLSPEC sqlrauth_sqlrelay : public sqlrauth {
	public:
			sqlrauth_sqlrelay(sqlrservercontroller *cont,
						sqlrauths *auths,
						sqlrpwdencs *sqlrpe,
						domnode *parameters);
			~sqlrauth_sqlrelay();
		const char	*auth(sqlrcredentials *cred);
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
		const char	*sqlrdebug;

		stringbuffer	query;

		sqlrconnection	*sqlrcon;
		sqlrcursor	*sqlrcur;
};

sqlrauth_sqlrelay::sqlrauth_sqlrelay(sqlrservercontroller *cont,
					sqlrauths *auths,
					sqlrpwdencs *sqlrpe,
					domnode *parameters) :
					sqlrauth(cont,auths,sqlrpe,parameters) {

	host=parameters->getAttributeValue("host");
	port=charstring::toInteger(parameters->getAttributeValue("port"));
	socket=parameters->getAttributeValue("socket");
	user=parameters->getAttributeValue("user");
	password=parameters->getAttributeValue("password");
	table=parameters->getAttributeValue("table");
	usercolumn=parameters->getAttributeValue("usercolumn");
	passwordcolumn=parameters->getAttributeValue("passwordcolumn");
	passwordfunction=parameters->getAttributeValue("passwordfunction");
	sqlrdebug=parameters->getAttributeValue("debug");

	sqlrcon=new sqlrconnection(host,port,socket,user,password,0,1);

	if (!charstring::compareIgnoringCase(sqlrdebug,"on")) {
		sqlrcon->debugOn();
	} else if (!charstring::isNullOrEmpty(sqlrdebug) &&
			charstring::compareIgnoringCase(sqlrdebug,"off")) {
		sqlrcon->debugOn();
		sqlrcon->setDebugFile(sqlrdebug);
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
	if (!charstring::isNullOrEmpty(passwordfunction)) {
		query.append(passwordfunction)->append('(');
	}
	query.append(bind2);
	if (!charstring::isNullOrEmpty(passwordfunction)) {
		query.append(')');
	}

	sqlrcur->prepareQuery(query.getString());
}

sqlrauth_sqlrelay::~sqlrauth_sqlrelay() {
	delete sqlrcur;
	delete sqlrcon;
}

const char *sqlrauth_sqlrelay::auth(sqlrcredentials *cred) {

	// this module only supports user/password credentials
	if (charstring::compare(cred->getType(),"userpassword")) {
		return NULL;
	}


	// get the user/password from the creds
	const char	*user=
			((sqlruserpasswordcredentials *)cred)->getUser();
	const char	*password=
			((sqlruserpasswordcredentials *)cred)->getPassword();

	sqlrcur->inputBind("1",user);
	sqlrcur->inputBind("2",password);
	bool	success=(sqlrcur->executeQuery() &&
				sqlrcur->rowCount() &&
				sqlrcur->getFieldAsInteger(0,(uint32_t)0));
	this->sqlrcon->endSession();
	return (success)?user:NULL;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_sqlrelay(
						sqlrservercontroller *cont,
						sqlrauths *auths,
						sqlrpwdencs *sqlrpe,
						domnode *parameters) {
		return new sqlrauth_sqlrelay(cont,auths,sqlrpe,parameters);
	}
}
