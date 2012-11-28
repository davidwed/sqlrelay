// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef DROPLOCALIZEDTEMPTABLES_H
#define DROPLOCALIZEDTEMPTABLES_H

#include <sqltrigger.h>
#include <sqltranslations.h>

class droplocalizedtemptables : public sqltrigger {
	public:
			droplocalizedtemptables(
					rudiments::xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldom *querytree,
					bool before,
					bool success);
	private:
		bool	dropTable(sqltranslations *sqlt,
					rudiments::xmldom *querytree);
		bool	dropIndex(sqltranslations *sqlt,
					rudiments::xmldom *querytree);
};

#endif
