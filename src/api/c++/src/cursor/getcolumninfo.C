// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

void	sqlrcursor::dontGetColumnInfo() {
	sendcolumninfo=DONT_SEND_COLUMN_INFO;
}

void	sqlrcursor::getColumnInfo() {
	sendcolumninfo=SEND_COLUMN_INFO;
}
