// Copyright (c) 2014-2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

extern "C" {
	SQLRSERVER_DLLSPEC sqlrparser *new_sqlrparser_default(
					xmldomnode *parameters, bool debug) {
		return new sqlrparser(parameters,debug);
	}
}
