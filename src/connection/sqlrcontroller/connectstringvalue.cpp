// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

const char *sqlrcontroller_svr::connectStringValue(const char *variable) {
	return constr->getConnectStringValue(variable);
}
