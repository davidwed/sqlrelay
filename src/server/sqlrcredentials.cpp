// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <config.h>

sqlrcredentials::sqlrcredentials() {
}

sqlrcredentials::~sqlrcredentials() {
}


sqlruserpasswordcredentials::sqlruserpasswordcredentials() : sqlrcredentials() {
	user=NULL;
	password=NULL;
}

sqlruserpasswordcredentials::~sqlruserpasswordcredentials() {
}

const char *sqlruserpasswordcredentials::getType() {
	return "userpassword";
}

void sqlruserpasswordcredentials::setUser(const char *user) {
	this->user=user;
}

void sqlruserpasswordcredentials::setPassword(const char *password) {
	this->password=password;
}

const char *sqlruserpasswordcredentials::getUser() {
	return user;
}

const char *sqlruserpasswordcredentials::getPassword() {
	return password;
}


sqlrgsscredentials::sqlrgsscredentials() : sqlrcredentials() {
	initiator=NULL;
}

sqlrgsscredentials::~sqlrgsscredentials() {
}

const char *sqlrgsscredentials::getType() {
	return "gss";
}

void sqlrgsscredentials::setInitiator(const char *initiator) {
	this->initiator=initiator;
}

const char *sqlrgsscredentials::getInitiator() {
	return initiator;
}


sqlrtlscredentials::sqlrtlscredentials() : sqlrcredentials() {
	commonname=NULL;
	subjectalternatenames=NULL;
}


sqlrtlscredentials::~sqlrtlscredentials() {
}

const char *sqlrtlscredentials::getType() {
	return "tls";
}

void sqlrtlscredentials::setCommonName(const char *commonname) {
	this->commonname=commonname;
}

void sqlrtlscredentials::setSubjectAlternateNames(
				linkedlist< char * > *subjectalternatenames) {
	this->subjectalternatenames=subjectalternatenames;
}

const char *sqlrtlscredentials::getCommonName() {
	return commonname;
}

linkedlist< char * > *sqlrtlscredentials::getSubjectAlternateNames() {
	return subjectalternatenames;
}
