// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrresultsettranslation.h>
#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>

sqlrresultsettranslation::sqlrresultsettranslation(
				sqlrresultsettranslations *sqlrrsts,
				xmldomnode *parameters) {
	this->sqlrrsts=sqlrrsts;
	this->parameters=parameters;
}

sqlrresultsettranslation::~sqlrresultsettranslation() {
}

bool sqlrresultsettranslation::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					uint16_t fieldindex,
					const char *field,
					uint32_t fieldlength,
					const char **newfield,
					uint32_t *newfieldlength) {
	return true;
}
