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

		bool		getIsReadOnly() const;
		bool		getIsBlockBased() const;
		bool		getIsSequentialAccess() const;

		const char	*getColumnName(uint64_t col) const;
		uint64_t	getColCount() const;

		const char	*getValue(uint64_t row,
						uint64_t col) const;
		const char	*getValue(uint64_t row,
						const char *colname) const;

		uint64_t	getRowCount() const;
		uint64_t	getBlockSize() const;
		bool		getAllRowsAvailable() const;
	
		#include <sqlrelay/private/sqlrresultsettable.h>
};

#endif
