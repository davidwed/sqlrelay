// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

extern "C" {
	sqlrparser	*new_default() {
		return new sqlrparser();
	}
}
