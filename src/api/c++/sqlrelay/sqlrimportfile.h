// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLRIMPORTFILE_H
#define SQLRIMPORTFILE_H

#include <sqlrelay/private/sqlrimportfileincludes.h>

/** The sqlrimportfile class provides a base class for child classes that wish
 *  to implement import of data from a database, via SQL Relay, to a file.  It
 *  provides various common methods.  Each child class should implement the
 *  importData() method. */
class SQLRCLIENT_DLLSPEC sqlrimportfile : virtual public sqlrimport {
	public:
		/** Creates an instance of the sqlrimportfile class. */
		sqlrimportfile();

		/** Destroys this instance of the sqlrimportfile class. */
		virtual ~sqlrimportfile();

		/** Sets the name of the file from which data will be imported.
		 *  If "filename" is NULL the data will be imported from
		 *  standard input.
		 *
		 *  Not commonly called by implementations of importData() or
		 *  of the *Start/End() methods. */
		void	setFileName(const char *filename);

		/** Gets the name of the file from which data will be be
		 *  imported.
		 *
		 *  May be called by implementations of importData() or by
		 *  implementations of the *Start/End() methods. */
		const char	*getFileName();

		/** Imports data from the file set by the most recent call to
		 *  setFileName() or from standard input if setFileName() was
		 *  never called or setFileName(NULL) was called.
		 *
		 *  If setCommitCount() was called with a non-zero value then a
		 *  commit will be called after every "commitcount" rows is
		 *  inserted.  No commit will be called if setCommitCount() was
		 *  never called, or if setCommitCount(0) was called.
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  Note that the default implementation of this method just
		 *  returns true. Child classes that support export to a file
		 *  should override this method. */
		bool	importData();

	protected:
		/** Sets the file descriptor from which data is being imported.
 		 *
		 *  Should be called by implementations of importData().  Not
		 *  commonly called by implementations of the *Start/End()
		 *  methods. */
		void	setFileDescriptor(filedescriptor *fd);

		/** Gets the file descriptor from which data is being imported.
		 *
		 *  May be called by implementations of importData() or by
		 *  implementations of the *Start/End() methods. */
		filedescriptor	*getFileDescriptor();

	#include <sqlrelay/private/sqlrimportfile.h>
};

#endif
