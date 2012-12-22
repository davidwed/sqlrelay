// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

void sqlrconnection::authenticate() {

	if (debug) {
		debugPreStart();
		debugPrint("Authenticating : ");
		debugPrint(user);
		debugPrint(":");
		debugPrint(password);
		debugPrint("\n");
		debugPreEnd();
	}

	cs->write((uint16_t)AUTHENTICATE);

	cs->write(userlen);
	cs->write(user,userlen);

	cs->write(passwordlen);
	cs->write(password,passwordlen);

	flushWriteBuffer();
}
