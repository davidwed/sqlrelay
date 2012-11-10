// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef NEOWIZ_H
#define NEOWIZ_H

#include <sqlrlogger.h>
#include <rudiments/file.h>

class neowiz : public sqlrlogger {
	public:
			neowiz(rudiments::xmldomnode *parameters);

		bool	init(sqlrconnection_svr *sqlrcon);
		bool	run(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur);
	private:
		int	strescape(const char *str, char *buf, int limit);
		bool	descInputBinds(sqlrcursor_svr *cursor,
						char *buf, int limit);
		rudiments::file	querylog;
		char		*querylogname;
		char		querylogbuf[102400];
};

#endif
