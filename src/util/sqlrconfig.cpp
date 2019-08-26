// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>

#include <config.h>
#include <defaults.h>

sqlrconfig::sqlrconfig() {
}

sqlrconfig::~sqlrconfig() {
}


connectstringcontainer::connectstringcontainer() {
	connectionid=NULL;
	string=NULL;
	metric=charstring::toInteger(DEFAULT_METRIC);
	behindloadbalancer=charstring::isYes(DEFAULT_BEHINDLOADBALANCER);
	pwdenc=NULL;
}

connectstringcontainer::~connectstringcontainer() {
	delete[] string;
	delete[] connectionid;
	delete[] pwdenc;
}

void connectstringcontainer::setConnectionId(const char *connectionid) {
	this->connectionid=charstring::duplicate(connectionid);
}

void connectstringcontainer::setString(const char *string) {
	this->string=charstring::duplicate(string);
}

void connectstringcontainer::setMetric(uint32_t metric) {
	this->metric=metric;
}

void connectstringcontainer::setBehindLoadBalancer(bool behindloadbalancer) {
	this->behindloadbalancer=behindloadbalancer;
}

void connectstringcontainer::setPasswordEncryption(const char *pwdenc) {
	this->pwdenc=charstring::duplicate(pwdenc);
}

const char *connectstringcontainer::getConnectionId() {
	return connectionid;
}

const char *connectstringcontainer::getString() {
	return string;
}

uint32_t connectstringcontainer::getMetric() {
	return metric;
}

bool connectstringcontainer::getBehindLoadBalancer() {
	return behindloadbalancer;
}

const char *connectstringcontainer::getPasswordEncryption() {
	return pwdenc;
}

void connectstringcontainer::parseConnectString() {
	connectstring.parse(string);
}

const char *connectstringcontainer::getConnectStringValue(
						const char *variable) {
	return connectstring.getValue(variable);
}


routecontainer::routecontainer() {
	isfilter=false;
	host=NULL;
	port=0;
	socket=NULL;
	user=NULL;
	password=NULL;
}

routecontainer::~routecontainer() {
	delete[] host;
	delete[] socket;
	delete[] user;
	delete[] password;
	regexlist.clearAndDelete();
}

void routecontainer::setIsFilter(bool isfilter) {
	this->isfilter=isfilter;
}

void routecontainer::setHost(const char *host) {
	this->host=charstring::duplicate(host);
}

void routecontainer::setPort(uint16_t port) {
	this->port=port;
}

void routecontainer::setSocket(const char *socket) {
	this->socket=charstring::duplicate(socket);
}

void routecontainer::setUser(const char *user) {
	this->user=charstring::duplicate(user);
}

void routecontainer::setPassword(const char *password) {
	this->password=charstring::duplicate(password);
}

bool routecontainer::getIsFilter() {
	return isfilter;
}

const char *routecontainer::getHost() {
	return host;
} 

uint16_t routecontainer::getPort() {
	return port;
} 

const char *routecontainer::getSocket() {
	return socket;
} 

const char *routecontainer::getUser() {
	return user;
} 

const char *routecontainer::getPassword() {
	return password;
} 

linkedlist< regularexpression * > *routecontainer::getRegexList() {
	return &regexlist;
}
