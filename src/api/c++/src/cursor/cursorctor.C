// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

sqlrcursor::sqlrcursor(sqlrconnection *sqlrc) {

	// copy references
	copyrefs=false;

	this->sqlrc=sqlrc;

	// put self in connection's cursor list
	if (sqlrc->lastcursor) {
		sqlrc->lastcursor->next=this;
		prev=sqlrc->lastcursor;
	} else {
		sqlrc->firstcursor=this;
		prev=NULL;
	}
	sqlrc->lastcursor=this;
	next=NULL;

	// session state
	cached=false;

	// query
	querybuffer=NULL;
	fullpath=NULL;

	// result set
	rsbuffersize=0;

	firstrowindex=0;
	rowcount=0;
	previousrowcount=0;
	actualrows=0;
	affectedrows=0;
	endofresultset=true;

	error=NULL;

	rows=NULL;
	extrarows=NULL;
	firstextrarow=NULL;
	rowstorage=new memorypool(OPTIMISTIC_RESULT_SET_SIZE,
			OPTIMISTIC_RESULT_SET_SIZE/OPTIMISTIC_ROW_COUNT,5);
	fields=NULL;
	fieldlengths=NULL;
	getrowcount=0;
	getrowlengthcount=0;

	colcount=0;
	previouscolcount=0;
	columns=NULL;
	extracolumns=NULL;
	colstorage=new memorypool(OPTIMISTIC_COLUMN_DATA_SIZE,
			OPTIMISTIC_COLUMN_DATA_SIZE/OPTIMISTIC_COLUMN_COUNT,5);
	columnnamearray=NULL;

	returnnulls=false;

	// cache file
	cachesource=NULL;
	cachesourceind=NULL;
	cachedestname=NULL;
	cachedestindname=NULL;
	cachedest=NULL;
	cachedestind=NULL;
	cacheon=false;

	// options...
	sendcolumninfo=SEND_COLUMN_INFO;
	sentcolumninfo=SEND_COLUMN_INFO;
	columntypeformat=COLUMN_TYPE_IDS;
	colcase=MIXED_CASE;

	// cursor id
	cursorid=0;
	havecursorid=false;

	// initialize all bind/substitution-related variables
	clearVariables();
	initVariables();
}
