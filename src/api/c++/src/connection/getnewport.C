// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

int sqlrconnection::getNewPort() {

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
