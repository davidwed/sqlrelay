// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconnection.h>

#include <rudiments/permissions.h>
#include <rudiments/file.h>
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

void sqlrconnection::markDatabaseAvailable() {

	#ifdef SERVER_DEBUG
	char	string[9+charstring::length(updown)+1];
	sprintf(string,"creating %s",updown);
	getDebugLogger()->write("connection",4,string);
	#endif

	// the database is up if the file is there, 
	// opening and closing it will create it
	file	fd;
	fd.create(updown,permissions::ownerReadWrite());
}

void sqlrconnection::markDatabaseUnavailable() {

	#ifdef SERVER_DEBUG
	char	string[10+charstring::length(updown)+1];
	sprintf(string,"unlinking %s",updown);
	getDebugLogger()->write("connection",4,string);
	#endif

	// the database is down if the file isn't there
	unlink(updown);
}
