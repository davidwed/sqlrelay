// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::changeUser(const char *newuser,
					const char *newpassword) {

	dbgfile.debugPrint("connection",2,"change user");

	closeCursors(false);
	logOutUpdateStats();
	setUser(newuser);
	setPassword(newpassword);
	return (logInUpdateStats(false) && initCursors());
}

void sqlrconnection_svr::setUser(const char *user) {
	this->user=(char *)user;
}

void sqlrconnection_svr::setPassword(const char *password) {
	this->password=(char *)password;
}
