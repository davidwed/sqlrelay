// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLREXPORTFILE_H
#define SQLREXPORTFILE_H

#include <sqlrelay/private/sqlrexportfileincludes.h>

/** The sqlrexportfile class provides a base class for child classes that wish
 *  to implement export of data from a database, via SQL Relay, to a file.  It
 *  provides various common methods.  Each child class should implement the
 *  exportData() method. */
class SQLRCLIENT_DLLSPEC sqlrexportfile : virtual public sqlrexport {
	public:
		/** Creates an instance of the sqlrexportfile class. */
		sqlrexportfile();

		/** Destroys this instance of the sqlrexportfile class. */
		virtual	~sqlrexportfile();

		/** Sets the name of the file to which data will be exported.
		 *  If "filename" is NULL the data will be exported to standard
		 *  output.
		 *
		 *  Not commonly called by implementation of exportData() or
		 *  of the *Start/End() methods. */
		void	setFileName(const char *filename);

		/** Gets the name of the file to which data will be be exported.
		 *
		 *  May be called by implementations of exportData() or by
		 *  implementations of the *Start/End() methods. */
		const char	*getFileName();

		/** Exports the result set of the cursor currently in use as
		 *  set by the most recent call to setSqlrCursor() to the
		 *  file currently in use as set by the most recent call to
		 *  setFileName(), or to standard output if setFileName() was
		 *  never called or setFileName(NULL) was called.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  Note that the default implementation of this method just
		 *  returns true. Child classes that support export to a file
		 *  should override this method. */
		virtual	bool	exportData();

	protected:
		/** Sets the file descriptor to which data is being exported.
 		 *
		 *  Should be called by implementations of exportData().  Not
		 *  commonly called by implementations of the *Start/End()
		 *  methods. */
		void	setFileDescriptor(filedescriptor *fd);

		/** Gets the file descriptor to which data is being exported.
		 *
		 *  May be called by implementations of exportData() or by
		 *  implementations of the *Start/End() methods. */
		filedescriptor	*getFileDescriptor();

	#include <sqlrelay/private/sqlrexportfile.h>
};

#endif
