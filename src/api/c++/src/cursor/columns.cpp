// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>
#define NEED_DATATYPESTRING
#include <datatypes.h>

uint32_t sqlrcursor::colCount() {
	return colcount;
}

column *sqlrcursor::getColumn(uint32_t index) {
	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			colcount && index<colcount) {
		return getColumnInternal(index);
	}
	return NULL;
}

column *sqlrcursor::getColumn(const char *name) {
	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO) {
		column	*whichcolumn;
		for (uint32_t i=0; i<colcount; i++) {
			whichcolumn=getColumnInternal(i);
			if (!charstring::compareIgnoringCase(
						whichcolumn->name,name)) {
				return whichcolumn;
			}
		}
	}
	return NULL;
}

column *sqlrcursor::getColumnInternal(uint32_t index) {
	if (index<OPTIMISTIC_COLUMN_COUNT) {
		return &columns[index];
	}
	return &extracolumns[index-OPTIMISTIC_COLUMN_COUNT];
}

const char * const *sqlrcursor::getColumnNames() {

	if (sendcolumninfo==DONT_SEND_COLUMN_INFO ||
			sentcolumninfo==DONT_SEND_COLUMN_INFO) {
		return NULL;
	}

	if (!columnnamearray) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Creating Column Arrays...\n");
			sqlrc->debugPreEnd();
		}
	
		// build a 2d array of pointers to the column names
		columnnamearray=new char *[colcount+1];
		columnnamearray[colcount]=NULL;
		for (uint32_t i=0; i<colcount; i++) {
			columnnamearray[i]=getColumnInternal(i)->name;
		}
	}
	return columnnamearray;
}

const char *sqlrcursor::getColumnName(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->name:NULL;
}

const char *sqlrcursor::getColumnType(uint32_t col) {
	column	*whichcol=getColumn(col);
	if (whichcol) {
		if (columntypeformat!=COLUMN_TYPE_IDS) {
			return whichcol->typestring;
		} else {
			return datatypestring[whichcol->type];
		}
	}
	return NULL;
}

uint32_t sqlrcursor::getColumnLength(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->length:0;
}

uint32_t sqlrcursor::getColumnPrecision(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->precision:0;
}

uint32_t sqlrcursor::getColumnScale(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->scale:0;
}

bool sqlrcursor::getColumnIsNullable(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->nullable!=0):false;
}

bool sqlrcursor::getColumnIsPrimaryKey(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->primarykey!=0):false;
}

bool sqlrcursor::getColumnIsUnique(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->unique!=0):false;
}

bool sqlrcursor::getColumnIsPartOfKey(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->partofkey!=0):false;
}

bool sqlrcursor::getColumnIsUnsigned(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->unsignednumber!=0):false;
}

bool sqlrcursor::getColumnIsZeroFilled(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->zerofill!=0):false;
}

bool sqlrcursor::getColumnIsBinary(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->binary!=0):false;
}

bool sqlrcursor::getColumnIsAutoIncrement(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->autoincrement!=0):false;
}

uint32_t sqlrcursor::getLongest(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->longest:0;
}

const char *sqlrcursor::getColumnType(const char *col) {
	column	*whichcol=getColumn(col);
	if (whichcol) {
		if (columntypeformat!=COLUMN_TYPE_IDS) {
			return whichcol->typestring;
		} else {
			return datatypestring[whichcol->type];
		}
	}
	return NULL;
}

uint32_t sqlrcursor::getColumnLength(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->length:0;
}

uint32_t sqlrcursor::getColumnPrecision(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->precision:0;
}

uint32_t sqlrcursor::getColumnScale(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->scale:0;
}

bool sqlrcursor::getColumnIsNullable(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->nullable:false;
}

bool sqlrcursor::getColumnIsPrimaryKey(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->primarykey!=0):false;
}

bool sqlrcursor::getColumnIsUnique(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->unique!=0):false;
}

bool sqlrcursor::getColumnIsPartOfKey(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->partofkey!=0):false;
}

bool sqlrcursor::getColumnIsUnsigned(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->unsignednumber!=0):false;
}

bool sqlrcursor::getColumnIsZeroFilled(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->zerofill!=0):false;
}

bool sqlrcursor::getColumnIsBinary(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->binary!=0):false;
}

bool sqlrcursor::getColumnIsAutoIncrement(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->autoincrement!=0):false;
}


uint32_t sqlrcursor::getLongest(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->longest:0;
}
