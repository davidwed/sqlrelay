// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

short sqlrconnection_svr::nonNullBindValue() {
	return 0;
}

short sqlrconnection_svr::nullBindValue() {
	return -1;
}

char sqlrconnection_svr::bindVariablePrefix() {
	return ':';
}

bool sqlrconnection_svr::bindValueIsNull(short isnull) {
	return (isnull==nullBindValue());
}
