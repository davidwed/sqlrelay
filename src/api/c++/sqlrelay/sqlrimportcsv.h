// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLRIMPORTCSV_H
#define SQLRIMPORTCSV_H

#include <sqlrelay/private/sqlrimportcsvincludes.h>

/** The sqlrimportcsv class implements sqlrimport for CSV files. */
class SQLRCLIENT_DLLSPEC sqlrimportcsv : virtual public sqlrimportfile,
							virtual public csvsax {
	public:

		/** Creates an instance of the sqlrimportcsv class. */
		sqlrimportcsv();

		/** Destroys this instance of the sqlrimportcsv class. */
		virtual ~sqlrimportcsv();

		/** Imports data from the file set by the most recent call to
		 *  setFileName().  The table (or sequence) to import the data
		 *  into will be derived from the import file name, and may be
		 *  overridden using setObjectName().
		 *
		 *  If setCommitCount() was called with a non-zero value then a
		 *  commit will be called after every "commitcount" rows is
		 *  inserted.  No commit will be called if setCommitCount() was
		 *  never called, or if setCommitCount(0) was called.
		 *
		 *  Returns true on success and false if an error occurred. */
		virtual bool	importData();

	#include <sqlrelay/private/sqlrimportcsv.h>
};

#endif
