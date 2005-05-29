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
	char	*buffer=NULL;
	uint32_t	colindex=0;
	column	*currentcol;
	row	*currentrow=NULL;

	// set firstrowindex to the index of the first row in the buffer
	firstrowindex=rowcount;

	// keep track of how large the buffer is
	uint32_t	rowbuffercount=0;

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

		} else if (type==NORMAL_DATA) {
		
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

			// handle a long datatype
			char		*oldbuffer=NULL;
			uint32_t	totallength=0;
			for (;;) {

				// get the type of the chunk
				if (getShort(&type)!=sizeof(uint16_t)) {
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

				// create a buffer to hold the chunk
				buffer=new char[totallength+length+1];
				if (totallength) {
					rawbuffer::copy(buffer,oldbuffer,
								totallength);
					delete[] oldbuffer;
					oldbuffer=buffer;
					buffer=buffer+totallength;
				} else {
					oldbuffer=buffer;
				}
				totallength=totallength+length;

				// get the chunk of data
				if ((uint32_t)getString(buffer,length)!=
								length) {
					delete[] buffer;
					setError("Failed to get chunk data.\n A network error may have occurred");
					return false;
				}

				// NULL terminate the buffer.  This makes 
				// certain operations safer and won't hurt
				// since the actual length (which doesn't
				// include the NULL) is available from
				// getFieldLength.
				buffer[length]=(char)NULL;
			}
			buffer=oldbuffer;
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
	for (uint32_t i=0; i<OPTIMISTIC_ROW_COUNT; i++) {
		rows[i]=new row(colcount);
	}
}

void sqlrcursor::createExtraRowArray() {

	// create the arrays
	uint32_t	howmany=rowcount-firstrowindex-OPTIMISTIC_ROW_COUNT;
	extrarows=new row *[howmany];
	
	// populate the arrays
	row	*currentrow=firstextrarow;
	for (uint32_t i=0; i<howmany; i++) {
		extrarows[i]=currentrow;
		currentrow=currentrow->next;
	}
}
