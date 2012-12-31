// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <defaults.h>

#include <sqlrconfigfile.h>
#include <sqlrpwdencs.h>

#include <sqlrauthenticator.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

sqlrauthenticator::sqlrauthenticator(sqlrconfigfile *cfgfile,
						sqlrpwdencs *sqlrpe) {

	// get the list of users from the config file
	linkedlist< usercontainer * >	*userlist=cfgfile->getUserList();
	usercount=userlist->getLength();

	// create an array of users and passwords and store the
	// users and passwords from the config file in them
	users=new char *[usercount];
	passwords=new char *[usercount];
	passwordencryptions=new char *[usercount];

	usernode	*current=userlist->getFirstNode();
	for (uint32_t i=0; i<usercount; i++) {
		users[i]=charstring::duplicate(
				current->getData()->getUser());
		passwords[i]=charstring::duplicate(
				current->getData()->getPassword());
		passwordencryptions[i]=charstring::duplicate(
				current->getData()->getPasswordEncryption());
		current=current->getNext();
	}

	this->sqlrpe=sqlrpe;
}

sqlrauthenticator::~sqlrauthenticator() {
	for (uint32_t i=0; i<usercount; i++) {
		delete[] users[i];
		delete[] passwords[i];
		delete[] passwordencryptions[i];
	}
	delete[] users;
	delete[] passwords;
	delete[] passwordencryptions;
}

bool sqlrauthenticator::authenticate(const char *user, const char *password) {

	// run through the user/password arrays...
	for (uint32_t i=0; i<usercount; i++) {

		// if the user matches...
		if (!charstring::compare(user,users[i])) {

			if (sqlrpe &&
				charstring::length(passwordencryptions[i])) {

				// if password encryption is being used...

				// get the module
				sqlrpwdenc	*pe=
					sqlrpe->getPasswordEncryptionById(
							passwordencryptions[i]);
				if (!pe) {
					return false;
				}

				// For one-way encryption, encrypt the password
				// that was passed in and compare it to the
				// encrypted password in the config file.
				// For two-way encryption, decrypt the password
				// from the config file and compare ot to the
				// password that was passed in...

				bool	retval=false;
				char	*pwd=NULL;
				if (pe->oneWay()) {

					// encrypt the password
					// that was passed in
					pwd=pe->encrypt(password);

					// compare it to the encrypted
					// password from the config file
					retval=!charstring::compare(
							pwd,passwords[i]);

				} else {

					// decrypt the password
					// from the config file
					pwd=pe->decrypt(passwords[i]);

					// compare it to the password
					// that was passed in
					retval=!charstring::compare(
							password,pwd);
				}

				// clean up
				delete[] pwd;

				// return true/false
				return retval;

			} else {

				// if password encryption isn't being used,
				// return true if the passwords match
				return !charstring::compare(password,
								passwords[i]);
			}
		}
	}
	return false;
}
