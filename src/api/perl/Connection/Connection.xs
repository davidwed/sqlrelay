/* Copyright (c) 2000-2001  David Muse
   See the file COPYING for more information */

#include <config.h>

// SCO OSR6 requires this
#ifdef SQLRELAY_HAVE_SYS_VNODE_H
	#include <sys/vnode.h>
#endif

#include "../../c++/include/sqlrelay/sqlrclient.h"
#include <EXTERN.h>
#define explicit
extern "C" {
	#include <perl.h>
}
#include <XSUB.h>
#ifdef CLASS
	#undef CLASS
#endif

/* xsubpp outputs __attribute__((noreturn)) this isn't
 * understood by gcc < 3.0. */
#ifdef __GNUC__
	#if __GNUC__ < 3
		#define __attribute__(x)
	#endif
#endif

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
					user,password,retrytime,tries,true);
	OUTPUT:
		RETVAL

	

void
sqlrconnection::DESTROY()

void
sqlrconnection::setConnectTimeout(timeoutsec,timeoutusec)
		int32_t		timeoutsec
		int32_t		timeoutusec

void
sqlrconnection::setAuthenticationTimeout(timeoutsec,timeoutusec)
		int32_t		timeoutsec
		int32_t		timeoutusec

void
sqlrconnection::setResponseTimeout(timeoutsec,timeoutusec)
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

const char *
sqlrconnection::getCurrentDatabase()

uint64_t
sqlrconnection::getLastInsertId()

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
sqlrconnection::dbHostName()

const char *
sqlrconnection::dbIpAddress()

const char *
sqlrconnection::serverVersion()

const char *
sqlrconnection::clientVersion()

const char *
sqlrconnection::bindFormat()

const char *
sqlrconnection::errorMessage()

int64_t
sqlrconnection::errorNumber()

void
sqlrconnection::debugOn()

void
sqlrconnection::debugOff()

bool
sqlrconnection::getDebug()

void
sqlrconnection::setClientInfo(clientinfo)
		const char	*clientinfo

const char *
sqlrconnection::getClientInfo()
