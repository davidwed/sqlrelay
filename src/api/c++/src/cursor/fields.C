// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

char *sqlrcursor::getFieldInternal(uint32_t row, uint32_t col) {
	if (row<OPTIMISTIC_ROW_COUNT) {
		return rows[row]->getField(col);
	}
	return extrarows[row-OPTIMISTIC_ROW_COUNT]->getField(col);
}

uint32_t sqlrcursor::getFieldLengthInternal(uint32_t row, uint32_t col) {
	if (row<OPTIMISTIC_ROW_COUNT) {
		return rows[row]->getFieldLength(col);
	}
	return extrarows[row-OPTIMISTIC_ROW_COUNT]->getFieldLength(col);
}

const char *sqlrcursor::getField(uint32_t row, uint32_t col) {

	if (rowcount && row>=firstrowindex && col<colcount) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		uint32_t	rowbufferindex;
		if (fetchRowIntoBuffer(false,row,&rowbufferindex)) {
			return getFieldInternal(rowbufferindex,col);
		}
	}
	return NULL;
}

int32_t sqlrcursor::getFieldAsLong(uint32_t row, uint32_t col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toLong(field):0;
}

double sqlrcursor::getFieldAsDouble(uint32_t row, uint32_t col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toDouble(field):0.0;
}

const char *sqlrcursor::getField(uint32_t row, const char *col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			rowcount && row>=firstrowindex) {
		for (uint32_t i=0; i<colcount; i++) {
			if (!charstring::compareIgnoringCase(
					getColumnInternal(i)->name,col)) {

				// in the event that we're stepping through the
				// result set instead of buffering the entire 
				// thing, the requested row may have to be 
				// fetched into the buffer...
				uint32_t	rowbufferindex;
				if (fetchRowIntoBuffer(false,row,
							&rowbufferindex)) {
					return getFieldInternal(
							rowbufferindex,i);
				}
				return NULL;
			}
		}
	}
	return NULL;
}

int32_t sqlrcursor::getFieldAsLong(uint32_t row, const char *col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toLong(field):0;
}

double sqlrcursor::getFieldAsDouble(uint32_t row, const char *col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toDouble(field):0.0;
}

uint32_t sqlrcursor::getFieldLength(uint32_t row, uint32_t col) {

	if (rowcount && row>=firstrowindex && col<colcount) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		uint32_t	rowbufferindex;
		if (fetchRowIntoBuffer(false,row,&rowbufferindex)) {
			return getFieldLengthInternal(rowbufferindex,col);
		}
	}
	return 0;
}

uint32_t sqlrcursor::getFieldLength(uint32_t row, const char *col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			rowcount && row>=firstrowindex) {

		for (uint32_t i=0; i<colcount; i++) {
			if (!charstring::compareIgnoringCase(
					getColumnInternal(i)->name,col)) {

				// in the event that we're stepping through the
				// result set instead of buffering the entire 
				// thing, the requested row may have to be 
				// fetched into the buffer...
				uint32_t	rowbufferindex;
				if (fetchRowIntoBuffer(false,row,
							&rowbufferindex)) {
					return getFieldLengthInternal(
							rowbufferindex,i);
				}
				return 0;
			}
		}
	}
	return 0;
}

const char * const *sqlrcursor::getRow(uint32_t row) {

	if (rowcount && row>=firstrowindex) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		uint32_t	rowbufferindex;
		if (fetchRowIntoBuffer(false,row,&rowbufferindex)) {
			if (!fields) {
				createFields();
			}
			return fields[rowbufferindex];
		}
	}
	return NULL;
}

void sqlrcursor::createFields() {
	// lets say that rowcount=5 and firstrowindex=3,
	// the fields array will contain 2 elements:
	// 	fields[0] (corresponding to row 3) and
	// 	fields[1] (corresponding to row 4)
	uint32_t	rowbuffercount=rowcount-firstrowindex;
	fields=new char **[rowbuffercount+1];
	fields[rowbuffercount]=(char **)NULL;
	for (uint32_t i=0; i<rowbuffercount; i++) {
		fields[i]=new char *[colcount+1];
		fields[i][colcount]=(char *)NULL;
		for (uint32_t j=0; j<colcount; j++) {
			fields[i][j]=getFieldInternal(i,j);
		}
	}
}

uint32_t *sqlrcursor::getRowLengths(uint32_t row) {

	if (rowcount && row>=firstrowindex) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		uint32_t	rowbufferindex;
		if (fetchRowIntoBuffer(false,row,&rowbufferindex)) {
			if (!fieldlengths) {
				createFieldLengths();
			}
			return fieldlengths[rowbufferindex];
		}
	}
	return NULL;
}

void sqlrcursor::createFieldLengths() {
	// lets say that rowcount=5 and firstrowindex=3,
	// the fieldlengths array will contain 2 elements:
	// 	fieldlengths[0] (corresponding to row 3) and
	// 	fieldlengths[1] (corresponding to row 4)
	uint32_t	rowbuffercount=rowcount-firstrowindex;
	fieldlengths=new uint32_t *[rowbuffercount+1];
	fieldlengths[rowbuffercount]=(uint32_t)NULL;
	for (uint32_t i=0; i<rowbuffercount; i++) {
		fieldlengths[i]=new uint32_t[colcount+1];
		fieldlengths[i][colcount]=(uint32_t)NULL;
		for (uint32_t j=0; j<colcount; j++) {
			fieldlengths[i][j]=getFieldLengthInternal(i,j);
		}
	}
}
