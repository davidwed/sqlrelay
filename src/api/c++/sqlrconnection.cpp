// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/inetsocketclient.h>
#include <rudiments/unixsocketclient.h>
#include <rudiments/file.h>
#include <rudiments/environment.h>
#include <rudiments/charstring.h>
#include <rudiments/stdio.h>
#include <rudiments/error.h>
#include <rudiments/permissions.h>
#include <rudiments/gss.h>
#include <rudiments/tls.h>
#include <rudiments/sys.h>
#include <defines.h>
#include <defaults.h>

#ifndef MAXPATHLEN
        #define MAXPATHLEN 256
#endif

class sqlrconnectionprivate {
	friend class sqlrconnection;
	private:

		// clients
		inetsocketclient	_ics;
		unixsocketclient	_ucs;
		socketclient		*_cs;

		// session state
		bool	_endsessionsent;
		bool	_suspendsessionsent;
		bool	_connected;

		// connection
		char		*_server;
		uint16_t	_listenerinetport;
		uint16_t	_connectioninetport;
		char		*_listenerunixport;
		const char	*_connectionunixport;
		char		_connectionunixportbuffer[MAXPATHLEN+1];
		int32_t		_connecttimeoutsec;
		int32_t		_connecttimeoutusec;
		int32_t		_authtimeoutsec;
		int32_t		_authtimeoutusec;
		int32_t		_responsetimeoutsec;
		int32_t		_responsetimeoutusec;
		int32_t		_retrytime;
		int32_t		_tries;

		// auth
		char		*_user;
		uint32_t	_userlen;
		char		*_password;
		uint32_t	_passwordlen;

		// gss
		bool		_usekrb;
		char		*_krbservice;
		char		*_krbmech;
		char		*_krbflags;
		gsscredentials	_gcred;
		gssmechanism	_gmech;
		gsscontext	_gctx;

		// tls
		bool		_usetls;
		char		*_tlsversion;
		char		*_tlscert;
		char		*_tlspassword;
		char		*_tlsciphers;
		char		*_tlsvalidate;
		char		*_tlsca;
		uint16_t	_tlsdepth;
		tlscontext	_tctx;

		securitycontext	*_ctx;

		// error
		int64_t		_errorno;
		char		*_error;

		// identify
		char		*_id;

		// db version
		char		*_dbversion;

		// db host name
		char		*_dbhostname;

		// db ip address
		char		*_dbipaddress;

		// server version
		char		*_serverversion;

		// current database name
		char		*_currentdbname;

		// current schema name
		char		*_currentschemaname;

		// bind format
		char		*_bindformat;

		// bind delimiters
		bool		_questionmarksupported;
		bool		_colonsupported;
		bool		_atsignsupported;
		bool		_dollarsignsupported;

		// client info
		char		*_clientinfo;
		uint64_t	_clientinfolen;

		// debug
		bool		_debug;
		int32_t		_webdebug;
		int		(*_printfunction)(const char *,...);
		file		_debugfile;

		// copy references flag
		bool		_copyrefs;

		// cursor list
		sqlrcursor	*_firstcursor;
		sqlrcursor	*_lastcursor;
};

sqlrconnection::sqlrconnection(const char *server, uint16_t port,
					const char *socket,
					const char *user, const char *password, 
					int32_t retrytime, int32_t tries,
					bool copyreferences) {
	init(server,port,socket,user,password,retrytime,tries,copyreferences);
}

sqlrconnection::sqlrconnection(const char *server, uint16_t port,
					const char *socket,
					const char *user, const char *password, 
					int32_t retrytime, int32_t tries) {
	init(server,port,socket,user,password,retrytime,tries,false);
}

void sqlrconnection::init(const char *server, uint16_t port,
					const char *socket,
					const char *user, const char *password, 
					int32_t retrytime, int32_t tries,
					bool copyreferences) {

	pvt=new sqlrconnectionprivate;

	pvt->_copyrefs=copyreferences;

	// retry reads if they get interrupted by signals
	pvt->_ucs.translateByteOrder();
	pvt->_ucs.retryInterruptedReads();
	pvt->_ics.retryInterruptedReads();
	pvt->_cs=&pvt->_ucs;

	// connection
	pvt->_server=(pvt->_copyrefs)?
			charstring::duplicate(server):
			(char *)server;
	pvt->_listenerinetport=port;
	pvt->_listenerunixport=(pvt->_copyrefs)?
				charstring::duplicate(socket):
				(char *)socket;
	pvt->_retrytime=retrytime;
	pvt->_tries=tries;

	// initialize timeouts
	setTimeoutFromEnv("SQLR_CLIENT_CONNECT_TIMEOUT",
			&pvt->_connecttimeoutsec,&pvt->_connecttimeoutusec);
	setTimeoutFromEnv("SQLR_CLIENT_AUTHENTICATION_TIMEOUT",
			&pvt->_authtimeoutsec,&pvt->_authtimeoutusec);
	setTimeoutFromEnv("SQLR_CLIENT_RESPONSE_TIMEOUT",
			&pvt->_responsetimeoutsec,&pvt->_responsetimeoutusec);

	// authentication
	pvt->_user=(pvt->_copyrefs)?
			charstring::duplicate(user):
			(char *)user;
	pvt->_password=(pvt->_copyrefs)?
			charstring::duplicate(password):
			(char *)password;
	pvt->_userlen=charstring::length(user);
	pvt->_passwordlen=charstring::length(password);
	pvt->_usekrb=false;
	pvt->_krbservice=NULL;
	pvt->_krbmech=NULL;
	pvt->_krbflags=NULL;

	pvt->_usetls=false;
	pvt->_tlsversion=NULL;
	pvt->_tlscert=NULL;
	pvt->_tlspassword=NULL;
	pvt->_tlsciphers=NULL;
	pvt->_tlsvalidate=(pvt->_copyrefs)?
				charstring::duplicate("no"):
				(char *)"no";
	pvt->_tlsca=NULL;
	pvt->_tlsdepth=0;

	pvt->_ctx=NULL;

	// database id
	pvt->_id=NULL;

	// db version
	pvt->_dbversion=NULL;

	// db host name
	pvt->_dbhostname=NULL;

	// db ip address
	pvt->_dbipaddress=NULL;

	// server version
	pvt->_serverversion=NULL;

	// current database name
	pvt->_currentdbname=NULL;

	// current schema name
	pvt->_currentschemaname=NULL;

	// bind format
	pvt->_bindformat=NULL;

	// bind delimiters
	pvt->_questionmarksupported=true;
	pvt->_colonsupported=true;
	pvt->_atsignsupported=true;
	pvt->_dollarsignsupported=true;

	// client info
	pvt->_clientinfo=NULL;
	pvt->_clientinfolen=0;

	// session state
	pvt->_connected=false;
	clearSessionFlags();

	// debug print function
	pvt->_printfunction=NULL;

	// enable/disable debug
	const char	*sqlrdebug=environment::getValue("SQLRDEBUG");
	if (!sqlrdebug || !*sqlrdebug) {
		sqlrdebug=environment::getValue("SQLR_CLIENT_DEBUG");
	}
	pvt->_debug=(sqlrdebug && *sqlrdebug && !charstring::isNo(sqlrdebug));
	if (pvt->_debug && !charstring::isYes(sqlrdebug) &&
				!charstring::isNo(sqlrdebug)) {
		setDebugFile(sqlrdebug);
	}
	pvt->_webdebug=-1;

	// error
	pvt->_errorno=0;
	pvt->_error=NULL;

	// cursor list
	pvt->_firstcursor=NULL;
	pvt->_lastcursor=NULL;
}

void sqlrconnection::clearSessionFlags() {

	// indicate that the session hasn't been suspended or ended
	pvt->_endsessionsent=false;
	pvt->_suspendsessionsent=false;
}

sqlrconnection::~sqlrconnection() {

	// unless it was already ended or suspended, end the session
	if (!pvt->_endsessionsent && !pvt->_suspendsessionsent) {
		endSession();
	}

	// deallocate error
	delete[] pvt->_error;

	// deallocate id
	delete[] pvt->_id;

	// deallocate dbversion
	delete[] pvt->_dbversion;

	// deallocate db host name
	delete[] pvt->_dbhostname;

	// deallocate db ip address
	delete[] pvt->_dbipaddress;

	// deallocate server version
	delete[] pvt->_serverversion;

	// deallocate current database name
	delete[] pvt->_currentdbname;

	// deallocate current schema name
	delete[] pvt->_currentschemaname;

	// deallocate bindformat
	delete[] pvt->_bindformat;

	// deallocate client info
	delete[] pvt->_clientinfo;

	// deallocate copied references
	if (pvt->_copyrefs) {
		delete[] pvt->_server;
		delete[] pvt->_listenerunixport;
		delete[] pvt->_user;
		delete[] pvt->_password;
		delete[] pvt->_krbservice;
		delete[] pvt->_krbmech;
		delete[] pvt->_krbflags;
		delete[] pvt->_tlsversion;
		delete[] pvt->_tlscert;
		delete[] pvt->_tlspassword;
		delete[] pvt->_tlsvalidate;
		delete[] pvt->_tlsca;
	}

	// detach all cursors attached to this client
	sqlrcursor	*currentcursor=pvt->_firstcursor;
	while (currentcursor) {
		pvt->_firstcursor=currentcursor;
		currentcursor=currentcursor->next();
		pvt->_firstcursor->sqlrc(NULL);
	}

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Deallocated connection\n");
		debugPreEnd();
	}

	pvt->_debugfile.close();

	delete pvt;
}

void sqlrconnection::enableKerberos(const char *service,
					const char *mech,
					const char *flags) {

	// clear any existing configuration
	if (pvt->_usekrb || pvt->_usetls) {
		disableEncryption();
	}

	if (!gss::supported()) {
		return;
	}

	pvt->_usekrb=true;

	// "Negotiate" is Windows' default mech.  Force Kerberos instead.
	char	*os=sys::getOperatingSystemName();
	if (!charstring::compare(os,"Windows",7) &&
			charstring::isNullOrEmpty(mech)) {
		mech="Kerberos";
	}
	delete[] os;

	if (pvt->_copyrefs) {
		delete[] pvt->_krbservice;
		pvt->_krbservice=charstring::duplicate(
			(!charstring::isNullOrEmpty(service))?
					service:DEFAULT_KRBSERVICE);
		delete[] pvt->_krbmech;
		pvt->_krbmech=charstring::duplicate(mech);
		delete[] pvt->_krbflags;
		pvt->_krbflags=charstring::duplicate(flags);
	} else {
		pvt->_krbservice=(char *)
			(!charstring::isNullOrEmpty(service)?
					service:DEFAULT_KRBSERVICE);
		pvt->_krbmech=(char *)mech;
		pvt->_krbflags=(char *)flags;
	}
}

void sqlrconnection::enableTls(const char *version,
					const char *cert,
					const char *password,
					const char *ciphers,
					const char *validate,
					const char *ca,
					uint16_t depth) {

	// clear any existing configuration
	if (pvt->_usekrb || pvt->_usetls) {
		disableEncryption();
	}

	if (!tls::supported()) {
		return;
	}

	pvt->_usetls=true;

	if (pvt->_copyrefs) {
		delete[] pvt->_tlsversion;
		pvt->_tlsversion=charstring::duplicate(version);
		delete[] pvt->_tlscert;
		pvt->_tlscert=charstring::duplicate(cert);
		delete[] pvt->_tlspassword;
		pvt->_tlspassword=charstring::duplicate(password);
		delete[] pvt->_tlsciphers;
		pvt->_tlsciphers=charstring::duplicate(ciphers);
		delete[] pvt->_tlsvalidate;
		pvt->_tlsvalidate=charstring::duplicate(validate);
		delete[] pvt->_tlsca;
		pvt->_tlsca=charstring::duplicate(ca);
	} else {
		pvt->_tlsversion=(char *)version;
		pvt->_tlscert=(char *)cert;
		pvt->_tlspassword=(char *)password;
		pvt->_tlsciphers=(char *)ciphers;
		pvt->_tlsvalidate=(char *)validate;
		pvt->_tlsca=(char *)ca;
	}
	pvt->_tlsdepth=depth;
}

void sqlrconnection::disableEncryption() {

	if (pvt->_copyrefs) {
		delete[] pvt->_krbservice;
		delete[] pvt->_krbmech;
		delete[] pvt->_krbflags;

		delete[] pvt->_tlsversion;
		delete[] pvt->_tlscert;
		delete[] pvt->_tlspassword;
		delete[] pvt->_tlsciphers;
		delete[] pvt->_tlsvalidate;
		delete[] pvt->_tlsca;
	}
	pvt->_krbservice=NULL;
	pvt->_krbmech=NULL;
	pvt->_krbflags=NULL;
	pvt->_usekrb=false;

	pvt->_tlsversion=NULL;
	pvt->_tlscert=NULL;
	pvt->_tlspassword=NULL;
	pvt->_tlsciphers=NULL;
	pvt->_tlsvalidate=NULL;
	pvt->_tlsca=NULL;
	pvt->_tlsdepth=0;
	pvt->_usetls=false;
}

void sqlrconnection::setConnectTimeout(int32_t timeoutsec,
					int32_t timeoutusec) {
	pvt->_connecttimeoutsec=timeoutsec;
	pvt->_connecttimeoutusec=timeoutusec;
}

void sqlrconnection::setAuthenticationTimeout(int32_t timeoutsec,
						int32_t timeoutusec) {
	pvt->_authtimeoutsec=timeoutsec;
	pvt->_authtimeoutusec=timeoutusec;
}

void sqlrconnection::setResponseTimeout(int32_t timeoutsec,
						int32_t timeoutusec) {
	pvt->_responsetimeoutsec=timeoutsec;
	pvt->_responsetimeoutusec=timeoutusec;
}

void sqlrconnection::setTimeoutFromEnv(const char *var,
					int32_t *timeoutsec,
					int32_t *timeoutusec) {
	const char	*timeout=environment::getValue(var);
	if (charstring::isNumber(timeout)) {
		*timeoutsec=charstring::toInteger(timeout);
		long double	dbl=charstring::toFloatC(timeout);
		dbl=dbl-(long double)(*timeoutsec);
		*timeoutusec=(int32_t)(dbl*1000000.0);
	} else {
		*timeoutsec=-1;
		*timeoutusec=-1;
	}
}

void sqlrconnection::getConnectTimeout(int32_t *timeoutsec,
					int32_t *timeoutusec) {
	*timeoutsec=pvt->_connecttimeoutsec;
	*timeoutusec=pvt->_connecttimeoutusec;
}

void sqlrconnection::getAuthenticationTimeout(int32_t *timeoutsec,
						int32_t *timeoutusec) {
	*timeoutsec=pvt->_authtimeoutsec;
	*timeoutusec=pvt->_authtimeoutusec;
}

void sqlrconnection::getResponseTimeout(int32_t *timeoutsec,
						int32_t *timeoutusec) {
	*timeoutsec=pvt->_responsetimeoutsec;
	*timeoutusec=pvt->_responsetimeoutusec;
}

void sqlrconnection::endSession() {

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Ending Session\n");
		debugPreEnd();
	}

	// abort each cursor's result set
	sqlrcursor	*currentcursor=pvt->_firstcursor;
	while (currentcursor) {
		// FIXME: do we need to clearResultSet() here too?
		if (!currentcursor->endofresultset()) {
			currentcursor->closeResultSet(false);
		}
		currentcursor->havecursorid(false);
		currentcursor=currentcursor->next();
	}

	// write an END_SESSION to the connection
	if (pvt->_connected) {
		pvt->_cs->write((uint16_t)END_SESSION);
		flushWriteBuffer();
		pvt->_endsessionsent=true;
		closeConnection();
	}
}

void sqlrconnection::flushWriteBuffer() {
	pvt->_cs->flushWriteBuffer(-1,-1);
}

void sqlrconnection::closeConnection() {
	pvt->_cs->close();
	pvt->_connected=false;
}

bool sqlrconnection::suspendSession() {

	if (!openSession()) {
		return 0;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Suspending Session\n");
		debugPreEnd();
	}

	// suspend the session
	pvt->_cs->write((uint16_t)SUSPEND_SESSION);
	flushWriteBuffer();
	pvt->_suspendsessionsent=true;

	// check for error
	if (gotError()) {
		return false;
	}

	// get port to resume on
	bool	retval=getNewPort();

	closeConnection();

	return retval;
}

bool sqlrconnection::openSession() {

	if (pvt->_connected) {
		return true;
	}

	if (!reConfigureSockets()) {
		pvt->_connected=false;
		return false;
	}

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Connecting to listener...");
		debugPrint("\n");
		debugPreEnd();
	}

	// open a connection to the listener
	int	openresult=RESULT_ERROR;

	// first, try for a unix connection
	if (!charstring::isNullOrEmpty(pvt->_listenerunixport)) {

		if (pvt->_debug) {
			debugPreStart();
			debugPrint("Unix socket: ");
			debugPrint(pvt->_listenerunixport);
			debugPrint("\n");
			debugPreEnd();
		}

		openresult=pvt->_ucs.connect(pvt->_listenerunixport,
						pvt->_connecttimeoutsec,
						pvt->_connecttimeoutusec,
						pvt->_retrytime,pvt->_tries);
		if (openresult==RESULT_SUCCESS) {

			pvt->_ucs.setSocketReadBufferSize(65536);
			pvt->_ucs.setSocketWriteBufferSize(65536);

			pvt->_cs=&pvt->_ucs;
		}
	}

	// then try for an inet connection
	if (openresult!=RESULT_SUCCESS && pvt->_listenerinetport) {

		if (pvt->_debug) {
			debugPreStart();
			debugPrint("Inet socket: ");
			debugPrint(pvt->_server);
			debugPrint(":");
			debugPrint((int64_t)pvt->_listenerinetport);
			debugPrint("\n");
			debugPreEnd();
		}

		openresult=pvt->_ics.connect(pvt->_server,
						pvt->_listenerinetport,
						pvt->_connecttimeoutsec,
						pvt->_connecttimeoutusec,
						pvt->_retrytime,pvt->_tries);
		if (openresult==RESULT_SUCCESS) {

			pvt->_ics.setSocketReadBufferSize(65536);
			pvt->_ics.setSocketWriteBufferSize(65536);

			pvt->_ics.dontUseNaglesAlgorithm();

			pvt->_cs=&pvt->_ics;
		}
	}

	// handle failures
	if (openresult!=RESULT_SUCCESS) {
		setConnectFailedError();
		return false;
	}

	// if tls is enabled and we're using an inet socket,
	// then we may need to validate the host
	if (pvt->_usetls && pvt->_cs==&pvt->_ics) {

		if (!validateCertificate()) {
			setError("TLS error: common name mismatch");
			pvt->_cs->close();
			return false;
		}
	}

	// if we made it here then everything went
	// well and we are successfully connected
	pvt->_connected=true;

	// send protocol info
	protocol();

	// auth
	auth();

	return true;
}

bool sqlrconnection::validateCertificate() {

	// If we're not doing any validation then just return true. If we're
	// just doing ca validation then the connect would have failed if the
	// certificate was invalid, so we can just return true for that too.
	if (charstring::isNo(pvt->_tlsvalidate) ||
		!charstring::compareIgnoringCase(pvt->_tlsvalidate,"ca")) {
		return true;
	}

	// get the cert from the server
	tlscertificate		*cert=((tlscontext *)pvt->_ctx)->
						getPeerCertificate();
	if (!cert) {
		// this should never happen, the connect()
		// should have failed if no cert was supplied
		return false;
	}

	// get the subject alternate names and common name from the cert
	linkedlist< char * >	*sans=cert->getSubjectAlternateNames();
	const char		*commonname=cert->getCommonName();

	// should we validate the host name or domain?
	bool	host=!charstring::compareIgnoringCase(
					pvt->_tlsvalidate,"ca+host");

	// get the server name to validate against
	const char	*server=pvt->_server;
	if (!host) {
		const char	*dot=charstring::findFirst(server,'.');
		if (dot) {
			server=dot+1;
		}
	}

	// if there are any subject alternate
	// names then validate against those
	if (sans && sans->getLength()) {

		for (linkedlistnode< char * > *node=sans->getFirst();
					node; node=node->getNext()) {

			const char	*san=node->getValue();
			if (!host) {
				const char	*dot=
					charstring::findFirst(san,'.');
				if (dot) {
					san=dot+1;
				}
			}

			if (pvt->_debug) {
				debugPreStart();
				debugPrint(pvt->_tlsvalidate);
				debugPrint(": ");
				debugPrint(server);
				debugPrint("=");
				debugPrint(san);
				debugPrint("\n");
				debugPreEnd();
			}

			if (!charstring::compareIgnoringCase(server,san)) {
				return true;
			}
		}

		return false;
	}


	// if there were no subject alternate names
	// then just validate against the common name
	if (!host) {
		const char	*dot=charstring::findFirst(commonname,'.');
		if (dot) {
			commonname=dot+1;
		}
	}

	if (pvt->_debug) {
		debugPreStart();
		debugPrint(pvt->_tlsvalidate);
		debugPrint(": ");
		debugPrint(server);
		debugPrint("=");
		debugPrint(commonname);
		debugPrint("\n");
		debugPreEnd();
	}

	return !charstring::compareIgnoringCase(server,commonname);
}


bool sqlrconnection::reConfigureSockets() {

	pvt->_ucs.setReadBufferSize(65536);
	pvt->_ucs.setWriteBufferSize(65536);
	//pvt->_ucs.useAsyncWrite();

	pvt->_ics.setReadBufferSize(65536);
	pvt->_ics.setWriteBufferSize(65536);
	//pvt->_ics.useAsyncWrite();


	if (pvt->_usekrb) {

		if (pvt->_debug) {
			debugPreStart();
			debugPrint("kerberos encryption/"
					"authentication enabled\n");
			debugPrint("  service: ");
			if (pvt->_krbservice) {
				debugPrint(pvt->_krbservice);
			}
			debugPrint("\n");
			debugPrint("  mech: ");
			if (pvt->_krbmech) {
				debugPrint(pvt->_krbmech);
			}
			debugPrint("\n");
			debugPrint("  flags: ");
			if (pvt->_krbflags) {
				debugPrint(pvt->_krbflags);
			}
			debugPrint("\n");
			debugPreEnd();
		}

		pvt->_gmech.clear();
		pvt->_gmech.initialize(pvt->_krbmech);

		pvt->_gcred.clearDesiredMechanisms();
		pvt->_gcred.addDesiredMechanism(&pvt->_gmech);

		if (!pvt->_gcred.acquired() &&
				!charstring::isNullOrEmpty(pvt->_user)) {

			if (pvt->_gcred.acquireForUser(pvt->_user)) {
				if (pvt->_debug) {
					debugPreStart();
					debugPrint("acquired kerberos "
							"credentials for: ");
					debugPrint(pvt->_user);
					debugPrint("\n");
					debugPreEnd();
				}
			} else {
				if (pvt->_debug) {
					debugPreStart();
					if (pvt->_gcred.getMajorStatus()) {
						debugPrint(pvt->_gcred.
						getMechanismMinorStatus());
					}
					debugPreEnd();
				}
				setError("Failed to acquire "
						"kerberos credentials.");
				return false;
			}
		}

		pvt->_gctx.close();
		pvt->_gctx.setDesiredMechanism(&pvt->_gmech);
		pvt->_gctx.setDesiredFlags(pvt->_krbflags);
		pvt->_gctx.setService(pvt->_krbservice);
		pvt->_gctx.setCredentials(&pvt->_gcred);

		pvt->_ctx=&pvt->_gctx;

	} else if (pvt->_usetls) {

		if (pvt->_debug) {
			debugPreStart();
			debugPrint("TLS encryption/authentication enabled\n");
			debugPrint("  version: ");
			if (pvt->_tlsversion) {
				debugPrint(pvt->_tlsversion);
			}
			debugPrint("\n");
			debugPrint("  cert: ");
			if (pvt->_tlscert) {
				debugPrint(pvt->_tlscert);
			}
			debugPrint("\n");
			debugPrint("  private key password: ");
			if (pvt->_tlspassword) {
				debugPrint(pvt->_tlspassword);
			}
			debugPrint("\n");
			debugPrint("  ciphers: ");
			if (pvt->_tlsciphers) {
				debugPrint(pvt->_tlsciphers);
			}
			debugPrint("\n");
			debugPrint("  validate: ");
			if (pvt->_tlsvalidate) {
				debugPrint(pvt->_tlsvalidate);
			}
			debugPrint("\n");
			debugPrint("  ca: ");
			if (pvt->_tlsca) {
				debugPrint(pvt->_tlsca);
			}
			debugPrint("\n");
			debugPrint("  depth: ");
			debugPrint((int64_t)pvt->_tlsdepth);
			debugPrint("\n");
			debugPreEnd();
		}

		pvt->_tctx.close();
		pvt->_tctx.setProtocolVersion(pvt->_tlsversion);
		pvt->_tctx.setCertificateChainFile(pvt->_tlscert);
		pvt->_tctx.setPrivateKeyPassword(pvt->_tlspassword);
		pvt->_tctx.setCiphers(pvt->_tlsciphers);
		pvt->_tctx.setValidatePeer(!charstring::compareIgnoringCase(
						pvt->_tlsvalidate,"ca",2));
		pvt->_tctx.setCertificateAuthority(pvt->_tlsca);
		pvt->_tctx.setValidationDepth(pvt->_tlsdepth);

		pvt->_ctx=&pvt->_tctx;

	} else {

		if (pvt->_debug) {
			debugPreStart();
			debugPrint("encryption disabled\n");
			debugPreEnd();
		}

		pvt->_ctx=NULL;
	}

	pvt->_ucs.setSecurityContext(pvt->_ctx);
	pvt->_ics.setSecurityContext(pvt->_ctx);

	return true;
}

void sqlrconnection::setConnectFailedError() {
	if (pvt->_usekrb && pvt->_gctx.getMajorStatus()) {
		setError(pvt->_gctx.getMechanismMinorStatus());
	} else if (pvt->_usetls && pvt->_tctx.getError()) {
		stringbuffer	err;
		err.append("TLS error: ");
		err.append(pvt->_tctx.getErrorString());
		setError(err.getString());
	} else {
		setError("Couldn't connect to the listener.");
	}
}

void sqlrconnection::protocol() {

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Protocol : sqlrclient version 2\n");
		debugPreEnd();
	}

	pvt->_cs->write((uint16_t)PROTOCOLVERSION);
	pvt->_cs->write((uint16_t)2);
}

void sqlrconnection::auth() {

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Auth : ");
		debugPrint(pvt->_user);
		debugPrint(":");
		// hide the password
		for (uint8_t i=0; i<pvt->_passwordlen; i++) {
			debugPrint("*");
		}
		debugPrint("\n");
		debugPreEnd();
	}

	pvt->_cs->write((uint16_t)AUTH);

	pvt->_cs->write(pvt->_userlen);
	pvt->_cs->write(pvt->_user,pvt->_userlen);

	pvt->_cs->write(pvt->_passwordlen);
	pvt->_cs->write(pvt->_password,pvt->_passwordlen);

	// I don't think this needs to be here...
	// Just commenting it out for now though.
	//flushWriteBuffer();
}

bool sqlrconnection::getNewPort() {

	// get the size of the unix port string
	uint16_t	size;
	if (pvt->_cs->read(&size)!=sizeof(uint16_t)) {
		setError("Failed to get the size of "
				"the unix connection port.\n"
				"A network error may have occurred.");
		return false;
	}
	
	if (size>MAXPATHLEN) {

		// if size is too big, return an error
		stringbuffer	errstr;
		errstr.append("The pathname of the unix port was too long: ");
		errstr.append(size);
		errstr.append(" bytes.  The maximum size is ");
		errstr.append((uint16_t)MAXPATHLEN);
		errstr.append(" bytes.");
		setError(errstr.getString());
		return false;
	}

	// get the unix port string
	if (size && pvt->_cs->read(pvt->_connectionunixportbuffer,size)!=size) {
		setError("Failed to get the unix connection port.\n"
				"A network error may have occurred.");
		return false;
	}
	pvt->_connectionunixportbuffer[size]='\0';
	pvt->_connectionunixport=pvt->_connectionunixportbuffer;

	// get the inet port
	if (pvt->_cs->read(&pvt->_connectioninetport)!=sizeof(uint16_t)) {
		setError("Failed to get the inet connection port.\n"
				"A network error may have occurred.");
		return false;
	}

	// the server will send 0 for both the size of the unixport and 
	// the inet port if a server error occurred
	if (!size && !pvt->_connectioninetport) {
		setError("An error occurred on the server.");
		return false;
	}
	return true;
}

uint16_t sqlrconnection::getConnectionPort() {

	if (!pvt->_suspendsessionsent && !openSession()) {
		return 0;
	}

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Getting connection port: ");
		debugPrint((int64_t)pvt->_connectioninetport);
		debugPrint("\n");
		debugPreEnd();
	}

	return pvt->_connectioninetport;
}

const char *sqlrconnection::getConnectionSocket() {

	if (!pvt->_suspendsessionsent && !openSession()) {
		return NULL;
	}

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Getting connection socket: ");
		if (pvt->_connectionunixport) {
			debugPrint(pvt->_connectionunixport);
		}
		debugPrint("\n");
		debugPreEnd();
	}

	if (pvt->_connectionunixport) {
		return pvt->_connectionunixport;
	}
	return NULL;
}

bool sqlrconnection::resumeSession(uint16_t port, const char *socket) {

	// if already pvt->_connected, end the session
	if (pvt->_connected) {
		endSession();
	}

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Resuming Session: \n");
		debugPrint("port: ");
		debugPrint((int64_t)port);
		debugPrint("\n");
		debugPrint("socket: ");
		debugPrint(socket);
		debugPrint("\n");
		debugPreEnd();
	}

	// set the connectionunixport and connectioninetport values
	if (pvt->_copyrefs) {
		if (charstring::length(socket)<=MAXPATHLEN) {
			charstring::copy(pvt->_connectionunixportbuffer,socket);
			pvt->_connectionunixport=pvt->_connectionunixportbuffer;
		} else {
			pvt->_connectionunixport="";
		}
	} else {
		pvt->_connectionunixport=(char *)socket;
	}
	pvt->_connectioninetport=port;

	if (!reConfigureSockets()) {
		pvt->_connected=false;
		return false;
	}

	// first, try for the unix port
	if (!charstring::isNullOrEmpty(socket)) {
		pvt->_connected=(pvt->_ucs.connect(
					socket,-1,-1,
					pvt->_retrytime,
					pvt->_tries)==RESULT_SUCCESS);
		if (pvt->_connected) {
			pvt->_cs=&pvt->_ucs;
		}
	}

	// then try for the inet port
	if (!pvt->_connected) {
		pvt->_connected=(pvt->_ics.connect(
					pvt->_server,port,-1,-1,
					pvt->_retrytime,
					pvt->_tries)==RESULT_SUCCESS);
		if (pvt->_connected) {
			pvt->_cs=&pvt->_ics;
		}
	}

	if (pvt->_connected) {

		// send protocol info
		protocol();

		if (pvt->_debug) {
			debugPreStart();
			debugPrint("success");
			debugPrint("\n");
			debugPreEnd();
		}
		clearSessionFlags();
	} else {
		setConnectFailedError();
		if (pvt->_debug) {
			debugPreStart();
			debugPrint("failure");
			debugPrint("\n");
			debugPreEnd();
		}
	}

	return pvt->_connected;
}

bool sqlrconnection::ping() {

	if (!openSession()) {
		return 0;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Pinging...\n");
		debugPreEnd();
	}

	pvt->_cs->write((uint16_t)PING);
	flushWriteBuffer();

	return !gotError();
}

const char *sqlrconnection::identify() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Identifying...\n");
		debugPreEnd();
	}

	// tell the server we want the identity of the db
	pvt->_cs->write((uint16_t)IDENTIFY);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the identity size
	uint16_t	size;
	if (pvt->_cs->read(&size,pvt->_responsetimeoutsec,
				pvt->_responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to identify.\n"
			"A network error may have occurred.");
		return NULL;
	}

	// get the identity
	delete[] pvt->_id;
	pvt->_id=new char[size+1];
	if (pvt->_cs->read(pvt->_id,size)!=size) {
		setError("Failed to identify.\n"
			"A network error may have occurred.");
		delete[] pvt->_id;
		pvt->_id=NULL;
		return NULL;
	}
	pvt->_id[size]='\0';

	if (pvt->_debug) {
		debugPreStart();
		debugPrint(pvt->_id);
		debugPrint("\n");
		debugPreEnd();
	}
	return pvt->_id;
}

const char *sqlrconnection::dbVersion() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("DB Version...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the db version
	pvt->_cs->write((uint16_t)DBVERSION);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the db version size
	uint16_t	size;
	if (pvt->_cs->read(&size,pvt->_responsetimeoutsec,
				pvt->_responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get DB version.\n"
			"A network error may have occurred.");
		return NULL;
	} 

	// get the db version
	delete[] pvt->_dbversion;
	pvt->_dbversion=new char[size+1];
	if (pvt->_cs->read(pvt->_dbversion,size)!=size) {
		setError("Failed to get DB version.\n"
			"A network error may have occurred.");
		delete[] pvt->_dbversion;
		pvt->_dbversion=NULL;
		return NULL;
	}
	pvt->_dbversion[size]='\0';

	if (pvt->_debug) {
		debugPreStart();
		debugPrint(pvt->_dbversion);
		debugPrint("\n");
		debugPreEnd();
	}
	return pvt->_dbversion;
}

const char *sqlrconnection::dbHostName() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("DB Host Name...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the db host name
	pvt->_cs->write((uint16_t)DBHOSTNAME);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the db host name size
	uint16_t	size;
	if (pvt->_cs->read(&size,pvt->_responsetimeoutsec,
				pvt->_responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get DB host name.\n"
				"A network error may have occurred.");
		return NULL;
	}

	// get the db host name
	delete[] pvt->_dbhostname;
	pvt->_dbhostname=new char[size+1];
	if (pvt->_cs->read(pvt->_dbhostname,size)!=size) {
		setError("Failed to get DB host name.\n"
				"A network error may have occurred.");
		delete[] pvt->_dbhostname;
		pvt->_dbhostname=NULL;
		return NULL;
	}
	pvt->_dbhostname[size]='\0';

	if (pvt->_debug) {
		debugPreStart();
		debugPrint(pvt->_dbhostname);
		debugPrint("\n");
		debugPreEnd();
	}
	return pvt->_dbhostname;
}

const char *sqlrconnection::dbIpAddress() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("DB Ip Address...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the db ip address
	pvt->_cs->write((uint16_t)DBIPADDRESS);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the db ip address size
	uint16_t	size;
	if (pvt->_cs->read(&size,pvt->_responsetimeoutsec,
				pvt->_responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get DB ip address.\n"
				"A network error may have occurred.");
		return NULL;
	}

	// get the db ip address
	delete[] pvt->_dbipaddress;
	pvt->_dbipaddress=new char[size+1];
	if (pvt->_cs->read(pvt->_dbipaddress,size)!=size) {
		setError("Failed to get DB ip address.\n"
				"A network error may have occurred.");
		delete[] pvt->_dbipaddress;
		pvt->_dbipaddress=NULL;
		return NULL;
	}
	pvt->_dbipaddress[size]='\0';

	if (pvt->_debug) {
		debugPreStart();
		debugPrint(pvt->_dbipaddress);
		debugPrint("\n");
		debugPreEnd();
	}
	return pvt->_dbipaddress;
}

const char *sqlrconnection::serverVersion() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Server Version...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the server version
	pvt->_cs->write((uint16_t)SERVERVERSION);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the server version size
	uint16_t	size;
	if (pvt->_cs->read(&size,pvt->_responsetimeoutsec,
				pvt->_responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get Server version.\n"
				"A network error may have occurred.");
		return NULL;
	}

	// get the server version
	delete[] pvt->_serverversion;
	pvt->_serverversion=new char[size+1];
	if (pvt->_cs->read(pvt->_serverversion,size)!=size) {
		setError("Failed to get Server version.\n"
				"A network error may have occurred.");
		delete[] pvt->_serverversion;
		pvt->_serverversion=NULL;
		return NULL;
	}
	pvt->_serverversion[size]='\0';

	if (pvt->_debug) {
		debugPreStart();
		debugPrint(pvt->_serverversion);
		debugPrint("\n");
		debugPreEnd();
	}
	return pvt->_serverversion;
}

const char *sqlrconnection::clientVersion() {
	return SQLR_VERSION;
}

const char *sqlrconnection::bindFormat() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("bind format...");
		debugPrint("\n");
		debugPreEnd();
	}

	// tell the server we want the bind format
	pvt->_cs->write((uint16_t)BINDFORMAT);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}


	// get the bindformat size
	uint16_t	size;
	if (pvt->_cs->read(&size,pvt->_responsetimeoutsec,
				pvt->_responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get bind format.\n"
				"A network error may have occurred.");
		return NULL;
	}

	// get the bindformat
	delete[] pvt->_bindformat;
	pvt->_bindformat=new char[size+1];
	if (pvt->_cs->read(pvt->_bindformat,size)!=size) {
		setError("Failed to get bind format.\n"
				"A network error may have occurred.");
		delete[] pvt->_bindformat;
		pvt->_bindformat=NULL;
		return NULL;
	}
	pvt->_bindformat[size]='\0';

	if (pvt->_debug) {
		debugPreStart();
		debugPrint(pvt->_bindformat);
		debugPrint("\n");
		debugPreEnd();
	}
	return pvt->_bindformat;
}

bool sqlrconnection::selectDatabase(const char *database) {

	if (!charstring::length(database)) {
		return true;
	}

	clearError();

	if (!openSession()) {
		return 0;
	}

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Selecting database ");
		debugPrint(database);
		debugPrint("...\n");
		debugPreEnd();
	}

	// tell the server we want to select a db
	pvt->_cs->write((uint16_t)SELECT_DATABASE);

	// send the database name
	uint32_t	len=charstring::length(database);
	pvt->_cs->write(len);
	if (len) {
		pvt->_cs->write(database,len);
	}
	flushWriteBuffer();

	return !gotError();
}

const char *sqlrconnection::getCurrentDatabase() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Getting the current database...\n");
		debugPreEnd();
	}

	clearError();

	// tell the server we want to get the current db
	pvt->_cs->write((uint16_t)GET_CURRENT_DATABASE);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the current db name size
	uint16_t	size;
	if (pvt->_cs->read(&size,pvt->_responsetimeoutsec,
				pvt->_responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get the current database.\n"
				"A network error may have occurred.");
		return NULL;
	}

	// get the current db name
	delete[] pvt->_currentdbname;
	pvt->_currentdbname=new char[size+1];
	if (pvt->_cs->read(pvt->_currentdbname,size)!=size) {
		setError("Failed to get the current database.\n"
				"A network error may have occurred.");
		delete[] pvt->_currentdbname;
		pvt->_currentdbname=NULL;
		return NULL;
	}
	pvt->_currentdbname[size]='\0';

	if (pvt->_debug) {
		debugPreStart();
		debugPrint(pvt->_currentdbname);
		debugPrint("\n");
		debugPreEnd();
	}
	return pvt->_currentdbname;
}

const char *sqlrconnection::getCurrentSchema() {

	if (!openSession()) {
		return NULL;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Getting the current schema...\n");
		debugPreEnd();
	}

	clearError();

	// tell the server we want to get the current schema
	pvt->_cs->write((uint16_t)GET_CURRENT_SCHEMA);
	flushWriteBuffer();

	if (gotError()) {
		return NULL;
	}

	// get the current schema name size
	uint16_t	size;
	if (pvt->_cs->read(&size,pvt->_responsetimeoutsec,
				pvt->_responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get the current database.\n"
				"A network error may have occurred.");
		return NULL;
	}

	// get the current schema name
	delete[] pvt->_currentschemaname;
	pvt->_currentschemaname=new char[size+1];
	if (pvt->_cs->read(pvt->_currentschemaname,size)!=size) {
		setError("Failed to get the current database.\n"
				"A network error may have occurred.");
		delete[] pvt->_currentschemaname;
		pvt->_currentschemaname=NULL;
		return NULL;
	}
	pvt->_currentschemaname[size]='\0';

	if (pvt->_debug) {
		debugPreStart();
		debugPrint(pvt->_currentschemaname);
		debugPrint("\n");
		debugPreEnd();
	}
	return pvt->_currentschemaname;
}

uint64_t sqlrconnection::getLastInsertId() {

	if (!openSession()) {
		return 0;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Getting the last insert id...\n");
		debugPreEnd();
	}

	// tell the server we want the last insert id
	pvt->_cs->write((uint16_t)GET_LAST_INSERT_ID);
	flushWriteBuffer();

	if (gotError()) {
		return 0;
	}

	// get the last insert id
	uint64_t	id=0;
	if (pvt->_cs->read(&id)!=sizeof(uint64_t)) {
		setError("Failed to get the last insert id.\n"
				"A network error may have occurred.");
		return 0;
	}

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Got the last insert id: ");
		debugPrint((int64_t)id);
		debugPrint("\n");
		debugPreEnd();
	}
	return id;
}

bool sqlrconnection::autoCommitOn() {
	return autoCommit(true);
}

bool sqlrconnection::autoCommitOff() {
	return autoCommit(false);
}

bool sqlrconnection::autoCommit(bool on) {

	if (!openSession()) {
		return false;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint((on)?"Setting autocommit on\n":
				"Setting autocommit off\n");
		debugPreEnd();
	}

	pvt->_cs->write((uint16_t)AUTOCOMMIT);
	pvt->_cs->write(on);
	flushWriteBuffer();

	return !gotError();
}

bool sqlrconnection::begin() {

	if (!openSession()) {
		return false;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Beginning...\n");
		debugPreEnd();
	}

	pvt->_cs->write((uint16_t)BEGIN);
	flushWriteBuffer();

	return !gotError();
}

bool sqlrconnection::commit() {

	if (!openSession()) {
		return false;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Committing...\n");
		debugPreEnd();
	}

	pvt->_cs->write((uint16_t)COMMIT);
	flushWriteBuffer();

	return !gotError();
}

bool sqlrconnection::rollback() {

	if (!openSession()) {
		return false;
	}

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Rolling Back...\n");
		debugPreEnd();
	}

	pvt->_cs->write((uint16_t)ROLLBACK);
	flushWriteBuffer();

	return !gotError();
}

const char *sqlrconnection::errorMessage() {
	return pvt->_error;
}

int64_t sqlrconnection::errorNumber() {
	return pvt->_errorno;
}

void sqlrconnection::clearError() {
	delete[] pvt->_error;
	pvt->_error=NULL;
	pvt->_errorno=0;
}

void sqlrconnection::setError(const char *err) {

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Setting Error\n");
		debugPreEnd();
	}

	delete[] pvt->_error;
	pvt->_error=charstring::duplicate(err);

	if (pvt->_debug) {
		debugPreStart();
		debugPrint(pvt->_error);
		debugPrint("\n");
		debugPreEnd();
	}
}

uint16_t sqlrconnection::getError() {

	clearError();

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Checking for error\n");
		debugPreEnd();
	}

	// get whether an error occurred or not
	uint16_t	status;
	if (pvt->_cs->read(&status,pvt->_responsetimeoutsec,
				pvt->_responsetimeoutusec)!=sizeof(uint16_t)) {
		setError("Failed to get the error status.\n"
				"A network error may have occurred.");
		return ERROR_OCCURRED;
	}

	// if no error occurred, return that
	if (status==NO_ERROR_OCCURRED) {
		if (pvt->_debug) {
			debugPreStart();
			debugPrint("No error occurred\n");
			debugPreEnd();
		}
		return status;
	}

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("An error occurred\n");
		debugPreEnd();
	}

	// get the error code
	if (pvt->_cs->read((uint64_t *)&pvt->_errorno)!=sizeof(uint64_t)) {
		setError("Failed to get the error code.\n"
				"A network error may have occurred.");
		return status;
	}

	// get the error size
	uint16_t	size;
	if (pvt->_cs->read(&size)!=sizeof(uint16_t)) {
		setError("Failed to get the error size.\n"
				"A network error may have occurred.");
		return status;
	}

	// get the error string
	pvt->_error=new char[size+1];
	if (pvt->_cs->read(pvt->_error,size)!=size) {
		setError("Failed to get the error string.\n"
				"A network error may have occurred.");
		return status;
	}
	pvt->_error[size]='\0';

	if (pvt->_debug) {
		debugPreStart();
		debugPrint("Got error:\n");
		debugPrint(pvt->_errorno);
		debugPrint(": ");
		debugPrint(pvt->_error);
		debugPrint("\n");
		debugPreEnd();
	}
	return status;
}

bool sqlrconnection::gotError() {
	uint16_t	status=getError();
	if (status==NO_ERROR_OCCURRED) {
		return false;
	}
	if (status==ERROR_OCCURRED_DISCONNECT) {
		endSession();
	}
	return true;
}

void sqlrconnection::debugOn() {
	pvt->_debug=true;
}

void sqlrconnection::debugOff() {
	pvt->_debug=false;
}

void sqlrconnection::setDebugFile(const char *filename) {
	pvt->_debugfile.close();
	error::clearError();
	if (filename && *filename &&
		!pvt->_debugfile.open(filename,O_WRONLY|O_APPEND) &&
				error::getErrorNumber()==ENOENT) {
		pvt->_debugfile.create(filename,
				permissions::evalPermString("rw-r--r--"));
	}
}

bool sqlrconnection::getDebug() {
	return pvt->_debug;
}

void sqlrconnection::debugPreStart() {
	if (pvt->_webdebug==-1) {
		const char	*docroot=environment::getValue("DOCUMENT_ROOT");
		if (!charstring::isNullOrEmpty(docroot)) {
			pvt->_webdebug=1;
		} else {
			pvt->_webdebug=0;
		}
	}
	if (pvt->_webdebug==1) {
		debugPrint("<pre>\n");
	}
}

void sqlrconnection::debugPreEnd() {
	if (pvt->_webdebug==1) {
		debugPrint("</pre>\n");
	}
}

void sqlrconnection::debugPrintFunction(
				int (*printfunction)(const char *,...)) {
	pvt->_printfunction=printfunction;
}

void sqlrconnection::debugPrint(const char *string) {
	if (pvt->_printfunction) {
		(*pvt->_printfunction)("%s",string);
	} else if (pvt->_debugfile.getFileDescriptor()!=-1) {
		pvt->_debugfile.printf("%s",string);
	} else {
		stdoutput.printf("%s",string);
	}
}

void sqlrconnection::debugPrint(int64_t number) {
	if (pvt->_printfunction) {
		(*pvt->_printfunction)("%lld",(long long)number);
	} else if (pvt->_debugfile.getFileDescriptor()!=-1) {
		pvt->_debugfile.printf("%lld",(long long)number);
	} else {
		stdoutput.printf("%lld",(long long)number);
	}
}

void sqlrconnection::debugPrint(double number) {
	if (pvt->_printfunction) {
		(*pvt->_printfunction)("%f",number);
	} else if (pvt->_debugfile.getFileDescriptor()!=-1) {
		pvt->_debugfile.printf("%f",number);
	} else {
		stdoutput.printf("%f",number);
	}
}

void sqlrconnection::debugPrint(char character) {
	if (pvt->_printfunction) {
		(*pvt->_printfunction)("%c",character);
	} else if (pvt->_debugfile.getFileDescriptor()!=-1) {
		pvt->_debugfile.printf("%c",character);
	} else {
		stdoutput.printf("%c",character);
	}
}

void sqlrconnection::debugPrintBlob(const char *blob, uint32_t length) {
	debugPrint('\n');
	int	column=0;
	for (uint32_t i=0; i<length; i++) {
		if (blob[i]>=' ' && blob[i]<='~') {
			debugPrint(blob[i]);
		} else {
			debugPrint('.');
		}
		column++;
		if (column==80) {
			debugPrint('\n');
			column=0;
		}
	}
	debugPrint('\n');
}

void sqlrconnection::debugPrintClob(const char *clob, uint32_t length) {
	debugPrint('\n');
	for (uint32_t i=0; i<length; i++) {
		if (clob[i]=='\0') {
			debugPrint("\\0");
		} else {
			debugPrint(clob[i]);
		}
	}
	debugPrint('\n');
}

void sqlrconnection::setClientInfo(const char *clientinfo) {
	delete[] pvt->_clientinfo;
	pvt->_clientinfo=charstring::duplicate(clientinfo);
	pvt->_clientinfolen=charstring::length(clientinfo);
}

const char *sqlrconnection::getClientInfo() const {
	return pvt->_clientinfo;
}

socketclient *sqlrconnection::cs() {
	return pvt->_cs;
}

bool sqlrconnection::endsessionsent() {
	return pvt->_endsessionsent;
}

bool sqlrconnection::suspendsessionsent() {
	return pvt->_suspendsessionsent;
}

bool sqlrconnection::connected() {
	return pvt->_connected;
}

int32_t sqlrconnection::responsetimeoutsec() {
	return pvt->_responsetimeoutsec;
}

int32_t sqlrconnection::responsetimeoutusec() {
	return pvt->_responsetimeoutusec;
}

int64_t sqlrconnection::errorno() {
	return pvt->_errorno;
}

char *sqlrconnection::error() {
	return pvt->_error;
}

char *sqlrconnection::clientinfo() {
	return pvt->_clientinfo;
}

uint64_t sqlrconnection::clientinfolen() {
	return pvt->_clientinfolen;
}

bool sqlrconnection::debug() {
	return pvt->_debug;
}

void sqlrconnection::firstcursor(sqlrcursor *cur) {
	pvt->_firstcursor=cur;
}

void sqlrconnection::lastcursor(sqlrcursor *cur) {
	pvt->_lastcursor=cur;
}

sqlrcursor *sqlrconnection::lastcursor() {
	return pvt->_lastcursor;
}

bool sqlrconnection::isYes(const char *str) {
	return charstring::isYes(str);
}

bool sqlrconnection::isNo(const char *str) {
	return charstring::isNo(str);
}

void sqlrconnection::setBindVariableDelimiters(const char *delimiters) {
	pvt->_questionmarksupported=charstring::contains(delimiters,'?');
	pvt->_colonsupported=charstring::contains(delimiters,':');
	pvt->_atsignsupported=charstring::contains(delimiters,'@');
	pvt->_dollarsignsupported=charstring::contains(delimiters,'$');
}

bool sqlrconnection::getBindVariableDelimiterQuestionMarkSupported() {
	return pvt->_questionmarksupported;
}

bool sqlrconnection::getBindVariableDelimiterColonSupported() {
	return pvt->_colonsupported;
}

bool sqlrconnection::getBindVariableDelimiterAtSignSupported() {
	return pvt->_atsignsupported;
}

bool sqlrconnection::getBindVariableDelimiterDollarSignSupported() {
	return pvt->_dollarsignsupported;
}
