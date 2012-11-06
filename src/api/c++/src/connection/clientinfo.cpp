// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>

using namespace rudiments;

void sqlrconnection::setClientInfo(const char *clientinfo) {
	delete[] this->clientinfo;
	this->clientinfo=charstring::duplicate(clientinfo);
	clientinfolen=charstring::length(clientinfo);
}

const char *sqlrconnection::getClientInfo() const {
	return clientinfo;
}
