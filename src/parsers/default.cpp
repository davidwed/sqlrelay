// Copyright (c) 2014-2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

extern "C" {
	SQLRSERVER_DLLSPEC sqlrparser *new_sqlrparser_default(
						sqlrservercontroller *cont,
						xmldomnode *parameters) {
		return new sqlrparser(cont,parameters);
	}
}
