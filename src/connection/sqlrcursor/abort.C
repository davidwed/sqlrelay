// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void	sqlrcursor::abort() {
	suspendresultset=0;
	busy=0;
}

void	sqlrcursor::cleanUpData() {
	// by default, do nothing...
	return;
}
