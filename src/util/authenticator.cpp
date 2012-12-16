// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <defaults.h>

#include <sqlrconfigfile.h>
#include <sqlrpwdencs.h>

#include <authenticator.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

authenticator::authenticator(sqlrconfigfile *cfgfile, sqlrpwdencs *sqlrpe) {

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

authenticator::~authenticator() {
	for (uint32_t i=0; i<usercount; i++) {
		delete[] users[i];
		delete[] passwords[i];
		delete[] passwordencryptions[i];
	}
	delete[] users;
	delete[] passwords;
	delete[] passwordencryptions;
}

bool authenticator::authenticate(const char *user, const char *password) {

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

				// decrypt the password
				char	*pwd=pe->decrypt(passwords[i]);

				// compare it to the password that was passed in
				bool	retval=!charstring::compare(
								password,pwd);

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
