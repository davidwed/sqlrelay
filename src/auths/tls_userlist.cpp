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
	tlscontext	*ctx=sqlrcon->cont->getTLSContext();
	if (!ctx) {
		return false;
	}

	// get the peer certificate
	tlscertificate	*cert=ctx->getPeerCertificate();
	if (!cert) {
		return false;
	}

	// get the subject alternate names and common name from the cert
	linkedlist< char * >	*sans=cert->getSubjectAlternateNames();
	const char		*commonname=cert->getCommonName();
	if (!sans->getLength() && !commonname) {
		return false;
	}

	// run through the user/password arrays...
	for (uint32_t i=0; i<usercount; i++) {

		if (sans->getLength()) {

			// if subject alternate names were
			// present then validate against those
			for (linkedlistnode< char * > *node=sans->getFirst();
						node; node=node->getNext()) {
				if (!charstring::compare(
						node->getValue(),users[i])) {
					return true;
				}
			}

		} else {

			// if no subject alternate names were present then
			// validate against the common name
			if (!charstring::compare(commonname,users[i])) {
				return true;
			}
		}
	}
	return false;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_tls_userlist(
						xmldomnode *users,
						sqlrpwdencs *sqlrpe) {
		return new sqlrauth_tls_userlist(users,sqlrpe);
	}
}
