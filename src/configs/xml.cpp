// Copyright (c) 2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>

// FIXME: just a stub... implement this for real

extern "C" {
	SQLRUTIL_DLLSPEC sqlrconfig *new_sqlrconfig_xml() {
		return new sqlrconfig();
	}
}
