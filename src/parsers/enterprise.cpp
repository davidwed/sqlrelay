// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlreparser.h>

extern "C" {
	sqlrparser	*new_enterprise() {
		return new sqlreparser();
	}
}
