// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

void sqlrcursor::getNullsAsEmptyStrings() {
	returnnulls=false;
}

void sqlrcursor::getNullsAsNulls() {
	returnnulls=true;
}
