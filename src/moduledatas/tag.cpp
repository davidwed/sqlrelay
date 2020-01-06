// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

extern "C" {
	SQLRSERVER_DLLSPEC
	sqlrmoduledata	*new_sqlrmoduledata_tag(domnode *parameters) {
		return new sqlrmoduledata_tag(parameters);
	}
}
