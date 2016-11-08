// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <config.h>

sqlrcredentials::sqlrcredentials() {

}

sqlrcredentials::~sqlrcredentials() {
}


class sqlruserpasswordcredentialsprivate {
	friend class sqlruserpasswordcredentials;
	private:
		const char	*_user;
		const char	*_password;
};

sqlruserpasswordcredentials::sqlruserpasswordcredentials() : sqlrcredentials() {
	pvt=new sqlruserpasswordcredentialsprivate;
	pvt->_user=NULL;
	pvt->_password=NULL;
}

sqlruserpasswordcredentials::~sqlruserpasswordcredentials() {
	delete pvt;
}

const char *sqlruserpasswordcredentials::getType() {
	return "userpassword";
}

void sqlruserpasswordcredentials::setUser(const char *user) {
	pvt->_user=user;
}

void sqlruserpasswordcredentials::setPassword(const char *password) {
	pvt->_password=password;
}

const char *sqlruserpasswordcredentials::getUser() {
	return pvt->_user;
}

const char *sqlruserpasswordcredentials::getPassword() {
	return pvt->_password;
}


class sqlrgsscredentialsprivate {
	friend class sqlrgsscredentials;
	private:
		const char	*_initiator;
};

sqlrgsscredentials::sqlrgsscredentials() : sqlrcredentials() {
	pvt=new sqlrgsscredentialsprivate;
	pvt->_initiator=NULL;
}

sqlrgsscredentials::~sqlrgsscredentials() {
	delete pvt;
}

const char *sqlrgsscredentials::getType() {
	return "gss";
}

void sqlrgsscredentials::setInitiator(const char *initiator) {
	pvt->_initiator=initiator;
}

const char *sqlrgsscredentials::getInitiator() {
	return pvt->_initiator;
}

class sqlrtlscredentialsprivate {
	friend class sqlrtlscredentials;
	private:
		const char		*_commonname;
		linkedlist< char * >	*_subjectalternatenames;
};

sqlrtlscredentials::sqlrtlscredentials() : sqlrcredentials() {
	pvt=new sqlrtlscredentialsprivate;
	pvt->_commonname=NULL;
	pvt->_subjectalternatenames=NULL;
}


sqlrtlscredentials::~sqlrtlscredentials() {
	delete pvt;
}

const char *sqlrtlscredentials::getType() {
	return "tls";
}

void sqlrtlscredentials::setCommonName(const char *commonname) {
	pvt->_commonname=commonname;
}

void sqlrtlscredentials::setSubjectAlternateNames(
				linkedlist< char * > *subjectalternatenames) {
	pvt->_subjectalternatenames=subjectalternatenames;
}

const char *sqlrtlscredentials::getCommonName() {
	return pvt->_commonname;
}

linkedlist< char * > *sqlrtlscredentials::getSubjectAlternateNames() {
	return pvt->_subjectalternatenames;
}
