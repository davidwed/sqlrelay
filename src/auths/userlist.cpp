// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

class SQLRSERVER_DLLSPEC sqlrauth_userlist : public sqlrauth {
	public:
			sqlrauth_userlist(sqlrservercontroller *cont,
						sqlrauths *auths,
						sqlrpwdencs *sqlrpe,
						domnode *parameters);
			~sqlrauth_userlist();

		const char	*auth(sqlrcredentials *cred);
	private:
		const char	*userPassword(const char *user,
						const char *password,
						uint64_t index);
		const char	**users;
		const char	**passwords;
		const char	**passwordencryptions;
		uint64_t	usercount;
};

sqlrauth_userlist::sqlrauth_userlist(sqlrservercontroller *cont,
					sqlrauths *auths,
					sqlrpwdencs *sqlrpe,
					domnode *parameters) :
					sqlrauth(cont,auths,sqlrpe,parameters) {

	users=NULL;
	passwords=NULL;
	passwordencryptions=NULL;
	usercount=parameters->getChildCount();
	if (!usercount) {
		return;
	}

	// create an array of users and passwords and store the
	// users and passwords from the configuration in them
	// this is faster than running through the xml over and over
	users=new const char *[usercount];
	passwords=new const char *[usercount];
	passwordencryptions=new const char *[usercount];

	domnode *user=parameters->getFirstTagChild("user");
	for (uint64_t i=0; i<usercount; i++) {

		users[i]=user->getAttributeValue("user");
		passwords[i]=user->getAttributeValue("password");

		// support modern "passwordencryptionid" and fall back to
		// older "passwordencryption" attribute
		const char	*pwdencid=
				user->getAttributeValue("passwordencryptionid");
		if (!pwdencid) {
			pwdencid=user->getAttributeValue("passwordencryption");
		}
		passwordencryptions[i]=pwdencid;

		user=user->getNextTagSibling("user");
	}
}

sqlrauth_userlist::~sqlrauth_userlist() {
	delete[] users;
	delete[] passwords;
	delete[] passwordencryptions;
}

const char *sqlrauth_userlist::auth(sqlrcredentials *cred) {

	// this module supports userpassword, gss, and tls credentials
	bool		up=!charstring::compare(cred->getType(),"userpassword");
	bool		gss=!charstring::compare(cred->getType(),"gss");
	bool		tls=!charstring::compare(cred->getType(),"tls");
	const char	*user=NULL;
	const char	*password=NULL;
	const char	*initiator=NULL;
	linkedlist< char * >	*sans=NULL;
	const char		*commonname=NULL;
	if (up) {
		user=((sqlruserpasswordcredentials *)cred)->getUser();
		password=((sqlruserpasswordcredentials *)cred)->getPassword();
	} else if (gss) {
		initiator=((sqlrgsscredentials *)cred)->getInitiator();
	} else if (tls) {
		sans=((sqlrtlscredentials *)cred)->getSubjectAlternateNames();
		commonname=((sqlrtlscredentials *)cred)->getCommonName();
	} else {
		return NULL;
	}

	// run through the user/password arrays...
	for (uint64_t i=0; i<usercount; i++) {
		if (up) {
			const char	*result=userPassword(user,password,i);
			if (result) {
				return result;
			}
		} else if (gss) {
			if (!charstring::compare(initiator,users[i])) {
				return initiator;
			}
		} else if (tls) {
			if (sans && sans->getLength()) {

				// if subject alternate names were
				// present then validate against those
				for (linkedlistnode< char * > *
						node=sans->getFirst();
						node; node=node->getNext()) {
					if (!charstring::compare(
						node->getValue(),users[i])) {
						return node->getValue();
					}
				}

			} else {

				// if no subject alternate names were present
				// then validate against the common name
				if (!charstring::compare(
						commonname,users[i])) {
					return commonname;
				}
			}
		}
	}
	return NULL;
}

const char *sqlrauth_userlist::userPassword(const char *user,
						const char *password,
						uint64_t index) {

	// bail if the user doesn't match
	if (charstring::compare(user,users[index])) {
		return NULL;
	}

	// if password encryption is being used...
	if (getPasswordEncryptions() &&
		charstring::length(passwordencryptions[index])) {

		// get the module
		sqlrpwdenc	*pe=getPasswordEncryptions()->
					getPasswordEncryptionById(
						passwordencryptions[index]);
		if (!pe) {
			return NULL;
		}

		// For one-way encryption, encrypt the password that was passed
		// in and compare it to the encrypted password in the
		// configuration.  For two-way encryption, decrypt the password
		// from the configuration and compare to to the password that
		// was passed in...

		bool	result=false;
		char	*pwd=NULL;
		if (pe->oneWay()) {

			// encrypt the password
			// that was passed in
			pwd=pe->encrypt(password);

			// compare it to the encrypted
			// password from the configuration
			result=!charstring::compare(pwd,passwords[index]);

		} else {

			// decrypt the password
			// from the configuration
			pwd=pe->decrypt(passwords[index]);

			// compare it to the password
			// that was passed in
			result=!charstring::compare(password,pwd);
		}

		// clean up
		delete[] pwd;

		// return the result
		return (result)?user:NULL;
	}

	// if password encryption isn't being used,
	// then return the user if the passwords match
	return (!charstring::compare(password,passwords[index]))?user:NULL;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_userlist(
						sqlrservercontroller *cont,
						sqlrauths *auths,
						sqlrpwdencs *sqlrpe,
						domnode *parameters) {
		return new sqlrauth_userlist(cont,auths,sqlrpe,parameters);
	}
}
