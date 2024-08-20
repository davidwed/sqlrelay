// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

class sqlrauthprivate {
	friend class sqlrauth;
	public:
};

sqlrauth::sqlrauth(sqlrservercontroller *cont,
				domnode *parameters) :
				sqlrservermodule(cont,parameters) {
	pvt=new sqlrauthprivate;
}

sqlrauth::~sqlrauth() {
	delete pvt;
}

const char *sqlrauth::auth(sqlrcredentials *cred) {
	return NULL;
}
