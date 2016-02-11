// Copyright (c) 2014-2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/stdio.h>

class SQLRSERVER_DLLSPEC sqlrauth_tls_userlist : public sqlrauth {
	public:
			sqlrauth_tls_userlist(xmldomnode *parameters,
						sqlrpwdencs *sqlrpe);
		bool	auth(sqlrserverconnection *sqlrcon,
					const char *user, const char *password);
	private:
		const char	**users;
		uint64_t	usercount;
};

sqlrauth_tls_userlist::sqlrauth_tls_userlist(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe) :
					sqlrauth(parameters,sqlrpe) {

	users=NULL;
	usercount=parameters->getChildCount();
	if (!usercount) {
		return;
	}

	// Create an array of users and store the users from the configuration
	// in them.  This is faster than running through the xml over and over.
	users=new const char *[usercount];

	xmldomnode *user=parameters->getFirstTagChild("user");
	for (uint64_t i=0; i<usercount; i++) {
		users[i]=user->getAttributeValue("user");
		user=user->getNextTagSibling("user");
	}
}

bool sqlrauth_tls_userlist::auth(sqlrserverconnection *sqlrcon,
						const char *user,
						const char *password) {

	// get the security context
	tlsservercontext	*ctx=sqlrcon->cont->getTLSContext();
	if (!ctx) {
stdoutput.printf("auth failed: no ctx\n");
		return false;
	}

	// validate the peer certificate
	if (!ctx->peerCertificateIsValid()) {
stdoutput.printf("auth failed: invalid cert\n");
		return false;
	}

	// get the peer certificate
	tlscertificate	*cert=ctx->getPeerCertificate();
	if (!cert) {
stdoutput.printf("auth failed: no cert\n");
		return false;
	}

	// get the common name from the cert
	const char	*commonname=cert->getCommonName();
	if (!commonname) {
stdoutput.printf("auth failed: no common name\n");
		return false;
	}

	// run through the user/password arrays...
	for (uint32_t i=0; i<usercount; i++) {

		// if the common name matches...
		if (!charstring::compare(commonname,users[i])) {
			return true;
		}
	}
stdoutput.printf("auth failed: \"%s\" not a valid common name\n",commonname);
	return false;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_tls_userlist(
						xmldomnode *users,
						sqlrpwdencs *sqlrpe) {
		return new sqlrauth_tls_userlist(users,sqlrpe);
	}
}
