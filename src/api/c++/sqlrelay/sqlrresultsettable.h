// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#ifndef SQLRRESULTSETTABLE_H
#define SQLRRESULTSETTABLE_H

#include <sqlrelay/private/sqlrresultsettableincludes.h>

/** Read-only, block-based, sequential-access. */
class SQLRCLIENT_DLLSPEC sqlrresultsettable :
				public tablecollection<const char *> {
	public:

		sqlrresultsettable();
		sqlrresultsettable(sqlrcursor *cursor);
		~sqlrresultsettable();

		void	setCursor(sqlrcursor *cursor);

		bool		getIsReadOnly();
		bool		getIsBlockBased();
		bool		getIsSequentialAccess();

		const char	*getColumnName(uint64_t col);
		uint64_t	getColCount();

		const char	*getValue(uint64_t row, uint64_t col);
		const char	*getValue(uint64_t row, const char *colname);

		uint64_t	getRowCount();
		uint64_t	getRowBlockSize();
		bool		getAllRowsAvailable();
	
		#include <sqlrelay/private/sqlrresultsettable.h>
};

#endif
