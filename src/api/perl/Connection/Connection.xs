/* Copyright (c) 2000-2001  David Muse
   See the file COPYING for more information */

#include <config.h>

#include "perlincludes.h"

typedef class sqlrconnection sqlrconnection;

MODULE = SQLRelay::Connection	PACKAGE = SQLRelay::Connection
REQUIRE: 1.925

sqlrconnection *
sqlrconnection::new(server,port,socket,user,password,retrytime,tries)
		char *server
		int port
		char *socket
		char *user
		char *password
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

char *
sqlrconnection::getConnectionSocket()

bool
sqlrconnection::resumeSession(port,socket)
		int	port
		char	*socket

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

char *
sqlrconnection::identify()

void
sqlrconnection::debugOn()

void
sqlrconnection::debugOff()

bool
sqlrconnection::getDebug()
