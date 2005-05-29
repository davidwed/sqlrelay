// Copyright (c) 1999-2005  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::flushWriteBuffer() {
	clientsock->flushWriteBuffer(-1,-1);
}
