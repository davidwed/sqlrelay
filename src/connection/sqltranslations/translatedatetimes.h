// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef TRANSLATEDATETIMES_H
#define TRANSLATEDATETIMES_H

#include <sqltranslation.h>

class translatedatetimes : public sqltranslation {
	public:
			translatedatetimes(sqltranslations *sqlts,
					rudiments::xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldom *querytree);
	private:
		char *convertDateTime(const char *format,
				int16_t year, int16_t month, int16_t day,
				int16_t hour, int16_t minute, int16_t second);
		bool translateDateTimesInQuery(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldomnode *querynode,
					rudiments::xmldomnode *parameters);
		bool translateDateTimesInBindVariables(
					sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldomnode *querynode,
					rudiments::xmldomnode *parameters);
		const char * const *getShortMonths();
		const char * const *getLongMonths();
};

#endif
