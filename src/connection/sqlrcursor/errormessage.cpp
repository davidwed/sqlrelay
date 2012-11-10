// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrcursor_svr::clearError() {
	setError(NULL,0,true);
}

void sqlrcursor_svr::setError(const char *err, int64_t errn, bool liveconn) {
	error=err;
	errnum=errn;
	liveconnection=liveconn;
}

void sqlrcursor_svr::errorMessage(const char **errorstring,
					int64_t *errorcode,
					bool *liveconnection) {
	return conn->errorMessage(errorstring,errorcode,liveconnection);
}
