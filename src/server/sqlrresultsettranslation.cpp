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
					uint16_t fieldindex,
					const char *field,
					uint32_t fieldlength,
					const char **newfield,
					uint32_t *newfieldlength) {
	return true;
}
