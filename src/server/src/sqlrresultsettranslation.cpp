// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrresultsettranslation.h>
#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>

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
					uint32_t fieldindex,
					const char *field,
					uint64_t fieldlength,
					const char **newfield,
					uint64_t newfieldlength) {
	return true;
}
