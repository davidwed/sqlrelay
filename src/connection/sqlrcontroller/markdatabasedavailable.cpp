// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrcontroller.h>

#include <rudiments/permissions.h>
#include <rudiments/file.h>

void sqlrcontroller_svr::markDatabaseAvailable() {

	size_t	stringlen=9+charstring::length(updown)+1;
	char	*string=new char[stringlen];
	snprintf(string,stringlen,"creating %s",updown);
	dbgfile.debugPrint("connection",4,string);
	delete[] string;

	// the database is up if the file is there, 
	// opening and closing it will create it
	file	fd;
	fd.create(updown,permissions::ownerReadWrite());
}

void sqlrcontroller_svr::markDatabaseUnavailable() {

	// if the database is behind a load balancer, don't mark it unavailable
	if (constr->getBehindLoadBalancer()) {
		return;
	}

	size_t	stringlen=10+charstring::length(updown)+1;
	char	*string=new char[stringlen];
	snprintf(string,stringlen,"unlinking %s",updown);
	dbgfile.debugPrint("connection",4,string);
	delete[] string;

	// the database is down if the file isn't there
	file::remove(updown);
}
