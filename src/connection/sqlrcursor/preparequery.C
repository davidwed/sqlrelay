// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int	sqlrcursor::prepareQuery(const char *query, long querylength) {
	// by default, do nothing...
	return 1;
}
