// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

short sqlrconnection::nonNullBindValue() {
	return 0;
}

short sqlrconnection::nullBindValue() {
	return -1;
}

char sqlrconnection::bindVariablePrefix() {
	return ':';
}

bool sqlrconnection::bindValueIsNull(short isnull) {
	return (isnull==nullBindValue());
}
