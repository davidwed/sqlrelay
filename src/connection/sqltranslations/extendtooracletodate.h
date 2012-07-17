// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef EXTENDTOORACLETODATE_H
#define EXTENDTOORACLETODATE_H

#include <sqltranslation.h>

using namespace rudiments;

class extendtooracletodate : public sqltranslation {
	public:
			extendtooracletodate(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		bool	translateFunctions(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *node);
		bool	translateExtend(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *node);
		bool	translateCurrentDate(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *node);
		bool	translateDateTime(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *node);
		bool	translateInterval(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *node);

		void	translateIntervalQualifier(
					stringbuffer *formatstring,
					xmldomnode *intervalqualifiernode);
		xmldomnode	*wrapBoth(xmldomnode *functionnode,
					const char *formatstring);
		xmldomnode	*wrapToChar(xmldomnode *functionnode,
					const char *formatstring);
		xmldomnode	*wrapToDate(xmldomnode *functionnode,
					const char *formatstring);
		xmldomnode	*wrap(xmldomnode *functionnode,
					const char *function,
					const char *formatstring);
};

#endif
