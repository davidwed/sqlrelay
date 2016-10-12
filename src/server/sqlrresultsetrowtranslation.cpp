// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrresultsetrowtranslation::sqlrresultsetrowtranslation(
				sqlrresultsetrowtranslations *sqlrrrsts,
				xmldomnode *parameters,
				bool debug) {
	this->sqlrrrsts=sqlrrrsts;
	this->parameters=parameters;
	this->debug=debug;
}

sqlrresultsetrowtranslation::~sqlrresultsetrowtranslation() {
}

bool sqlrresultsetrowtranslation::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char * const *fields,
					uint32_t *fieldlengths,
					const char ***newfield,
					uint32_t **newfieldlength) {
	return true;
}
