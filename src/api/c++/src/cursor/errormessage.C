// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

const char *sqlrcursor::errorMessage() {
	if (error) {
		return error;
	} else if (sqlrc->error) {
		return sqlrc->error;
	}
	return NULL;
}
