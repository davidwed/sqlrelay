// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

bool sqlrcursor::sendQuery(const char *query) {
	prepareQuery(query);
	return executeQuery();
}

bool sqlrcursor::sendQuery(const char *query, int length) {
	prepareQuery(query,length);
	return executeQuery();
}

bool sqlrcursor::sendFileQuery(const char *path, const char *filename) {
	return prepareFileQuery(path,filename) && executeQuery();
}
