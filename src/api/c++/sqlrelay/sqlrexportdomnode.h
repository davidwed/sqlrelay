// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLREXPORTDOMNODE_H
#define SQLREXPORTDOMNODE_H

#include <sqlrelay/private/sqlrexportdomnodeincludes.h>

/** The sqlrexportdomnode class provides a base class for child class that wish
 *  to implement export of data from a database, vis SQL Relay, to a domnode. It
 *  provides various common methods.  Each child class should implement the
 *  exportData() method. */
class SQLRCLIENT_DLLSPEC sqlrexportdomnode : virtual public sqlrexport {
	public:
		/** Creates an instance of the sqlrexportdomnode class. */
		sqlrexportdomnode();

		/** Destroys this instance of the sqlrexportdomnode
		 *  class. */
		virtual	~sqlrexportdomnode();

		/** Sets the top-level domnode that data will be exported to.
		 *
		 *  Should be called by implementations of exportData().  Not
		 *  commonly called by implementations of the *Start/End()
		 *  methods. */
		void	setDomNode(domnode *dn);

		/** Gets the top-level domnode.
		 *
		 *  May be called by implementations of exportData() or by
		 *  implementations of the *Start/End() methods. */
		domnode	*getDomNode();

		/** Set the domnode that columns will be written to.
		 *
		 *  Should be called by implementations of exportData().  Not
		 *  commonly called by implementations of the *Start/End()
		 *  methods. */
		void	setColumnsDomNode(domnode *dn);

		/** Gets the domnode that columns are being written to.
		 *
		 *  May be called by implementations of exportData() or by
		 *  implementations of the *Start/End() methods. */
		domnode	*getColumnsDomNode();

		/** Sets the domnode that the current column will be written to.
		 * 
		 *  Should be called by implementations of exportData().  Not
		 *  commonly called by implementations of the *Start/End()
		 *  methods. */
		void	setCurrentColumnDomNode(domnode *dn);

		/** Gets the domnode that the current column is being written
		 *  to.
		 *
		 *  May be called by implementations of exportData()or by
		 *  implementations of the *Start/End() methods. */
		domnode	*getCurrentColumnDomNode();

		/** Sets the domnode that rows are being written to.
		 * 
		 *  Should be called by implementations of exportData().  Not
		 *  commonly called by implementations of the *Start/End()
		 *  methods. */
		void	setRowsDomNode(domnode *dn);

		/** Gets the domnode that rows are being written to.
		 *
		 *  May be called by implementations of exportData() or by
		 *  implementations of the *Start/End() methods. */
		domnode	*getRowsDomNode();

		/** Sets the domnode that the current row will be written to.
		 * 
		 *  Should be called by implementations of exportData().  Not
		 *  commonly called by implementations of the *Start/End()
		 *  methods. */
		void	setCurrentRowDomNode(domnode *dn);

		/** Gets the domnode that the current row is being written to.
		 *
		 *  May be called by implementations of exportData()or by
		 *  implementations of the *Start/End() methods. */
		domnode	*getCurrentRowDomNode();

		/** Sets the domnode that the current field will be written to.
		 * 
		 *  Should be called by implementations of exportData().  Not
		 *  commonly called by implementations of the *Start/End()
		 *  methods. */
		void	setCurrentFieldDomNode(domnode *dn);

		/** Gets the domnode that the current field is being written to.
		 *
		 *  May be called by implementations of exportData() or by
		 *  implementations of the *Start/End() methods. */
		domnode	*getCurrentFieldDomNode();

		/** Exports the result set of the cursor currently in use as
		 *  set by the most recent call to setSqlrCursor() to the
		 *  domnode set by the most recent call to setDomNode().
		 *
		 *  Returns true on success and false if an error occurred.
		 *
		 *  Note that the default implementation of this method just
		 *  return true.  Child classes that support export to a domnode
		 *  should override this method. */
		virtual	bool	exportData();

	#include <sqlrelay/private/sqlrexportdomnode.h>
};

#endif
