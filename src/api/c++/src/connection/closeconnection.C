// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>

void sqlrconnection::closeConnection() {
	close();
	connected=false;
}
