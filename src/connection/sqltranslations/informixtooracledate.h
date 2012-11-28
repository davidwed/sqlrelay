// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef INFORMIXTOORACLETODATE_H
#define INFORMIXTOORACLETODATE_H

#include <sqltranslation.h>

class informixtooracledate : public sqltranslation {
	public:
			informixtooracledate(sqltranslations *sqlts,
					rudiments::xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldom *querytree);
	private:
		bool	translateFunctions(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldomnode *node);
		bool	translateExtend(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldomnode *node);
		bool	translateCurrentDate(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldomnode *node);
		bool	translateDateTime(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldomnode *node);
		bool	translateInterval(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldomnode *node);
		void	compressIntervalQualifier(
				rudiments::xmldomnode *iqnode);
		void	translateIntervalQualifier(
				rudiments::stringbuffer *formatstring,
				rudiments::xmldomnode *iqnode,
				bool *containsfraction);

		rudiments::xmldomnode	*wrapBoth(
					rudiments::xmldomnode *functionnode,
					const char *formatstring,
					bool containsfraction);
		rudiments::xmldomnode	*wrapToChar(
					rudiments::xmldomnode *functionnode,
					const char *formatstring);
		rudiments::xmldomnode	*wrapToDate(
					rudiments::xmldomnode *functionnode,
					const char *formatstring,
					bool containsfraction);
		rudiments::xmldomnode	*wrap(
					rudiments::xmldomnode *functionnode,
					const char *function,
					const char *formatstring);
};

#endif
