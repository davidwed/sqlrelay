// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

bool sqlrconnection::getNewPort() {

	// get the size of the unix port string
	unsigned short	size;
	if (cs->read(&size)!=sizeof(unsigned short)) {
		setError("Failed to get the size of the unix connection port.\n A network error may have ocurred.");
		return false;
	}
	
	if (size>MAXPATHLEN) {

		// if size is too big, return an error
		stringbuffer	errstr;
		errstr.append("The pathname of the unix port was too long: ");
		errstr.append(size);
		errstr.append(" bytes.  The maximum size is ");
		errstr.append((unsigned short)MAXPATHLEN);
		errstr.append(" bytes.");
		setError(errstr.getString());
		return false;
	}

	// get the unix port string
	if (size && cs->read(connectionunixportbuffer,size)!=size) {
		setError("Failed to get the unix connection port.\n A network error may have ocurred.");
		return false;
	}
	connectionunixportbuffer[size]=(char)NULL;
	connectionunixport=connectionunixportbuffer;

	// get the inet port
	if (cs->read(&connectioninetport)!=sizeof(unsigned short)) {
		setError("Failed to get the inet connection port.\n A network error may have ocurred.");
		return false;
	}

	// the server will send 0 for both the size of the unixport and 
	// the inet port if a server error occurred
	if (!size && !connectioninetport) {
		setError("An error occurred on the server.");
		return false;
	}
	return true;
}
