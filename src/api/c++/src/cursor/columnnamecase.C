// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

void	sqlrcursor::mixedCaseColumnNames() {
	colcase=MIXED_CASE;
}

void	sqlrcursor::upperCaseColumnNames() {
	colcase=UPPER_CASE;
}

void	sqlrcursor::lowerCaseColumnNames() {
	colcase=LOWER_CASE;
}
