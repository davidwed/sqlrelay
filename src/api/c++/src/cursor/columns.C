// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>
#include <datatypes.h>

int	sqlrcursor::colCount() {
	return colcount;
}

column	*sqlrcursor::getColumn(int index) {
	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO &&
			colcount && index>=0 && index<(int)colcount) {
		return getColumnInternal(index);
	}
	return NULL;
}

column	*sqlrcursor::getColumn(const char *name) {
	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO) {
		column	*whichcolumn;
		for (unsigned long i=0; i<colcount; i++) {
			whichcolumn=getColumnInternal(i);
			if (!strcasecmp(whichcolumn->name,name)) {
				return whichcolumn;
			}
		}
	}
	return NULL;
}

column	*sqlrcursor::getColumnInternal(int index) {
	if (index<OPTIMISTIC_COLUMN_COUNT) {
		return &columns[index];
	}
	return &extracolumns[index-OPTIMISTIC_COLUMN_COUNT];
}

char	**sqlrcursor::getColumnNames() {

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
		for (unsigned long i=0; i<colcount; i++) {
			columnnamearray[i]=getColumnInternal(i)->name;
		}
	}
	return columnnamearray;
}

char	*sqlrcursor::getColumnName(int col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->name:NULL;
}

char	*sqlrcursor::getColumnType(int col) {
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

int	sqlrcursor::getColumnLength(int col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->length:0;
}

unsigned long	sqlrcursor::getColumnPrecision(int col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->precision:0;
}

unsigned long	sqlrcursor::getColumnScale(int col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->scale:0;
}

unsigned short	sqlrcursor::getColumnIsNullable(int col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->nullable:0;
}

unsigned short	sqlrcursor::getColumnIsPrimaryKey(int col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->primarykey:0;
}

int	sqlrcursor::getLongest(int col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->longest:0;
}

char	*sqlrcursor::getColumnType(const char *col) {
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

int	sqlrcursor::getColumnLength(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->length:0;
}

unsigned long	sqlrcursor::getColumnPrecision(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->precision:0;
}

unsigned long	sqlrcursor::getColumnScale(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->scale:0;
}

unsigned short	sqlrcursor::getColumnIsNullable(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->nullable:0;
}

unsigned short	sqlrcursor::getColumnIsPrimaryKey(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->primarykey:0;
}

int	sqlrcursor::getLongest(const char *col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->longest:0;
}
