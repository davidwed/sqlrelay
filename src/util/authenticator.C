// Copyright (c) 2001  David Muse
// See the file COPYING for more information

#include <sys/types.h>
#include <unistd.h>

#include <defaults.h>

#include <sqlrconfigfile.h>

#include <string.h>

#include <authenticator.h>

authenticator::authenticator(sqlrconfigfile *cfgfile) {

	// get the list of users from the config file
	usernode	*currentnode=cfgfile->getUsers();
	usercount=cfgfile->getUserCount();

	if (usercount==0) {

		// if no users were listed in the config file,
		// create a default set
		users=new char *[1];
		passwords=new char *[1];

		users[0]=strdup(DEFAULT_USER);
		passwords[0]=strdup(DEFAULT_PASSWORD);

		usercount=1;

	} else {

		// create an array of users and passwords and store the
		// users and passwords from the config file in them
		users=new char *[usercount];
		passwords=new char *[usercount];

		for (int i=0; i<usercount; i++) {

			users[i]=strdup(currentnode->getUser());
			passwords[i]=strdup(currentnode->getPassword());

			currentnode=currentnode->getNext();
		}
	}
}

authenticator::~authenticator() {
	for (int i=0; i<usercount; i++) {
		delete[] users[i];
		delete[] passwords[i];
	}
	if (users) {
		delete[] users;
	}
	if (passwords) {
		delete[] passwords;
	}
}

int	authenticator::authenticate(const char *user, const char *password) {

	// Return 1 if what the client sent matches one of the 
	// user/password sets and 0 if no match is found.
	for (int i=0; i<usercount; i++) {
		if (!strcmp(user,users[i]) && !strcmp(password,passwords[i])) {
			return 1;
		}
	}
	return 0;
}
