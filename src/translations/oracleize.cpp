// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class oracleize : public sqlrtranslation {
	public:
			oracleize(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
};

oracleize::oracleize(sqlrtranslations *sqlts,
				xmldomnode *parameters,
				bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool oracleize::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {

	return true;
}

extern "C" {
	sqlrtranslation	*new_oracleize(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new oracleize(sqlts,parameters,debug);
	}
}
