// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <defaults.h>

#include <sqlrconfigfile.h>

#include <authenticator.h>

authenticator::authenticator(sqlrconfigfile *cfgfile) {

	// get the list of users from the config file
	linkedlist< usercontainer * >	*userlist=cfgfile->getUserList();
	usercount=userlist->getLength();

	// create an array of users and passwords and store the
	// users and passwords from the config file in them
	users=new char *[usercount];
	passwords=new char *[usercount];

	usernode	*current=userlist->getNodeByIndex(0);
	for (uint32_t i=0; i<usercount; i++) {
		users[i]=charstring::duplicate(current->
						getData()->getUser());
		passwords[i]=charstring::duplicate(current->
						getData()->getPassword());
		current=current->getNext();
	}
}

authenticator::~authenticator() {
	for (uint32_t i=0; i<usercount; i++) {
		delete[] users[i];
		delete[] passwords[i];
	}
	delete[] users;
	delete[] passwords;
}

bool authenticator::authenticate(const char *user, const char *password) {

	// Return true if what the client sent matches one of the 
	// user/password sets and false if no match is found.
	for (uint32_t i=0; i<usercount; i++) {
		if (!charstring::compare(user,users[i]) &&
			!charstring::compare(password,passwords[i])) {
			return true;
		}
	}
	return false;
}
