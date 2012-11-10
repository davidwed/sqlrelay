// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef CUSTOM_NW_H
#define CUSTOM_NW_H

#include <sqlrlogger.h>
#include <rudiments/file.h>

class custom_nw : public sqlrlogger {
	public:
			custom_nw(rudiments::xmldomnode *parameters);

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
