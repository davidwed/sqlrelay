// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/string.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/permissions.h>

#include <ctype.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif
#include <errno.h>

#include <defines.h>
#include <defaults.h>
#include <datatypes.h>

sqlrconnection::sqlrconnection(const char *server, int port, const char *socket,
					const char *user, const char *password, 
					int retrytime, int tries) 
: inetclientsocket(), unixclientsocket() {

	// initialize...

	// retry reads if they get interrupted by signals
	retryInterruptedReads();

	// connection
	this->server=(char *)server;
	listenerinetport=(unsigned short)port;
	listenerunixport=(char *)socket;
	this->retrytime=retrytime;
	this->tries=tries;

	// authentication
	this->user=(char *)user;
	this->password=(char *)password;
	userlen=strlen(user);
	passwordlen=strlen(password);
	reconnect=0;

	// database id
	id=NULL;

	// session state
	connected=0;
	clearSessionFlags();

	// debug print function
	printfunction=NULL;

	// debug off
	debug=0;
	webdebug=-1;

	// copy references, delete cursors flags
	copyrefs=0;

	// error string
	error=NULL;

	// cursor list
	firstcursor=NULL;
	lastcursor=NULL;
}

sqlrconnection::~sqlrconnection() {

	// unless it was already ended or suspended, end the session
	if (!endsessionsent && !suspendsessionsent) {
		endSession();
	}

	// deallocate id
	delete[] id;

	// deallocate copied references
	if (copyrefs) {
		delete[] server;
		delete[] listenerunixport;
		delete[] user;
		delete[] password;
	}

	// detach all cursors attached to this client
	sqlrcursor	*currentcursor=firstcursor;
	while (currentcursor) {
		firstcursor=currentcursor;
		currentcursor=currentcursor->next;
		firstcursor->sqlrc=NULL;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Deallocated connection\n");
		debugPreEnd();
	}
}

int	sqlrconnection::endSession() {

	if (debug) {
		debugPreStart();
		debugPrint("Ending Session\n");
		debugPreEnd();
	}

	// abort each cursor's result set
	sqlrcursor	*currentcursor=firstcursor;
	while (currentcursor) {
		if (!currentcursor->endofresultset) {
			currentcursor->abortResultSet();
		}
		currentcursor=currentcursor->next;
	}

	// write a ~ to the connection
	int	retval=1;
	if (connected) {
		retval=0;
		write((unsigned short)END_SESSION);
		endsessionsent=1;
		retval=1;
		closeConnection();
	}
	return retval;
}

int	sqlrconnection::suspendSession() {

	if (!connected && !openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Suspending Session\n");
		debugPreEnd();
	}

	// suspend the session
	int	retval=0;
	write((unsigned short)SUSPEND_SESSION);
	suspendsessionsent=1;
	retval=1;

	// If the server is passing around file descriptors to handoff clients
	// from listener to connection, then it will have to open a socket and
	// port to enable suspend/resume.   It will pass that socket/port to
	// us here.
	if (!reconnect) {
		retval=getNewPort();
	}

	closeConnection();

	return retval;
}

int	sqlrconnection::getConnectionPort() {

	if (!suspendsessionsent && !connected && !openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Getting connection port: ");
		debugPrint((long)connectioninetport);
		debugPrint("\n");
		debugPreEnd();
	}

	return (int)connectioninetport;
}

char	*sqlrconnection::getConnectionSocket() {

	if (!suspendsessionsent && !connected && !openSession()) {
		return NULL;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Getting connection socket: ");
		if (connectionunixport) {
			debugPrint(connectionunixport);
		}
		debugPrint("\n");
		debugPreEnd();
	}

	if (connectionunixport) {
		return connectionunixport;
	}
	return NULL;
}

int	sqlrconnection::authenticateWithListener() {
	if (debug) {
		debugPreStart();
		debugPrint("Authenticating with listener : ");
	}
	return genericAuthentication();
}

int	sqlrconnection::genericAuthentication() {

	if (debug) {
		debugPrint(user);
		debugPrint(":");
		debugPrint(password);
		debugPrint("\n");
		debugPreEnd();
	}

	write((unsigned long)userlen);
	write(user,userlen);

	write((unsigned long)passwordlen);
	write(password,passwordlen);

	// check whether authentication was successful or not
	unsigned short	authsuccess;
	if (read(&authsuccess)!=sizeof(unsigned short)) {
		setError("Failed to authenticate.\n A network error may have ocurred.");
		return -1;
	}
	if (authsuccess==ERROR) {
		
		// clear all result sets
		sqlrcursor	*currentcursor=firstcursor;
		while (currentcursor) {
			currentcursor->clearResultSet();
			currentcursor=currentcursor->next;
		}

		setError("Authentication Error.");
		return 0;
	}
	if (debug) {
		debugPreStart();
		debugPrint("No authentication error.\n");
		debugPreEnd();
	}
	return 1;
}

void	sqlrconnection::getReconnect() {

	unsigned short	recon;
	if (read(&recon)!=sizeof(unsigned short)) {
		reconnect=-1;
		return;
	}

	if (recon==DONT_RECONNECT) {
		if (debug) {
			debugPreStart();
			debugPrint("Must Not Reconnect.\n");
			debugPreEnd();
		}
		reconnect=0;
		return;
	}
	if (debug) {
		debugPreStart();
		debugPrint("Must Reconnect.\n");
		debugPreEnd();
	}
	reconnect=1;
}

int	sqlrconnection::getNewPort() {

	// get the size of the unix port string
	unsigned short	size;
	if (read(&size)!=sizeof(unsigned short)) {
		return -1;
	}
	
	if (size<=MAXPATHLEN) {

		// get the unix port string
		if (size && read(connectionunixportbuffer,size)!=size) {
			return -1;
		}
		connectionunixportbuffer[size]=(char)NULL;
		connectionunixport=connectionunixportbuffer;

	} else {

		// if size is too big, return an error
		stringbuffer	errstr;
		errstr.append("The pathname of the unix port was too long: ");
		errstr.append((long)size);
		errstr.append(" bytes.  The maximum size is ");
		errstr.append((long)MAXPATHLEN);
		errstr.append(" bytes.");
		setError(errstr.getString());
		return 0;

	}

	// get the inet port
	if (read((unsigned short *)&connectioninetport)!=
					sizeof(unsigned short)) {
		return -1;
	}

	// the server will send 0 for both the size of the unixport and 
	// the inet port if a server error occurred
	if (!size && !connectioninetport) {
		setError("An error occurred on the server.");
		return 0;
	}

	return 1;
}

int	sqlrconnection::ping() {

	if (!connected && !openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Pinging...");
		debugPrint("\n");
		debugPreEnd();
	}

	write((unsigned short)PING);

	// get the ping result
	unsigned short	result;
	if (read(&result)!=sizeof(unsigned short)) {
		setError("Failed to ping.\n A network error may have ocurred.");
		result=0;
	}
	return (int)result;
}


char	*sqlrconnection::identify() {

	if (!connected && !openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Identifying...");
		debugPrint("\n");
		debugPreEnd();
	}

	write((unsigned short)IDENTIFY);

	// get the id
	unsigned short	size;
	if (read(&size)==sizeof(unsigned short)) {
		id=new char[size+1];
		if (read(id,size)!=size) {
			setError("Failed to identify.\n A network error may have ocurred.");
			delete[] id;
			return NULL;
		}
		id[size]=(char)NULL;

		if (debug) {
			debugPreStart();
			debugPrint(id);
			debugPrint("\n");
			debugPreEnd();
		}
	} else {
		setError("Failed to identify.\n A network error may have ocurred.");
		return NULL;
	}
	return id;
}

int	sqlrconnection::autoCommitOn() {
	return autoCommit(1);
}

int	sqlrconnection::autoCommitOff() {
	return autoCommit(0);
}

int	sqlrconnection::autoCommit(unsigned short on) {

	if (!connected && !openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Setting AutoCommit");
		if (on) {
			debugPrint("on");
		} else {
			debugPrint("off");
		}
		debugPrint("...\n");
		debugPreEnd();
	}

	write((unsigned short)AUTOCOMMIT);
	write(on);

	unsigned short	response;
	if (read(&response)!=sizeof(unsigned short)) {
		setError("Failed to get autocommit status.\n A network error may have ocurred.");
		return -1;
	}
	return (int)response;
}

int	sqlrconnection::commit() {

	if (!connected && !openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Committing...");
		debugPrint("\n");
		debugPreEnd();
	}

	write((unsigned short)COMMIT);

	unsigned short	response;
	if (read(&response)!=sizeof(unsigned short)) {
		setError("Failed to get commit status.\n A network error may have ocurred.");
		return -1;
	}
	return (int)response;
}

int	sqlrconnection::rollback() {

	if (!connected && !openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Rolling Back...");
		debugPrint("\n");
		debugPreEnd();
	}

	write((unsigned short)ROLLBACK);

	unsigned short	response;
	if (read(&response)!=sizeof(unsigned short)) {
		setError("Failed to get rollback status.\n A network error may have ocurred.");
		return -1;
	}
	return (int)response;
}

void	sqlrconnection::debugOn() {
	debug=1;
}

void	sqlrconnection::debugOff() {
	debug=0;
}

int	sqlrconnection::getDebug() {
	return debug;
}

void	sqlrconnection::clearError() {
	delete[] error;
	error=NULL;
}

void	sqlrconnection::setError(const char *err) {

	if (debug) {
		debugPreStart();
		debugPrint("Setting Error\n");
		debugPreEnd();
	}

	error=new char[strlen(err)+1];
	strcpy(error,err);

	if (debug) {
		debugPreStart();
		debugPrint(error);
		debugPrint("\n");
		debugPreEnd();
	}
}

void	sqlrconnection::debugPreStart() {
	if (webdebug==-1) {
		char	*docroot=getenv("DOCUMENT_ROOT");
		if (docroot && docroot[0]) {
			webdebug=1;
		} else {
			webdebug=0;
		}
	}
	if (webdebug==1) {
		debugPrint("<pre>\n");
	}
}

void	sqlrconnection::debugPreEnd() {
	if (webdebug==1) {
		debugPrint("</pre>\n");
	}
}

void	sqlrconnection::debugPrintFunction(
				int (*printfunction)(const char *,...)) {
	this->printfunction=printfunction;
}

void	sqlrconnection::debugPrint(const char *string) {
	if (printfunction) {
		(*printfunction)("%s",string);
	} else {
		printf("%s",string);
	}
}

void	sqlrconnection::debugPrint(long number) {
	if (printfunction) {
		(*printfunction)("%ld",number);
	} else {
		printf("%ld",number);
	}
}

void	sqlrconnection::debugPrint(double number) {
	if (printfunction) {
		(*printfunction)("%f",number);
	} else {
		printf("%f",number);
	}
}

void	sqlrconnection::debugPrint(char character) {
	if (printfunction) {
		(*printfunction)("%c",character);
	} else {
		printf("%c",character);
	}
}

void	sqlrconnection::debugPrintBlob(const char *blob, unsigned long length) {
	debugPrint('\n');
	int	column=0;
	for (unsigned long i=0; i<length; i++) {
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

void	sqlrconnection::debugPrintClob(const char *clob, unsigned long length) {
	debugPrint('\n');
	for (unsigned long i=0; i<length; i++) {
		if (clob[i]==(char)NULL) {
			debugPrint("\\0");
		} else {
			debugPrint(clob[i]);
		}
	}
	debugPrint('\n');
}

void	sqlrconnection::copyReferences() {

	// set the flag
	copyrefs=1;

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

void	sqlrconnection::clearSessionFlags() {

	// indicate that the session hasn't been suspended or ended
	endsessionsent=0;
	suspendsessionsent=0;
}

int	sqlrconnection::openSession() {

	if (debug) {
		debugPreStart();
		debugPrint("Connecting to listener...");
		debugPrint("\n");
		debugPreEnd();
	}

	// open a connection to the listener
	int	openresult=0;

	// first, try for a unix connection
	if (listenerunixport && listenerunixport[0]) {

		if (debug) {
			debugPreStart();
			debugPrint("Unix socket: ");
			debugPrint(listenerunixport);
			debugPrint("\n");
			debugPreEnd();
		}

		openresult=unixclientsocket::
				connectToServer(listenerunixport,
						retrytime,tries);
	}

	// then try for an inet connection
	if (!openresult && listenerinetport) {

		if (debug) {
			debugPreStart();
			debugPrint("Inet socket: ");
			debugPrint(server);
			debugPrint(":");
			debugPrint((long)listenerinetport);
			debugPrint("\n");
			debugPreEnd();
		}

		openresult=inetclientsocket::
				connectToServer(server,listenerinetport,
						retrytime,tries);
	}

	// handle failure to connect to listener
	if (!openresult) {
		setError("Couldn't connect to the listener.");
		return 0;
	}

	// authenticate with the listner
	if (authenticateWithListener()<1) {
		closeConnection();
		return 0;
	}

	// do we need to reconnect to the connection daemon
	getReconnect();


	if (reconnect==-1) {

		// if an error ocurred, set the error and close the connection
		setError("Failed to get whether we need to reconnect.\n A network error may have ocurred.");
		closeConnection();
		return 0;

	} else if (!reconnect) {

		// if we don't need to reconnect, just authenticate with the
		// connection daemon
		if (!authenticateWithConnection()) {
			closeConnection();
			return 0;
		}
		connected=1;

	} else if (reconnect==1) {

		// if we do need to reconnect, get which port(s) to reconnect
		// to and reconnect

		// try to get the connection daemon ports
		int	success=getNewPort();

		// close the connection to the listener
		closeConnection();

		// If getNewPort() returns -1 or 0 then an error ocurred.
		// If it returns 0, the error is already set.
		if (success<1) {
			if (success==-1) {
				setError("Failed to get connection ports.\n A network error may have ocurred.");
			}
			return 0;
		}

		if (debug) {
			debugPreStart();
			debugPrint("Reconnecting to ");
			debugPrint("\n");
			debugPrint("	unix port: ");
			debugPrint(connectionunixport);
			debugPrint("\n");
			debugPrint("	inet port: ");
			debugPrint((long)connectioninetport);
			debugPrint("\n");
			debugPreEnd();
		}

		// first, try for the unix port
		if (listenerunixport && listenerunixport[0] &&
					connectionunixport) {
			connected=unixclientsocket::
					connectToServer(connectionunixport,
							retrytime,tries);
			if (debug && !connected) {
				debugPreStart();
				debugPrint("ERROR:\n");
				debugPrint("connection to unix port failed: ");
				debugPrint(strerror(errno));
				debugPrint("\n");
				debugPreEnd();
			}
		}

		// then try for the inet port
		if (!connected && connectioninetport) {
			connected=inetclientsocket::
					connectToServer(server,
							connectioninetport,
							retrytime,tries);
			if (debug && !connected) {
				debugPreStart();
				debugPrint("ERROR:\n");
				debugPrint("connection to inet port failed: ");
				debugPrint(strerror(errno));
				debugPrint("\n");
				debugPreEnd();
			}
		}

		// did we successfully reconnect?
		if (connected) {

			if (debug) {
				debugPreStart();
				debugPrint("Connected.");
				debugPrint("\n");
				debugPreEnd();
			}

			clearSessionFlags();

			if (!authenticateWithConnection()) {
				return 0;
			}

		} else {

			// handle failure to connect to database 
			// connection daemon
			stringbuffer	errstr;
			errstr.append("Couldn't connect to the database");
			errstr.append("connection daemon.\n");
			if (connectionunixport) {
				errstr.append("Tried unix port ");
				errstr.append((long)connectionunixport);
			}
			if (connectioninetport) {
				errstr.append("Tried inet port ");
				errstr.append((long)connectioninetport);
			}
			errstr.append("\n");
			setError(errstr.getString());
			return 0;
		}
	}

	// if we made it here then everything went well;  listener 
	// authentication succeeded and we either didn't have to reconnect or
	// we sucessfully reconnected and sucessfully authenticated with the 
	// connection daemon
	return 1;
}

int	sqlrconnection::authenticateWithConnection() {

	write((unsigned short)AUTHENTICATE);
	if (debug) {
		debugPreStart();
		debugPrint("Authenticating with connection : ");
	}
	return genericAuthentication();
}

int	sqlrconnection::resumeSession(int port, const char *socket) {

	// if already connected, end the session
	if (connected) {
		endSession();
	}

	// set the connectionunixport and connectioninetport values
	if (copyrefs) {
		if (strlen(socket)<=MAXPATHLEN) {
			strcpy(connectionunixportbuffer,socket);
			connectionunixport=connectionunixportbuffer;
		} else {
			connectionunixport="";
		}
	} else {
		connectionunixport=(char *)socket;
	}
	connectioninetport=(unsigned short)port;

	// first, try for the unix port
	if (socket && socket[0]) {
		connected=unixclientsocket::
				connectToServer(socket,retrytime,tries);
	}

	// then try for the inet port
	if (!connected) {
		connected=inetclientsocket::
				connectToServer(server,port,retrytime,tries);
	}

	if (debug) {
		debugPreStart();
		debugPrint("Resuming Session: ");
		debugPreEnd();
	}

	if (connected) {
		if (debug) {
			debugPreStart();
			debugPrint("success");
		       	debugPrint("\n");
			debugPreEnd();
		}
		clearSessionFlags();
	} else {
		if (debug) {
			debugPreStart();
			debugPrint("failure");
			debugPrint("\n");
			debugPreEnd();
		}
	}

	return connected;
}

void	sqlrconnection::closeConnection() {
	close();
	connected=0;
}

sqlrcursor::sqlrcursor(sqlrconnection *sqlrc) {

	// copy references
	copyrefs=0;

	this->sqlrc=sqlrc;

	// put self in connection's cursor list
	if (sqlrc->lastcursor) {
		sqlrc->lastcursor->next=this;
		prev=sqlrc->lastcursor;
	} else {
		sqlrc->firstcursor=this;
		prev=NULL;
	}
	sqlrc->lastcursor=this;
	next=NULL;

	// session state
	cached=0;

	// query
	querybuffer=NULL;
	fullpath=NULL;

	// result set
	rsbuffersize=0;

	firstrowindex=0;
	rowcount=0;
	previousrowcount=0;
	actualrows=0;
	affectedrows=0;
	endofresultset=1;

	error=NULL;

	rows=NULL;
	extrarows=NULL;
	firstextrarow=NULL;
	rowstorage=new memorypool(OPTIMISTIC_RESULT_SET_SIZE,
			OPTIMISTIC_RESULT_SET_SIZE/OPTIMISTIC_ROW_COUNT,5);
	fields=NULL;
	fieldlengths=NULL;
	getrowcount=0;
	getrowlengthcount=0;

	colcount=0;
	previouscolcount=0;
	columns=NULL;
	extracolumns=NULL;
	colstorage=new memorypool(OPTIMISTIC_COLUMN_DATA_SIZE,
			OPTIMISTIC_COLUMN_DATA_SIZE/OPTIMISTIC_COLUMN_COUNT,5);
	columnnamearray=NULL;

	returnnulls=0;

	// cache file
	cachesource=NULL;
	cachesourceind=NULL;
	cachedestname=NULL;
	cachedestindname=NULL;
	cachedest=NULL;
	cachedestind=NULL;
	cacheon=0;

	// options...
	sendcolumninfo=SEND_COLUMN_INFO;
	colcase=MIXED_CASE;

	// cursor id
	cursorid=0;

	initVariables();
}

sqlrcursor::~sqlrcursor() {

	// abort result set if necessary
	if (!endofresultset && sqlrc &&
			!sqlrc->endsessionsent && 
			!sqlrc->suspendsessionsent) {
		abortResultSet();
	}

	// deallocate copied references
	deleteVariables();

	// deallocate the query buffer
	delete[] querybuffer;

	// deallocate the fullpath (used for file queries)
	delete[] fullpath;

	clearResultSet();
	delete[] columns;
	delete[] extracolumns;
	delete colstorage;
	if (rows) {
		for (int i=0; i<OPTIMISTIC_ROW_COUNT; i++) {
			delete rows[i];
		}
		delete[] rows;
	}
	delete rowstorage;

	// it's possible for the connection to be deleted before the 
	// cursor is, in that case, don't do any of this stuff
	if (sqlrc) {

		// remove self from connection's cursor list
		if (!next && !prev) {
			sqlrc->firstcursor=NULL;
			sqlrc->lastcursor=NULL;
		} else {
			sqlrcursor	*temp=next;
			if (next) {
				next->prev=prev;
			} else {
				sqlrc->lastcursor=prev;
			}
			if (prev) {
				prev->next=temp;
			} else {
				sqlrc->firstcursor=next;
			}
		}

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Deallocated cursor\n");
			sqlrc->debugPreEnd();
		}
	}

	if (copyrefs && cachedestname) {
		delete[] cachedestname;
	}
	delete[] cachedestindname;
}

void	sqlrcursor::copyReferences() {

	// set the flag
	copyrefs=1;
}

void	sqlrcursor::clearVariables() {

	// setting the bind/substitution variable 
	// counts to 0 effectively clears them
	subcount=0;
	inbindcount=0;
	outbindcount=0;
}

void	sqlrcursor::initVariables() {

	// initialize the bind and substitution variables
	for (int i=0; i<MAXVAR; i++) {
		inbindvars[i].variable=NULL;
		inbindvars[i].value.stringval=NULL;
		inbindvars[i].type=STRING_BIND;
		outbindvars[i].variable=NULL;
		outbindvars[i].value.stringval=NULL;
		outbindvars[i].type=STRING_BIND;
		subvars[i].variable=NULL;
		subvars[i].value.stringval=NULL;
		subvars[i].type=STRING_BIND;
	}
}

void	sqlrcursor::deleteVariables() {

	// if we were copying values, delete them
	if (copyrefs) {
		for (int i=0; i<MAXVAR; i++) {
			delete[] inbindvars[i].variable;
			if (inbindvars[i].type==STRING_BIND) {
				delete[] inbindvars[i].value.stringval;
			}
			if (inbindvars[i].type==BLOB_BIND ||
				inbindvars[i].type==CLOB_BIND) {
				delete[] inbindvars[i].value.lobval;
			}
			delete[] outbindvars[i].variable;
			delete[] subvars[i].variable;
			if (subvars[i].type==STRING_BIND) {
				delete[] subvars[i].value.stringval;
			}
		}
	}
	for (int i=0; i<MAXVAR; i++) {
		if (outbindvars[i].type==STRING_BIND) {
			delete[] outbindvars[i].value.stringval;
		}
		if (outbindvars[i].type==BLOB_BIND ||
			outbindvars[i].type==CLOB_BIND) {
			delete[] outbindvars[i].value.lobval;
		}
	}
}

void	sqlrcursor::clearRows() {

	// delete data in rows for long datatypes
	unsigned long	rowbuffercount=rowcount-firstrowindex;
	for (unsigned long i=0; i<rowbuffercount; i++) {
	        for (unsigned long j=0; j<colcount; j++) {
		        // char	*data=getFieldInternal(i,j);
			if (getColumn(j)->longdatatype) {
				delete[] getFieldInternal(i,j);
			}
		}
	}

	// delete linked list storing extra result set fields
	row	*currentrow;
	if (firstextrarow) {
		currentrow=firstextrarow;
		while (currentrow) {
			firstextrarow=currentrow->next;
			delete currentrow;
			currentrow=firstextrarow;
		}
		firstextrarow=NULL;
	}
	currentrow=NULL;

	// delete array pointing to linked list items
	delete[] extrarows;
	extrarows=NULL;

	// delete arrays of fields and field lengths
	if (fields) {
		for (unsigned long i=0; i<rowbuffercount; i++) {
			delete[] fields[i];
		}
		delete[] fields;
		fields=NULL;
	}
	if (fieldlengths) {
		for (unsigned long i=0; i<rowbuffercount; i++) {
			delete[] fieldlengths[i];
		}
		delete[] fieldlengths;
		fieldlengths=NULL;
	}

	// reset the row storage pool
	rowstorage->free();
}

void	sqlrcursor::clearCacheDest() {
	if (cachedest) {
		cachedest->close();
		delete cachedest;
		cachedest=NULL;
		cachedestind->close();
		delete cachedestind;
		cachedestind=NULL;
		cacheon=0;
	}
}

void	sqlrcursor::clearCacheSource() {
	if (cachesource) {
		cachesource->close();
		delete cachesource;
		cachesource=NULL;
	}
	if (cachesourceind) {
		cachesourceind->close();
		delete cachesourceind;
		cachesourceind=NULL;
	}
}

void	sqlrcursor::clearError() {
	delete[] error;
	error=NULL;
	if (sqlrc) {
		sqlrc->clearError();
	}
}

int	sqlrcursor::fetchRowIntoBuffer(int row) {

	// if we getting the entire result set at once, then the result set 
	// buffer index is the requested row-firstrowindex
	if (!rsbuffersize) {
		if (row<(int)rowcount && row>=(int)firstrowindex) {
			return row-firstrowindex;
		}
		return -1;
	}

	// but, if we're not getting the entire result set at once
	// and if the requested row is not in the current range, 
	// fetch more data from the connection
	while (row>=(int)(firstrowindex+rsbuffersize) && !endofresultset) {
		if (sqlrc->connected || 
				(cachesource && cachesourceind)) {
			clearRows();

			// if we're not fetching from a cached result set,
			// tell the server to send one 
			if (!cachesource && !cachesourceind) {
				sqlrc->write((unsigned short)FETCH_RESULT_SET);
				sqlrc->write(cursorid);
			}

			if (skipAndFetch(row)==-1 || parseData()==-1) {
				return -1;
			}
		} else {
			return -1;
		}
	}

	// return the buffer index corresponding to the requested row
	// or -1 if the requested row is past the end of the result set
	if (row<rowcount) {
		return row%rsbuffersize;
	}
	return -1;
}

char	**sqlrcursor::getColumnNames() {

	if (sendcolumninfo==DONT_SEND_COLUMN_INFO ||
			sentcolumninfo==DONT_SEND_COLUMN_INFO) {
		return NULL;
	}

	if (!columnnamearray) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Creating Column Arrays...\n");
			sqlrc->debugPreEnd();
		}
	
		// build a 2d array of pointers to the column names
		columnnamearray=new char *[colcount+1];
		columnnamearray[colcount]=NULL;
		for (unsigned long i=0; i<colcount; i++) {
			columnnamearray[i]=getColumn(i)->name;
		}
	}
	return columnnamearray;
}

char	*sqlrcursor::getColumnName(int col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			colcount && col>=0 && col<(int)colcount) {
		return getColumn(col)->name;
	}
	return NULL;
}

char	*sqlrcursor::getColumnType(int col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			colcount && col>=0 && col<(int)colcount) {
		column	*whichcolumn=getColumn(col);
		if (columntypeformat!=COLUMN_TYPE_IDS) {
			return whichcolumn->typestring;
		}
		return datatypestring[whichcolumn->type];
	}
	return NULL;
}

int	sqlrcursor::getColumnLength(int col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			colcount && col>=0 && col<(int)colcount) {
		return getColumn(col)->length;
	}
	return 0;
}

unsigned long	sqlrcursor::getColumnPrecision(int col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			colcount && col>=0 && col<(int)colcount) {
		return getColumn(col)->precision;
	}
	return 0;
}

unsigned long	sqlrcursor::getColumnScale(int col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			colcount && col>=0 && col<(int)colcount) {
		return getColumn(col)->scale;
	}
	return 0;
}

int	sqlrcursor::getLongest(int col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			colcount && col>=0 && col<(int)colcount) {
		return getColumn(col)->longest;
	} 
	return 0;
}

char	*sqlrcursor::getColumnType(const char *col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO) {
		column	*whichcolumn;
		for (unsigned long i=0; i<colcount; i++) {
			whichcolumn=getColumn(i);
			if (!strcasecmp(whichcolumn->name,col)) {
				if (columntypeformat!=COLUMN_TYPE_IDS) {
					return whichcolumn->typestring;
				}
				return datatypestring[whichcolumn->type];
			}
		}
	}
	return NULL;
}

int	sqlrcursor::getColumnLength(const char *col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO) {
		column	*whichcolumn;
		for (unsigned long i=0; i<colcount; i++) {
			whichcolumn=getColumn(i);
			if (!strcasecmp(whichcolumn->name,col)) {
				return whichcolumn->length;
			}
		}
	}
	return 0;
}

unsigned long	sqlrcursor::getColumnPrecision(const char *col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO) {
		column	*whichcolumn;
		for (unsigned long i=0; i<colcount; i++) {
			whichcolumn=getColumn(i);
			if (!strcasecmp(whichcolumn->name,col)) {
				return whichcolumn->precision;
			}
		}
	}
	return 0;
}

unsigned long	sqlrcursor::getColumnScale(const char *col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO) {
		column	*whichcolumn;
		for (unsigned long i=0; i<colcount; i++) {
			whichcolumn=getColumn(i);
			if (!strcasecmp(whichcolumn->name,col)) {
				return whichcolumn->scale;
			}
		}
	}
	return 0;
}

int	sqlrcursor::getLongest(const char *col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO) {
		column	*whichcolumn;
		for (unsigned long i=0; i<colcount; i++) {
			whichcolumn=getColumn(i);
			if (!strcasecmp(whichcolumn->name,col)) {
				return whichcolumn->longest;
			}
		}
	}
	return 0;
}

void	sqlrcursor::cacheData() {

	if (!cachedest) {
		return;
	}

	// write the data to the cache file
	int	rowbuffercount=rowcount-firstrowindex;
	for (int i=0; i<rowbuffercount; i++) {

		// get the current offset in the cache destination file
		long	position=cachedest->getCurrentPosition();

		// seek to the right place in the index file and write the
		// destination file offset
		cachedestind->setPositionRelativeToBeginning(
			13+sizeof(long)+((firstrowindex+i)*sizeof(long)));
		cachedestind->write(position);

		// write the row to the cache file
		for (unsigned long j=0; j<colcount; j++) {
			unsigned short	type;
			long	len;
			char	*field=getFieldInternal(i,j);
			if (field) {
				type=NORMAL_DATA;
				len=strlen(field);
				cachedest->write(type);
				cachedest->write(len);
				if (len>0) {
					cachedest->write(field);
				}
			} else {
				type=NULL_DATA;
				cachedest->write(type);
			}
		}
	}

	if (endofresultset) {
		finishCaching();
	}
}

int	sqlrcursor::firstRowIndex() {
	return firstrowindex;
}

int	sqlrcursor::endOfResultSet() {
	return endofresultset;
}

int	sqlrcursor::rowCount() {
	return rowcount;
}

int	sqlrcursor::affectedRows() {
	if (knowsaffectedrows==AFFECTED_ROWS) {
		return affectedrows;
	}
	return -1;
}

int	sqlrcursor::totalRows() {
	if (knowsactualrows==ACTUAL_ROWS) {
		return actualrows;
	}
	return -1;
}

int	sqlrcursor::colCount() {
	return colcount;
}

char	*sqlrcursor::errorMessage() {

	if (error) {
		return error;
	} else if (sqlrc->error) {
		return sqlrc->error;
	}
	return NULL;
}

void	sqlrcursor::getNullsAsEmptyStrings() {
	returnnulls=0;
}

void	sqlrcursor::getNullsAsNulls() {
	returnnulls=1;
}

char	*sqlrcursor::getField(int row, int col) {

	if (rowcount && row>=0 && row>=(int)firstrowindex && 
					col>=0 && col<(int)colcount) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		int	rowbufferindex=fetchRowIntoBuffer(row);

		if (rowbufferindex>-1) {
			char	*retval=getFieldInternal(rowbufferindex,col);
			return retval;
		}
	}
	return NULL;
}

char	*sqlrcursor::getField(int row, const char *col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			rowcount && row>=0 && row>=(int)firstrowindex) {
		for (unsigned long i=0; i<colcount; i++) {
			if (!strcasecmp(getColumn(i)->name,col)) {

				// in the event that we're stepping through the
				// result set instead of buffering the entire 
				// thing, the requested row may have to be 
				// fetched into the buffer...
				int	rowbufferindex=fetchRowIntoBuffer(row);

				if (rowbufferindex>-1) {
					return getFieldInternal(
							rowbufferindex,i);
				}
				return NULL;
			}
		}
	}
	return NULL;
}

long	sqlrcursor::getFieldLength(int row, int col) {

	if (rowcount && row>=0 && row>=(int)firstrowindex && 
					col>=0 && col<(int)colcount) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		int	rowbufferindex=fetchRowIntoBuffer(row);

		if (rowbufferindex>-1) {
			return getFieldLengthInternal(rowbufferindex,col);
		}
	}
	return -1;
}

long	sqlrcursor::getFieldLength(int row, const char *col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			rowcount && row>=0 && row>=(int)firstrowindex) {

		for (unsigned long i=0; i<colcount; i++) {
			if (!strcasecmp(getColumn(i)->name,col)) {

				// in the event that we're stepping through the
				// result set instead of buffering the entire 
				// thing, the requested row may have to be 
				// fetched into the buffer...
				int	rowbufferindex=fetchRowIntoBuffer(row);

				if (rowbufferindex>-1) {
					return getFieldLengthInternal(
							rowbufferindex,i);
				}
				return -1;
			}
		}
	}
	return -1;
}

char	**sqlrcursor::getRow(int row) {

	if (rowcount && row>=0 && row>=(int)firstrowindex) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		int	rowbufferindex=fetchRowIntoBuffer(row);

		if (rowbufferindex>-1) {
			if (!fields) {
				createFields();
			}
			return fields[rowbufferindex];
		}
	}
	return NULL;
}

long	*sqlrcursor::getRowLengths(int row) {

	if (rowcount && row>=0 && row>=(int)firstrowindex) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		int	rowbufferindex=fetchRowIntoBuffer(row);

		if (rowbufferindex>-1) {
			if (!fieldlengths) {
				createFieldLengths();
			}
			return (long *)fieldlengths[rowbufferindex];
		}
	}
	return NULL;
}

void	sqlrcursor::createExtraRowArray() {

	// create the arrays
	int	howmany=rowcount-firstrowindex-OPTIMISTIC_ROW_COUNT;
	extrarows=new row *[howmany];
	
	// populate the arrays
	row	*currentrow=firstextrarow;
	for (int i=0; i<howmany; i++) {
		extrarows[i]=currentrow;
		currentrow=currentrow->next;
	}
}

void	sqlrcursor::createFields() {
	// lets say that rowcount=5 and firstrowindex=3,
	// the fields array will contain 2 elements:
	// 	fields[0] (corresponding to row 3) and
	// 	fields[1] (corresponding to row 4)
	unsigned long	rowbuffercount=rowcount-firstrowindex;
	fields=new char **[rowbuffercount+1];
	fields[rowbuffercount]=(char **)NULL;
	for (unsigned long i=0; i<rowbuffercount; i++) {
		fields[i]=new char *[colcount+1];
		fields[i][colcount]=(char *)NULL;
		for (unsigned long j=0; j<colcount; j++) {
			fields[i][j]=getFieldInternal(i,j);
		}
	}
}

void	sqlrcursor::createFieldLengths() {
	// lets say that rowcount=5 and firstrowindex=3,
	// the fieldlengths array will contain 2 elements:
	// 	fieldlengths[0] (corresponding to row 3) and
	// 	fieldlengths[1] (corresponding to row 4)
	unsigned long	rowbuffercount=rowcount-firstrowindex;
	fieldlengths=new unsigned long *[rowbuffercount+1];
	fieldlengths[rowbuffercount]=(unsigned long)NULL;
	for (unsigned long i=0; i<rowbuffercount; i++) {
		fieldlengths[i]=new unsigned long[colcount+1];
		fieldlengths[i][colcount]=(unsigned long)NULL;
		for (unsigned long j=0; j<colcount; j++) {
			fieldlengths[i][j]=getFieldLengthInternal(i,j);
		}
	}
}

void	sqlrcursor::setError(const char *err) {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Setting Error\n");
		sqlrc->debugPreEnd();
	}
	error=strdup(err);
	handleError();
}

void	sqlrcursor::getErrorFromServer() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Getting Error From Server\n");
		sqlrc->debugPreEnd();
	}

	// get the length of the error string
	unsigned short	length;
	if (getShort(&length)!=sizeof(unsigned short)) {
		error=new char[77];
		strcpy(error,"There was an error, but the connection died trying to retrieve it.  Sorry.");
	} else {
		// get the error string
		error=new char[length+1];
		sqlrc->read(error,length);
		error[length]=(char)NULL;
	}
	
	handleError();
}

void	sqlrcursor::handleError() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint(error);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	endofresultset=1;

	if (resumed || !cachedest) {
		return;
	}

	// write the number of returned rows, affected rows 
	// and a zero to terminate the column descriptions
	cachedest->write((unsigned short)NO_ACTUAL_ROWS);
	cachedest->write((unsigned short)NO_AFFECTED_ROWS);
	cachedest->write((unsigned short)END_COLUMN_INFO);
	finishCaching();
}

int	sqlrcursor::skipAndFetch(int rowtoget) {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Skipping and Fetching\n");
		if (rowtoget>-1) {
			sqlrc->debugPrint("	row to get: ");
			sqlrc->debugPrint((long)rowtoget);
			sqlrc->debugPrint("\n");
		}
		sqlrc->debugPreEnd();
	}

	// if we're stepping through the result set, we can possibly 
	// skip a big chunk of it...
	if (!skipRows(rowtoget)) {
		return -1;
	}

	// tell the connection how many rows to send
	fetchRows();
	return 1;
}

void	sqlrcursor::fetchRows() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Fetching ");
		sqlrc->debugPrint(rsbuffersize);
		sqlrc->debugPrint(" rows\n");
		sqlrc->debugPreEnd();
	}

	// if we're reading from a cached result set, do nothing
	if (cachesource && cachesourceind) {
		return;
	}

	// otherwise, send to the connection the number of rows to send back
	sqlrc->write((unsigned long)rsbuffersize);
}

int	sqlrcursor::skipRows(int rowtoget) {

	// if we're reading from a cached result set we have to manually skip
	if (cachesource && cachesourceind) {

		// if rowtoget is -1 then don't skip,
		// otherwise skip to the next block of rows
		if (rowtoget==-1) {
			return 1;
		} else {
			rowcount=rowtoget-(rowtoget%rsbuffersize);
		}

		// get the row offset from the index
		cachesourceind->setPositionRelativeToBeginning(
				13+sizeof(long)+(rowcount*sizeof(long)));
		long	rowoffset;
		if (cachesourceind->read(&rowoffset)!=sizeof(long)) {
			setError("The cache file index appears to be corrupt.");
			return 0;
		}

		// skip to that offset in the cache file
		cachesource->setPositionRelativeToBeginning(rowoffset);
		return 1;
	}

	// calculate how many rows to skip unless we're buffering the entire
	// result set or caching the result set
	unsigned long	skip=0;
	if (rsbuffersize && !cachedest && rowtoget>-1) {
		skip=(long)((rowtoget-(rowtoget%rsbuffersize))-rowcount); 
		rowcount=rowcount+skip;
	}
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Skipping ");
		sqlrc->debugPrint((long)skip);
		sqlrc->debugPrint(" rows\n");
		sqlrc->debugPreEnd();
	}

	// if we're reading from a connection, send the connection the 
	// number of rows to skip
	sqlrc->write(skip);
	return 1;
}

int	sqlrcursor::parseData() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Parsing Data\n");
		sqlrc->debugPreEnd();
	}

	// if we're already at the end of the result set, then just return
	if (endofresultset) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Already at the end of the result set\n");
			sqlrc->debugPreEnd();
		}
		return 1;
	}

	// useful variables
	unsigned short	type;
	unsigned long	length;
	char	*buffer=NULL;
	unsigned long	colindex=0;
	column	*currentcol;
	row	*currentrow=NULL;

	// set firstrowindex to the index of the first row in the buffer
	firstrowindex=rowcount;

	// keep track of how large the buffer is
	int	rowbuffercount=0;

	// get rows
	for (;;) {

		// get the type of the field
		if (getShort(&type)!=sizeof(unsigned short)) {
			return -1;
		}

		// check for the end of the result set
		if (type==END_RESULT_SET) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("Got end of result set.\n");
				sqlrc->debugPreEnd();
			}
			endofresultset=1;

			// if we were stepping through a cached result set
			// then we need to close the file
			clearCacheSource();
			break;
		} 

		// if we're on the first column, start a new row,
		// reset the column pointer, and increment the
		// buffer counter and total row counter
		if (colindex==0) {

			if (rowbuffercount<OPTIMISTIC_ROW_COUNT) {
				if (!rows) {
					createRowBuffers();
				}
				currentrow=rows[rowbuffercount];
			} else {
				if (sqlrc->debug) {
					sqlrc->debugPreStart();
					sqlrc->debugPrint("Creating extra rows.\n");
					sqlrc->debugPreEnd();
				}
				if (!firstextrarow) {
					currentrow=new row(colcount);
					firstextrarow=currentrow;
				} else {
					currentrow->next=new row(colcount);
					currentrow=currentrow->next;
				}
			}
			if (colcount>previouscolcount) {
				currentrow->resize(colcount);
			}

			rowbuffercount++;
			rowcount++;
		}

		if (type==NULL_DATA) {

			// handle null data
			if (returnnulls) {
				buffer=NULL;
			} else {
				buffer=(char *)rowstorage->malloc(1);
				buffer[0]=(char)NULL;
			}
			length=0;

		} else if (type==NORMAL_DATA) {
		
			// handle non-null data
			if (getLong(&length)!=sizeof(unsigned long)) {
				return -1;
			}

			// for non-long, non-NULL datatypes...
			// get the field into a buffer
			buffer=(char *)rowstorage->malloc(length+1);
			if ((unsigned long)getString(buffer,length)!=length) {
				return -1;
			}
			buffer[length]=(char)NULL;

		} else if (type==START_LONG_DATA) {

			// handle a long datatype
			char	*oldbuffer=NULL;
			int	totallength=0;
			for (;;) {

				// get the type of the chunk
				if (getShort(&type)!=sizeof(unsigned short)) {
					return -1;
				}

				// check to see if we're done
				if (type==END_LONG_DATA) {
					break;
				}

				// get the length of the chunk
				if (getLong(&length)!=sizeof(unsigned long)) {
					delete[] buffer;
					return -1;
				}

				buffer=new char[totallength+length+1];
				if (totallength) {
					memcpy(buffer,oldbuffer,totallength);
					delete[] oldbuffer;
					oldbuffer=buffer;
					buffer=buffer+totallength;
				} else {
					oldbuffer=buffer;
				}
				totallength=totallength+length;

				if ((unsigned long)getString(buffer,length)!=length) {
					delete[] buffer;
					return -1;
				}

				// NULL terminate the buffer.  This makes 
				// certain operations safer and won't hurt
				// since the actual length (which doesn't
				// include the NULL) is available from
				// getFieldLength.
				buffer[length]=(char)NULL;
			}
			buffer=oldbuffer;
			length=totallength;

		}

		// add the buffer to the current row
		currentrow->addField(colindex,buffer,length);
	
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			if (buffer) {
				sqlrc->debugPrint("\"");
				sqlrc->debugPrint(buffer);
				sqlrc->debugPrint("\",");
			} else {
				sqlrc->debugPrint(buffer);
				sqlrc->debugPrint(",");
			}
			sqlrc->debugPreEnd();
		}

		// tag the column as a long data type or not
		currentcol=getColumn(colindex);
		currentcol->longdatatype=(type==END_LONG_DATA)?1:0;

		// keep track of the longest field
		if (sendcolumninfo==SEND_COLUMN_INFO && 
				sentcolumninfo==SEND_COLUMN_INFO) {
			if (length>(unsigned long)(currentcol->longest)) {
				currentcol->longest=length;
			}
		}

		// move to the next column, handle end of row 
		colindex++;
		if (colindex==colcount) {

			colindex=0;

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

			// check to see if we've gotten enough rows
			if (rsbuffersize && rowbuffercount==rsbuffersize) {
				break;
			}
		}
	}

	// terminate the row list
	if (rowbuffercount>=OPTIMISTIC_ROW_COUNT && currentrow) {
		currentrow->next=NULL;
		createExtraRowArray();
	}

	// cache the rows
	cacheData();

	return 1;
}

int	sqlrcursor::parseOutputBinds() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Receiving Output Bind Values: \n");
		sqlrc->debugPreEnd();
	}

	// useful variables
	unsigned short	type;
	unsigned long	length;
	int	count=0;

	// get the bind values
	for (;;) {

		// get the data type
		if (getShort(&type)!=sizeof(unsigned short)) {
			return -1;
		}

		// check for end of bind values
		if (type==END_BIND_VARS) {

			break;

		} else if (type==NULL_DATA) {

			// handle a null value
			if (returnnulls) {
				outbindvars[count].value.stringval=NULL;
			} else {
				outbindvars[count].value.stringval=new char[1];
				outbindvars[count].value.stringval[0]=
								(char)NULL;
			}

		} else if (type==NORMAL_DATA) {

			// get the value length
			if (getLong(&length)!=sizeof(unsigned long)) {
				return -1;
			}
			outbindvars[count].valuesize=length;
			outbindvars[count].value.stringval=new char[length+1];
			if ((unsigned long)getString(outbindvars[count].value.
						stringval,length)!=length) {
				return -1;
			}
			outbindvars[count].value.stringval[length]=(char)NULL;

		} else if (type==CURSOR_DATA) {

			if (getShort((unsigned short *)
					&(outbindvars[count].value.cursorid))!=
						sizeof(unsigned short)) {
				return -1;
			}

		} else {

			char	*buffer=NULL;
			char	*oldbuffer=NULL;
			unsigned long	totallength=0;
			unsigned long	length;
			while (1) {

				// get the type of the chunk
				if (getShort(&type)!=
						sizeof(unsigned short)) {
					return -1;
				}

				// check to see if we're done
				if (type==END_LONG_DATA) {
					break;
				}

				// get the length of the chunk
				if (getLong(&length)!=sizeof(unsigned long)) {
					delete[] buffer;
					return -1;
				}

				buffer=new char[totallength+length+1];
				if (totallength) {
					memcpy(buffer,oldbuffer,totallength);
					delete[] oldbuffer;
					oldbuffer=buffer;
					buffer=buffer+totallength;
				} else {
					oldbuffer=buffer;
				}
				totallength=totallength+length;

				if ((unsigned long)getString(buffer,length)!=length) {
					delete[] buffer;
					return -1;
				}

				// NULL terminate the buffer.  This makes 
				// certain operations safer and won't hurt
				// since the actual length (which doesn't
				// include the NULL) is available from
				// getOutputBindLength.
				buffer[length]=(char)NULL;
			}
			outbindvars[count].value.lobval=oldbuffer;
			outbindvars[count].valuesize=totallength;
		}

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint(outbindvars[count].variable);
			sqlrc->debugPrint("=");
			if (outbindvars[count].type==BLOB_BIND) {
				sqlrc->debugPrintBlob(
					outbindvars[count].value.lobval,
					outbindvars[count].valuesize);
			} else if (outbindvars[count].type==CLOB_BIND) {
				sqlrc->debugPrintClob(
					outbindvars[count].value.lobval,
					outbindvars[count].valuesize);
			} else if (outbindvars[count].type==CURSOR_BIND) {
				sqlrc->debugPrint((long)outbindvars[count].
								value.cursorid);
			} else {
				sqlrc->debugPrint(outbindvars[count].
							value.stringval);
			}
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		count++;
	}

	// cache the output binds
	cacheOutputBinds(count);

	return 1;
}

void	sqlrcursor::cacheNoError() {

	if (resumed || !cachedest) {
		return;
	}

	// write the number of returned rows
	cachedest->write((unsigned short)NO_ERROR);
}

void	sqlrcursor::cacheColumnInfo() {

	if (resumed || !cachedest) {
		return;
	}

	// write the number of returned rows
	cachedest->write(knowsactualrows);
	if (knowsactualrows==ACTUAL_ROWS) {
		cachedest->write(actualrows);
	}

	// write the number of affected rows
	cachedest->write(knowsaffectedrows);
	if (knowsaffectedrows==AFFECTED_ROWS) {
		cachedest->write(affectedrows);
	}

	// write whether or not the column info is is cached
	cachedest->write(sentcolumninfo);

	// write the column count
	cachedest->write(colcount);

	// write column descriptions to the cache file
	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO) {

		// write column type format
		cachedest->write(columntypeformat);

		// write the columns themselves
		unsigned short	namelen;
		column	*whichcolumn;
		for (unsigned long i=0; i<colcount; i++) {
			whichcolumn=getColumn(i);
			namelen=strlen(whichcolumn->name);
			cachedest->write(namelen);
			cachedest->write(whichcolumn->name,namelen);
			if (columntypeformat==COLUMN_TYPE_IDS) {
				cachedest->write(whichcolumn->type);
			} else {
				cachedest->write(whichcolumn->typestringlength);
				cachedest->write(whichcolumn->typestring,
						whichcolumn->typestringlength);
			}
			cachedest->write(whichcolumn->length);
			cachedest->write(whichcolumn->precision);
			cachedest->write(whichcolumn->scale);
		}
	}
}

void	sqlrcursor::cacheOutputBinds(int count) {

	if (resumed || !cachedest) {
		return;
	}

	// write the variable/value pairs to the cache file
	unsigned short	len;
	for (int i=0; i<count; i++) {

		cachedest->write((unsigned short)outbindvars[i].type);

		len=strlen(outbindvars[i].variable);
		cachedest->write(len);
		cachedest->write(outbindvars[i].variable,len);

		len=outbindvars[i].valuesize;
		cachedest->write(len);
		if (outbindvars[i].type==STRING_BIND) {
			cachedest->write(outbindvars[i].value.stringval,len);
		} else if (outbindvars[i].type!=NULL_BIND) {
			cachedest->write(outbindvars[i].value.lobval,len);
		}
	}

	// terminate the list of output binds
	cachedest->write((unsigned short)END_BIND_VARS);
}

void	sqlrcursor::suspendCaching() {

	if (!cachedest) {
		return ;
	}

	// close the cache file and clean up
	clearCacheDest();
}

int	sqlrcursor::noError() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Checking For An Error... ");
		sqlrc->debugPreEnd();
	}

	// get a flag indicating whether there's been an error or not
	unsigned short	success;
	if (getShort(&success)!=sizeof(unsigned short)) {
		return -1;
	}
	if (success==NO_ERROR) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("none.\n");
			sqlrc->debugPreEnd();
		}
		cacheNoError();
		return 1;
	} else if (success==ERROR) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("error!!!\n");
			sqlrc->debugPreEnd();
		}
		return 0;
	}
	return -1;
}

int	sqlrcursor::getCursorId() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Getting Cursor ID...\n");
		sqlrc->debugPreEnd();
	}
	if (sqlrc->read(&cursorid)!=sizeof(unsigned short)) {
		return 0;
	}
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Cursor ID: ");
		sqlrc->debugPrint((long)cursorid);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}
	return 1;
}

int	sqlrcursor::getSuspended() {

	// see if the result set of that cursor is actually suspended
	unsigned short	suspendedresultset;
	if (sqlrc->read(&suspendedresultset)!=
					sizeof(unsigned short)) {
		return -1;
	}

	if (suspendedresultset==SUSPENDED_RESULT_SET) {

		// If it was suspended the server will send the index of the 
		// last row from the previous result set.
		// Initialize firstrowindex and rowcount from this index.
		sqlrc->read(&firstrowindex);
		rowcount=firstrowindex+1;
	
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Previous result set was ");
	       		sqlrc->debugPrint("suspended at row index: ");
			sqlrc->debugPrint((long)firstrowindex);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}
		return 1;
	} else if (suspendedresultset==NO_SUSPENDED_RESULT_SET) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Previous result set was ");
	       		sqlrc->debugPrint("not suspended.\n");
			sqlrc->debugPreEnd();
		}
		return 1;
	}
	return 0;
}

int	sqlrcursor::parseColumnInfo() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Parsing Column Info\n");
		sqlrc->debugPreEnd();
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Actual row count: ");
		sqlrc->debugPreEnd();
	}

	// first get whether the server knows the total number of rows or not
	if (getShort(&knowsactualrows)!=sizeof(unsigned short)) {
		return -1;
	}

	// get the number of rows returned by the query
	if (knowsactualrows==ACTUAL_ROWS) {
		if (getLong(&actualrows)!=sizeof(unsigned long)) {
			return -1;
		}
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint((long)actualrows);
			sqlrc->debugPreEnd();
		}
	} else {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("unknown");
			sqlrc->debugPreEnd();
		}
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("\n");
		sqlrc->debugPrint("Affected row count: ");
		sqlrc->debugPreEnd();
	}

	// get whether the server knows the number of affected rows or not
	if (getShort(&knowsaffectedrows)!=sizeof(unsigned short)) {
		return -1;
	}

	// get the number of rows affected by the query
	if (knowsaffectedrows==AFFECTED_ROWS) {
		if (getLong(&affectedrows)!=sizeof(unsigned long)) {
			return -1;
		}
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint((long)affectedrows);
			sqlrc->debugPreEnd();
		}
	} else {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("unknown");
			sqlrc->debugPreEnd();
		}
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	// get whether the server is sending column info or not
	if (getShort(&sentcolumninfo)!=sizeof(unsigned short)) {
		return -1;
	}

	// get column count
	if (getLong(&colcount)!=sizeof(unsigned long)) {
		return -1;
	}
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Column count: ");
		sqlrc->debugPrint((long)colcount);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	// we have to do this here even if we're not getting the column
	// descriptions because we are going to use the longdatatype member
	// variable no matter what
	createColumnBuffers();

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO) {

		// get whether column types will be predefined id's or strings
		if (getShort(&columntypeformat)!=sizeof(unsigned short)) {
			return -1;
		}

		// some useful variables
		unsigned short	length;
		column		*currentcol;

		// get the columninfo segment
		for (unsigned long i=0; i<colcount; i++) {
	
			// get the column name length
			if (getShort(&length)!=sizeof(unsigned short)) {
				return -1;
			}
	
			// which column to use
			currentcol=getColumn(i);
	
			// get the column name
			currentcol->name=(char *)colstorage->malloc(length+1);
			if (getString(currentcol->name,length)!=length) {
				return -1;
			}
			currentcol->name[length]=(char)NULL;

			// upper/lowercase column name if necessary
			if (colcase==UPPER_CASE) {
				string::upper(currentcol->name);
			} else if (colcase==LOWER_CASE) {
				string::lower(currentcol->name);
			}

			if (columntypeformat==COLUMN_TYPE_IDS) {

				// get the column type
				if (getShort(&currentcol->type)!=
						sizeof(unsigned short)) {
					return -1;
				}

			} else {

				// get the column type length
				if (getShort(&currentcol->typestringlength)!=
						sizeof(unsigned short)) {
					return -1;
				}

				// get the column data
				currentcol->typestring=new
					char[currentcol->typestringlength+1];
				currentcol->typestring[
					currentcol->typestringlength]=
								(char)NULL;
				if (getString(currentcol->typestring,
						currentcol->typestringlength)!=
						currentcol->typestringlength) {
					return -1;
				}
			}

			// get the column length
			if (getLong(&currentcol->length)!=
						sizeof(unsigned long)) {
				return -1;
			}

			// get the column precision
			if (getLong(&currentcol->precision)!=
						sizeof(unsigned long)) {
				return -1;
			}

			// get the column scale
			if (getLong(&currentcol->scale)!=
						sizeof(unsigned long)) {
				return -1;
			}

			// initialize the longest value
			currentcol->longest=0;
	
			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("\"");
				sqlrc->debugPrint(currentcol->name);
				sqlrc->debugPrint("\",");
				sqlrc->debugPrint("\"");
				if (columntypeformat!=COLUMN_TYPE_IDS) {
					sqlrc->debugPrint(
						currentcol->typestring);
				} else {
					sqlrc->debugPrint(datatypestring[
							currentcol->type]);
				}
				sqlrc->debugPrint("\", ");
				sqlrc->debugPrint((long)currentcol->length);
				sqlrc->debugPrint(" (");
				sqlrc->debugPrint((long)currentcol->precision);
				sqlrc->debugPrint(",");
				sqlrc->debugPrint((long)currentcol->scale);
				sqlrc->debugPrint(")\n");
				sqlrc->debugPreEnd();
			}

		}
	}

	// cache the column definitions
	cacheColumnInfo();

	return 1;
}

void	sqlrcursor::createColumnBuffers() {

	// we could get really sophisticated here and keep stats on the number
	// of columns that previous queries returned and adjust the size of
	// "columns" periodically, but for now, we'll just use a static size

	// create the standard set of columns, this will hang around until
	// the cursor is deleted
	if (!columns) {
		columns=new column[OPTIMISTIC_COLUMN_COUNT];
	}

	// if there are more columns than our static column buffer
	// can handle, create extra columns, these will be deleted after each
	// query
	if (colcount>OPTIMISTIC_COLUMN_COUNT && colcount>previouscolcount) {
		delete[] extracolumns;
		extracolumns=new column[colcount-OPTIMISTIC_COLUMN_COUNT];
	}
}

column	*sqlrcursor::getColumn(int index) {
	if (index<OPTIMISTIC_COLUMN_COUNT) {
		return &columns[index];
	}
	return &extracolumns[index-OPTIMISTIC_COLUMN_COUNT];
}

void	sqlrcursor::abortResultSet() {

	// If the end of the previous result set was never reached, abort it.
	// If we're caching data to a local file, get the rest of the data; we
	// won't have to abort the result set in that case, the server will
	// do it.
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Aborting Result Set For Cursor: ");
		sqlrc->debugPrint((long)cursorid);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	if (sqlrc->connected || cached) {
		if (cachedest && cachedestind) {
			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("Getting the rest of the result set, since this is a cached result set.\n");
				sqlrc->debugPreEnd();
			}
			while (!endofresultset) {
				clearRows();

				// if we're not fetching from a cached result 
				// set tell the server to send one 
				if (!cachesource && !cachesourceind) {
					sqlrc->write((unsigned short)
							FETCH_RESULT_SET);
					sqlrc->write(cursorid);
				}

				// parseData should call finishCaching when
				// it hits the end of the result set, but
				// if it or skipAndFetch return a -1 (network
				// error) we'll have to call it ourselves.
				if (skipAndFetch(-1)==-1 || parseData()==-1) {
					finishCaching();
					return;
				}
			}
		} else {
			sqlrc->write((unsigned short)ABORT_RESULT_SET);
			sqlrc->write(cursorid);
		}
	}
}

void	sqlrcursor::suspendResultSet() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Suspending Result Set\n");
		sqlrc->debugPreEnd();
	}
	if (sqlrc->connected && !cached) {
		sqlrc->write((unsigned short)SUSPEND_RESULT_SET);
		sqlrc->write(cursorid);
	}
	suspendCaching();
	suspendresultsetsent=1;
}

int	sqlrcursor::processResultSet(int rowtoget) {

	// start caching the result set
	if (cacheon) {
		startCaching();
	}

	// parse the columninfo and data
	int	success=1;

	// skip and fetch here if we're not reading from a cached result set
	// this way, everything gets done in 1 round trip
	if (!cachesource) {
		success=skipAndFetch(firstrowindex+rowtoget);
	}

	// get data back from the server
	if (success>0 && (success=noError())>0 &&
			((cachesource && cachesourceind) ||
			((!cachesource && !cachesourceind)  && 
				(success=getCursorId()) && 
				(success=getSuspended())>0)) &&
			(success=parseColumnInfo())>0 && 
			(success=parseOutputBinds())>0) {

		// skip and fetch here if we're reading from a cached result set
		if (cachesource) {
			success=skipAndFetch(firstrowindex+rowtoget);
		}

		// parse the data
		if (success>-1) {
			success=parseData();
		}
	}

	// handle error responses
	if (success==0) {
		getErrorFromServer();
		return 0;
	} else if (success==-1) {
		clearResultSet();
		setError("Failed to execute the query and/or process the result set.\n A query, bind variable or bind value could be too large, there could be too \n many bind variables, or a network error may have ocurred.");
		sqlrc->endSession();
		return 0;
	}
	return 1;
}

void	sqlrcursor::startCaching() {

	if (!resumed) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Caching data to ");
			sqlrc->debugPrint(cachedestname);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}
	} else {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Resuming caching data to ");
			sqlrc->debugPrint(cachedestname);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}
	}

	// create the cache file, truncate it unless we're 
	// resuming a previous session
	cachedest=new file();
	cachedestind=new file();
	if (!resumed) {
		cachedest->open(cachedestname,O_RDWR|O_TRUNC|O_CREAT,
					permissions::ownerReadWrite());
		cachedestind->open(cachedestindname,O_RDWR|O_TRUNC|O_CREAT,
					permissions::ownerReadWrite());
	} else {
		cachedest->open(cachedestname,O_RDWR|O_CREAT|O_APPEND);
		cachedestind->open(cachedestindname,O_RDWR|O_CREAT|O_APPEND);
	}

	if (cachedest && cachedestind) {

		if (!resumed) {

			// write "magic" identifier to head of files
			cachedest->write("SQLRELAYCACHE",13);
			cachedestind->write("SQLRELAYCACHE",13);
			
			// write ttl to files
			long	expiration=time(NULL)+cachettl;
			cachedest->write(expiration);
			cachedestind->write(expiration);
		}

	} else {

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Error caching data to ");
			sqlrc->debugPrint(cachedestname);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		// in case of an error, clean up
		clearCacheDest();
	}
}

void	sqlrcursor::finishCaching() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Finishing caching.\n");
		sqlrc->debugPreEnd();
	}

	// terminate the result set
	cachedest->write((unsigned short)END_RESULT_SET);

	// close the cache file and clean up
	clearCacheDest();
}

void	sqlrcursor::sendInputBinds() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Sending ");
		sqlrc->debugPrint((long)inbindcount);
		sqlrc->debugPrint(" Input Bind Variables:\n");
		sqlrc->debugPreEnd();
	}

	// write the input bind variables/values to the server.
	sqlrc->write((unsigned short)inbindcount);
	unsigned long	size;
	int	count=inbindcount;
	for (int i=0; i<count; i++) {

		// don't send anything if the send flag is turned off
		if (!inbindvars[i].send) {
			count++;
			continue;
		}

		// send the variable
		size=(unsigned long)strlen(inbindvars[i].variable);
		sqlrc->write((unsigned short)size);
		sqlrc->write(inbindvars[i].variable,(size_t)size);
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint(inbindvars[i].variable);
			sqlrc->debugPrint("(");
			sqlrc->debugPrint((long)size);
		}

		// send the type
		sqlrc->write((unsigned short)inbindvars[i].type);
		if (inbindvars[i].type==NULL_BIND) {
			if (sqlrc->debug) {
				sqlrc->debugPrint(":NULL)\n");
				sqlrc->debugPreEnd();
			}
		} else if (inbindvars[i].type==STRING_BIND) {
			// send the value
			sqlrc->write(
				(unsigned long)inbindvars[i].valuesize);
			if (inbindvars[i].valuesize>0) {
				sqlrc->write(inbindvars[i].
					value.stringval,
					(size_t)inbindvars[i].valuesize);
			}

			if (sqlrc->debug) {
				sqlrc->debugPrint(":STRING)=");
				sqlrc->debugPrint(inbindvars[i].
							value.stringval);
				sqlrc->debugPrint("(");
				sqlrc->debugPrint((long)inbindvars[i].
							valuesize);
				sqlrc->debugPrint(")");
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}
		} else if (inbindvars[i].type==LONG_BIND) {
			// send the value
			char	negative=inbindvars[i].value.longval<0?1:0;
			sqlrc->write(negative);
			sqlrc->write((unsigned long)
					(inbindvars[i].value.longval*
					 		((negative)?-1:1)));

			if (sqlrc->debug) {
				sqlrc->debugPrint(":LONG)=");
				sqlrc->debugPrint((long)inbindvars[i].
							value.longval);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}
		} else if (inbindvars[i].type==DOUBLE_BIND) {
			// send the value
			sqlrc->write((double)inbindvars[i].value.
							doubleval.value);
			sqlrc->write((unsigned short)inbindvars[i].value.
							doubleval.precision);
			sqlrc->write((unsigned short)inbindvars[i].value.
							doubleval.scale);

			if (sqlrc->debug) {
				sqlrc->debugPrint(":DOUBLE)=");
				sqlrc->debugPrint(inbindvars[i].value.
							doubleval.value);
				sqlrc->debugPrint(":");
				sqlrc->debugPrint((long)inbindvars[i].value.
							doubleval.precision);
				sqlrc->debugPrint(",");
				sqlrc->debugPrint((long)inbindvars[i].value.
							doubleval.scale);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		} else if (inbindvars[i].type==BLOB_BIND ||
				inbindvars[i].type==CLOB_BIND) {

			// send the value
			sqlrc->write(
				(unsigned long)inbindvars[i].valuesize);
			if (inbindvars[i].valuesize>0) {
				sqlrc->write(inbindvars[i].
					value.lobval,
					(size_t)inbindvars[i].valuesize);
			}

			if (sqlrc->debug) {
				if (inbindvars[i].type==BLOB_BIND) {
					sqlrc->debugPrint(
						":BLOB)=");
					sqlrc->debugPrintBlob(
						inbindvars[i].value.lobval,
						inbindvars[i].valuesize);
				} else if (inbindvars[i].type==CLOB_BIND) {
					sqlrc->debugPrint(
						":CLOB)=");
					sqlrc->debugPrintClob(
						inbindvars[i].value.lobval,
						inbindvars[i].valuesize);
				}
				sqlrc->debugPrint("(");
				sqlrc->debugPrint((long)inbindvars[i].
							valuesize);
				sqlrc->debugPrint(")");
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}
		}
	}
}

void	sqlrcursor::sendOutputBinds() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Sending Output Bind Variables:\n");
		sqlrc->debugPreEnd();
	}

	// write the output bind variables to the server.
	sqlrc->write((unsigned short)outbindcount);
	unsigned short	size;
	int	count=outbindcount;
	for (int i=0; i<count; i++) {

		// don't send anything if the send flag is turned off
		if (!outbindvars[i].send) {
			count++;
			continue;
		}

		// send the variable, type and size that the buffer needs to be
		size=(unsigned short)strlen(outbindvars[i].variable);
		sqlrc->write((unsigned short)size);
		sqlrc->write(outbindvars[i].variable,(size_t)size);
		sqlrc->write((unsigned short)outbindvars[i].type);
		if (outbindvars[i].type!=CURSOR_BIND) {
			sqlrc->write((unsigned long)outbindvars[i].valuesize);
		}

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint(outbindvars[i].variable);
			if (outbindvars[i].type!=CURSOR_BIND) {
				sqlrc->debugPrint("(");
				sqlrc->debugPrint((long)outbindvars[i].
								valuesize);
				sqlrc->debugPrint(")");
			}
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}
	}
}

void	sqlrcursor::sendGetColumnInfo() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Send Column Info: ");
	}

	if (sendcolumninfo==SEND_COLUMN_INFO) {
		if (sqlrc->debug) {
			sqlrc->debugPrint("yes\n");
		}
		sqlrc->write((unsigned short)SEND_COLUMN_INFO);
	} else {
		if (sqlrc->debug) {
			sqlrc->debugPrint("no\n");
		}
		sqlrc->write((unsigned short)DONT_SEND_COLUMN_INFO);
	}

	if (sqlrc->debug) {
		sqlrc->debugPreEnd();
	}
}

int	sqlrcursor::getShort(unsigned short *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(integer);
	} else {
		return sqlrc->read(integer);
	}
}

int	sqlrcursor::getLong(unsigned long *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(integer);
	} else {
		return sqlrc->read(integer);
	}
}

int	sqlrcursor::getString(char *string, int size) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(string,size);
	} else {
		return sqlrc->read(string,size);
	}
}

void	sqlrcursor::clearColumns() {

	// delete the column type strings (if necessary)
	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
				columntypeformat!=COLUMN_TYPE_IDS) {
		for (unsigned long i=0; i<colcount; i++) {
			delete[] getColumn(i)->typestring;
		}
	}

	// reset the column storage pool
	colstorage->free();

	// reset the column count
	previouscolcount=colcount;
	colcount=0;

	// delete array pointing to each column name
	delete[] columnnamearray;
	columnnamearray=NULL;
}

void	sqlrcursor::dontGetColumnInfo() {
	sendcolumninfo=DONT_SEND_COLUMN_INFO;
}

void	sqlrcursor::getColumnInfo() {
	sendcolumninfo=SEND_COLUMN_INFO;
}

void	sqlrcursor::mixedCaseColumnNames() {
	colcase=MIXED_CASE;
}

void	sqlrcursor::upperCaseColumnNames() {
	colcase=UPPER_CASE;
}

void	sqlrcursor::lowerCaseColumnNames() {
	colcase=LOWER_CASE;
}

void	sqlrcursor::setResultSetBufferSize(int rows) {
	rsbuffersize=rows;
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Result Set Buffer Size: ");
		sqlrc->debugPrint((long)rows);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}
}

int	sqlrcursor::getResultSetBufferSize() {
	return	rsbuffersize;
}


void	sqlrcursor::cacheToFile(const char *filename) {

	cacheon=1;
	cachettl=600;
	if (copyrefs) {
		delete[] cachedestname;
		cachedestname=strdup(filename);
	} else {
		cachedestname=(char *)filename;
	}

	// create the index name
	delete[] cachedestindname;
	cachedestindname=new char[strlen(filename)+5];
	sprintf(cachedestindname,"%s.ind",filename);
}

void	sqlrcursor::setCacheTtl(int ttl) {
	cachettl=ttl;
}

void	sqlrcursor::cacheOff() {
	cacheon=0;
}

int	sqlrcursor::getResultSetId() {
	return cursorid;
}

int	sqlrcursor::resumeResultSet(int id) {
	return resumeCachedResultSet(id,NULL);
}

int	sqlrcursor::resumeCachedResultSet(int id, const char *filename) {

	if (!endofresultset && !suspendresultsetsent) {
		abortResultSet();
	}
	clearResultSet();

	if (!sqlrc->connected) {
		return 0;
	}

	cached=0;
	resumed=1;
	endofresultset=0;

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Resuming Result Set of Cursor: ");
		sqlrc->debugPrint((long)id);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	// tell the server we want to resume the result set
	sqlrc->write((unsigned short)RESUME_RESULT_SET);

	// send the id of the cursor we want to 
	// resume the result set of to the server
	sqlrc->write((unsigned short)id);

	// process the result set
	if (filename && filename[0]) {
		cacheToFile(filename);
	}
	if (rsbuffersize) {
		if (processResultSet(firstrowindex+rsbuffersize-1)) {
			return 1;
		}
	} else {
		if (processResultSet(-1)) {
			return 1;
		}
	}
	return 0;
}

int	sqlrcursor::sendQuery(const char *query) {
	prepareQuery(query);
	return executeQuery();
}

int	sqlrcursor::sendQuery(const char *query, int length) {
	prepareQuery(query,length);
	return executeQuery();
}

int	sqlrcursor::sendFileQuery(const char *path, const char *filename) {
	return prepareFileQuery(path,filename) && executeQuery();
}

void	sqlrcursor::initQueryBuffer() {
	if (!querybuffer) {
		querybuffer=new char[MAXQUERYSIZE+1];
		queryptr=querybuffer;
	}
}

void	sqlrcursor::prepareQuery(const char *query) {
	prepareQuery(query,strlen(query));
}

void	sqlrcursor::prepareQuery(const char *query, int length) {
	reexecute=0;
	validatebinds=0;
	resumed=0;
	clearVariables();
	if (copyrefs) {
		initQueryBuffer();
		strcpy(queryptr,query);
	} else {
		queryptr=(char *)query;
	}
	querylen=length;
	if (querylen>MAXQUERYSIZE) {
		querylen=MAXQUERYSIZE;
	}
}

int	sqlrcursor::prepareFileQuery(const char *path, const char *filename) {

	// init some variables
	reexecute=0;
	validatebinds=0;
	resumed=0;
	clearVariables();

	// init the fullpath buffer
	if (!fullpath) {
		fullpath=new char[MAXPATHLEN+1];
	}

	// add the path to the fullpath
	int	index=0;
	int	counter=0;
	while (path[index] && counter<MAXPATHLEN) {
		fullpath[counter]=path[index];
		index++;
		counter++;
	}

	// add the "/" to the fullpath
	if (counter<=MAXPATHLEN) {
		fullpath[counter]='/';
		counter++;
	}

	// add the file to the fullpath
	index=0;
	while (filename[index] && counter<MAXPATHLEN) {
		fullpath[counter]=filename[index];
		index++;
		counter++;
	}

	// handle a filename that's too long
	if (counter>MAXPATHLEN) {

		// sabotage the file name so it can't be opened
		fullpath[0]=(char)NULL;

		// debug info
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("File name ");
			sqlrc->debugPrint((char *)path);
			sqlrc->debugPrint("/");
			sqlrc->debugPrint((char *)filename);
			sqlrc->debugPrint(" is too long.");
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

	} else {

		// terminate the string
		fullpath[counter]=(char)NULL;

		// debug info
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("File: ");
			sqlrc->debugPrint(fullpath);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}
	}

	// open the file
	file	queryfile;
	if (!queryfile.open(fullpath,O_RDONLY)) {

		// set the error
		char	*err=new char[32+strlen(fullpath)];
		strcpy(err,"The file ");
		strcat(err,fullpath);
		strcat(err," could not be opened.\n");
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint(err);
			sqlrc->debugPreEnd();
		}
		setError(err);
		delete[] err;

		// set queryptr to NULL so executeQuery won't try to do
		// anything with it in the event that it gets called
		queryptr=NULL;

		return 0;
	}

	initQueryBuffer();

	// read the file into the query buffer
	querylen=queryfile.getSize();
	if (querylen<MAXQUERYSIZE) {
		queryfile.read((unsigned char *)querybuffer,querylen);
		querybuffer[querylen]=(char)NULL;
	} else {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("The query in ");
		sqlrc->debugPrint(fullpath);
		sqlrc->debugPrint(" is too large.\n");
		sqlrc->debugPreEnd();
	}

	queryfile.close();

	return 1;
}

void	sqlrcursor::substitution(const char *variable, const char *value) {
	if (subcount<MAXVAR && variable && variable[0]) {
		stringVar(&subvars[subcount],variable,value);
		subcount++;
	}
}

void	sqlrcursor::substitution(const char *variable, long value) {
	if (subcount<MAXVAR && variable && variable[0]) {
		longVar(&subvars[subcount],variable,value);
		subcount++;
	}
}

void	sqlrcursor::substitution(const char *variable, double value, 
			unsigned short precision, unsigned short scale) {
	if (subcount<MAXVAR && variable && variable[0]) {
		doubleVar(&subvars[subcount],variable,value,precision,scale);
		subcount++;
	}
}

void	sqlrcursor::clearBinds() {
	inbindcount=0;
	outbindcount=0;
}

void	sqlrcursor::inputBindBlob(const char *variable, const char *value,
						unsigned long size) {
	if (inbindcount<MAXVAR && variable && variable[0]) {
		lobVar(&inbindvars[inbindcount],variable,value,size,BLOB_BIND);
		inbindvars[inbindcount].send=1;
		inbindcount++;
	}
}

void	sqlrcursor::inputBindClob(const char *variable, const char *value,
						unsigned long size) {
	if (inbindcount<MAXVAR && variable && variable[0]) {
		lobVar(&inbindvars[inbindcount],variable,value,size,CLOB_BIND);
		inbindvars[inbindcount].send=1;
		inbindcount++;
	}
}

void	sqlrcursor::inputBind(const char *variable, const char *value) {
	if (inbindcount<MAXVAR && variable && variable[0]) {
		stringVar(&inbindvars[inbindcount],variable,value);
		inbindvars[inbindcount].send=1;
		inbindcount++;
	}
}

void	sqlrcursor::inputBind(const char *variable, long value) {
	if (inbindcount<MAXVAR && variable && variable[0]) {
		longVar(&inbindvars[inbindcount],variable,value);
		inbindvars[inbindcount].send=1;
		inbindcount++;
	}
}

void	sqlrcursor::inputBind(const char *variable, double value, 
				unsigned short precision, 
				unsigned short scale) {
	if (inbindcount<MAXVAR && variable && variable[0]) {
		doubleVar(&inbindvars[inbindcount],variable,value,
						precision, scale);
		inbindvars[inbindcount].send=1;
		inbindcount++;
	}
}

void	sqlrcursor::substitutions(const char **variables, const char **values) {
	int	index=0;
	while (variables[index] && subcount<MAXVAR) {
		if (variables[index] && variables[index][0]) {
			stringVar(&subvars[subcount],
					variables[index],values[index]);
			subcount++;
		}
		index++;
	}
}

void	sqlrcursor::substitutions(const char **variables, const long *values) {
	int	index=0;
	while (variables[index] && subcount<MAXVAR) {
		if (variables[index] && variables[index][0]) {
			longVar(&subvars[subcount],
					variables[index],values[index]);
			subcount++;
		}
		index++;
	}
}

void	sqlrcursor::substitutions(const char **variables, const double *values, 
					const unsigned short *precisions,
					const unsigned short *scales) {
	int	index=0;
	while (variables[index] && subcount<MAXVAR) {
		if (variables[index] && variables[index][0]) {
			doubleVar(&subvars[subcount],
					variables[index],
					values[index],
					precisions[index],
					scales[index]);
			subcount++;
		}
		index++;
	}
}

void	sqlrcursor::inputBinds(const char **variables, const char **values) {
	int	index=0;
	while (variables[index] && inbindcount<MAXVAR) {
		if (variables[index] && variables[index][0]) {
			stringVar(&inbindvars[inbindcount],
					variables[index],values[index]);
			inbindvars[inbindcount].send=1;
			inbindcount++;
		}
		index++;
	}
}

void	sqlrcursor::inputBinds(const char **variables,
				const unsigned long *values) {
	int	index=0;
	while (variables[index] && inbindcount<MAXVAR) {
		if (variables[index] && variables[index][0]) {
			longVar(&inbindvars[inbindcount],
					variables[index],values[index]);
			inbindvars[inbindcount].send=1;
			inbindcount++;
		}
		index++;
	}
}

void	sqlrcursor::inputBinds(const char **variables, const double *values, 
					const unsigned short *precisions,
					const unsigned short *scales) {
	int	index=0;
	while (variables[index] && inbindcount<MAXVAR) {
		if (variables[index] && variables[index][0]) {
			doubleVar(&inbindvars[inbindcount],
					variables[index],
					values[index],
					precisions[index],
					scales[index]);
			inbindvars[inbindcount].send=1;
			inbindcount++;
		}
		index++;
	}
}

void	sqlrcursor::stringVar(bindvar *var, const char *variable,
						const char *value) {

	initVar(var,variable);

	// store the value, handle NULL values too
	if (value) {
		if (copyrefs) {
			var->value.stringval=strdup(value);
		} else {
			var->value.stringval=(char *)value;
		}
		var->valuesize=strlen(value);
		var->type=STRING_BIND;
	} else {
		var->type=NULL_BIND;
	}
}

void	sqlrcursor::longVar(bindvar *var, const char *variable, long value) {
	initVar(var,variable);
	var->type=LONG_BIND;
	var->value.longval=value;
}

void	sqlrcursor::doubleVar(bindvar *var, const char *variable, double value,
					unsigned short precision, 
					unsigned short scale) {
	initVar(var,variable);
	var->type=DOUBLE_BIND;
	var->value.doubleval.value=value;
	var->value.doubleval.precision=precision;
	var->value.doubleval.scale=scale;
}

void	sqlrcursor::lobVar(bindvar *var, const char *variable,
			const char *value, unsigned long size, bindtype type) {

	initVar(var,variable);

	// Store the value, handle NULL values too.
	// For LOB's empty strings are handled as NULL's as well, this is
	// probably not right, but I can't get empty string lob binds to work.
	if (value && size>0) {
		if (copyrefs) {
			var->value.lobval=new char[size];
			memcpy(var->value.lobval,value,size);
		} else {
			var->value.lobval=(char *)value;
		}
		var->valuesize=size;
		var->type=type;
	} else {
		var->type=NULL_BIND;
	}
}

void	sqlrcursor::initVar(bindvar *var, const char *variable) {

	// clear any old variable name that was stored and assign the new 
	// variable name also clear any old value that was stored in this 
	// variable
	if (copyrefs) {
		delete[] var->variable;
		var->variable=strdup(variable);

		if (var->type==STRING_BIND &&
				var->value.stringval) {
			delete[] var->value.stringval;
		} else if ((var->type==BLOB_BIND ||
				var->type==CLOB_BIND) &&
				var->value.lobval) {
			delete[] var->value.lobval;
		}
	} else {
		var->variable=(char *)variable;
	}
}

void	sqlrcursor::defineOutputBind(const char *variable,
					unsigned long length) {
	defineOutputBindGeneric(variable,STRING_BIND,length);
}

void	sqlrcursor::defineOutputBindBlob(const char *variable) {
	defineOutputBindGeneric(variable,BLOB_BIND,0);
}

void	sqlrcursor::defineOutputBindClob(const char *variable) {
	defineOutputBindGeneric(variable,CLOB_BIND,0);
}

void	sqlrcursor::defineOutputBindCursor(const char *variable) {
	defineOutputBindGeneric(variable,CURSOR_BIND,0);
}

void	sqlrcursor::defineOutputBindGeneric(const char *variable,
				bindtype type, unsigned long valuesize) {

	if (outbindcount<MAXVAR && variable && variable[0]) {

		// clean up old values
		if (outbindvars[outbindcount].type==STRING_BIND) {
			delete[] outbindvars[outbindcount].value.stringval;
		} else if (outbindvars[outbindcount].type==BLOB_BIND ||
			outbindvars[outbindcount].type==CLOB_BIND) {
			delete[] outbindvars[outbindcount].value.lobval;
		}
		if (copyrefs) {
			// clean up old variable and set new variable
			delete[] outbindvars[outbindcount].variable;
			outbindvars[outbindcount].variable=strdup(variable);

		} else {
			outbindvars[outbindcount].variable=(char *)variable;
		}
		outbindvars[outbindcount].type=type;
		outbindvars[outbindcount].value.stringval=NULL;
		outbindvars[outbindcount].value.lobval=NULL;
		outbindvars[outbindcount].valuesize=valuesize;
		outbindvars[outbindcount].send=1;
		outbindcount++;
	}
}

char	*sqlrcursor::getOutputBind(const char *variable) {

	if (variable) {
		for (int i=0; i<outbindcount; i++) {
			if (!strcmp(outbindvars[i].variable,variable)) {
				if (outbindvars[i].type==STRING_BIND) {
					return outbindvars[i].value.stringval;
				} else {
					return outbindvars[i].value.lobval;
				}
			}
		}
	}
	return NULL;
}

long	sqlrcursor::getOutputBindLength(const char *variable) {

	if (variable) {
		for (int i=0; i<outbindcount; i++) {
			if (!strcmp(outbindvars[i].variable,variable)) {
				return outbindvars[i].valuesize;
			}
		}
	}
	return -1;
}

sqlrcursor	*sqlrcursor::getOutputBindCursor(const char *variable) {

	short	bindcursorid=getOutputBindCursorId(variable);
	if (bindcursorid==-1) {
		return NULL;
	}

	sqlrcursor	*bindcursor=new sqlrcursor(sqlrc);
	bindcursor->attachToBindCursor(bindcursorid);
	return bindcursor;
}

short	sqlrcursor::getOutputBindCursorId(const char *variable) {

	if (variable) {
		for (int i=0; i<outbindcount; i++) {
			if (!strcmp(outbindvars[i].variable,variable)) {
				return outbindvars[i].value.cursorid;
			}
		}
	}
	return -1;
}

void	sqlrcursor::attachToBindCursor(short bindcursorid) {
	prepareQuery("");
	reexecute=1;
	cursorid=bindcursorid;
}

void	sqlrcursor::validateBinds() {
	validatebinds=1;
}

int	sqlrcursor::executeQuery() {

	if (!queryptr) {
		setError("No query to execute.");
		return 0;
	}

	// a useful variable
	int	retval=0;

	if (!subcount) {

		// validate the bind variables
		if (validatebinds) {
			validateBindsInternal(queryptr);
		}
		
		// run the query
		retval=runQuery(queryptr);

	} else {

		// perform substitutions
		stringbuffer	container;
		char		*ptr=queryptr;
		int		found=0;
		int		inquotes=0;
		int		inbraces=0;
		int		len=0;
		stringbuffer	*braces;

		// iterate through the string
		while (*ptr) {
		
			// figure out whether we're inside a quoted 
			// string or not
			if (*ptr=='\'' && *(ptr-1)!='\\') {
				if (inquotes) {
					inquotes=0;
				} else {
					inquotes=1;
				}
			}
		
			// if we find an open-brace then start 
			// sending to a new buffer
			if (*ptr=='[' && !inbraces && !inquotes) {
				braces=new stringbuffer();
				inbraces=1;
				ptr++;
			}
		
			// if we find a close-brace then process 
			// the brace buffer
			if (*ptr==']' && inbraces && !inquotes) {
		
				// look for an = sign, skipping whitespace
				char	*bptr=braces->getString();
				while (*bptr && (*bptr==' ' || 
					*bptr=='	' || *bptr=='\n')) {
					bptr++;
				}
		
				if (*bptr=='=') {
					// if we find an equals sign first 
					// then process the rest of the buffer
					bptr++;
		
					// skip whitespace
					while (*bptr && (*bptr==' ' || 
						*bptr=='	' || 
					 	*bptr=='\n')) {
						bptr++;
					}
		
					// if the remaining contents of the 
					// buffer are '' or nothing then we 
					// must have an ='' or just an = with 
					// some whitespace, replace this
					// with "is NULL" otherwise, just write
					// out the contents of the buffer
					if (!bptr || 
						(bptr && !strcmp(bptr,"''"))) {
						container.append(" is NULL ");
					} else {
						container.append(
							braces->getString());
					}
				} else {
					// if we don't find an equals sign, 
					// then write the contents out directly
					container.append(braces->getString());
				}
				delete braces;
				inbraces=0;
				ptr++;
			}
		
			// if we encounter $(....) then replace the 
			// variable within
			if ((*ptr)=='$' && (*(ptr+1))=='(') {
		
				// first iterate through the arrays passed in
				found=0;
				for (int i=0; i<subcount && !found; i++) {
		
					// if we find a match, write the 
					// value to the container and skip 
					// past the $(variable)
					len=strlen(subvars[i].variable);
					if (!strncmp((ptr+2),
						subvars[i].variable,len) &&
						(*(ptr+2+len))==')') {
		
						if (inbraces) {
							performSubstitution(
								braces,i);
						} else {
							performSubstitution(
								&container,i);
						}
						ptr=ptr+3+len;
						found=1;
					}
				}
		
				// if the variable wasn't found, then 
				// just write the $(
				if (!found) {
					if (inbraces) {
						braces->append("$(");
					} else {
						container.append("$(");
					}
					ptr=ptr+2;
				}
		
			} else {
		
				// print out the current character and proceed
				if (inbraces) {
					braces->append(*ptr);
				} else {
					container.append(*ptr);
				}
				ptr++;
			}
		}

		// validate the bind variables
		if (validatebinds) {
			validateBindsInternal(container.getString());
		}

		// run the query
		querylen=strlen(container.getString());
		retval=runQuery(container.getString());
	}

	// set up to re-execute the same query if executeQuery is called
	// again before calling prepareQuery
	reexecute=1;

	return retval;
}

int	sqlrcursor::fetchFromBindCursor() {

	if (!endofresultset || !sqlrc->connected) {
		return 0;
	}

	// FIXME: should these be here?
	clearVariables();
	clearResultSet();

	cached=0;
	endofresultset=0;

	// tell the server we're fetching from a bind cursor
	sqlrc->write((unsigned short)FETCH_FROM_BIND_CURSOR);

	// send the cursor id to the server
	sqlrc->write((unsigned short)cursorid);

	// 0 input binds and 0 output binds
	sqlrc->write((unsigned short)0);
	sqlrc->write((unsigned short)0);

	sendGetColumnInfo();

	if (processResultSet(rsbuffersize-1)) {
		return 1;
	}
	return 0;
}

void	sqlrcursor::performSubstitution(stringbuffer *buffer, int which) {

	if (subvars[which].type==STRING_BIND) {
		buffer->append(subvars[which].value.stringval);
	} else if (subvars[which].type==LONG_BIND) {
		buffer->append(subvars[which].value.longval);
	} else if (subvars[which].type==DOUBLE_BIND) {
		buffer->append(subvars[which].value.doubleval.value,
				subvars[which].value.doubleval.precision,
				subvars[which].value.doubleval.scale);
	}
}

int	sqlrcursor::runQuery(const char *query) {

	// send the query
	if (sendQueryInternal(query)) {

		sendInputBinds();
		sendOutputBinds();
		sendGetColumnInfo();

		if (processResultSet(rsbuffersize-1)) {
			return 1;
		}
	}
	return 0;
}

char	*sqlrcursor::getCacheFileName() {
	return cachedestname;
}

int	sqlrcursor::openCachedResultSet(const char *filename) {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Opening cached result set: ");
		sqlrc->debugPrint(filename);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	if (!endofresultset) {
		abortResultSet();
	}
	clearResultSet();

	cached=1;
	endofresultset=0;

	// create the index file name
	char	*indexfilename=new char[strlen(filename)+5];
	sprintf(indexfilename,"%s.ind",filename);

	// open the file
	cachesource=new file();
	cachesourceind=new file();
	if ((cachesource->open(filename,O_RDWR|O_EXCL)) &&
		(cachesourceind->open(indexfilename,O_RDWR|O_EXCL))) {

		// don't need this anymore
		delete[] indexfilename;

		// initialize firstrowindex and rowcount
		firstrowindex=0;
		rowcount=firstrowindex;

		// make sure it's a cache file and skip the ttl
		char		magicid[13];
		unsigned long	longvar;
		if (getString(magicid,13)==13 &&
			!strncmp(magicid,"SQLRELAYCACHE",13) &&
			getLong(&longvar)==sizeof(unsigned long)) {

			// process the result set
			return processResultSet(firstrowindex+rsbuffersize-1);
		} else {

			// if the test above failed, the file is either not
			// a cache file or is corrupt
			stringbuffer	errstr;
			errstr.append("File ");
			errstr.append(filename);
			errstr.append(" is either corrupt");
			errstr.append(" or not a cache file.");
			setError(errstr.getString());
		}

	} else {

		// if we couldn't open the file, set the error message
		stringbuffer	errstr;
		errstr.append("Couldn't open ");
		errstr.append(filename);
		errstr.append(" and ");
		errstr.append(indexfilename);
		setError(errstr.getString());
	}

	// don't need this anymore
	delete[] indexfilename;

	// if we fell through to here, then an error has ocurred
	clearCacheSource();
	return 0;
}

int	sqlrcursor::sendQueryInternal(const char *query) {

	// if the first 8 characters of the query are "-- debug" followed
	// by a return, then set debugging on
	if (!strncmp(query,"-- debug\n",9)) {
		sqlrc->debugOn();
	}

	if (!endofresultset) {
		abortResultSet();
	}
	clearResultSet();

	if (!sqlrc->connected && !sqlrc->openSession()) {
		return 0;
	}

	cached=0;
	endofresultset=0;

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Sending Query:");
		sqlrc->debugPrint("\n");
		sqlrc->debugPrint(query);
		sqlrc->debugPrint("\n");
		sqlrc->debugPrint("Length: ");
		sqlrc->debugPrint((long)querylen);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	// send the query to the server.
	if (!reexecute) {

		// tell the server we're sending a query
		sqlrc->write((unsigned short)NEW_QUERY);

		// send the query
		sqlrc->write((unsigned long)querylen);
		sqlrc->write(query,querylen);

	} else {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Requesting re-execution of ");
			sqlrc->debugPrint("previous query.");
			sqlrc->debugPrint("\n");
			sqlrc->debugPrint("Requesting Cursor: ");
			sqlrc->debugPrint((long)cursorid);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		// tell the server we're sending a query
		sqlrc->write((unsigned short)REEXECUTE_QUERY);

		// send the cursor id to the server
		sqlrc->write((unsigned short)cursorid);
	}

	return 1;
}

void	sqlrcursor::validateBindsInternal(const char *query) {

	// some useful variables
	char	*ptr;
	char	*start;
	char	*after;
	int	found;
	int	len;
	int	count;

	// check each input bind
	count=inbindcount;
	for (int i=0; i<count; i++) {

		// don't check bind-by-position variables
		len=strlen(inbindvars[i].variable);
		if (isNumber(inbindvars[i].variable,len)) {
			continue;
		}

		found=0;
		start=((char *)query)+1;

		// there may be more than 1 match for the variable name as in
		// "select * from table where table_name=:table_name", both
		// table_name's would match, but only the second is a bind
		// variable
		while ((ptr=strstr(start,inbindvars[i].variable))) {

			// for a match to be a bind variable, it must be 
			// preceded by a colon and can't be followed by an
			// alphabet character, number or underscore
			after=ptr+len;
			if ((*(ptr-1)==':' || *(ptr-1)=='@') && *after!='_' &&
				!(*(after)>='a' && *(after)<='z') &&
				!(*(after)>='A' && *(after)<='Z') &&
				!(*(after)>='0' && *(after)<='9')) {
				found=1;
				break;
			} else {
				// jump past this instance to look for the
				// next one
				start=ptr+len;
			}
		}

		if (!found) {
			inbindvars[i].send=0;
			inbindcount--;
		}
	}

	// check each output bind
	count=outbindcount;
	for (int i=0; i<count; i++) {

		// don't check bind-by-position variables
		len=strlen(outbindvars[i].variable);
		if (isNumber(outbindvars[i].variable,len)) {
			continue;
		}

		found=0;
		start=((char *)query)+1;

		// there may be more than 1 match for the variable name as in
		// "select * from table where table_name=:table_name", both
		// table_name's would match, but only 1 is correct
		while ((ptr=strstr(start,outbindvars[i].variable))) {

			// for a match to be a bind variable, it must be 
			// preceded by a colon and can't be followed by an
			// alphabet character, number or underscore
			after=ptr+len;
			if (*(ptr-1)==':' && *after!='_' &&
				!(*(after)>='a' && *(after)<='z') &&
				!(*(after)>='A' && *(after)<='Z') &&
				!(*(after)>='0' && *(after)<='9')) {
				found=1;
				break;
			} else {
				// jump past this instance to look for the
				// next one
				start=ptr+len;
			}
		}

		if (!found) {
			outbindvars[i].send=0;
			outbindcount--;
		}
	}
}

void	sqlrcursor::clearResultSet() {

	clearCacheDest();
	clearCacheSource();
	clearError();

	// columns is cleared after rows because colcount is used in 
	// clearRows() and set to 0 in clearColumns()
	clearRows();
	clearColumns();

	// clear row counters, since fetchRowIntoBuffer() and clearResultSet()
	// are the only methods that call clearRows() and fetchRowIntoBuffer()
	// needs these values not to be cleared, we'll clear them here...
	firstrowindex=0;
	previousrowcount=rowcount;
	rowcount=0;
	actualrows=0;
	affectedrows=0;
	endofresultset=1;
	suspendresultsetsent=0;
	getrowcount=0;
}

char	*sqlrcursor::getFieldInternal(int row, int col) {
	if (row<OPTIMISTIC_ROW_COUNT) {
		return rows[row]->getField(col);
	}
	return extrarows[row-OPTIMISTIC_ROW_COUNT]->getField(col);
}

unsigned long	sqlrcursor::getFieldLengthInternal(int row, int col) {
	if (row<OPTIMISTIC_ROW_COUNT) {
		return rows[row]->getFieldLength(col);
	}
	return extrarows[row-OPTIMISTIC_ROW_COUNT]->getFieldLength(col);
}

void	sqlrcursor::createRowBuffers() {

	// rows will hang around from now until the cursor is deleted,
	// getting reused with each query
	rows=new row *[OPTIMISTIC_ROW_COUNT];
	for (int i=0; i<OPTIMISTIC_ROW_COUNT; i++) {
		rows[i]=new row(colcount);
	}
}
