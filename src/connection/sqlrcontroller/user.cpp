// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::setUser(const char *user) {
	this->user=user;
}

void sqlrconnection_svr::setPassword(const char *password) {
	this->password=password;
}

const char *sqlrconnection_svr::getUser() {
	return user;
}

const char *sqlrconnection_svr::getPassword() {
	return password;
}
