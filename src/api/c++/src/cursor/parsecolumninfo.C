// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>
#include <defines.h>
#define NEED_DATATYPESTRING
#include <datatypes.h>

int	sqlrcursor::parseColumnInfo() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Parsing Column Info\n");
		sqlrc->debugPreEnd();
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Actual row count: ");
		sqlrc->debugPreEnd();
	}

	// first get whether the server knows the total number of rows or not
	if (getShort(&knowsactualrows)!=sizeof(unsigned short)) {
		return -1;
	}

	// get the number of rows returned by the query
	if (knowsactualrows==ACTUAL_ROWS) {
		if (getLong(&actualrows)!=sizeof(unsigned long)) {
			return -1;
		}
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint((long)actualrows);
			sqlrc->debugPreEnd();
		}
	} else {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("unknown");
			sqlrc->debugPreEnd();
		}
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("\n");
		sqlrc->debugPrint("Affected row count: ");
		sqlrc->debugPreEnd();
	}

	// get whether the server knows the number of affected rows or not
	if (getShort(&knowsaffectedrows)!=sizeof(unsigned short)) {
		return -1;
	}

	// get the number of rows affected by the query
	if (knowsaffectedrows==AFFECTED_ROWS) {
		if (getLong(&affectedrows)!=sizeof(unsigned long)) {
			return -1;
		}
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint((long)affectedrows);
			sqlrc->debugPreEnd();
		}
	} else {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("unknown");
			sqlrc->debugPreEnd();
		}
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	// get whether the server is sending column info or not
	if (getShort(&sentcolumninfo)!=sizeof(unsigned short)) {
		return -1;
	}

	// get column count
	if (getLong(&colcount)!=sizeof(unsigned long)) {
		return -1;
	}
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Column count: ");
		sqlrc->debugPrint((long)colcount);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	// we have to do this here even if we're not getting the column
	// descriptions because we are going to use the longdatatype member
	// variable no matter what
	createColumnBuffers();

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO) {

		// get whether column types will be predefined id's or strings
		if (getShort(&columntypeformat)!=sizeof(unsigned short)) {
			return -1;
		}

		// some useful variables
		unsigned short	length;
		column		*currentcol;

		// get the columninfo segment
		for (unsigned long i=0; i<colcount; i++) {
	
			// get the column name length
			if (getShort(&length)!=sizeof(unsigned short)) {
				return -1;
			}
	
			// which column to use
			currentcol=getColumnInternal(i);
	
			// get the column name
			currentcol->name=(char *)colstorage->malloc(length+1);
			if (getString(currentcol->name,length)!=length) {
				return -1;
			}
			currentcol->name[length]=(char)NULL;

			// upper/lowercase column name if necessary
			if (colcase==UPPER_CASE) {
				charstring::upper(currentcol->name);
			} else if (colcase==LOWER_CASE) {
				charstring::lower(currentcol->name);
			}

			if (columntypeformat==COLUMN_TYPE_IDS) {

				// get the column type
				if (getShort(&currentcol->type)!=
						sizeof(unsigned short)) {
					return -1;
				}

			} else {

				// get the column type length
				if (getShort(&currentcol->typestringlength)!=
						sizeof(unsigned short)) {
					return -1;
				}

				// get the column type
				currentcol->typestring=new
					char[currentcol->typestringlength+1];
				currentcol->typestring[
					currentcol->typestringlength]=
								(char)NULL;
				if (getString(currentcol->typestring,
						currentcol->typestringlength)!=
						currentcol->typestringlength) {
					return -1;
				}
			}

			// get the column length
			if (getLong(&currentcol->length)!=
						sizeof(unsigned long)) {
				return -1;
			}

			// get the column precision
			if (getLong(&currentcol->precision)!=
						sizeof(unsigned long)) {
				return -1;
			}

			// get the column scale
			if (getLong(&currentcol->scale)!=
						sizeof(unsigned long)) {
				return -1;
			}

			// get whether the column is nullable
			if (getShort(&currentcol->nullable)!=
						sizeof(unsigned short)) {
				return -1;
			}

			// get whether the column is a primary key
			if (getShort(&currentcol->primarykey)!=
						sizeof(unsigned short)) {
				return -1;
			}

			// get whether the column is unique
			if (getShort(&currentcol->unique)!=
						sizeof(unsigned short)) {
				return -1;
			}

			// get whether the column is part of a key
			if (getShort(&currentcol->partofkey)!=
						sizeof(unsigned short)) {
				return -1;
			}

			// get whether the column is unsigned
			if (getShort(&currentcol->unsignednumber)!=
						sizeof(unsigned short)) {
				return -1;
			}

			// get whether the column is zero-filled
			if (getShort(&currentcol->zerofill)!=
						sizeof(unsigned short)) {
				return -1;
			}

			// get whether the column is binary
			if (getShort(&currentcol->binary)!=
						sizeof(unsigned short)) {
				return -1;
			}

			// get whether the column is auto-incremented
			if (getShort(&currentcol->autoincrement)!=
						sizeof(unsigned short)) {
				return -1;
			}

			// initialize the longest value
			currentcol->longest=0;
	
			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("\"");
				sqlrc->debugPrint(currentcol->name);
				sqlrc->debugPrint("\",");
				sqlrc->debugPrint("\"");
				if (columntypeformat!=COLUMN_TYPE_IDS) {
					sqlrc->debugPrint(
						currentcol->typestring);
				} else {
					sqlrc->debugPrint(datatypestring[
							currentcol->type]);
				}
				sqlrc->debugPrint("\", ");
				sqlrc->debugPrint((long)currentcol->length);
				sqlrc->debugPrint(" (");
				sqlrc->debugPrint((long)currentcol->precision);
				sqlrc->debugPrint(",");
				sqlrc->debugPrint((long)currentcol->scale);
				sqlrc->debugPrint(") ");
				if (!(long)currentcol->nullable) {
					sqlrc->debugPrint("NOT NULL ");
				}
				if ((long)currentcol->primarykey) {
					sqlrc->debugPrint("Primary Key ");
				}
				if ((long)currentcol->unique) {
					sqlrc->debugPrint("Unique ");
				}
				if ((long)currentcol->partofkey) {
					sqlrc->debugPrint("Part of a Key ");
				}
				if ((long)currentcol->unsignednumber) {
					sqlrc->debugPrint("Unsigned ");
				}
				if ((long)currentcol->zerofill) {
					sqlrc->debugPrint("Zero Filled ");
				}
				if ((long)currentcol->binary) {
					sqlrc->debugPrint("Binary ");
				}
				if ((long)currentcol->autoincrement) {
					sqlrc->debugPrint("Auto-Increment ");
				}
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		}
	}

	// cache the column definitions
	cacheColumnInfo();

	return 1;
}

void	sqlrcursor::createColumnBuffers() {

	// we could get really sophisticated here and keep stats on the number
	// of columns that previous queries returned and adjust the size of
	// "columns" periodically, but for now, we'll just use a static size

	// create the standard set of columns, this will hang around until
	// the cursor is deleted
	if (!columns) {
		columns=new column[OPTIMISTIC_COLUMN_COUNT];
	}

	// if there are more columns than our static column buffer
	// can handle, create extra columns, these will be deleted after each
	// query
	if (colcount>OPTIMISTIC_COLUMN_COUNT && colcount>previouscolcount) {
		delete[] extracolumns;
		extracolumns=new column[colcount-OPTIMISTIC_COLUMN_COUNT];
	}
}
