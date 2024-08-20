// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrresultsettranslationprivate {
	friend class sqlrresultsettranslation;
	private:
};

sqlrresultsettranslation::sqlrresultsettranslation(
				sqlrservercontroller *cont,
				domnode *parameters) :
				sqlrservermodule(cont,parameters) {
	pvt=new sqlrresultsettranslationprivate;
}

sqlrresultsettranslation::~sqlrresultsettranslation() {
	delete pvt;
}

bool sqlrresultsettranslation::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint32_t fieldindex,
					const char **field,
					uint64_t *fieldsize) {
	return true;
}

const char *sqlrresultsettranslation::getError() {
	return NULL;
}
