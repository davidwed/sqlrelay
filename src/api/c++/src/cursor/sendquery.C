// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

int sqlrcursor::sendQuery(const char *query) {
	prepareQuery(query);
	return executeQuery();
}

int sqlrcursor::sendQuery(const char *query, int length) {
	prepareQuery(query,length);
	return executeQuery();
}

int sqlrcursor::sendFileQuery(const char *path, const char *filename) {
	return prepareFileQuery(path,filename) && executeQuery();
}
