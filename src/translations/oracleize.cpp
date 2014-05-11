// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlrtranslation.h>
#include <debugprint.h>

class oracleize : public sqlrtranslation {
	public:
			oracleize(sqlrtranslations *sqlts,
					xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
};

oracleize::oracleize(sqlrtranslations *sqlts, xmldomnode *parameters) :
					sqlrtranslation(sqlts,parameters) {
}

bool oracleize::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {

	return true;
}

extern "C" {
	sqlrtranslation	*new_oracleize(sqlrtranslations *sqlts,
					xmldomnode *parameters) {
		return new oracleize(sqlts,parameters);
	}
}
