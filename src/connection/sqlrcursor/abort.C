// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrcursor::abort() {
	suspendresultset=false;
	busy=false;
}

void sqlrcursor::cleanUpData(bool freerows, bool freecols, bool freebinds) {
	// by default, do nothing...
	return;
}
