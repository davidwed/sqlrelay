// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/rawbuffer.h>
#include <defines.h>
#include <datatypes.h>

bool sqlrcursor::parseData() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Parsing Data\n");
		sqlrc->debugPreEnd();
	}

	// if we're already at the end of the result set, then just return
	if (endofresultset) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Already at the end of the result set\n");
			sqlrc->debugPreEnd();
		}
		return true;
	}

	// useful variables
	uint16_t	type;
	uint32_t	length;
	char		*buffer=NULL;
	uint32_t	colindex=0;
	column		*currentcol;
	row		*currentrow=NULL;

	// set firstrowindex to the index of the first row in the buffer
	firstrowindex=rowcount;

	// keep track of how large the buffer is
	uint64_t	rowbuffercount=0;

	// get rows
	for (;;) {

		// get the type of the field
		if (getShort(&type)!=sizeof(uint16_t)) {
			setError("Failed to get the field type.\n A network error may have occurred");
			return false;
		}

		// check for the end of the result set
		if (type==END_RESULT_SET) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("Got end of result set.\n");
				sqlrc->debugPreEnd();
			}
			endofresultset=true;

			// if we were stepping through a cached result set
			// then we need to close the file
			clearCacheSource();
			break;
		} 

		// if we're on the first column, start a new row,
		// reset the column pointer, and increment the
		// buffer counter and total row counter
		if (colindex==0) {

			if (rowbuffercount<OPTIMISTIC_ROW_COUNT) {
				if (!rows) {
					createRowBuffers();
				}
				currentrow=rows[rowbuffercount];
			} else {
				if (sqlrc->debug) {
					sqlrc->debugPreStart();
					sqlrc->debugPrint("Creating extra rows.\n");
					sqlrc->debugPreEnd();
				}
				if (!firstextrarow) {
					currentrow=new row(colcount);
					firstextrarow=currentrow;
				} else {
					currentrow->next=new row(colcount);
					currentrow=currentrow->next;
				}
			}
			if (colcount>previouscolcount) {
				currentrow->resize(colcount);
			}

			rowbuffercount++;
			rowcount++;
		}

		if (type==NULL_DATA) {

			// handle null data
			if (returnnulls) {
				buffer=NULL;
			} else {
				buffer=(char *)rowstorage->malloc(1);
				buffer[0]=(char)NULL;
			}
			length=0;

		} else if (type==STRING_DATA) {
		
			// handle non-null data
			if (getLong(&length)!=sizeof(uint32_t)) {
				setError("Failed to get the field length.\n A network error may have occurred");
				return false;
			}

			// for non-long, non-NULL datatypes...
			// get the field into a buffer
			buffer=(char *)rowstorage->malloc(length+1);
			if ((uint32_t)getString(buffer,length)!=length) {
				setError("Failed to get the field data.\n A network error may have occurred");
				return false;
			}
			buffer[length]=(char)NULL;

		} else if (type==START_LONG_DATA) {

			uint64_t	totallength;
			if (getLongLong(&totallength)!=sizeof(uint64_t)) {
				setError("Failed to get total length.\n A network error may have occurred");
				return false;
			}

			// create a buffer to hold the data
			buffer=new char[totallength+1];

			// handle a long datatype
			uint64_t	offset=0;
			for (;;) {

				// get the type of the chunk
				if (getShort(&type)!=sizeof(uint16_t)) {
					delete[] buffer;
					setError("Failed to get chunk type.\n A network error may have occurred");
					return false;
				}

				// check to see if we're done
				if (type==END_LONG_DATA) {
					break;
				}

				// get the length of the chunk
				if (getLong(&length)!=sizeof(uint32_t)) {
					delete[] buffer;
					setError("Failed to get chunk length.\n A network error may have occurred");
					return false;
				}

				// Oracle in particular has a function that
				// returns the number of characters in a CLOB,
				// but not the number of bytes.  Since
				// varying-width character data can be stored
				// in a CLOB, characters may be less than bytes.
				// AFAIK, there's no way to get the number of
				// bytes.  So, we use the number of characters
				// as a starting point, and extend buffer if
				// necessary.
				if (offset+length>totallength) {
					char	*newbuffer=
						new char[offset+length+1];
					rawbuffer::copy(
						newbuffer,buffer,offset);
					delete[] buffer;
					buffer=newbuffer;
					totallength=offset+length;
				}

				// get the chunk of data
				if ((uint32_t)getString(buffer+offset,
							length)!=length) {
					delete[] buffer;
					setError("Failed to get chunk data.\n A network error may have occurred");
					return false;
				}

				offset=offset+length;
			}
			// NULL terminate the buffer.  This makes 
			// certain operations safer and won't hurt
			// since the actual length (which doesn't
			// include the NULL) is available from
			// getFieldLength.
			buffer[totallength]=(char)NULL;
			length=totallength;
		}

		// add the buffer to the current row
		currentrow->addField(colindex,buffer,length);
	
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			if (buffer) {
				sqlrc->debugPrint("\"");
				sqlrc->debugPrint(buffer);
				sqlrc->debugPrint("\",");
			} else {
				sqlrc->debugPrint(buffer);
				sqlrc->debugPrint(",");
			}
			sqlrc->debugPreEnd();
		}

		// tag the column as a long data type or not
		currentcol=getColumnInternal(colindex);

		// set whether this column is a "long type" or not
		currentcol->longdatatype=(type==END_LONG_DATA)?1:0;

		if (sendcolumninfo==SEND_COLUMN_INFO && 
				sentcolumninfo==SEND_COLUMN_INFO) {

			// keep track of the longest field
			if (length>currentcol->longest) {
				currentcol->longest=length;
			}
		}

		// move to the next column, handle end of row 
		colindex++;
		if (colindex==colcount) {

			colindex=0;

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

			// check to see if we've gotten enough rows
			if (rsbuffersize && rowbuffercount==rsbuffersize) {
				break;
			}
		}
	}

	// terminate the row list
	if (rowbuffercount>=OPTIMISTIC_ROW_COUNT && currentrow) {
		currentrow->next=NULL;
		createExtraRowArray();
	}

	// cache the rows
	cacheData();

	return true;
}

void sqlrcursor::createRowBuffers() {

	// rows will hang around from now until the cursor is deleted,
	// getting reused with each query
	rows=new row *[OPTIMISTIC_ROW_COUNT];
	for (uint64_t i=0; i<OPTIMISTIC_ROW_COUNT; i++) {
		rows[i]=new row(colcount);
	}
}

void sqlrcursor::createExtraRowArray() {

	// create the arrays
	uint64_t	howmany=rowcount-firstrowindex-OPTIMISTIC_ROW_COUNT;
	extrarows=new row *[howmany];
	
	// populate the arrays
	row	*currentrow=firstextrarow;
	for (uint64_t i=0; i<howmany; i++) {
		extrarows[i]=currentrow;
		currentrow=currentrow->next;
	}
}
