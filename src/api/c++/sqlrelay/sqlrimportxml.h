// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLRIMPORTXML_H
#define SQLRIMPORTXML_H

#include <sqlrelay/private/sqlrimportxmlincludes.h>

/** The sqlrimportxml class implements sqlrimport for XML files, specifically
 *  XML files exported by the sqlrexportxml class. */
class SQLRCLIENT_DLLSPEC sqlrimportxml : virtual public sqlrimportfile,
							virtual public xmlsax {
	public:
		/** Creates an instance of the sqlrimportxml class. */
		sqlrimportxml();

		/** Destroys this instance of the sqlrimportxml class. */
		virtual ~sqlrimportxml();

		/** Imports data from the file set by the most recent call to
		 *  setFileName().  The table or sequence to import the data
		 *  into will be deried from the name attribute of the table or
		 *  sequence tag inside the file and may be overridden using
		 *  setObjectName().
		 *
		 *  The XML format is as follows.
		 *
		 *  For a table:
		 *  (This example is a table of US states, exported from
		 *  PostgreSQL)
		 *
		 *  <?xml version="1.0"?>
		 *  <table name="states">
		 *  <columns>
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
		 *
		 *  For a sequence:
		 *  (This example is a sequence used to generate ids for the
		 *  table above, exported from PostgreSQL)
		 *
		 *  <?xml version="1.0"?>
		 *  <sequence name="state_ids" value="4"/>
		 *
		 *  If setCommitCount() was called with a non-zero value then a
		 *  commit will be called after every "commitcount" rows is
		 *  inserted.  No commit will be called if setCommitCount() was
		 *  never called, or if setCommitCount(0) was called.
		 *
		 *  Returns true on success and false if an error occurred. */
		virtual bool	importData();

	#include <sqlrelay/private/sqlrimportxml.h>
};

#endif
