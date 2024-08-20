// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrresultsetrowtranslationprivate {
	friend class sqlrresultsetrowtranslation;
	private:
};

sqlrresultsetrowtranslation::sqlrresultsetrowtranslation(
				sqlrservercontroller *cont,
				domnode *parameters) :
				sqlrservermodule(cont,parameters) {
	pvt=new sqlrresultsetrowtranslationprivate;
}

sqlrresultsetrowtranslation::~sqlrresultsetrowtranslation() {
	delete pvt;
}

bool sqlrresultsetrowtranslation::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char ***field,
					uint64_t **fieldsize) {
	return true;
}

const char *sqlrresultsetrowtranslation::getError() {
	return NULL;
}
