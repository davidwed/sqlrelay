// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

const char *sqlrconnection::clientVersion() {
	return SQLR_VERSION;
}
