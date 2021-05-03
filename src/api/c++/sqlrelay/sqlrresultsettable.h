// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#ifndef SQLRRESULTSETTABLE_H
#define SQLRRESULTSETTABLE_H

#include <sqlrelay/private/sqlrresultsettableincludes.h>

class SQLRCLIENT_DLLSPEC sqlrresultsettable :
				public tablecollection<const char *> {
	public:

		sqlrresultsettable();
		~sqlrresultsettable();

		void	attachCursor(sqlrcursor *cursor);

		uint64_t	getRowCount();
		uint64_t	getColCount();

		const char	*getColumnName(uint64_t col);
		const char	*getValue(uint64_t row, uint64_t col);
	
		#include <sqlrelay/private/sqlrresultsettable.h>
};

#endif
