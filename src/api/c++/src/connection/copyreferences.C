// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

void sqlrconnection::copyReferences() {

	// set the flag
	copyrefs=true;

	// make copies of some specific things
	if (server) {
		char	*tempserver=strdup(server);
		server=tempserver;
	}
	if (listenerunixport) {
		char	*templistenerunixport=strdup(listenerunixport);
		listenerunixport=templistenerunixport;
	}
	if (user) {
		char	*tempuser=strdup(user);
		user=tempuser;
	}
	if (password) {
		char	*temppassword=strdup(password);
		password=temppassword;
	}
}
