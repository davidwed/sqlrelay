// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrcursor::openCursor(int id) {
	this->id=id;
	return true;
}

bool sqlrcursor::closeCursor() {
	// by default do nothing
	return true;
}
