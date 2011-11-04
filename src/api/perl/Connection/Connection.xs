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
		uint16_t port
		const char *socket
		const char *user
		const char *password
		int32_t retrytime
		int32_t tries
	CODE:
		RETVAL=new sqlrconnection(server,port,socket,
					user,password,retrytime,tries);
		RETVAL->copyReferences();
	OUTPUT:
		RETVAL

	

void
sqlrconnection::DESTROY()

void
sqlrconnection::setTimeout(timeoutsec,timeoutusec)
		int32_t		timeoutsec
		int32_t		timeoutusec

void
sqlrconnection::endSession()

bool
sqlrconnection::suspendSession()

int16_t
sqlrconnection::getConnectionPort()

const char *
sqlrconnection::getConnectionSocket()

bool
sqlrconnection::resumeSession(port,socket)
		int16_t		port
		const char	*socket

bool
sqlrconnection::ping()

bool
sqlrconnection::selectDatabase(database)
		const char	*database

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

const char *
sqlrconnection::dbVersion()

const char *
sqlrconnection::serverVersion()

const char *
sqlrconnection::clientVersion()

const char *
sqlrconnection::bindFormat()

void
sqlrconnection::debugOn()

void
sqlrconnection::debugOff()

bool
sqlrconnection::getDebug()
