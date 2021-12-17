// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLRIMPORT_H
#define SQLRIMPORT_H

#include <sqlrelay/private/sqlrimportincludes.h>

class SQLRCLIENT_DLLSPEC sqlrimport {
	public:
		/** Creates an instance of the sqlrimport class. */
		sqlrimport();

		/** Destroys this instance of the sqlrimport class. */
		virtual	~sqlrimport();

		/** Sets the instance of sqlrconnection that this instance
		 *  will use to connect to the database. */
		void	setSqlrConnection(sqlrconnection *sqlrcon);

		/** Sets the instance of sqlrcursor that this instance
		 *  will use to run queries. */
		void	setSqlrCursor(sqlrcursor *sqlrcur);

		/** Sets the database type, which impacts how things like
		 *  escaping, sequences, and auto-increment fields are handled.
		 *  Should be one of "postgresql", "mysql", "firebird",
		 *  "oracle", "db2", or "informix".  Or may be left empty or
		 *  NULL for generic handling.  Defaults to NULL. */
		void	setDbType(const char *dbtype);

		/** By default, the name of the table or sequence to import
		 *  data into is derived from the import file (eg. from the
		 *  CSV file name, or from an XML tag inside of the file).
		 *  This method may be used to explicitly override that name,
		 *  or provide one if none can be derived. */
		void	setObjectName(const char *objectname);

		/** If "ignorecolumns" is set false, then column information
		 *  will be read from the import file (eg, from the CSV header,
		 *  or from XML tags inside of the file) and used to define the
		 *  column-order of the import data, which may be different
		 *  from the column-order of the table, and may exclude
		 *  nullable columns.
		 *
		 *  If "ignorecolumns" is set true, then any column information
		 *  included in the import file will be ignored.  Import data
		 *  will be assumed to be in the same column-order as the
		 *  column-order of the table.  This is useful, for example,
		 *  when a CSV header contains different column names than the
		 *  table.
		 *
		 *  Defaults to false. */
		void	setIgnoreColumns(bool ignorecolumns);

		/** Maps column name "from" to "to".  If "to" is NULL then
		 *  the column is unmapped. */
		void	mapColumnName(const char *from, const char *to);

		/** Leaves column names as-is. */
		void	mixedCaseColumnNames();

		/** Lower-cases colum names. */
		void	lowerCaseColumnNames();

		/** Upper-cases colum names. */
		void	upperCaseColumnNames();

		/** Call commit after every "commitcount" inserts.  If set to 0
		 *  then no commits will be called and the commit behavior will
		 *  depend on the behavior of the instance of sqlrelay that we
		 *  are connecting to.  Defaults to 0. */
		void	setCommitCount(uint64_t commitcount);

		/** Sets the logger instance to use.  If set to NULL then no
		 *  logging will be done.  Defaults to NULL. */
		void	setLogger(logger *lg);

		/** Sets the coarse log level.  General log messages will be
		 *  logged at this level.  If the log level of "lg" (set by
		 *  setLogger() above) is set equal to or greater than
		 *  "coarseloglevel" then general log messages will be logged.
		 *  Defaults to 0. */
		void	setCoarseLogLevel(uint8_t coarseloglevel);

		/** Sets the fine log level.  Detailed log messages will be
		 *  logged at this level.  If the log level of "lg" (set by
		 *  setLogger() above) is set equal to or greater than
		 *  "coarseloglevel" then general log messages will be logged.
		 *  Defaults to 9. */
		void	setFineLogLevel(uint8_t fineloglevel);

		/** Sets the log indent level to "logindent".  Defaults to 0. */
		void	setLogIndent(uint32_t logindent);

		/** If "logerrors" is set true then SQL errors will be logged
		 *  at the coarse log level.  If set false then SQL errors will
		 *  not be logged.  Defaults to false. */
		void	setLogErrors(bool logerrors);

		/** Imports data from "filename".  The table (or sequence)
		 *  to import the data into will be derived from the import
		 *  file (eg. from the CSV file name, or from an XML tag inside
		 *  of the file) or may be overridden using setObjectName()
		 *  above. */
		virtual	bool	importFromFile(const char *filename)=0;

	#include <sqlrelay/private/sqlrimport.h>
};

#endif
