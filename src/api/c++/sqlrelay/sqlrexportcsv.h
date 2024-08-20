// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLREXPORTCSV_H
#define SQLREXPORTCSV_H

#include <sqlrelay/private/sqlrexportcsvincludes.h>

/** The sqlrexportcsv class implements sqlrexport for CSV files. */
class SQLRCLIENT_DLLSPEC sqlrexportcsv : virtual public sqlrexportfile {
	public:
		/** Creates an instance of the sqlrexportcsv class. */
		sqlrexportcsv();

		/** Destroys this instance of the sqlrexportcsv class. */
		virtual	~sqlrexportcsv();

		/** Exports the result set of the cursor currently in use as
		 *  set by the most recent call to setSqlrCursor() to the file
		 *  set by the most recent call to setFileName(), or to
		 *  standard output if setFileName() was never called, or if
		 *  setFileName(NULL) was called.
		 *
		 *  The following result set:
		 *
		 *  col1,col2,col3,col4
		 *  =====================
		 *  0,0.0,field00,field01
		 *  1,1.1,field10,field11
		 *
		 *  would be exported as:
		 *
		 *  "col1","col2","col3","col4"
		 *  0,0.0,"field00","field01"
		 *  1,1.1,"field10","field11"
		 *
		 *  Returns true on success and false if an error occurred. */
		virtual	bool	exportData();

	#include <sqlrelay/private/sqlrexportcsv.h>
};

#endif
