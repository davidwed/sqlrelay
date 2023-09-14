// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/sensitivevalue.h>

class SQLRSERVER_DLLSPEC sqlrauth_connectstrings : public sqlrauth {
	public:
			sqlrauth_connectstrings(sqlrservercontroller *cont,
						sqlrauths *auths,
						sqlrpwdencs *sqlrpe,
						domnode *parameters);
			~sqlrauth_connectstrings();

		const char	*auth(sqlrcredentials *cred);
	private:
		const char	*userPassword(const char *user,
						const char *password,
						uint64_t index);
		const char	**users;
		char		**passwords;
		const char	**passwordencryptions;
		uint64_t	usercount;

		sensitivevalue	passwordvalue;
};

sqlrauth_connectstrings::sqlrauth_connectstrings(sqlrservercontroller *cont,
					sqlrauths *auths,
					sqlrpwdencs *sqlrpe,
					domnode *parameters) :
					sqlrauth(cont,auths,sqlrpe,parameters) {

	linkedlist< connectstringcontainer * >	*connectstrings=
				cont->getConfig()->getConnectStringList();

	users=NULL;
	passwords=NULL;
	passwordencryptions=NULL;
	usercount=connectstrings->getCount();
	if (!usercount) {
		return;
	}

	// create an array of users and passwords and store the
	// users and passwords from the configuration in them
	// this is faster than running through the xml over and over
	users=new const char *[usercount];
	passwords=new char *[usercount];
	passwordencryptions=new const char *[usercount];

	passwordvalue.setPath(cont->getConfig()->getPasswordPath());

	uint64_t	i=0;
	for (listnode< connectstringcontainer * > *node=
				connectstrings->getFirst();
				node; node=node->getNext()) {

		users[i]=node->getValue()->
				getConnectStringValue("user");
		passwordvalue.parse(node->getValue()->
				getConnectStringValue("password"));
		passwords[i]=passwordvalue.detachTextValue();

		passwordencryptions[i]=node->getValue()->
						getPasswordEncryption();

		i++;
	}
}

sqlrauth_connectstrings::~sqlrauth_connectstrings() {
	delete[] users;
	for (uint64_t i=0; i<usercount; i++) {
		delete[] passwords[i];
	}
	delete[] passwords;
	delete[] passwordencryptions;
}

const char *sqlrauth_connectstrings::auth(sqlrcredentials *cred) {

	// this module only supports user/password credentials
	if (charstring::compare(cred->getType(),"userpassword")) {
		return NULL;
	}

	// get the user/password from the creds
	const char	*user=
			((sqlruserpasswordcredentials *)cred)->getUser();
	const char	*password=
			((sqlruserpasswordcredentials *)cred)->getPassword();

	// run through the user/password arrays...
	for (uint64_t i=0; i<usercount; i++) {
		const char	*result=userPassword(user,password,i);
		if (result) {
			return result;
		}
	}
	return NULL;
}

const char *sqlrauth_connectstrings::userPassword(const char *user,
						const char *password,
						uint64_t index) {

	// bail if the user doesn't match
	if (charstring::compare(user,users[index])) {
		return NULL;
	}

	// if password encryption is being used...
	if (getPasswordEncryptions() &&
		charstring::getLength(passwordencryptions[index])) {

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
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_connectstrings(
						sqlrservercontroller *cont,
						sqlrauths *auths,
						sqlrpwdencs *sqlrpe,
						domnode *parameters) {
		return new sqlrauth_connectstrings(
					cont,auths,sqlrpe,parameters);
	}
}
