// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrerrortranslationprivate {
	friend class sqlrerrortranslation;
	private:
};

sqlrerrortranslation::sqlrerrortranslation(
				sqlrservercontroller *cont,
				domnode *parameters) :
				sqlrservermodule(cont,parameters) {
	pvt=new sqlrerrortranslationprivate;
}

sqlrerrortranslation::~sqlrerrortranslation() {
	delete pvt;
}

bool sqlrerrortranslation::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				int64_t errornumber,
				const char *error,
				uint32_t errorsize,
				int64_t *translatederrornumber,
				stringbuffer *translatederror) {
	return true;
}

const char *sqlrerrortranslation::getError() {
	return NULL;
}
