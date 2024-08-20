// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLREXPORTTABLE_H
#define SQLREXPORTTABLE_H

#include <sqlrelay/private/sqlrexporttableincludes.h>

/** The sqlrexporttable class implements sqlrexport for database tables. */
class SQLRCLIENT_DLLSPEC sqlrexporttable : virtual public sqlrexport {
	public:
		/** Creates an instance of the sqlrexporttable class. */
		sqlrexporttable();

		/** Destroys this instance of the sqlrexporttable class. */
		virtual ~sqlrexporttable();

		/** Sets the instance of sqlrconnection that this instance
		 *  will use to export data to the table. */
		void	setExportSqlrConnection(sqlrconnection *exportcon);

		/** Gets the instance of sqlrconnection that this instance
		 *  will use to export data to the table. */
		sqlrconnection	*getExportSqlrConnection();

		/** Sets the instance of sqlrcursor that this instance
		 *  will use to export data to the table. */
		void	setExportSqlrCursor(sqlrcursor *exportcur);

		/** Gets the instance of sqlrcursor that this instance
		 *  will use to export data to the table. */
		sqlrcursor	*getExportSqlrCursor();

		/** Sets the commit count. */
		void	setCommitCount(uint64_t commitcount);

		/** Gets the commit count. */
		uint64_t	getCommitCount();

		/** Exports the result set of the cursor currently in use as
		 *  set by the most recent call to setSqlrCursor() to the
		 *  database table set by the most recent call to setTable()
		 *  using the sqlrconnection and sqlrcuror set by the most
		 *  recent calls to setExportSqlrConnection() and
		 *  setExportSqlrCursor().  If setCommitCount() was called with
		 *  a non-zero value then a commit will be called after every
		 *  "commitcount" rows is inserted.  No commit will be called
		 *  if setCommitCount() was never called, or if
		 *  setCommitCount(0) was called.
		 *
		 *  Returns true on success and false if an error occurred. */
		virtual bool	exportData();

	protected:
		/** Gets the buffer that the insert query is written when
		 *  exporting data to a table.
		 *
		 *  Should be called by implementations of exportData().
		 *  May also be called by implementations of the *Start/End()
		 *  methods. */
		stringbuffer	*getInsertQueryBuffer();

	#include <sqlrelay/private/sqlrexporttable.h>
};

#endif
