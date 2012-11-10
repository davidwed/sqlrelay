// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrlogger.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

sqlrlogger::sqlrlogger(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqlrlogger::~sqlrlogger() {
}

bool sqlrlogger::init(sqlrconnection_svr *sqlrcon) {
	return true;
}

bool sqlrlogger::run(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur) {
	return true;
}
