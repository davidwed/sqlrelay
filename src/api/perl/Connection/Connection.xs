/* Copyright (c) 2000-2001  David Muse
   See the file COPYING for more information */

#include <config.h>

#include "perlincludes.h"

typedef class sqlrconnection sqlrconnection;

MODULE = SQLRelay::Connection	PACKAGE = SQLRelay::Connection
REQUIRE: 1.925

sqlrconnection *
sqlrconnection::new(server,port,socket,user,password,retrytime,tries)
		const char *server
		int port
		const char *socket
		const char *user
		const char *password
		int retrytime
		int tries
	CODE:
		RETVAL=new sqlrconnection(server,port,socket,
					user,password,retrytime,tries);
		RETVAL->copyReferences();
	OUTPUT:
		RETVAL

	

void
sqlrconnection::DESTROY()

void
sqlrconnection::endSession()

bool
sqlrconnection::suspendSession()

int
sqlrconnection::getConnectionPort()

const char *
sqlrconnection::getConnectionSocket()

bool
sqlrconnection::resumeSession(port,socket)
		int	port
		const char	*socket

bool
sqlrconnection::ping()

bool
sqlrconnection::autoCommitOn()

bool
sqlrconnection::autoCommitOff()

bool
sqlrconnection::commit()

bool
sqlrconnection::rollback()

const char *
sqlrconnection::identify()

void
sqlrconnection::debugOn()

void
sqlrconnection::debugOff()

bool
sqlrconnection::getDebug()
