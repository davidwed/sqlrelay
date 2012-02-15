// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

int64_t sqlrcursor::errorNumber() {
	// if we have a code then we should have a message too,
	// the codes could be any number, including 0, so check
	// the message to see which code to return
	if (error) {
		return errorno;
	} else if (sqlrc->error) {
		return sqlrc->errorno;
	}
	return 0;
}

const char *sqlrcursor::errorMessage() {
	if (error) {
		return error;
	} else if (sqlrc->error) {
		return sqlrc->error;
	}
	return NULL;
}
