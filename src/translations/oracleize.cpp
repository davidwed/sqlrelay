// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqltranslation.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class oracleize : public sqltranslation {
	public:
			oracleize(sqltranslations *sqlts,
					xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
};

oracleize::oracleize(sqltranslations *sqlts, xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool oracleize::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {

	return true;
}

extern "C" {
	sqltranslation	*new_oracleize(sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new oracleize(sqlts,parameters);
	}
}
