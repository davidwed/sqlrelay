// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

uint16_t sqlrcursor::getResultSetId() {
	return cursorid;
}
