// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

const char *sqlrconnection::connectStringValue(const char *variable) {
	return constr->getConnectStringValue(variable);
}
