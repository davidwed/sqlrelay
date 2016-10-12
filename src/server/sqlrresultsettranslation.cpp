// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrresultsettranslation::sqlrresultsettranslation(
				sqlrresultsettranslations *sqlrrsts,
				xmldomnode *parameters,
				bool debug) {
	this->sqlrrsts=sqlrrsts;
	this->parameters=parameters;
	this->debug=debug;
}

sqlrresultsettranslation::~sqlrresultsettranslation() {
}

bool sqlrresultsettranslation::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint32_t fieldindex,
					const char **field,
					uint64_t *fieldlength) {
	return true;
}
