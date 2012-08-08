// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef TEMPTABLESLOCALIZE_H
#define TEMPTABLESLOCALIZE_H

#include <sqltranslation.h>

using namespace rudiments;

class temptableslocalize : public sqltranslation {
	public:
			temptableslocalize(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		void		mapCreateTemporaryTableName(
						sqlrconnection_svr *sqlrcon,
						xmldomnode *query,
						const char *uniqueid);
		void		mapCreateIndexOnTemporaryTableName(
						xmldomnode *query,
						const char *uniqueid);
		const char	*generateTempTableName(const char *oldtable,
							const char *uniqueid);
		bool		replaceTempNames(xmldomnode *node);
		bool		verbatimTableReference(xmldomnode *node);
};

#endif
