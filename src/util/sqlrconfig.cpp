// Copyright (c) 2000-2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>

#include <defaults.h>

sqlrconfig::sqlrconfig() {
}

sqlrconfig::~sqlrconfig() {
}


listenercontainer::listenercontainer() {
	addresses=NULL;
	addresscount=0;
	socket=NULL;
	protocol=charstring::duplicate(DEFAULT_PROTOCOL);
}

listenercontainer::~listenercontainer() {
	for (uint64_t i=0; i<addresscount; i++) {
		delete[] addresses[i];
	}
	delete[] addresses;
	delete[] socket;
	delete[] protocol;
}

void listenercontainer::setAddresses(char **addresses,
					uint64_t addresscount) {
	for (uint64_t i=0; i<this->addresscount; i++) {
		delete[] this->addresses[i];
	}
	delete[] this->addresses;
	this->addresses=addresses;
	this->addresscount=addresscount;
}

void listenercontainer::setPort(uint16_t port) {
	this->port=port;
}

void listenercontainer::setSocket(const char *socket) {
	delete[] this->socket;
	this->socket=charstring::duplicate(socket);
}

void listenercontainer::setProtocol(const char *protocol) {
	delete[] this->protocol;
	this->protocol=charstring::duplicate(protocol);
}

const char * const *listenercontainer::getAddresses() {
	return addresses;
}

uint64_t listenercontainer::getAddressCount() {
	return addresscount;
}

uint16_t listenercontainer::getPort() {
	return port;
}

const char *listenercontainer::getSocket() {
	return socket;
}

const char *listenercontainer::getProtocol() {
	return protocol;
}


usercontainer::usercontainer() {
	user=NULL;
	password=NULL;
	pwdenc=NULL;
}

usercontainer::~usercontainer() {
	delete[] user;
	delete[] password;
	delete[] pwdenc;
}

void usercontainer::setUser(const char *user) {
	this->user=charstring::duplicate(user);
}

void usercontainer::setPassword(const char *password) {
	this->password=charstring::duplicate(password);
}

void usercontainer::setPasswordEncryption(const char *pwdenc) {
	this->pwdenc=charstring::duplicate(pwdenc);
}

const char *usercontainer::getUser() {
	return user;
}

const char *usercontainer::getPassword() {
	return password;
}

const char *usercontainer::getPasswordEncryption() {
	return pwdenc;
}


connectstringcontainer::connectstringcontainer() {
	connectionid=NULL;
	string=NULL;
	metric=charstring::toInteger(DEFAULT_METRIC);
	behindloadbalancer=!charstring::compareIgnoringCase(
					DEFAULT_BEHINDLOADBALANCER,"yes");
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
	for (linkedlistnode< regularexpression * > *re=
					regexlist.getFirst();
						re; re=re->getNext()) {
		delete re->getValue();
	}
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
