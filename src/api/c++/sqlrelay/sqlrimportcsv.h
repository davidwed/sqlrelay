// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLRIMPORTCSV_H
#define SQLRIMPORTCSV_H

#include <sqlrelay/private/sqlrimportcsvincludes.h>

class SQLRCLIENT_DLLSPEC sqlrimportcsv : public sqlrimport, public csvsax {
	public:

		/** Creates an instance of the sqlrimportcsv class. */
		sqlrimportcsv();

		/** Destroys this intance of the sqlrimportcsv class. */
		~sqlrimportcsv();

		/** Inserts a primary key at "primarykeycolumnindex".
		 *
		 *  If setIgnoreColumns(false) is set (the default) then
		 *  "primarykeycolumnname" must be supplied.  Otherwise it can
		 *  be set to NULL or an empty string.
		 *
		 *  If "primarykeycolumnsequence" is non-empty and non-null
		 *  then nextval('"primarykeycolumnsequence"') will be used to
		 *  generate the key.  Otherwise a NULL will be used in an
		 *  attempt to trigger an autoincrement/serial column to
		 *  generate a key. */
		void	insertPrimaryKey(const char *primarykeycolumnname,
						uint32_t primarykeycolumnindex,
						const char *primarykeysequence);

		/** Removes any primary key configuaration set by a prior call
		 *  to insertPrimaryKey(). */
		void	removePrimaryKey();

		/** Inserts static value "value" at "columnindex" for all rows.
		 *
		 *  If setIgnoreColumns(false) is set (the default) then
		 *  "columnname" must be supplied.  Otherwise it can be set to
		 *  NULL or an empty string. */
		void	insertStaticValue(const char *columnname,
						uint32_t columnindex,
						const char *value);

		/** Removes any static value configuaration at "columnindex"
		 *  set by a prior call to insertStaticValue(). */
		void	removeStaticValue(uint32_t columnindex);

		/** If "ignorecolumnswithemptynames" is set true, then columns
		 *  with empty column names will be completely ignored.  It
		 *  will be as if those columns are completely absent from the
		 *  CSV, which may be important to keep in mind when specifying
		 *  indexes for primary keys or static values.
		 *
		 *  Note that "ignorecolumsnwithemptyname" is observed even if
		 *  setIgnoreColumns(true) is set. */
		void	setIgnoreColumnsWithEmptyNames(
					bool ignorecolumnswithemptynames);

		/** Configures the instance to ignore empty rows. */
		void	setIgnoreEmptyRows(bool ignoreemptyrows);

		/** Imports data from "filename".  The table (or sequence)
		 *  to import the data into will be derived from the import
		 *  file name or may be overridden using setObjectName(). */
		bool	importFromFile(const char *filename);

	protected:
		/** Called when a column is encountered in the CSV header, even
		 *  if setColumnNames(true) is set.  May be overridden to do
		 *  application-specific work upon encountering a column.
		 *  "name" contains the column name.  "quoted" is set true if
		 *  the colum name is quoted in the file, and false otherwise.
		 *  Returns false if an error occurred when processing the
		 *  column and true otherwise.  Overridden methods may return
		 *  false to stop processing. */
		virtual	bool	column(const char *name, bool quoted);

		/** Called when the end of the CSV header is encountered, even
		 *  if setColumnNames(true) is set.  May be overridden to do
		 *  application-specific work upon encountering the end of the
		 *  header. Returns false if an error occurred when processing
		 *  the end of the header and true otherwise.  Overridden
		 *  methods may return false to stop processing. */
		virtual	bool	headerEnd();

		/** Called when the beginning of the CSV body is encountered.
		 *  May be overridden to do application-specific work upon
		 *  encountering the start of the body. Returns false if an
		 *  error occurred when processing the start of the body and
		 *  true otherwise.  Overridden methods may return false to
		 *  stop processing. */
		virtual	bool	bodyStart();

		/** Called when the beginning of a row is encountered.
		 *  May be overridden to do application-specific work upon
		 *  encountering the start of a row. Returns false if an
		 *  error occurred when processing the start of a row and
		 *  true otherwise.  Overridden methods may return false to
		 *  stop processing. */
		virtual	bool	rowStart();

		/** Called when a field is encountered.  May be overridden to
		 *  do application-specific work upon encountering a field.
		 *  "value" contains the column name.  "quoted" is set true if
		 *  the colum name is quoted in the file, and false otherwise.
		 *  Returns false if an error occurred when processing the
		 *  field and true otherwise.  Overridden methods may return
		 *  false to stop processing. */
		virtual	bool	field(const char *value, bool quoted);

		/** Called when the end of a row is encountered.  May be
		 *  overridden to do application-specific work upon
		 *  encountering the end of a row. Returns false if an
		 *  error occurred when processing the end of a row and
		 *  true otherwise.  Overridden methods may return false to
		 *  stop processing. */
		virtual	bool	rowEnd();

		/** Called when the end of the CSV body is encountered.  May
		 *  be overridden to do application-specific work upon
		 *  encountering the end of the CSV body. Returns false if an
		 *  error occurred when processing the end of the CSV body and
		 *  true otherwise.  Overridden methods may return false to
		 *  stop processing. */
		virtual	bool	bodyEnd();

	#include <sqlrelay/private/sqlrimportcsv.h>
};

#endif
