// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection::changeUser(const char *newuser,
					const char *newpassword) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"change user");
	#endif

	closeCursors(false);
	logOut();
	setUser(newuser);
	setPassword(newpassword);
	return (logIn() && initCursors(false));
}

void sqlrconnection::setUser(const char *user) {
	this->user=(char *)user;
}

void sqlrconnection::setPassword(const char *password) {
	this->password=(char *)password;
}
