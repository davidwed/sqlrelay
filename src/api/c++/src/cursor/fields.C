// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

char *sqlrcursor::getFieldInternal(int row, int col) {
	if (row<OPTIMISTIC_ROW_COUNT) {
		return rows[row]->getField(col);
	}
	return extrarows[row-OPTIMISTIC_ROW_COUNT]->getField(col);
}

unsigned long sqlrcursor::getFieldLengthInternal(int row, int col) {
	if (row<OPTIMISTIC_ROW_COUNT) {
		return rows[row]->getFieldLength(col);
	}
	return extrarows[row-OPTIMISTIC_ROW_COUNT]->getFieldLength(col);
}

const char *sqlrcursor::getField(int row, int col) {

	if (rowcount && row>=0 && row>=(int)firstrowindex && 
					col>=0 && col<(int)colcount) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		int	rowbufferindex=fetchRowIntoBuffer(row);

		if (rowbufferindex>-1) {
			return getFieldInternal(rowbufferindex,col);
		}
	}
	return NULL;
}

long sqlrcursor::getFieldAsLong(int row, int col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toLong(field):0;
}

double sqlrcursor::getFieldAsDouble(int row, int col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toDouble(field):0.0;
}

const char *sqlrcursor::getField(int row, const char *col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			rowcount && row>=0 && row>=(int)firstrowindex) {
		for (unsigned long i=0; i<colcount; i++) {
			if (!charstring::compareIgnoringCase(
					getColumnInternal(i)->name,col)) {

				// in the event that we're stepping through the
				// result set instead of buffering the entire 
				// thing, the requested row may have to be 
				// fetched into the buffer...
				int	rowbufferindex=fetchRowIntoBuffer(row);

				if (rowbufferindex>-1) {
					return getFieldInternal(
							rowbufferindex,i);
				}
				return NULL;
			}
		}
	}
	return NULL;
}

long sqlrcursor::getFieldAsLong(int row, const char *col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toLong(field):0;
}

double sqlrcursor::getFieldAsDouble(int row, const char *col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toDouble(field):0.0;
}

long sqlrcursor::getFieldLength(int row, int col) {

	if (rowcount && row>=0 && row>=(int)firstrowindex && 
					col>=0 && col<(int)colcount) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		int	rowbufferindex=fetchRowIntoBuffer(row);

		if (rowbufferindex>-1) {
			return getFieldLengthInternal(rowbufferindex,col);
		}
	}
	return -1;
}

long sqlrcursor::getFieldLength(int row, const char *col) {

	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			rowcount && row>=0 && row>=(int)firstrowindex) {

		for (unsigned long i=0; i<colcount; i++) {
			if (!charstring::compareIgnoringCase(
					getColumnInternal(i)->name,col)) {

				// in the event that we're stepping through the
				// result set instead of buffering the entire 
				// thing, the requested row may have to be 
				// fetched into the buffer...
				int	rowbufferindex=fetchRowIntoBuffer(row);

				if (rowbufferindex>-1) {
					return getFieldLengthInternal(
							rowbufferindex,i);
				}
				return -1;
			}
		}
	}
	return -1;
}

const char * const *sqlrcursor::getRow(int row) {

	if (rowcount && row>=0 && row>=(int)firstrowindex) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		int	rowbufferindex=fetchRowIntoBuffer(row);

		if (rowbufferindex>-1) {
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
	unsigned long	rowbuffercount=rowcount-firstrowindex;
	fields=new char **[rowbuffercount+1];
	fields[rowbuffercount]=(char **)NULL;
	for (unsigned long i=0; i<rowbuffercount; i++) {
		fields[i]=new char *[colcount+1];
		fields[i][colcount]=(char *)NULL;
		for (unsigned long j=0; j<colcount; j++) {
			fields[i][j]=getFieldInternal(i,j);
		}
	}
}

long *sqlrcursor::getRowLengths(int row) {

	if (rowcount && row>=0 && row>=(int)firstrowindex) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		int	rowbufferindex=fetchRowIntoBuffer(row);

		if (rowbufferindex>-1) {
			if (!fieldlengths) {
				createFieldLengths();
			}
			return (long *)fieldlengths[rowbufferindex];
		}
	}
	return NULL;
}

void sqlrcursor::createFieldLengths() {
	// lets say that rowcount=5 and firstrowindex=3,
	// the fieldlengths array will contain 2 elements:
	// 	fieldlengths[0] (corresponding to row 3) and
	// 	fieldlengths[1] (corresponding to row 4)
	unsigned long	rowbuffercount=rowcount-firstrowindex;
	fieldlengths=new unsigned long *[rowbuffercount+1];
	fieldlengths[rowbuffercount]=(unsigned long)NULL;
	for (unsigned long i=0; i<rowbuffercount; i++) {
		fieldlengths[i]=new unsigned long[colcount+1];
		fieldlengths[i][colcount]=(unsigned long)NULL;
		for (unsigned long j=0; j<colcount; j++) {
			fieldlengths[i][j]=getFieldLengthInternal(i,j);
		}
	}
}
