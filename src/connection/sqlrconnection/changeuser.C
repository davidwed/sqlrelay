// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int	sqlrconnection::changeUser(const char *newuser,
					const char *newpassword) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"change user");
	#endif

	closeCursors(1);
	logOut();
	setUser(newuser);
	setPassword(newpassword);
	return (initCursors(1) && logIn());
}

void	sqlrconnection::setUser(const char *user) {
	this->user=(char *)user;
}

void	sqlrconnection::setPassword(const char *password) {
	this->password=(char *)password;
}
