// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrcursor_svr::errorMessage(const char **errorstring,
					int64_t *errorcode,
					bool *liveconnection) {
	return conn->errorMessage(errorstring,errorcode,liveconnection);
}
