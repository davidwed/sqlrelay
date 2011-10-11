// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>
#include <defines.h>
#define NEED_DATATYPESTRING
#include <datatypes.h>

bool sqlrcursor::parseColumnInfo() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Parsing Column Info\n");
		sqlrc->debugPrint("Actual row count: ");
		sqlrc->debugPreEnd();
	}

	// first get whether the server knows the total number of rows or not
	if (getShort(&knowsactualrows)!=sizeof(uint16_t)) {
		setError("Failed to get whether the server knows the number actual rows or not.\n A network error may have occurred.");
		return false;
	}

	// get the number of rows returned by the query
	if (knowsactualrows==ACTUAL_ROWS) {
		if (getLongLong(&actualrows)!=sizeof(uint64_t)) {
			setError("Failed to get the number of actual rows.\n A network error may have occurred.");
			return false;
		}
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint((int64_t)actualrows);
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
	if (getShort(&knowsaffectedrows)!=sizeof(uint16_t)) {
		setError("Failed to get whether the server knows the number of affected rows or not.\n A network error may have occurred.");
		return false;
	}

	// get the number of rows affected by the query
	if (knowsaffectedrows==AFFECTED_ROWS) {
		if (getLongLong(&affectedrows)!=sizeof(uint64_t)) {
			setError("Failed to get the number of affected rows.\n A network error may have occurred.");
			return false;
		}
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint((int64_t)affectedrows);
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
	if (getShort(&sentcolumninfo)!=sizeof(uint16_t)) {
		setError("Failed to get whether the server is sending column info or not.\n A network error may have occurred.");
		return false;
	}

	// get column count
	if (getLong(&colcount)!=sizeof(uint32_t)) {
		setError("Failed to get the column count.\n A network error may have occurred.");
		return false;
	}
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Column count: ");
		sqlrc->debugPrint((int64_t)colcount);
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
		if (getShort(&columntypeformat)!=sizeof(uint16_t)) {
			setError("Failed to whether column types will be predefined id's or strings.\n A network error may have occurred.");
			return false;
		}

		// some useful variables
		uint16_t	length;
		column		*currentcol;

		// get the columninfo segment
		for (uint32_t i=0; i<colcount; i++) {
	
			// get the column name length
			if (getShort(&length)!=sizeof(uint16_t)) {
				setError("Failed to get the column name length.\n A network error may have occurred.");
				return false;
			}
	
			// which column to use
			currentcol=getColumnInternal(i);
	
			// get the column name
			currentcol->name=(char *)colstorage->malloc(length+1);
			if (getString(currentcol->name,length)!=length) {
				setError("Failed to get the column name.\n A network error may have occurred.");
				return false;
			}
			currentcol->name[length]='\0';

			// upper/lowercase column name if necessary
			if (colcase==UPPER_CASE) {
				charstring::upper(currentcol->name);
			} else if (colcase==LOWER_CASE) {
				charstring::lower(currentcol->name);
			}

			if (columntypeformat==COLUMN_TYPE_IDS) {

				// get the column type
				if (getShort(&currentcol->type)!=
						sizeof(uint16_t)) {
					setError("Failed to get the column type.\n A network error may have occurred.");
					return false;
				}

			} else {

				// get the column type length
				if (getShort(&currentcol->typestringlength)!=
						sizeof(uint16_t)) {
					setError("Failed to get the column type length.\n A network error may have occurred.");
					return false;
				}

				// get the column type
				currentcol->typestring=new
					char[currentcol->typestringlength+1];
				currentcol->typestring[
					currentcol->typestringlength]='\0';
				if (getString(currentcol->typestring,
						currentcol->typestringlength)!=
						currentcol->typestringlength) {
					setError("Failed to get the column type.\n A network error may have occurred.");
					return false;
				}
			}

			// get the column length
			// get the column precision
			// get the column scale
			// get whether the column is nullable
			// get whether the column is a primary key
			// get whether the column is unique
			// get whether the column is part of a key
			// get whether the column is unsigned
			// get whether the column is zero-filled
			// get whether the column is binary
			// get whether the column is auto-incremented
			if (getLong(&currentcol->length)!=
						sizeof(uint32_t) ||
				getLong(&currentcol->precision)!=
						sizeof(uint32_t) ||
				getLong(&currentcol->scale)!=
						sizeof(uint32_t) ||
				getShort(&currentcol->nullable)!=
						sizeof(uint16_t) ||
				getShort(&currentcol->primarykey)!=
						sizeof(uint16_t) ||
				getShort(&currentcol->unique)!=
						sizeof(uint16_t) ||
				getShort(&currentcol->partofkey)!=
						sizeof(uint16_t) ||
				getShort(&currentcol->unsignednumber)!=
						sizeof(uint16_t) ||
				getShort(&currentcol->zerofill)!=
						sizeof(uint16_t) ||
				getShort(&currentcol->binary)!=
						sizeof(uint16_t) ||
				getShort(&currentcol->autoincrement)!=
						sizeof(uint16_t)) {
				setError("Failed to get column info.\n A network error may have occurred.");
				return false;
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
				sqlrc->debugPrint((int64_t)currentcol->length);
				sqlrc->debugPrint(" (");
				sqlrc->debugPrint((int64_t)
							currentcol->precision);
				sqlrc->debugPrint(",");
				sqlrc->debugPrint((int64_t)currentcol->scale);
				sqlrc->debugPrint(") ");
				if (!currentcol->nullable) {
					sqlrc->debugPrint("NOT NULL ");
				}
				if (currentcol->primarykey) {
					sqlrc->debugPrint("Primary Key ");
				}
				if (currentcol->unique) {
					sqlrc->debugPrint("Unique ");
				}
				if (currentcol->partofkey) {
					sqlrc->debugPrint("Part of a Key ");
				}
				if (currentcol->unsignednumber) {
					sqlrc->debugPrint("Unsigned ");
				}
				if (currentcol->zerofill) {
					sqlrc->debugPrint("Zero Filled ");
				}
				if (currentcol->binary) {
					sqlrc->debugPrint("Binary ");
				}
				if (currentcol->autoincrement) {
					sqlrc->debugPrint("Auto-Increment ");
				}
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		}
	}

	// cache the column definitions
	cacheColumnInfo();

	return true;
}

void sqlrcursor::createColumnBuffers() {

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
