// Copyright (c) 1999-2005  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

void sqlrconnection::flushWriteBuffer() {
	cs->flushWriteBuffer(-1,-1);
}
