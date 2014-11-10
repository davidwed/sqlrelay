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
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
};

oracleize::oracleize(sqlrtranslations *sqlts,
				xmldomnode *parameters,
				bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool oracleize::usesTree() {
	return true;
}

bool oracleize::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
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
