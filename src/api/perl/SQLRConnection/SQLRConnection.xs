/* Copyright (c) 2000-2001  David Muse
   See the file COPYING for more information */

#include <config.h>

#include "perlincludes.h"

typedef class sqlrconnection sqlrconnection;

MODULE = Firstworks::SQLRConnection	PACKAGE = Firstworks::SQLRConnection
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

int
sqlrconnection::endSession()

int
sqlrconnection::suspendSession()

int
sqlrconnection::getConnectionPort()

char *
sqlrconnection::getConnectionSocket()

int
sqlrconnection::resumeSession(port,socket)
		int	port
		char	*socket

int
sqlrconnection::ping()

int
sqlrconnection::autoCommitOn()

int
sqlrconnection::autoCommitOff()

int
sqlrconnection::commit()

int
sqlrconnection::rollback()

char *
sqlrconnection::identify()

void
sqlrconnection::debugOn()

void
sqlrconnection::debugOff()

int
sqlrconnection::getDebug()
