// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLREXPORTXML_H
#define SQLREXPORTXML_H

#include <sqlrelay/private/sqlrexportxmlincludes.h>

/** The sqlrexportxml class implements sqlrexport for XML files. */
class SQLRCLIENT_DLLSPEC sqlrexportxml : virtual public sqlrexportfile {
	public:
		/** Creates an instance of the sqlrexportxml class. */
		sqlrexportxml();

		/** Destroys this instance of the sqlrexportxml class. */
		virtual	~sqlrexportxml();

		/** Exports the result set of the cursor currently in use as
		 *  set by the most recent call to setSqlrCursor() to the file
		 *  set by the most recent call to setFileName(), or to
		 *  standard output if setFileName() was never called, or if
		 *  setFileName(NULL) was called.
		 *
		 *  If setTable() was called with a non-null value then the
		 *  table name is included as the "name" attribute of the table
		 *  tag.  If setTable() was never called, or setTable(NULL) was
		 *  called, then the name attribute is omitted.
		 *
		 *  The XML format is as follows:
		 *  (This example is a table of US states, exported from
		 *  PostgreSQL)
		 *
		 *  <?xml version="1.0"?>
		 *  <table name="states">
		 *  <columns count="6">
		 *          <column name="state_id" type="int4"/>
		 *          <column name="state_name" type="varchar"/>
		 *          <column name="state_abbreviation" type="varchar"/>
		 *          <column name="latitude" type="numeric"/>
		 *          <column name="longitude" type="numeric"/>
		 *          <column name="creation_date" type="date"/>
		 *  </columns>
		 *  <rows>
		 *          <row>
		 *          <field>1</field>
		 *          <field>Louisiana</field>
		 *          <field>LA</field>
		 *          <field>31.259800</field>
		 *          <field>-92.658700</field>
		 *          <field>2005-08-16</field>
		 *          </row>
		 *          <row>
		 *          <field>2</field>
		 *          <field>Texas</field>
		 *          <field>TX</field>
		 *          <field>31.653400</field>
		 *          <field>-100.546900</field>
		 *          <field>2005-08-17</field>
		 *          </row>
		 *          <row>
		 *          <field>3</field>
		 *          <field>Georgia</field>
		 *          <field>GA</field>
		 *          <field>32.750300</field>
		 *          <field>-83.737800</field>
		 *          <field>2005-08-16</field>
		 *          </row>
		 *  </rows>
		 *  </table>
		 *
		 *  Returns true on success and false if an error occurred. */
		virtual	bool	exportData();

	#include <sqlrelay/private/sqlrexportxml.h>
};

#endif
