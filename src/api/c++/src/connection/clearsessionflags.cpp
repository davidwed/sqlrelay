// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

void sqlrconnection::clearSessionFlags() {

	// indicate that the session hasn't been suspended or ended
	endsessionsent=false;
	suspendsessionsent=false;
}
