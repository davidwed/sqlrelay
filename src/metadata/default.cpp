// Copyright (c) 2014-2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

extern "C" {
	SQLRSERVER_DLLSPEC sqlrmetadata *new_sqlrmetadata_default(
						sqlrservercontroller *cont,
						xmldomnode *parameters,
						bool debug) {
		return new sqlrmetadata(cont,parameters,debug);
	}
}
