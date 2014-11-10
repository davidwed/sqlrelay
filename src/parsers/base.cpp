// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrparser.h>

extern "C" {
	sqlrparser	*new_base() {
		return new sqlrparser();
	}
}
