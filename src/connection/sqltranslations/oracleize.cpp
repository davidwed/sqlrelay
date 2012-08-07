// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltranslations/oracleize.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <debugprint.h>

extern "C" {
	sqltranslation	*new_oracleize(sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new oracleize(sqlts,parameters);
	}
}

oracleize::oracleize(sqltranslations *sqlts, xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool oracleize::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {

	return true;
}
