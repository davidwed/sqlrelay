// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrbindvariabletranslationprivate {
	friend class sqlrbindvariabletranslation;
};

sqlrbindvariabletranslation::sqlrbindvariabletranslation(
				sqlrservercontroller *cont,
				domnode *parameters) :
				sqlrservermodule(cont,parameters) {
	pvt=new sqlrbindvariabletranslationprivate;
}

sqlrbindvariabletranslation::~sqlrbindvariabletranslation() {
	delete pvt;
}

bool sqlrbindvariabletranslation::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {
	return true;
}

const char *sqlrbindvariabletranslation::getError() {
	return NULL;
}
