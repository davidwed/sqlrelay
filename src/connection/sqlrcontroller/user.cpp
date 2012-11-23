// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::setUser(const char *user) {
	this->user=user;
}

void sqlrcontroller_svr::setPassword(const char *password) {
	this->password=password;
}

const char *sqlrcontroller_svr::getUser() {
	return user;
}

const char *sqlrcontroller_svr::getPassword() {
	return password;
}
