// Copyright (c) 1999-2018 David Muse
// All rights reserved

#include <config.h>
#include <sqlrelay/sqlrserver.h>

sqlrmysqlcredentials::sqlrmysqlcredentials() : sqlrcredentials() {
	user=NULL;
	password=NULL;
	passwordlength=0;
	method=NULL;
	extra=NULL;
}

sqlrmysqlcredentials::~sqlrmysqlcredentials() {
}

const char *sqlrmysqlcredentials::getType() {
	return "mysql";
}

void sqlrmysqlcredentials::setUser(const char *user) {
	this->user=user;
}

void sqlrmysqlcredentials::setPassword(const char *password) {
	this->password=password;
}

void sqlrmysqlcredentials::setPasswordLength(uint64_t passwordlength) {
	this->passwordlength=passwordlength;
}

void sqlrmysqlcredentials::setMethod(const char *method) {
	this->method=method;
}

void sqlrmysqlcredentials::setExtra(const char *extra) {
	this->extra=extra;
}

const char *sqlrmysqlcredentials::getUser() {
	return user;
}

const char *sqlrmysqlcredentials::getPassword() {
	return password;
}

uint64_t sqlrmysqlcredentials::getPasswordLength() {
	return passwordlength;
}

const char *sqlrmysqlcredentials::getMethod() {
	return method;
}

const char *sqlrmysqlcredentials::getExtra() {
	return extra;
}
