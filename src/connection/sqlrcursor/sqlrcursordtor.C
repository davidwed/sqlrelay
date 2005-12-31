// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

sqlrcursor_svr::~sqlrcursor_svr() {
	delete sid_sqlrcur;
	delete[] querybuffer;
}
