// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int	sqlrcursor::openCursor(int id) {
	// by default do nothing
	return 1;
}

int	sqlrcursor::closeCursor() {
	// by default do nothing
	return 1;
}
