// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

void sqlrconnection::copyReferences() {

	// set the flag
	copyrefs=true;

	// make copies of some specific things
	if (server) {
		char	*tempserver=charstring::duplicate(server);
		server=tempserver;
	}
	if (listenerunixport) {
		char	*templistenerunixport=
				charstring::duplicate(listenerunixport);
		listenerunixport=templistenerunixport;
	}
	if (user) {
		char	*tempuser=charstring::duplicate(user);
		user=tempuser;
	}
	if (password) {
		char	*temppassword=charstring::duplicate(password);
		password=temppassword;
	}
}
