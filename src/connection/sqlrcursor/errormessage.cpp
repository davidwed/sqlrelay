// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrcursor_svr::clearError() {
	setError(NULL,0,true);
}

void sqlrcursor_svr::setError(const char *err, int64_t errn, bool liveconn) {
	errorlength=charstring::length(err);
	if (errorlength>conn->maxerrorlength) {
		errorlength=conn->maxerrorlength;
	}
	charstring::copy(error,err,errorlength);
	errnum=errn;
	liveconnection=liveconn;
}

void sqlrcursor_svr::errorMessage(char *errorbuffer,
					uint32_t errorbuffersize,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	return conn->errorMessage(errorbuffer,errorbuffersize,
					errorlength,errorcode,liveconnection);
}
