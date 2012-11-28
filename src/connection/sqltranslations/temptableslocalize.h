// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef TEMPTABLESLOCALIZE_H
#define TEMPTABLESLOCALIZE_H

#include <sqltranslation.h>

class temptableslocalize : public sqltranslation {
	public:
			temptableslocalize(sqltranslations *sqlts,
					rudiments::xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldom *querytree);
	private:
		void		mapCreateTemporaryTableName(
						sqlrconnection_svr *sqlrcon,
						rudiments::xmldomnode *query,
						const char *uniqueid);
		void		mapCreateIndexOnTemporaryTableName(
						rudiments::xmldomnode *query,
						const char *uniqueid);
		const char	*generateTempTableName(const char *oldtable,
							const char *uniqueid);
		bool		replaceTempNames(rudiments::xmldomnode *node);
};

#endif
