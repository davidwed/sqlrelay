// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrcursor_svr::openCursor(uint16_t id) {
	this->id=id;
	return true;
}

bool sqlrcursor_svr::closeCursor() {
	// by default do nothing
	return true;
}
