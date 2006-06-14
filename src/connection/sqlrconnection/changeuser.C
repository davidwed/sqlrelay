// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::changeUser(const char *newuser,
					const char *newpassword) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"change user");
	#endif

	closeCursors(false);
	logOut();
	setUser(newuser);
	setPassword(newpassword);
	return (logInUpdateStats() && initCursors(false));
}

void sqlrconnection_svr::setUser(const char *user) {
	this->user=(char *)user;
}

void sqlrconnection_svr::setPassword(const char *password) {
	this->password=(char *)password;
}
