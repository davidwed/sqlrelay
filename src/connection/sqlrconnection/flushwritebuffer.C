// Copyright (c) 1999-2005  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::flushWriteBuffer() {

	// toggle TCP_NODELAY to flush the write buffer
	clientsock->dontBufferWrites();
	clientsock->bufferWrites();
}
