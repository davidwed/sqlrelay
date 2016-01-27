// Copyright (c) 1999-2016  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>
#include <rudiments/permissions.h>
#include <rudiments/datetime.h>
#include <rudiments/bytestring.h>
#include <rudiments/character.h>
#include <rudiments/filesystem.h>
#include <rudiments/error.h>
#include <defines.h>
#define NEED_DATATYPESTRING
#include <datatypes.h>

// we're optimistic that the average query will contain 16 bind variables
#define OPTIMISTIC_BIND_COUNT 16

class sqlrcursorprivate {
	friend class sqlrcursor;
	private:
		bool		resumed;
		bool		cached;

		// query
		char		*querybuffer;
		const char	*queryptr;
		uint32_t	querylen;
		char		*fullpath;
		bool		reexecute;

		// substitution variables
		dynamicarray<bindvar>	*subvars;
		bool			dirtysubs;

		// bind variables
		dynamicarray<bindvar>	*inbindvars;
		dynamicarray<bindvar>	*outbindvars;
		bool			validatebinds;
		bool			dirtybinds;

		// result set
		uint64_t	rsbuffersize;
		uint16_t	sendcolumninfo;
		uint16_t	sentcolumninfo;

		uint16_t	suspendresultsetsent;
		bool		endofresultset;

		uint16_t	columntypeformat;
		uint32_t	colcount;
		uint32_t	previouscolcount;

		columncase	colcase;

		column		*columns;
		column		*extracolumns;
		memorypool	*colstorage;
		char		**columnnamearray;

		uint64_t	firstrowindex;
		uint64_t	rowcount;
		uint64_t	previousrowcount;
		uint16_t	knowsactualrows;
		uint64_t	actualrows;
		uint16_t	knowsaffectedrows;
		uint64_t	affectedrows;

		row		**rows;
		row		**extrarows;
		memorypool	*rowstorage;
		row		*firstextrarow;
		char		***fields;
		uint32_t	**fieldlengths;

		bool		returnnulls;

		// result set caching
		bool		cacheon;
		int32_t		cachettl;
		char		*cachedestname;
		char		*cachedestindname;
		file		*cachedest;
		file		*cachedestind;
		file		*cachesource;
		file		*cachesourceind;

		// error
		int64_t		errorno;
		char		*error;

		// copy references flag
		bool		copyrefs;

		// parent connection
		sqlrconnection	*sqlrc;

		// next/previous pointers
		sqlrcursor	*next;
		sqlrcursor	*prev;

		// cursor id
		uint16_t	cursorid;
		bool		havecursorid;
};

sqlrcursor::sqlrcursor(sqlrconnection *sqlrc, bool copyreferences) {
	init(sqlrc,copyreferences);
}

sqlrcursor::sqlrcursor(sqlrconnection *sqlrc) {
	init(sqlrc,false);
}

void sqlrcursor::init(sqlrconnection *sqlrc, bool copyreferences) {

	pvt=new sqlrcursorprivate;

	// copy references
	pvt->copyrefs=copyreferences;

	this->pvt->sqlrc=sqlrc;

	// put self in connection's cursor list
	if (pvt->sqlrc->lastcursor()) {
		pvt->sqlrc->lastcursor()->pvt->next=this;
		pvt->prev=pvt->sqlrc->lastcursor();
	} else {
		pvt->sqlrc->firstcursor(this);
		pvt->prev=NULL;
	}
	pvt->sqlrc->lastcursor(this);
	pvt->next=NULL;

	// session state
	pvt->cached=false;

	// query
	pvt->querybuffer=NULL;
	pvt->fullpath=NULL;

	// result set
	pvt->rsbuffersize=0;

	pvt->firstrowindex=0;
	pvt->rowcount=0;
	pvt->previousrowcount=0;
	pvt->actualrows=0;
	pvt->affectedrows=0;
	pvt->endofresultset=true;

	pvt->errorno=0;
	pvt->error=NULL;

	pvt->rows=NULL;
	pvt->extrarows=NULL;
	pvt->firstextrarow=NULL;
	pvt->rowstorage=new memorypool(OPTIMISTIC_RESULT_SET_SIZE,
			OPTIMISTIC_RESULT_SET_SIZE/OPTIMISTIC_ROW_COUNT,5);
	pvt->fields=NULL;
	pvt->fieldlengths=NULL;

	pvt->colcount=0;
	pvt->previouscolcount=0;
	pvt->columns=NULL;
	pvt->extracolumns=NULL;
	pvt->colstorage=new memorypool(OPTIMISTIC_COLUMN_DATA_SIZE,
			OPTIMISTIC_COLUMN_DATA_SIZE/OPTIMISTIC_COLUMN_COUNT,5);
	pvt->columnnamearray=NULL;

	pvt->returnnulls=false;

	// cache file
	pvt->cachesource=NULL;
	pvt->cachesourceind=NULL;
	pvt->cachedestname=NULL;
	pvt->cachedestindname=NULL;
	pvt->cachedest=NULL;
	pvt->cachedestind=NULL;
	pvt->cacheon=false;

	// options...
	pvt->sendcolumninfo=SEND_COLUMN_INFO;
	pvt->sentcolumninfo=SEND_COLUMN_INFO;
	pvt->columntypeformat=COLUMN_TYPE_IDS;
	pvt->colcase=MIXED_CASE;

	// cursor id
	pvt->cursorid=0;
	pvt->havecursorid=false;

	// initialize all bind/substitution-related variables
	pvt->subvars=new dynamicarray<bindvar>(OPTIMISTIC_BIND_COUNT,16);
	pvt->inbindvars=new dynamicarray<bindvar>(OPTIMISTIC_BIND_COUNT,16);
	pvt->outbindvars=new dynamicarray<bindvar>(OPTIMISTIC_BIND_COUNT,16);
	clearVariables();
}

sqlrcursor::~sqlrcursor() {

	// abort result set if necessary
	if (pvt->sqlrc && !pvt->sqlrc->endsessionsent() && !pvt->sqlrc->suspendsessionsent()) {
		closeResultSet(true);
	}

	// deallocate copied references
	deleteVariables();
	delete pvt->outbindvars;
	delete pvt->inbindvars;
	delete pvt->subvars;

	// deallocate the query buffer
	delete[] pvt->querybuffer;

	// deallocate the fullpath (used for file queries)
	delete[] pvt->fullpath;

	clearResultSet();
	delete[] pvt->columns;
	delete[] pvt->extracolumns;
	delete pvt->colstorage;
	if (pvt->rows) {
		for (uint32_t i=0; i<OPTIMISTIC_ROW_COUNT; i++) {
			delete pvt->rows[i];
		}
		delete[] pvt->rows;
	}
	delete pvt->rowstorage;

	// it's possible for the connection to be deleted before the 
	// cursor is, in that case, don't do any of this stuff
	if (pvt->sqlrc) {

		// remove self from connection's cursor list
		if (!pvt->next && !pvt->prev) {
			pvt->sqlrc->firstcursor(NULL);
			pvt->sqlrc->lastcursor(NULL);
		} else {
			sqlrcursor	*temp=pvt->next;
			if (pvt->next) {
				pvt->next->pvt->prev=pvt->prev;
			} else {
				pvt->sqlrc->lastcursor(pvt->prev);
			}
			if (pvt->prev) {
				pvt->prev->pvt->next=temp;
			} else {
				pvt->sqlrc->firstcursor(pvt->next);
			}
		}

		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Deallocated cursor\n");
			pvt->sqlrc->debugPreEnd();
		}
	}

	if (pvt->copyrefs && pvt->cachedestname) {
		delete[] pvt->cachedestname;
	}
	delete[] pvt->cachedestindname;

	delete pvt;
}

void sqlrcursor::setResultSetBufferSize(uint64_t rows) {
	pvt->rsbuffersize=rows;
	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Result Set Buffer Size: ");
		pvt->sqlrc->debugPrint((int64_t)rows);
		pvt->sqlrc->debugPrint("\n");
		pvt->sqlrc->debugPreEnd();
	}
}

uint64_t sqlrcursor::getResultSetBufferSize() {
	return pvt->rsbuffersize;
}

void sqlrcursor::dontGetColumnInfo() {
	pvt->sendcolumninfo=DONT_SEND_COLUMN_INFO;
}

void sqlrcursor::getColumnInfo() {
	pvt->sendcolumninfo=SEND_COLUMN_INFO;
}

void sqlrcursor::mixedCaseColumnNames() {
	pvt->colcase=MIXED_CASE;
}

void sqlrcursor::upperCaseColumnNames() {
	pvt->colcase=UPPER_CASE;
}

void sqlrcursor::lowerCaseColumnNames() {
	pvt->colcase=LOWER_CASE;
}

void sqlrcursor::cacheToFile(const char *filename) {

	pvt->cacheon=true;
	pvt->cachettl=600;
	if (pvt->copyrefs) {
		delete[] pvt->cachedestname;
		pvt->cachedestname=charstring::duplicate(filename);
	} else {
		pvt->cachedestname=(char *)filename;
	}

	// create the index name
	delete[] pvt->cachedestindname;
	size_t	cachedestindnamelen=charstring::length(filename)+5;
	pvt->cachedestindname=new char[cachedestindnamelen];
	charstring::copy(pvt->cachedestindname,filename);
	charstring::append(pvt->cachedestindname,".ind");
}

void sqlrcursor::setCacheTtl(uint32_t ttl) {
	pvt->cachettl=ttl;
}

const char *sqlrcursor::getCacheFileName() {
	return pvt->cachedestname;
}

void sqlrcursor::cacheOff() {
	pvt->cacheon=false;
}

void sqlrcursor::startCaching() {

	if (!pvt->resumed) {
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Caching data to ");
			pvt->sqlrc->debugPrint(pvt->cachedestname);
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPreEnd();
		}
	} else {
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Resuming caching data to ");
			pvt->sqlrc->debugPrint(pvt->cachedestname);
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPreEnd();
		}
	}

	// create the cache file, truncate it unless we're 
	// resuming a previous session
	pvt->cachedest=new file();
	pvt->cachedestind=new file();
	if (!pvt->resumed) {
		pvt->cachedest->open(pvt->cachedestname,
					O_RDWR|O_TRUNC|O_CREAT,
					permissions::ownerReadWrite());
		pvt->cachedestind->open(pvt->cachedestindname,
					O_RDWR|O_TRUNC|O_CREAT,
					permissions::ownerReadWrite());
	} else {
		pvt->cachedest->open(pvt->cachedestname,
					O_RDWR|O_CREAT|O_APPEND);
		pvt->cachedestind->open(pvt->cachedestindname,
					O_RDWR|O_CREAT|O_APPEND);
	}

	if (pvt->cachedest && pvt->cachedestind) {

		// calculate and set write buffer size
		// FIXME: I think rudiments bugs keep this from working...
		/*filesystem	fs;
		if (fs.initialize(pvt->cachedestname)) {
			off64_t	optblocksize=fs.getOptimumTransferBlockSize();
			pvt->cachedest->setWriteBufferSize(
					(optblocksize)?optblocksize:1024);
			pvt->cachedestind->setWriteBufferSize(
					(optblocksize)?optblocksize:1024);
		}*/

		if (!pvt->resumed) {

			// write "magic" identifier to head of files
			pvt->cachedest->write("SQLRELAYCACHE",13);
			pvt->cachedestind->write("SQLRELAYCACHE",13);
			
			// write ttl to files
			datetime	dt;
			dt.getSystemDateAndTime();
			int64_t	expiration=dt.getEpoch()+pvt->cachettl;
			pvt->cachedest->write(expiration);
			pvt->cachedestind->write(expiration);
		}

	} else {

		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Error caching data to ");
			pvt->sqlrc->debugPrint(pvt->cachedestname);
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPreEnd();
		}

		// in case of an error, clean up
		clearCacheDest();
	}
}

void sqlrcursor::cacheError() {

	if (pvt->resumed || !pvt->cachedest) {
		return;
	}

	// write the number of returned rows, affected rows 
	// and a zero to terminate the column descriptions
	pvt->cachedest->write((uint16_t)NO_ACTUAL_ROWS);
	pvt->cachedest->write((uint16_t)NO_AFFECTED_ROWS);
	pvt->cachedest->write((uint16_t)END_COLUMN_INFO);
}

void sqlrcursor::cacheNoError() {

	if (pvt->resumed || !pvt->cachedest) {
		return;
	}

	pvt->cachedest->write((uint16_t)NO_ERROR_OCCURRED);
}

void sqlrcursor::cacheColumnInfo() {

	if (pvt->resumed || !pvt->cachedest) {
		return;
	}

	// write the number of returned rows
	pvt->cachedest->write(pvt->knowsactualrows);
	if (pvt->knowsactualrows==ACTUAL_ROWS) {
		pvt->cachedest->write(pvt->actualrows);
	}

	// write the number of affected rows
	pvt->cachedest->write(pvt->knowsaffectedrows);
	if (pvt->knowsaffectedrows==AFFECTED_ROWS) {
		pvt->cachedest->write(pvt->affectedrows);
	}

	// write whether or not the column info is is cached
	pvt->cachedest->write(pvt->sentcolumninfo);

	// write the column count
	pvt->cachedest->write(pvt->colcount);

	// write column descriptions to the cache file
	if (pvt->sendcolumninfo==SEND_COLUMN_INFO && 
			pvt->sentcolumninfo==SEND_COLUMN_INFO) {

		// write column type format
		pvt->cachedest->write(pvt->columntypeformat);

		// write the columns themselves
		uint16_t	namelen;
		column		*whichcolumn;
		for (uint32_t i=0; i<pvt->colcount; i++) {

			// get the column
			whichcolumn=getColumnInternal(i);

			// write the name
			namelen=charstring::length(whichcolumn->name);
			pvt->cachedest->write(namelen);
			pvt->cachedest->write(whichcolumn->name,namelen);

			// write the type
			if (pvt->columntypeformat==COLUMN_TYPE_IDS) {
				pvt->cachedest->write(whichcolumn->type);
			} else {
				pvt->cachedest->write(whichcolumn->typestringlength);
				pvt->cachedest->write(whichcolumn->typestring,
						whichcolumn->typestringlength);
			}

			// write the length, precision and scale
			pvt->cachedest->write(whichcolumn->length);
			pvt->cachedest->write(whichcolumn->precision);
			pvt->cachedest->write(whichcolumn->scale);

			// write the flags
			pvt->cachedest->write(whichcolumn->nullable);
			pvt->cachedest->write(whichcolumn->primarykey);
			pvt->cachedest->write(whichcolumn->unique);
			pvt->cachedest->write(whichcolumn->partofkey);
			pvt->cachedest->write(whichcolumn->unsignednumber);
			pvt->cachedest->write(whichcolumn->zerofill);
			pvt->cachedest->write(whichcolumn->binary);
			pvt->cachedest->write(whichcolumn->autoincrement);
		}
	}
}

void sqlrcursor::cacheOutputBinds(uint32_t count) {

	if (pvt->resumed || !pvt->cachedest) {
		return;
	}

	// write the variable/value pairs to the cache file
	uint16_t	len;
	for (uint32_t i=0; i<count; i++) {

		pvt->cachedest->write((uint16_t)(*pvt->outbindvars)[i].type);

		len=charstring::length((*pvt->outbindvars)[i].variable);
		pvt->cachedest->write(len);
		pvt->cachedest->write((*pvt->outbindvars)[i].variable,len);

		len=(*pvt->outbindvars)[i].resultvaluesize;
		pvt->cachedest->write(len);
		if ((*pvt->outbindvars)[i].type==BINDVARTYPE_STRING ||
				(*pvt->outbindvars)[i].type==BINDVARTYPE_BLOB ||
				(*pvt->outbindvars)[i].type==BINDVARTYPE_CLOB) {
			pvt->cachedest->write((*pvt->outbindvars)[i].value.stringval,len);
			pvt->cachedest->write((*pvt->outbindvars)[i].value.lobval,len);
		} else if ((*pvt->outbindvars)[i].type==BINDVARTYPE_INTEGER) {
			pvt->cachedest->write((*pvt->outbindvars)[i].value.integerval);
		} else if ((*pvt->outbindvars)[i].type==BINDVARTYPE_DOUBLE) {
			pvt->cachedest->write((*pvt->outbindvars)[i].value.
						doubleval.value);
			pvt->cachedest->write((*pvt->outbindvars)[i].value.
						doubleval.precision);
			pvt->cachedest->write((*pvt->outbindvars)[i].value.
						doubleval.scale);
		}
	}

	// terminate the list of output binds
	pvt->cachedest->write((uint16_t)END_BIND_VARS);
}

void sqlrcursor::cacheData() {

	if (!pvt->cachedest) {
		return;
	}

	// write the data to the cache file
	uint32_t	rowbuffercount=pvt->rowcount-pvt->firstrowindex;
	for (uint32_t i=0; i<rowbuffercount; i++) {

		// get the current offset in the cache destination file
		int64_t	position=pvt->cachedest->getCurrentPosition();

		// seek to the right place in the index file and write the
		// destination file offset
		pvt->cachedestind->setPositionRelativeToBeginning(
			13+sizeof(int64_t)+((pvt->firstrowindex+i)*sizeof(int64_t)));
		pvt->cachedestind->write(position);

		// write the row to the cache file
		for (uint32_t j=0; j<pvt->colcount; j++) {
			uint16_t	type;
			int32_t		len;
			char		*field=getFieldInternal(i,j);
			if (field) {
				type=STRING_DATA;
				len=charstring::length(field);
				pvt->cachedest->write(type);
				pvt->cachedest->write(len);
				if (len>0) {
					pvt->cachedest->write(field);
				}
			} else {
				type=NULL_DATA;
				pvt->cachedest->write(type);
			}
		}
	}

	if (pvt->endofresultset) {
		finishCaching();
	}
}

void sqlrcursor::finishCaching() {

	if (!pvt->cachedest) {
		return;
	}

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Finishing caching.\n");
		pvt->sqlrc->debugPreEnd();
	}

	// terminate the result set
	pvt->cachedest->write((uint16_t)END_RESULT_SET);
	// FIXME: I think rudiments bugs keep this from working...
	/*pvt->cachedest->flushWriteBuffer(-1,-1);
	pvt->cachedestind->flushWriteBuffer(-1,-1);*/

	// close the cache file and clean up
	clearCacheDest();
}

void sqlrcursor::clearCacheDest() {

	// close the cache file and clean up
	if (pvt->cachedest) {
		pvt->cachedest->close();
		delete pvt->cachedest;
		pvt->cachedest=NULL;
		pvt->cachedestind->close();
		delete pvt->cachedestind;
		pvt->cachedestind=NULL;
		pvt->cacheon=false;
	}
}

bool sqlrcursor::getDatabaseList(const char *wild) {
	return getDatabaseList(wild,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getDatabaseList(const char *wild,
					sqlrclientlistformat_t listformat) {
	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("getting database list");
		if (wild) {
			pvt->sqlrc->debugPrint("\"");
			pvt->sqlrc->debugPrint(wild);
			pvt->sqlrc->debugPrint("\"");
		}
		pvt->sqlrc->debugPrint("\n");
		pvt->sqlrc->debugPreEnd();
	}
	return getList(GETDBLIST,listformat,NULL,wild);
}

bool sqlrcursor::getTableList(const char *wild) {
	return getTableList(wild,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getTableList(const char *wild,
					sqlrclientlistformat_t listformat) {
	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("getting table list");
		if (wild) {
			pvt->sqlrc->debugPrint("\"");
			pvt->sqlrc->debugPrint(wild);
			pvt->sqlrc->debugPrint("\"");
		}
		pvt->sqlrc->debugPrint("\n");
		pvt->sqlrc->debugPreEnd();
	}
	return getList(GETTABLELIST,listformat,NULL,wild);
}

bool sqlrcursor::getColumnList(const char *table, const char *wild) {
	return getColumnList(table,wild,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getColumnList(const char *table,
				const char *wild,
				sqlrclientlistformat_t listformat) {
	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("getting column list for: \"");
		pvt->sqlrc->debugPrint(table);
		pvt->sqlrc->debugPrint("\"");
		if (wild) {
			pvt->sqlrc->debugPrint(" - \"");
			pvt->sqlrc->debugPrint(wild);
			pvt->sqlrc->debugPrint("\"");
		}
		pvt->sqlrc->debugPrint("\n");
		pvt->sqlrc->debugPreEnd();
	}
	return getList(GETCOLUMNLIST,listformat,(table)?table:"",wild);
}

bool sqlrcursor::getList(uint16_t command, sqlrclientlistformat_t listformat,
					const char *table, const char *wild) {

	pvt->reexecute=false;
	pvt->validatebinds=false;
	pvt->resumed=false;
	clearVariables();

	if (!pvt->endofresultset) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!pvt->sqlrc->openSession()) {
		return false;
	}

	pvt->cached=false;
	pvt->endofresultset=false;

	// tell the server we want to get a db list
	pvt->sqlrc->cs()->write(command);

	// tell the server whether we'll need a cursor or not
	sendCursorStatus();

	// send the list format
	pvt->sqlrc->cs()->write((uint16_t)listformat);

	// send the wild parameter
	uint32_t	len=charstring::length(wild);
	pvt->sqlrc->cs()->write(len);
	if (len) {
		pvt->sqlrc->cs()->write(wild,len);
	}

	// send the table parameter
	if (table) {
		len=charstring::length(table);
		pvt->sqlrc->cs()->write(len);
		if (len) {
			pvt->sqlrc->cs()->write(table,len);
		}
	}

	pvt->sqlrc->flushWriteBuffer();

	// process the result set
	bool	retval=true;
	if (pvt->rsbuffersize) {
		if (!processResultSet(false,pvt->rsbuffersize-1)) {
			retval=false;
		}
	} else {
		if (!processResultSet(true,0)) {
			retval=false;
		}
	}

	// set up not to re-execute the same query if executeQuery is called
	// again before calling prepareQuery on a new query
	pvt->reexecute=false;

	return retval;
}

bool sqlrcursor::sendQuery(const char *query) {
	prepareQuery(query);
	return executeQuery();
}

bool sqlrcursor::sendQuery(const char *query, uint32_t length) {
	prepareQuery(query,length);
	return executeQuery();
}

bool sqlrcursor::sendFileQuery(const char *path, const char *filename) {
	return prepareFileQuery(path,filename) && executeQuery();
}

void sqlrcursor::prepareQuery(const char *query) {
	prepareQuery(query,charstring::length(query));
}

void sqlrcursor::prepareQuery(const char *query, uint32_t length) {
	pvt->reexecute=false;
	pvt->validatebinds=false;
	pvt->resumed=false;
	clearVariables();
	pvt->querylen=length;
	if (pvt->copyrefs) {
		initQueryBuffer(pvt->querylen);
		charstring::copy(pvt->querybuffer,query,pvt->querylen);
		pvt->querybuffer[pvt->querylen]='\0';
	} else {
		pvt->queryptr=query;
	}
}

bool sqlrcursor::prepareFileQuery(const char *path, const char *filename) {

	// init some variables
	pvt->reexecute=false;
	pvt->validatebinds=false;
	pvt->resumed=false;
	clearVariables();

	// init the fullpath buffer
	if (!pvt->fullpath) {
		pvt->fullpath=new char[MAXPATHLEN+1];
	}

	// add the path to the fullpath
	uint32_t	index=0;
	uint32_t	counter=0;
	if (path) {
		while (path[index] && counter<MAXPATHLEN) {
			pvt->fullpath[counter]=path[index];
			index++;
			counter++;
		}

		// add the "/" to the fullpath
		if (counter<=MAXPATHLEN) {
			pvt->fullpath[counter]='/';
			counter++;
		}
	}

	// add the file to the fullpath
	index=0;
	while (filename[index] && counter<MAXPATHLEN) {
		pvt->fullpath[counter]=filename[index];
		index++;
		counter++;
	}

	// handle a filename that's too long
	if (counter>MAXPATHLEN) {

		// sabotage the file name so it can't be opened
		pvt->fullpath[0]='\0';

		// debug info
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("File name ");
			if (path) {
				pvt->sqlrc->debugPrint((char *)path);
				pvt->sqlrc->debugPrint("/");
			}
			pvt->sqlrc->debugPrint((char *)filename);
			pvt->sqlrc->debugPrint(" is too long.");
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPreEnd();
		}

	} else {

		// terminate the string
		pvt->fullpath[counter]='\0';

		// debug info
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("File: ");
			pvt->sqlrc->debugPrint(pvt->fullpath);
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPreEnd();
		}
	}

	// open the file
	file	queryfile;
	if (!queryfile.open(pvt->fullpath,O_RDONLY)) {

		// set the error
		char	*err=new char[32+charstring::length(pvt->fullpath)];
		charstring::append(err,"The file ");
		charstring::append(err,pvt->fullpath);
		charstring::append(err," could not be opened.\n");
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint(err);
			pvt->sqlrc->debugPreEnd();
		}
		setError(err);

		// set queryptr to NULL so executeQuery won't try to do
		// anything with it in the event that it gets called
		pvt->queryptr=NULL;

		delete[] err;

		return false;
	}

	initQueryBuffer(queryfile.getSize());

	// read the file into the query buffer
	pvt->querylen=queryfile.getSize();
	queryfile.read((unsigned char *)pvt->querybuffer,pvt->querylen);
	pvt->querybuffer[pvt->querylen]='\0';

	queryfile.close();

	return true;
}

void sqlrcursor::initQueryBuffer(uint32_t querylength) {
	delete[] pvt->querybuffer;
	pvt->querybuffer=new char[querylength+1];
	pvt->queryptr=pvt->querybuffer;
}

void sqlrcursor::attachToBindCursor(uint16_t bindcursorid) {
	prepareQuery("");
	pvt->reexecute=true;
	pvt->cursorid=bindcursorid;
}

uint16_t sqlrcursor::countBindVariables() const {

	if (!pvt->queryptr) {
		return 0;
	}

	char	lastchar='\0';
	bool	inquotes=false;

	uint16_t	questionmarkcount=0;
	uint16_t	coloncount=0;
	uint16_t	atsigncount=0;
	uint16_t	dollarsigncount=0;

	for (const char *ptr=pvt->queryptr; *ptr; ptr++) {

		if (*ptr=='\'' && lastchar!='\\') {
			if (inquotes) {
				inquotes=false;
			} else {
				inquotes=true;
			}
		}

		// If we're not inside of a quoted string and we run into
		// a ?, : (for oracle-style binds), @ (for sap/sybase-style
		// binds) or $ (for postgresql-style binds) and the previous
		// character was something that might come before a bind
		// variable then we must have found a bind variable.
		// count ?, :, @, $ separately
		if (!inquotes &&
			character::inSet(lastchar," \t\n\r=<>,(+-*/%|&!~^")) {
			if (*ptr=='?') {
				questionmarkcount++;
			} else if (*ptr==':') {
				coloncount++;
			} else if (*ptr=='@') {
				atsigncount++;
			} else if (*ptr=='$') {
				dollarsigncount++;
			}
		}

		lastchar=*ptr;
	}

	// if we got $'s or ?'s, ignore the :'s or @'s
	if (dollarsigncount) {
		return dollarsigncount;
	}
	if (questionmarkcount) {
		return questionmarkcount;
	}
	if (coloncount) {
		return coloncount;
	}
	if (atsigncount) {
		return atsigncount;
	}
	return 0;
}

void sqlrcursor::clearVariables() {

	deleteSubstitutionVariables();
	pvt->subvars->clear();
	pvt->dirtysubs=false;
	pvt->dirtybinds=false;
	clearBinds();
}

void sqlrcursor::deleteVariables() {
	deleteSubstitutionVariables();
	deleteInputBindVariables();
	deleteOutputBindVariables();
}

void sqlrcursor::deleteSubstitutionVariables() {

	if (pvt->copyrefs) {
		for (uint64_t i=0; i<pvt->subvars->getLength(); i++) {
			delete[] (*pvt->subvars)[i].variable;
			if ((*pvt->subvars)[i].type==BINDVARTYPE_STRING) {
				delete[] (*pvt->subvars)[i].value.stringval;
			}
			if ((*pvt->subvars)[i].type==BINDVARTYPE_DATE) {
				delete[] (*pvt->subvars)[i].value.dateval.tz;
			}
		}
	}
}

void sqlrcursor::deleteInputBindVariables() {

	if (pvt->copyrefs) {
		for (uint64_t i=0; i<pvt->inbindvars->getLength(); i++) {
			delete[] (*pvt->inbindvars)[i].variable;
			if ((*pvt->inbindvars)[i].type==BINDVARTYPE_STRING) {
				delete[] (*pvt->inbindvars)[i].value.stringval;
			}
			if ((*pvt->inbindvars)[i].type==BINDVARTYPE_BLOB ||
				(*pvt->inbindvars)[i].type==BINDVARTYPE_CLOB) {
				delete[] (*pvt->inbindvars)[i].value.lobval;
			}
		}
	}
}

void sqlrcursor::deleteOutputBindVariables() {

	for (uint64_t i=0; i<pvt->outbindvars->getLength(); i++) {
		if (pvt->copyrefs) {
			delete[] (*pvt->outbindvars)[i].variable;
		}
		if ((*pvt->outbindvars)[i].type==BINDVARTYPE_STRING) {
			delete[] (*pvt->outbindvars)[i].value.stringval;
		}
		if ((*pvt->outbindvars)[i].type==BINDVARTYPE_BLOB ||
			(*pvt->outbindvars)[i].type==BINDVARTYPE_CLOB) {
			delete[] (*pvt->outbindvars)[i].value.lobval;
		}
	}
}

void sqlrcursor::substitution(const char *variable, const char *value) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,pvt->subvars);
	if (!bv) {
		bv=&(*pvt->subvars)[pvt->subvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	stringVar(bv,variable,value);
	pvt->dirtysubs=true;
}

void sqlrcursor::substitution(const char *variable, int64_t value) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,pvt->subvars);
	if (!bv) {
		bv=&(*pvt->subvars)[pvt->subvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	integerVar(bv,variable,value);
	pvt->dirtysubs=true;
}

void sqlrcursor::substitution(const char *variable, double value, 
				uint32_t precision, uint32_t scale) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,pvt->subvars);
	if (!bv) {
		bv=&(*pvt->subvars)[pvt->subvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	doubleVar(bv,variable,value,precision,scale);
	pvt->dirtysubs=true;
}

void sqlrcursor::clearBinds() {

	deleteInputBindVariables();
	pvt->inbindvars->clear();

	deleteOutputBindVariables();
	pvt->outbindvars->clear();
}

void sqlrcursor::inputBindBlob(const char *variable, const char *value,
							uint32_t size) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,pvt->inbindvars);
	if (!bv) {
		bv=&(*pvt->inbindvars)[pvt->inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	lobVar(bv,variable,value,size,BINDVARTYPE_BLOB);
	bv->send=true;
	pvt->dirtybinds=true;
}

void sqlrcursor::inputBindClob(const char *variable, const char *value,
							uint32_t size) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,pvt->inbindvars);
	if (!bv) {
		bv=&(*pvt->inbindvars)[pvt->inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	lobVar(bv,variable,value,size,BINDVARTYPE_CLOB);
	bv->send=true;
	pvt->dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable, const char *value) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,pvt->inbindvars);
	if (!bv) {
		bv=&(*pvt->inbindvars)[pvt->inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	stringVar(bv,variable,value);
	bv->send=true;
	pvt->dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable, const char *value,
						uint32_t valuesize) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,pvt->inbindvars);
	if (!bv) {
		bv=&(*pvt->inbindvars)[pvt->inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	stringVar(bv,variable,value,valuesize);
	bv->send=true;
	pvt->dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable, int64_t value) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,pvt->inbindvars);
	if (!bv) {
		bv=&(*pvt->inbindvars)[pvt->inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	integerVar(bv,variable,value);
	bv->send=true;
	pvt->dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable, double value, 
				uint32_t precision, uint32_t scale) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,pvt->inbindvars);
	if (!bv) {
		bv=&(*pvt->inbindvars)[pvt->inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	doubleVar(bv,variable,value,precision,scale);
	bv->send=true;
	pvt->dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable,
				int16_t year, int16_t month, int16_t day,
				int16_t hour, int16_t minute, int16_t second,
				int32_t microsecond, const char *tz) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,pvt->inbindvars);
	if (!bv) {
		bv=&(*pvt->inbindvars)[pvt->inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	dateVar(bv,variable,year,month,day,hour,minute,second,microsecond,tz);
	bv->send=true;
	pvt->dirtybinds=true;
}

void sqlrcursor::substitutions(const char **variables, const char **values) {
	for (uint16_t i=0; variables[i]; i++) {
		substitution(variables[i],values[i]);
	}
}

void sqlrcursor::substitutions(const char **variables, const int64_t *values) {
	for (uint16_t i=0; variables[i]; i++) {
		substitution(variables[i],values[i]);
	}
}

void sqlrcursor::substitutions(const char **variables, const double *values, 
					const uint32_t *precisions,
					const uint32_t *scales) {
	for (uint16_t i=0; variables[i]; i++) {
		substitution(variables[i],values[i],precisions[i],scales[i]);
	}
}

void sqlrcursor::inputBinds(const char **variables, const char **values) {
	for (uint16_t i=0; variables[i]; i++) {
		inputBind(variables[i],values[i]);
	}
}

void sqlrcursor::inputBinds(const char **variables, const int64_t *values) {
	for (uint16_t i=0; variables[i]; i++) {
		inputBind(variables[i],values[i]);
	}
}

void sqlrcursor::inputBinds(const char **variables, const double *values, 
					const uint32_t *precisions,
					const uint32_t *scales) {
	for (uint16_t i=0; variables[i]; i++) {
		inputBind(variables[i],values[i],
				precisions[i],scales[i]);
	}
}

void sqlrcursor::stringVar(bindvar *var, const char *variable,
						const char *value) {
	stringVar(var,variable,value,charstring::length(value));
}

void sqlrcursor::stringVar(bindvar *var, const char *variable,
						const char *value,
						uint32_t valuesize) {

	// store the value, handle NULL values too
	if (value) {
		if (pvt->copyrefs) {
			var->value.stringval=charstring::duplicate(value);
		} else {
			var->value.stringval=(char *)value;
		}
		var->valuesize=valuesize;
		var->type=BINDVARTYPE_STRING;
	} else {
		var->type=BINDVARTYPE_NULL;
	}
}

void sqlrcursor::integerVar(bindvar *var, const char *variable, int64_t value) {
	var->type=BINDVARTYPE_INTEGER;
	var->value.integerval=value;
}

void sqlrcursor::doubleVar(bindvar *var, const char *variable, double value,
					uint32_t precision, uint32_t scale) {
	var->type=BINDVARTYPE_DOUBLE;
	var->value.doubleval.value=value;
	var->value.doubleval.precision=precision;
	var->value.doubleval.scale=scale;
}

void sqlrcursor::dateVar(bindvar *var, const char *variable,
				int16_t year, int16_t month, int16_t day,
				int16_t hour, int16_t minute, int16_t second,
				int32_t microsecond, const char *tz) {
	var->type=BINDVARTYPE_DATE;
	var->value.dateval.year=year;
	var->value.dateval.month=month;
	var->value.dateval.day=day;
	var->value.dateval.hour=hour;
	var->value.dateval.minute=minute;
	var->value.dateval.second=second;
	var->value.dateval.microsecond=microsecond;
	if (pvt->copyrefs) {
		var->value.dateval.tz=charstring::duplicate(tz);
	} else {
		var->value.dateval.tz=(char *)tz;
	}
}

void sqlrcursor::lobVar(bindvar *var, const char *variable,
			const char *value, uint32_t size, bindvartype_t type) {

	// Store the value, handle NULL values too.
	// For LOB's empty strings are handled as NULL's as well, this is
	// probably not right, but I can't get empty string lob binds to work.
	if (value && size>0) {
		if (pvt->copyrefs) {
			var->value.lobval=new char[size];
			bytestring::copy(var->value.lobval,value,size);
		} else {
			var->value.lobval=(char *)value;
		}
		var->valuesize=size;
		var->type=type;
	} else {
		var->type=BINDVARTYPE_NULL;
	}
}

bindvar *sqlrcursor::findVar(const char *variable,
				dynamicarray<bindvar> *vars) {
	for (uint16_t i=0; i<vars->getLength(); i++) {
		if (!charstring::compare((*vars)[i].variable,variable)) {
			return &((*vars)[i]);
		}
	}
	return NULL;
}

void sqlrcursor::initVar(bindvar *var, const char *variable, bool preexisting) {

	// clear any old variable name that was stored and assign the new 
	// variable name also clear any old value that was stored in this 
	// variable
	if (pvt->copyrefs) {
		if (preexisting) {
			delete[] var->variable;
			if (var->type==BINDVARTYPE_STRING) {
				delete[] var->value.stringval;
			} else if (var->type==BINDVARTYPE_BLOB ||
					var->type==BINDVARTYPE_CLOB) {
				delete[] var->value.lobval;
			}
		}
		var->variable=charstring::duplicate(variable);
	} else {
		var->variable=(char *)variable;
	}

	var->substituted=false;
	var->donesubstituting=false;
}

void sqlrcursor::defineOutputBindString(const char *variable,
						uint32_t length) {
	defineOutputBindGeneric(variable,BINDVARTYPE_STRING,length);
}

void sqlrcursor::defineOutputBindInteger(const char *variable) {
	defineOutputBindGeneric(variable,BINDVARTYPE_INTEGER,sizeof(int64_t));
}

void sqlrcursor::defineOutputBindDouble(const char *variable) {
	defineOutputBindGeneric(variable,BINDVARTYPE_DOUBLE,sizeof(double));
}

void sqlrcursor::defineOutputBindDate(const char *variable) {
	defineOutputBindGeneric(variable,BINDVARTYPE_DATE,sizeof(double));
}

void sqlrcursor::defineOutputBindBlob(const char *variable) {
	defineOutputBindGeneric(variable,BINDVARTYPE_BLOB,0);
}

void sqlrcursor::defineOutputBindClob(const char *variable) {
	defineOutputBindGeneric(variable,BINDVARTYPE_CLOB,0);
}

void sqlrcursor::defineOutputBindCursor(const char *variable) {
	defineOutputBindGeneric(variable,BINDVARTYPE_CURSOR,0);
}

void sqlrcursor::defineOutputBindGeneric(const char *variable,
				bindvartype_t type, uint32_t valuesize) {

	if (charstring::isNullOrEmpty(variable)) {
		return;
	}

	bindvar	*bv=findVar(variable,pvt->outbindvars);
	bool	preexisting=true;
	if (!bv) {
		bv=&(*pvt->outbindvars)[pvt->outbindvars->getLength()];
		preexisting=false;
		pvt->dirtybinds=true;
	}

	// clean up old values and set new values
	if (preexisting) {
		if (bv->type==BINDVARTYPE_STRING) {
			delete[] bv->value.stringval;
		} else if (bv->type==BINDVARTYPE_BLOB ||
				bv->type==BINDVARTYPE_CLOB) {
			delete[] bv->value.lobval;
		}
	}
	if (pvt->copyrefs) {
		if (preexisting) {
			delete[] bv->variable;
		}
		bv->variable=charstring::duplicate(variable);
	} else {
		bv->variable=(char *)variable;
	}
	bv->type=type;
	if (bv->type==BINDVARTYPE_STRING) {
		bv->value.stringval=NULL;
	} else if (bv->type==BINDVARTYPE_BLOB || bv->type==BINDVARTYPE_CLOB) {
		bv->value.lobval=NULL;
	}
	bv->valuesize=valuesize;
	bv->resultvaluesize=0;
	bv->send=true;
}

const char *sqlrcursor::getOutputBindString(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->outbindvars)[i].variable,variable) &&
				(*pvt->outbindvars)[i].type==BINDVARTYPE_STRING) {
				return (*pvt->outbindvars)[i].value.stringval;
			}
		}
	}
	return NULL;
}

uint32_t sqlrcursor::getOutputBindLength(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->outbindvars)[i].variable,variable)) {
				return (*pvt->outbindvars)[i].resultvaluesize;
			}
		}
	}
	return 0;
}

const char *sqlrcursor::getOutputBindBlob(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->outbindvars)[i].variable,variable) &&
				(*pvt->outbindvars)[i].type==BINDVARTYPE_BLOB) {
				return (*pvt->outbindvars)[i].value.lobval;
			}
		}
	}
	return NULL;
}

const char *sqlrcursor::getOutputBindClob(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->outbindvars)[i].variable,variable) &&
				(*pvt->outbindvars)[i].type==BINDVARTYPE_CLOB) {
				return (*pvt->outbindvars)[i].value.lobval;
			}
		}
	}
	return NULL;
}

int64_t sqlrcursor::getOutputBindInteger(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->outbindvars)[i].variable,variable) &&
				(*pvt->outbindvars)[i].type==BINDVARTYPE_INTEGER) {
				return (*pvt->outbindvars)[i].value.integerval;
			}
		}
	}
	return -1;
}

double sqlrcursor::getOutputBindDouble(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->outbindvars)[i].variable,variable) &&
				(*pvt->outbindvars)[i].type==BINDVARTYPE_DOUBLE) {
				return (*pvt->outbindvars)[i].value.doubleval.value;
			}
		}
	}
	return -1.0;
}

bool sqlrcursor::getOutputBindDate(const char *variable,
			int16_t *year, int16_t *month, int16_t *day,
			int16_t *hour, int16_t *minute, int16_t *second,
			int32_t *microsecond, const char **tz) {

	if (variable) {
		for (uint64_t i=0; i<pvt->outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->outbindvars)[i].variable,variable) &&
				(*pvt->outbindvars)[i].type==BINDVARTYPE_DATE) {
				*year=(*pvt->outbindvars)[i].value.dateval.year;
				*month=(*pvt->outbindvars)[i].value.dateval.month;
				*day=(*pvt->outbindvars)[i].value.dateval.day;
				*hour=(*pvt->outbindvars)[i].value.dateval.hour;
				*minute=(*pvt->outbindvars)[i].value.dateval.minute;
				*second=(*pvt->outbindvars)[i].value.dateval.second;
				*microsecond=(*pvt->outbindvars)[i].
						value.dateval.microsecond;
				*tz=(*pvt->outbindvars)[i].value.dateval.tz;
				return true;
			}
		}
	}
	return false;
}

sqlrcursor *sqlrcursor::getOutputBindCursor(const char *variable) {
	return getOutputBindCursor(variable,false);
}

sqlrcursor *sqlrcursor::getOutputBindCursor(const char *variable,
							bool copyrefs) {

	if (!outputBindCursorIdIsValid(variable)) {
		return NULL;
	}
	uint16_t	bindcursorid=getOutputBindCursorId(variable);
	sqlrcursor	*bindcursor=new sqlrcursor(pvt->sqlrc,copyrefs);
	bindcursor->attachToBindCursor(bindcursorid);
	return bindcursor;
}

bool sqlrcursor::outputBindCursorIdIsValid(const char *variable) {
	if (variable) {
		for (uint64_t i=0; i<pvt->outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->outbindvars)[i].variable,variable)) {
				return true;
			}
		}
	}
	return false;
}

uint16_t sqlrcursor::getOutputBindCursorId(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->outbindvars)[i].variable,variable)) {
				return (*pvt->outbindvars)[i].value.cursorid;
			}
		}
	}
	return 0;
}

void sqlrcursor::validateBinds() {
	pvt->validatebinds=true;
}

bool sqlrcursor::validBind(const char *variable) {
	performSubstitutions();
	validateBindsInternal();
	for (uint64_t in=0; in<pvt->inbindvars->getLength(); in++) {
		if (!charstring::compare(
			(*pvt->inbindvars)[in].variable,variable)) {
			return (*pvt->inbindvars)[in].send;
		}
	}
	for (uint64_t out=0; out<pvt->outbindvars->getLength(); out++) {
		if (!charstring::compare(
			(*pvt->outbindvars)[out].variable,variable)) {
			return (*pvt->outbindvars)[out].send;
		}
	}
	return false;
}

bool sqlrcursor::executeQuery() {

	if (!pvt->queryptr) {
		setError("No query to execute.");
		return false;
	}

	performSubstitutions();

	// validate the bind variables
	if (pvt->validatebinds) {
		validateBindsInternal();
	}
		
	// run the query
	bool	retval=runQuery(pvt->queryptr);

	// set up to re-execute the same query if executeQuery is called
	// again before calling prepareQuery
	pvt->reexecute=true;

	return retval;
}

void sqlrcursor::performSubstitutions() {

	if (!pvt->subvars->getLength() || !pvt->dirtysubs) {
		return;
	}

	// perform substitutions
	stringbuffer	container;
	const char	*ptr=pvt->queryptr;
	bool		found=false;
	bool		inquotes=false;
	bool		inbraces=false;
	int		len=0;
	stringbuffer	*braces=NULL;

	// iterate through the string
	while (*ptr) {
	
		// figure out whether we're inside a quoted 
		// string or not
		if (*ptr=='\'' && *(ptr-1)!='\\') {
			if (inquotes) {
				inquotes=false;
			} else {
				inquotes=true;
			}
		}
	
		// if we find an open-brace then start 
		// sending to a new buffer
		if (*ptr=='[' && !inbraces && !inquotes) {
			braces=new stringbuffer();
			inbraces=true;
			ptr++;
		}
	
		// if we find a close-brace then process 
		// the brace buffer
		if (*ptr==']' && inbraces && !inquotes) {
	
			// look for an = sign, skipping whitespace
			const char	*bptr=braces->getString();
			while (*bptr && (*bptr==' ' || 
				*bptr=='	' || *bptr=='\n')) {
				bptr++;
			}
	
			if (*bptr=='=') {
				// if we find an equals sign first 
				// then process the rest of the buffer
				bptr++;
	
				// skip whitespace
				while (*bptr && (*bptr==' ' || 
					*bptr=='	' || 
				 	*bptr=='\n')) {
					bptr++;
				}
	
				// if the remaining contents of the 
				// buffer are '' or nothing then we 
				// must have an ='' or just an = with 
				// some whitespace, replace this
				// with "is NULL" otherwise, just write
				// out the contents of the buffer
				if (!bptr || 
					(bptr &&
					!charstring::compare(bptr,
							"''"))) {
					container.append(" is NULL ");
				} else {
					container.append(
						braces->getString());
				}
			} else {
				// if we don't find an equals sign, 
				// then write the contents out directly
				container.append(braces->getString());
			}
			delete braces;
			inbraces=false;
			ptr++;
		}
	
		// if we encounter $(....) then replace the 
		// variable within
		if ((*ptr)=='$' && (*(ptr+1))=='(') {
	
			// first iterate through the arrays passed in
			found=false;
			for (uint64_t i=0;
				i<pvt->subvars->getLength() && !found; i++) {

	
				// if we find a match, write the 
				// value to the container and skip 
				// past the $(variable)
				len=charstring::length(
						(*pvt->subvars)[i].variable);
				if (!(*pvt->subvars)[i].donesubstituting &&
					!charstring::compare((ptr+2),
						(*pvt->subvars)[i].variable,len) &&
						(*(ptr+2+len))==')') {
	
					if (inbraces) {
						performSubstitution(
							braces,i);
					} else {
						performSubstitution(
							&container,i);
					}
					ptr=ptr+3+len;
					found=true;
				}
			}
	
			// if the variable wasn't found, then 
			// just write the $(
			if (!found) {
				if (inbraces) {
					braces->append("$(");
				} else {
					container.append("$(");
				}
				ptr=ptr+2;
			}
	
		} else {
	
			// print out the current character and proceed
			if (inbraces) {
				braces->append(*ptr);
			} else {
				container.append(*ptr);
			}
			ptr++;
		}
	}

	// mark all vars that were substituted in as "done" so the next time
	// this method gets called, they won't be processed.
	for (uint64_t i=0; i<pvt->subvars->getLength(); i++) {
		(*pvt->subvars)[i].donesubstituting=(*pvt->subvars)[i].substituted;
	}

	delete[] pvt->querybuffer;
	pvt->querylen=container.getStringLength();
	pvt->querybuffer=container.detachString();
	pvt->queryptr=pvt->querybuffer;

	pvt->dirtysubs=false;
}

void sqlrcursor::validateBindsInternal() {

	if (!pvt->dirtybinds) {
		return;
	}

	// some useful variables
	const char	*ptr;
	const char	*start;
	const char	*after;
	bool		found;
	int		len;

	// check each input bind
	for (uint64_t in=0; in<pvt->inbindvars->getLength(); in++) {

		// don't check bind-by-position variables
		len=charstring::length((*pvt->inbindvars)[in].variable);
		if (charstring::isInteger((*pvt->inbindvars)[in].variable,len)) {
			continue;
		}

		found=false;
		start=pvt->queryptr+1;

		// there may be more than 1 match for the variable name as in
		// "select * from table where table_name=:table_name", both
		// table_name's would match, but only the second is a bind
		// variable
		while ((ptr=charstring::findFirst(start,
					(*pvt->inbindvars)[in].variable))) {

			// for a match to be a bind variable, it must be 
			// preceded by a colon or at-sign and can't be followed
			// by an alphabet character, number or underscore
			after=ptr+len;
			if ((*(ptr-1)==':' || *(ptr-1)=='@') && *after!='_' &&
				!(*(after)>='a' && *(after)<='z') &&
				!(*(after)>='A' && *(after)<='Z') &&
				!(*(after)>='0' && *(after)<='9')) {
				found=true;
				break;
			} else {
				// jump past this instance to look for the
				// next one
				start=ptr+len;
			}
		}

		(*pvt->inbindvars)[in].send=found;
	}

	// check each output bind
	for (uint64_t out=0; out<pvt->outbindvars->getLength(); out++) {

		// don't check bind-by-position variables
		len=charstring::length((*pvt->outbindvars)[out].variable);
		if (charstring::isInteger((*pvt->outbindvars)[out].variable,len)) {
			continue;
		}

		found=false;
		start=pvt->queryptr+1;

		// there may be more than 1 match for the variable name as in
		// "select * from table where table_name=:table_name", both
		// table_name's would match, but only 1 is correct
		while ((ptr=charstring::findFirst(start,
					(*pvt->outbindvars)[out].variable))) {

			// for a match to be a bind variable, it must be 
			// preceded by a colon and can't be followed by an
			// alphabet character, number or underscore
			after=ptr+len;
			if (*(ptr-1)==':' && *after!='_' &&
				!(*(after)>='a' && *(after)<='z') &&
				!(*(after)>='A' && *(after)<='Z') &&
				!(*(after)>='0' && *(after)<='9')) {
				found=true;
				break;
			} else {
				// jump past this instance to look for the
				// next one
				start=ptr+len;
			}
		}

		(*pvt->outbindvars)[out].send=found;
	}
}

void sqlrcursor::performSubstitution(stringbuffer *buffer, uint16_t which) {

	if ((*pvt->subvars)[which].type==BINDVARTYPE_STRING) {
		buffer->append((*pvt->subvars)[which].value.stringval);
	} else if ((*pvt->subvars)[which].type==BINDVARTYPE_INTEGER) {
		buffer->append((*pvt->subvars)[which].value.integerval);
	} else if ((*pvt->subvars)[which].type==BINDVARTYPE_DOUBLE) {
		buffer->append((*pvt->subvars)[which].value.doubleval.value,
			(*pvt->subvars)[which].value.doubleval.precision,
			(*pvt->subvars)[which].value.doubleval.scale);
	}
	(*pvt->subvars)[which].substituted=true;
}

bool sqlrcursor::runQuery(const char *query) {

	// send the query
	if (sendQueryInternal(query)) {

		sendInputBinds();
		sendOutputBinds();
		sendGetColumnInfo();

		pvt->sqlrc->flushWriteBuffer();

		if (pvt->rsbuffersize) {
			if (processResultSet(false,pvt->rsbuffersize-1)) {
				return true;
			}
		} else {
			if (processResultSet(true,0)) {
				return true;
			}
		}
	}
	return false;
}

bool sqlrcursor::sendQueryInternal(const char *query) {

	// if the first 8 characters of the query are "-- debug" followed
	// by a return, then set debugging on
	if (!charstring::compare(query,"-- debug\n",9)) {
		pvt->sqlrc->debugOn();
	}

	if (!pvt->endofresultset) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!pvt->sqlrc->openSession()) {
		return false;
	}

	pvt->cached=false;
	pvt->endofresultset=false;

	// send the query to the server.
	if (!pvt->reexecute) {

		// tell the server we're sending a query
		pvt->sqlrc->cs()->write((uint16_t)NEW_QUERY);

		// tell the server whether we'll need a cursor or not
		sendCursorStatus();

		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Sending Client Info:");
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPrint("Length: ");
			pvt->sqlrc->debugPrint((int64_t)pvt->sqlrc->clientinfolen());
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPrint(pvt->sqlrc->clientinfo());
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPreEnd();
		}

		// send the client info
		// FIXME: arguably this should be its own command
		pvt->sqlrc->cs()->write(pvt->sqlrc->clientinfolen());
		pvt->sqlrc->cs()->write(pvt->sqlrc->clientinfo(),pvt->sqlrc->clientinfolen());

		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Sending Query:");
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPrint("Length: ");
			pvt->sqlrc->debugPrint((int64_t)pvt->querylen);
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPrint(query);
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPreEnd();
		}

		// send the query
		pvt->sqlrc->cs()->write(pvt->querylen);
		pvt->sqlrc->cs()->write(query,pvt->querylen);

	} else {

		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Requesting re-execution of ");
			pvt->sqlrc->debugPrint("previous query.");
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPrint("Requesting Cursor: ");
			pvt->sqlrc->debugPrint((int64_t)pvt->cursorid);
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPreEnd();
		}

		// tell the server we're sending a query
		pvt->sqlrc->cs()->write((uint16_t)REEXECUTE_QUERY);

		// send the cursor id to the server
		pvt->sqlrc->cs()->write(pvt->cursorid);
	}

	return true;
}

void sqlrcursor::sendCursorStatus() {

	if (pvt->havecursorid) {

		// tell the server we already have a cursor
		pvt->sqlrc->cs()->write((uint16_t)DONT_NEED_NEW_CURSOR);

		// send the cursor id to the server
		pvt->sqlrc->cs()->write(pvt->cursorid);

		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Requesting Cursor: ");
			pvt->sqlrc->debugPrint((int64_t)pvt->cursorid);
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPreEnd();
		}

	} else {

		// tell the server we need a cursor
		pvt->sqlrc->cs()->write((uint16_t)NEED_NEW_CURSOR);

		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Requesting a new cursor.\n");
			pvt->sqlrc->debugPreEnd();
		}
	}
}

void sqlrcursor::sendInputBinds() {

	// index
	uint16_t	i=0;

	// count number of vars to send
	uint16_t	count=pvt->inbindvars->getLength();
	for (i=0; i<count; i++) {
		if (!(*pvt->inbindvars)[i].send) {
			count--;
		}
	}

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Sending ");
		pvt->sqlrc->debugPrint((int64_t)count);
		pvt->sqlrc->debugPrint(" Input Bind Variables:\n");
		pvt->sqlrc->debugPreEnd();
	}

	// write the input bind variables/values to the server.
	pvt->sqlrc->cs()->write(count);
	uint16_t	size;
	i=0;
	while (i<count) {

		// don't send anything if the send flag is turned off
		if (!(*pvt->inbindvars)[i].send) {
			continue;
		}

		// send the variable
		size=charstring::length((*pvt->inbindvars)[i].variable);
		pvt->sqlrc->cs()->write(size);
		pvt->sqlrc->cs()->write((*pvt->inbindvars)[i].variable,(size_t)size);
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint((*pvt->inbindvars)[i].variable);
			pvt->sqlrc->debugPrint("(");
			pvt->sqlrc->debugPrint((int64_t)size);
		}

		// send the type
		pvt->sqlrc->cs()->write((uint16_t)(*pvt->inbindvars)[i].type);

		// send the value
		if ((*pvt->inbindvars)[i].type==BINDVARTYPE_NULL) {

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPrint(":NULL)\n");
				pvt->sqlrc->debugPreEnd();
			}

		} else if ((*pvt->inbindvars)[i].type==BINDVARTYPE_STRING) {

			pvt->sqlrc->cs()->write((*pvt->inbindvars)[i].valuesize);
			if ((*pvt->inbindvars)[i].valuesize>0) {
				pvt->sqlrc->cs()->write((*pvt->inbindvars)[i].
							value.stringval,
					(size_t)(*pvt->inbindvars)[i].valuesize);
			}

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPrint(":STRING)=");
				pvt->sqlrc->debugPrint((*pvt->inbindvars)[i].
							value.stringval);
				pvt->sqlrc->debugPrint("(");
				pvt->sqlrc->debugPrint((int64_t)(*pvt->inbindvars)[i].
								valuesize);
				pvt->sqlrc->debugPrint(")");
				pvt->sqlrc->debugPrint("\n");
				pvt->sqlrc->debugPreEnd();
			}

		} else if ((*pvt->inbindvars)[i].type==BINDVARTYPE_INTEGER) {

			pvt->sqlrc->cs()->write((uint64_t)(*pvt->inbindvars)[i].
							value.integerval);

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPrint(":LONG)=");
				pvt->sqlrc->debugPrint((int64_t)(*pvt->inbindvars)[i].
							value.integerval);
				pvt->sqlrc->debugPrint("\n");
				pvt->sqlrc->debugPreEnd();
			}

		} else if ((*pvt->inbindvars)[i].type==BINDVARTYPE_DOUBLE) {

			pvt->sqlrc->cs()->write((double)(*pvt->inbindvars)[i].value.
							doubleval.value);
			pvt->sqlrc->cs()->write((*pvt->inbindvars)[i].value.
							doubleval.precision);
			pvt->sqlrc->cs()->write((*pvt->inbindvars)[i].value.
							doubleval.scale);

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPrint(":DOUBLE)=");
				pvt->sqlrc->debugPrint((*pvt->inbindvars)[i].value.
							doubleval.value);
				pvt->sqlrc->debugPrint(":");
				pvt->sqlrc->debugPrint((int64_t)(*pvt->inbindvars)[i].
						value.doubleval.precision);
				pvt->sqlrc->debugPrint(",");
				pvt->sqlrc->debugPrint((int64_t)(*pvt->inbindvars)[i].
						value.doubleval.scale);
				pvt->sqlrc->debugPrint("\n");
				pvt->sqlrc->debugPreEnd();
			}

		} else if ((*pvt->inbindvars)[i].type==BINDVARTYPE_DATE) {

			pvt->sqlrc->cs()->write((uint16_t)
					(*pvt->inbindvars)[i].value.dateval.year);
			pvt->sqlrc->cs()->write((uint16_t)
					(*pvt->inbindvars)[i].value.dateval.month);
			pvt->sqlrc->cs()->write((uint16_t)
					(*pvt->inbindvars)[i].value.dateval.day);
			pvt->sqlrc->cs()->write((uint16_t)
					(*pvt->inbindvars)[i].value.dateval.hour);
			pvt->sqlrc->cs()->write((uint16_t)
					(*pvt->inbindvars)[i].value.dateval.minute);
			pvt->sqlrc->cs()->write((uint16_t)
					(*pvt->inbindvars)[i].value.dateval.second);
			pvt->sqlrc->cs()->write((uint32_t)
					(*pvt->inbindvars)[i].value.
							dateval.microsecond);
			pvt->sqlrc->cs()->write((uint16_t)
					charstring::length(
					(*pvt->inbindvars)[i].value.dateval.tz));
			pvt->sqlrc->cs()->write((*pvt->inbindvars)[i].value.dateval.tz);

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPrint(":DATE)=");
				pvt->sqlrc->debugPrint((int64_t)
					(*pvt->inbindvars)[i].value.dateval.year);
				pvt->sqlrc->debugPrint("-");
				pvt->sqlrc->debugPrint((int64_t)
					(*pvt->inbindvars)[i].value.dateval.month);
				pvt->sqlrc->debugPrint("-");
				pvt->sqlrc->debugPrint((int64_t)
					(*pvt->inbindvars)[i].value.dateval.day);
				pvt->sqlrc->debugPrint(" ");
				pvt->sqlrc->debugPrint((int64_t)
					(*pvt->inbindvars)[i].value.dateval.hour);
				pvt->sqlrc->debugPrint(":");
				pvt->sqlrc->debugPrint((int64_t)
					(*pvt->inbindvars)[i].value.dateval.minute);
				pvt->sqlrc->debugPrint(":");
				pvt->sqlrc->debugPrint((int64_t)
					(*pvt->inbindvars)[i].value.dateval.second);
				pvt->sqlrc->debugPrint(":");
				pvt->sqlrc->debugPrint((int64_t)
					(*pvt->inbindvars)[i].value.
						dateval.microsecond);
				pvt->sqlrc->debugPrint(" ");
				pvt->sqlrc->debugPrint(
					(*pvt->inbindvars)[i].value.dateval.tz);
				pvt->sqlrc->debugPrint("\n");
				pvt->sqlrc->debugPreEnd();
			}

		} else if ((*pvt->inbindvars)[i].type==BINDVARTYPE_BLOB ||
				(*pvt->inbindvars)[i].type==BINDVARTYPE_CLOB) {

			pvt->sqlrc->cs()->write((*pvt->inbindvars)[i].valuesize);
			if ((*pvt->inbindvars)[i].valuesize>0) {
				pvt->sqlrc->cs()->write((*pvt->inbindvars)[i].
					value.lobval,
					(size_t)(*pvt->inbindvars)[i].valuesize);
			}

			if (pvt->sqlrc->debug()) {
				if ((*pvt->inbindvars)[i].type==
							BINDVARTYPE_BLOB) {
					pvt->sqlrc->debugPrint(":BLOB)=");
					pvt->sqlrc->debugPrintBlob(
						(*pvt->inbindvars)[i].value.lobval,
						(*pvt->inbindvars)[i].valuesize);
				} else if ((*pvt->inbindvars)[i].type==
							BINDVARTYPE_CLOB) {
					pvt->sqlrc->debugPrint(":CLOB)=");
					pvt->sqlrc->debugPrintClob(
						(*pvt->inbindvars)[i].value.lobval,
						(*pvt->inbindvars)[i].valuesize);
				}
				pvt->sqlrc->debugPrint("(");
				pvt->sqlrc->debugPrint((int64_t)(*pvt->inbindvars)[i].
								valuesize);
				pvt->sqlrc->debugPrint(")");
				pvt->sqlrc->debugPrint("\n");
				pvt->sqlrc->debugPreEnd();
			}
		}

		i++;
	}
}

void sqlrcursor::sendOutputBinds() {

	// index
	uint16_t	i=0;

	// count number of vars to send
	uint16_t	count=pvt->outbindvars->getLength();
	for (i=0; i<count; i++) {
		if (!(*pvt->outbindvars)[i].send) {
			count--;
		}
	}

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Sending ");
		pvt->sqlrc->debugPrint((int64_t)count);
		pvt->sqlrc->debugPrint(" Output Bind Variables:\n");
		pvt->sqlrc->debugPreEnd();
	}

	// write the output bind variables to the server.
	pvt->sqlrc->cs()->write(count);
	uint16_t	size;
	i=0;
	while (i<count) {

		// don't send anything if the send flag is turned off
		if (!(*pvt->outbindvars)[i].send) {
			continue;
		}

		// send the variable, type and size that the buffer needs to be
		size=charstring::length((*pvt->outbindvars)[i].variable);
		pvt->sqlrc->cs()->write(size);
		pvt->sqlrc->cs()->write((*pvt->outbindvars)[i].variable,(size_t)size);
		pvt->sqlrc->cs()->write((uint16_t)(*pvt->outbindvars)[i].type);
		if ((*pvt->outbindvars)[i].type==BINDVARTYPE_STRING ||
			(*pvt->outbindvars)[i].type==BINDVARTYPE_BLOB ||
			(*pvt->outbindvars)[i].type==BINDVARTYPE_CLOB ||
			(*pvt->outbindvars)[i].type==BINDVARTYPE_NULL) {
			pvt->sqlrc->cs()->write((*pvt->outbindvars)[i].valuesize);
		}

		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint((*pvt->outbindvars)[i].variable);
			const char	*bindtype=NULL;
			switch ((*pvt->outbindvars)[i].type) {
				case BINDVARTYPE_NULL:
					bindtype="(NULL)";
					break;
				case BINDVARTYPE_STRING:
					bindtype="(STRING)";
					break;
				case BINDVARTYPE_INTEGER:
					bindtype="(INTEGER)";
					break;
				case BINDVARTYPE_DOUBLE:
					bindtype="(DOUBLE)";
					break;
				case BINDVARTYPE_DATE:
					bindtype="(DATE)";
					break;
				case BINDVARTYPE_BLOB:
					bindtype="(BLOB)";
					break;
				case BINDVARTYPE_CLOB:
					bindtype="(CLOB)";
					break;
				case BINDVARTYPE_CURSOR:
					bindtype="(CURSOR)";
					break;
			}
			pvt->sqlrc->debugPrint(bindtype);
			if ((*pvt->outbindvars)[i].type==BINDVARTYPE_STRING ||
				(*pvt->outbindvars)[i].type==BINDVARTYPE_BLOB ||
				(*pvt->outbindvars)[i].type==BINDVARTYPE_CLOB ||
				(*pvt->outbindvars)[i].type==BINDVARTYPE_NULL) {
				pvt->sqlrc->debugPrint("(");
				pvt->sqlrc->debugPrint((int64_t)(*pvt->outbindvars)[i].
								valuesize);
				pvt->sqlrc->debugPrint(")");
			}
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPreEnd();
		}

		i++;
	}
}

void sqlrcursor::sendGetColumnInfo() {

	if (pvt->sendcolumninfo==SEND_COLUMN_INFO) {
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Send Column Info: yes\n");
			pvt->sqlrc->debugPreEnd();
		}
		pvt->sqlrc->cs()->write((uint16_t)SEND_COLUMN_INFO);
	} else {
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Send Column Info: no\n");
			pvt->sqlrc->debugPreEnd();
		}
		pvt->sqlrc->cs()->write((uint16_t)DONT_SEND_COLUMN_INFO);
	}
}

bool sqlrcursor::processResultSet(bool getallrows, uint64_t rowtoget) {

	// start caching the result set
	if (pvt->cacheon) {
		startCaching();
	}

	// parse the columninfo and data
	bool	success=true;

	// skip and fetch here if we're not reading from a cached result set
	// this way, everything gets done in 1 round trip
	if (!pvt->cachesource) {
		success=skipAndFetch(getallrows,pvt->firstrowindex+rowtoget);
	}

	// check for an error
	if (success) {

		uint16_t	err=getErrorStatus();
		if (err!=NO_ERROR_OCCURRED) {

			// if there was a timeout, then end
			// the session and bail immediately
			if (err==TIMEOUT_GETTING_ERROR_STATUS) {
				pvt->sqlrc->endSession();
				return false;
			}

			// otherwise, get the error from the server
			getErrorFromServer();

			// don't get the cursor if the error was that there
			// were no cursors available
			if (pvt->errorno!=SQLR_ERROR_NOCURSORS) {
				getCursorId();
			}

			// if we need to disconnect then end the session
			if (err==ERROR_OCCURRED_DISCONNECT) {
				pvt->sqlrc->endSession();
			}
			return false;
		}
	}

	// get data back from the server
	if (success && ((pvt->cachesource && pvt->cachesourceind) ||
			((!pvt->cachesource && !pvt->cachesourceind)  && 
				(success=getCursorId()) && 
				(success=getSuspended()))) &&
			(success=parseColumnInfo()) && 
			(success=parseOutputBinds())) {

		// skip and fetch here if we're reading from a cached result set
		if (pvt->cachesource) {
			success=skipAndFetch(getallrows,pvt->firstrowindex+rowtoget);
		}

		// parse the data
		if (success) {
			success=parseData();
		}
	}

	// if success is false, then some kind of network error occurred,
	// end the session
	if (!success) {
		clearResultSet();
		pvt->sqlrc->endSession();
	}
	return success;
}

bool sqlrcursor::skipAndFetch(bool getallrows, uint64_t rowtoget) {

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Skipping and Fetching\n");
		if (!getallrows) {
			pvt->sqlrc->debugPrint("	row to get: ");
			pvt->sqlrc->debugPrint((int64_t)rowtoget);
			pvt->sqlrc->debugPrint("\n");
		}
		pvt->sqlrc->debugPreEnd();
	}

	// if we're stepping through the result set, we can possibly 
	// skip a big chunk of it...
	if (!skipRows(getallrows,rowtoget)) {
		return false;
	}

	// tell the connection how many rows to send
	fetchRows();

	pvt->sqlrc->flushWriteBuffer();
	return true;
}

void sqlrcursor::fetchRows() {

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Fetching ");
		pvt->sqlrc->debugPrint((int64_t)pvt->rsbuffersize);
		pvt->sqlrc->debugPrint(" rows\n");
		pvt->sqlrc->debugPreEnd();
	}

	// if we're reading from a cached result set, do nothing
	if (pvt->cachesource && pvt->cachesourceind) {
		return;
	}

	// otherwise, send to the connection the number of rows to send back
	pvt->sqlrc->cs()->write(pvt->rsbuffersize);
}

bool sqlrcursor::skipRows(bool getallrows, uint64_t rowtoget) {

	// if we're reading from a cached result set we have to manually skip
	if (pvt->cachesource && pvt->cachesourceind) {

		// skip to the next block of rows
		if (getallrows) {
			return true;
		} else {
			pvt->rowcount=rowtoget-(rowtoget%pvt->rsbuffersize);
		}

		// get the row offset from the index
		pvt->cachesourceind->setPositionRelativeToBeginning(
				13+sizeof(int64_t)+(pvt->rowcount*sizeof(int64_t)));
		int64_t	rowoffset;
		if (pvt->cachesourceind->read(&rowoffset)!=sizeof(int64_t)) {
			setError("The cache file index appears to be corrupt.");
			return false;
		}

		// skip to that offset in the cache file
		pvt->cachesource->setPositionRelativeToBeginning(rowoffset);
		return true;
	}

	// calculate how many rows to skip unless we're buffering the entire
	// result set or caching the result set
	uint64_t	skip=0;
	if (pvt->rsbuffersize && !pvt->cachedest && !getallrows) {
		skip=(rowtoget-(rowtoget%pvt->rsbuffersize))-pvt->rowcount; 
		pvt->rowcount=pvt->rowcount+skip;
	}
	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Skipping ");
		pvt->sqlrc->debugPrint((int64_t)skip);
		pvt->sqlrc->debugPrint(" rows\n");
		pvt->sqlrc->debugPreEnd();
	}

	// if we're reading from a connection, send the connection the 
	// number of rows to skip
	pvt->sqlrc->cs()->write(skip);
	return true;
}

uint16_t sqlrcursor::getErrorStatus() {

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Checking For An Error...\n");
		pvt->sqlrc->debugPreEnd();
	}

	// get a flag indicating whether there's been an error or not
	uint16_t	err;
	int32_t	result=getShort(&err,pvt->sqlrc->responsetimeoutsec(),
					pvt->sqlrc->responsetimeoutusec());
	if (result==RESULT_TIMEOUT) {
		setError("Timeout while determining whether "
				"an error occurred or not.\n");
		return TIMEOUT_GETTING_ERROR_STATUS;
	} else if (result!=sizeof(uint16_t)) {
		setError("Failed to determine whether an "
				"error occurred or not.\n "
				"A network error may have ocurred.");
		return ERROR_OCCURRED;
	}

	if (err==NO_ERROR_OCCURRED) {
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("none.\n");
			pvt->sqlrc->debugPreEnd();
		}
		cacheNoError();
		return NO_ERROR_OCCURRED;
	}

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("error!!!\n");
		pvt->sqlrc->debugPreEnd();
	}
	return err;
}

bool sqlrcursor::getCursorId() {

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Getting Cursor ID...\n");
		pvt->sqlrc->debugPreEnd();
	}
	if (pvt->sqlrc->cs()->read(&pvt->cursorid)!=sizeof(uint16_t)) {
		if (!pvt->error) {
			char	*err=error::getErrorString();
			stringbuffer	errstr;
			errstr.append("Failed to get a cursor id.\n "
					"A network error may have ocurred. ");
			errstr.append(err);
			setError(errstr.getString());
			delete[] err;
		}
		return false;
	}
	pvt->havecursorid=true;
	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Cursor ID: ");
		pvt->sqlrc->debugPrint((int64_t)pvt->cursorid);
		pvt->sqlrc->debugPrint("\n");
		pvt->sqlrc->debugPreEnd();
	}
	return true;
}

bool sqlrcursor::getSuspended() {

	// see if the result set of that cursor is actually suspended
	uint16_t	suspendedresultset;
	if (pvt->sqlrc->cs()->read(&suspendedresultset)!=sizeof(uint16_t)) {
		setError("Failed to determine whether "
			"the session was suspended or not.\n "
			"A network error may have ocurred.");
		return false;
	}

	if (suspendedresultset==SUSPENDED_RESULT_SET) {

		// If it was suspended the server will send the index of the 
		// last row from the previous result set.
		// Initialize firstrowindex and rowcount from this index.
		if (pvt->sqlrc->cs()->read(&pvt->firstrowindex)!=sizeof(uint64_t)) {
			setError("Failed to get the index of the "
				"last row of a previously suspended result "
				"set.\n A network error may have ocurred.");
			return false;
		}
		pvt->rowcount=pvt->firstrowindex+1;
	
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Previous result set was ");
	       		pvt->sqlrc->debugPrint("suspended at row index: ");
			pvt->sqlrc->debugPrint((int64_t)pvt->firstrowindex);
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPreEnd();
		}

	} else {

		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Previous result set was ");
	       		pvt->sqlrc->debugPrint("not suspended.\n");
			pvt->sqlrc->debugPreEnd();
		}
	}
	return true;
}

bool sqlrcursor::parseColumnInfo() {

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Parsing Column Info\n");
		pvt->sqlrc->debugPrint("Actual row count: ");
		pvt->sqlrc->debugPreEnd();
	}

	// first get whether the server knows the total number of rows or not
	if (getShort(&pvt->knowsactualrows)!=sizeof(uint16_t)) {
		setError("Failed to get whether the server knows the number actual rows or not.\n A network error may have occurred.");
		return false;
	}

	// get the number of rows returned by the query
	if (pvt->knowsactualrows==ACTUAL_ROWS) {
		if (getLongLong(&pvt->actualrows)!=sizeof(uint64_t)) {
			setError("Failed to get the number of actual rows.\n A network error may have occurred.");
			return false;
		}
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint((int64_t)pvt->actualrows);
			pvt->sqlrc->debugPreEnd();
		}
	} else {
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("unknown");
			pvt->sqlrc->debugPreEnd();
		}
	}

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("\n");
		pvt->sqlrc->debugPrint("Affected row count: ");
		pvt->sqlrc->debugPreEnd();
	}

	// get whether the server knows the number of affected rows or not
	if (getShort(&pvt->knowsaffectedrows)!=sizeof(uint16_t)) {
		setError("Failed to get whether the server knows the number of affected rows or not.\n A network error may have occurred.");
		return false;
	}

	// get the number of rows affected by the query
	if (pvt->knowsaffectedrows==AFFECTED_ROWS) {
		if (getLongLong(&pvt->affectedrows)!=sizeof(uint64_t)) {
			setError("Failed to get the number of affected rows.\n A network error may have occurred.");
			return false;
		}
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint((int64_t)pvt->affectedrows);
			pvt->sqlrc->debugPreEnd();
		}
	} else {
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("unknown");
			pvt->sqlrc->debugPreEnd();
		}
	}

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("\n");
		pvt->sqlrc->debugPreEnd();
	}

	// get whether the server is sending column info or not
	if (getShort(&pvt->sentcolumninfo)!=sizeof(uint16_t)) {
		setError("Failed to get whether the server is sending column info or not.\n A network error may have occurred.");
		return false;
	}

	// get column count
	if (getLong(&pvt->colcount)!=sizeof(uint32_t)) {
		setError("Failed to get the column count.\n A network error may have occurred.");
		return false;
	}
	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Column count: ");
		pvt->sqlrc->debugPrint((int64_t)pvt->colcount);
		pvt->sqlrc->debugPrint("\n");
		pvt->sqlrc->debugPreEnd();
	}

	// we have to do this here even if we're not getting the column
	// descriptions because we are going to use the longdatatype member
	// variable no matter what
	createColumnBuffers();

	if (pvt->sendcolumninfo==SEND_COLUMN_INFO && 
			pvt->sentcolumninfo==SEND_COLUMN_INFO) {

		// get whether column types will be predefined id's or strings
		if (getShort(&pvt->columntypeformat)!=sizeof(uint16_t)) {
			setError("Failed to whether column types will be predefined id's or strings.\n A network error may have occurred.");
			return false;
		}

		// some useful variables
		uint16_t	length;
		column		*currentcol;

		// get the columninfo segment
		for (uint32_t i=0; i<pvt->colcount; i++) {
	
			// get the column name length
			if (getShort(&length)!=sizeof(uint16_t)) {
				setError("Failed to get the column name length.\n A network error may have occurred.");
				return false;
			}
	
			// which column to use
			currentcol=getColumnInternal(i);
	
			// get the column name
			currentcol->name=(char *)pvt->colstorage->allocate(length+1);
			if (getString(currentcol->name,length)!=length) {
				setError("Failed to get the column name.\n A network error may have occurred.");
				return false;
			}
			currentcol->name[length]='\0';

			// upper/lowercase column name if necessary
			if (pvt->colcase==UPPER_CASE) {
				charstring::upper(currentcol->name);
			} else if (pvt->colcase==LOWER_CASE) {
				charstring::lower(currentcol->name);
			}

			if (pvt->columntypeformat==COLUMN_TYPE_IDS) {

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
	
			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("\"");
				pvt->sqlrc->debugPrint(currentcol->name);
				pvt->sqlrc->debugPrint("\",");
				pvt->sqlrc->debugPrint("\"");
				if (pvt->columntypeformat!=COLUMN_TYPE_IDS) {
					pvt->sqlrc->debugPrint(
						currentcol->typestring);
				} else {
					pvt->sqlrc->debugPrint(datatypestring[
							currentcol->type]);
				}
				pvt->sqlrc->debugPrint("\", ");
				pvt->sqlrc->debugPrint((int64_t)currentcol->length);
				pvt->sqlrc->debugPrint(" (");
				pvt->sqlrc->debugPrint((int64_t)
							currentcol->precision);
				pvt->sqlrc->debugPrint(",");
				pvt->sqlrc->debugPrint((int64_t)currentcol->scale);
				pvt->sqlrc->debugPrint(") ");
				if (!currentcol->nullable) {
					pvt->sqlrc->debugPrint("NOT NULL ");
				}
				if (currentcol->primarykey) {
					pvt->sqlrc->debugPrint("Primary Key ");
				}
				if (currentcol->unique) {
					pvt->sqlrc->debugPrint("Unique ");
				}
				if (currentcol->partofkey) {
					pvt->sqlrc->debugPrint("Part of a Key ");
				}
				if (currentcol->unsignednumber) {
					pvt->sqlrc->debugPrint("Unsigned ");
				}
				if (currentcol->zerofill) {
					pvt->sqlrc->debugPrint("Zero Filled ");
				}
				if (currentcol->binary) {
					pvt->sqlrc->debugPrint("Binary ");
				}
				if (currentcol->autoincrement) {
					pvt->sqlrc->debugPrint("Auto-Increment ");
				}
				pvt->sqlrc->debugPrint("\n");
				pvt->sqlrc->debugPreEnd();
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
	if (!pvt->columns) {
		pvt->columns=new column[OPTIMISTIC_COLUMN_COUNT];
	}

	// if there are more columns than our static column buffer
	// can handle, create extra columns, these will be deleted after each
	// query
	if (pvt->colcount>OPTIMISTIC_COLUMN_COUNT && pvt->colcount>pvt->previouscolcount) {
		delete[] pvt->extracolumns;
		pvt->extracolumns=new column[pvt->colcount-OPTIMISTIC_COLUMN_COUNT];
	}
}

bool sqlrcursor::parseOutputBinds() {

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Receiving Output Bind Values: \n");
		pvt->sqlrc->debugPreEnd();
	}

	// useful variables
	uint16_t	type;
	uint32_t	length;
	uint16_t	count=0;

	// get the bind values
	for (;;) {

		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("	getting type...\n");
			pvt->sqlrc->debugPreEnd();
		}

		// get the data type
		if (getShort(&type)!=sizeof(uint16_t)) {
			setError("Failed to get data type.\n "
				"A network error may have occurred.");

			return false;
		}

		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("	done getting type: ");
			pvt->sqlrc->debugPrint((int64_t)type);
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPreEnd();
		}

		// check for end of bind values
		if (type==END_BIND_VARS) {

			break;

		} else if (type==NULL_DATA) {

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("	NULL output bind\n");
				pvt->sqlrc->debugPreEnd();
			}

			// handle a null value
			(*pvt->outbindvars)[count].resultvaluesize=0;
			if ((*pvt->outbindvars)[count].type==BINDVARTYPE_STRING) {
				if (pvt->returnnulls) {
					(*pvt->outbindvars)[count].value.
							stringval=NULL;
				} else {
					(*pvt->outbindvars)[count].value.
							stringval=new char[1];
					(*pvt->outbindvars)[count].value.
							stringval[0]='\0';
				}
			} else if ((*pvt->outbindvars)[count].type==
						BINDVARTYPE_INTEGER) {
				(*pvt->outbindvars)[count].value.integerval=0;
			} else if ((*pvt->outbindvars)[count].type==
						BINDVARTYPE_DOUBLE) {
				(*pvt->outbindvars)[count].value.doubleval.value=0;
				(*pvt->outbindvars)[count].value.doubleval.precision=0;
				(*pvt->outbindvars)[count].value.doubleval.scale=0;
			} else if ((*pvt->outbindvars)[count].type==
						BINDVARTYPE_DATE) {
				(*pvt->outbindvars)[count].value.dateval.year=0;
				(*pvt->outbindvars)[count].value.dateval.month=0;
				(*pvt->outbindvars)[count].value.dateval.day=0;
				(*pvt->outbindvars)[count].value.dateval.hour=0;
				(*pvt->outbindvars)[count].value.dateval.minute=0;
				(*pvt->outbindvars)[count].value.dateval.second=0;
				(*pvt->outbindvars)[count].value.dateval.microsecond=0;
				if (pvt->returnnulls) {
					(*pvt->outbindvars)[count].
						value.dateval.tz=NULL;
				} else {
					(*pvt->outbindvars)[count].
						value.dateval.tz=new char[1];
					(*pvt->outbindvars)[count].
						value.dateval.tz[0]='\0';
				}
			} 

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPrint("		");
				pvt->sqlrc->debugPrint("done fetching.\n");
			}

		} else if (type==STRING_DATA) {

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("	STRING output bind\n");
				pvt->sqlrc->debugPreEnd();
			}

			// get the value length
			if (getLong(&length)!=sizeof(uint32_t)) {
				setError("Failed to get string value length.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->outbindvars)[count].resultvaluesize=length;
			(*pvt->outbindvars)[count].value.stringval=new char[length+1];

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("		length=");
				pvt->sqlrc->debugPrint((int64_t)length);
				pvt->sqlrc->debugPrint("\n");
				pvt->sqlrc->debugPreEnd();
			}

			// get the value
			if ((uint32_t)getString((*pvt->outbindvars)[count].value.
						stringval,length)!=length) {
				setError("Failed to get string value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->outbindvars)[count].value.stringval[length]='\0';

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("		");
				pvt->sqlrc->debugPrint("done fetching\n");
				pvt->sqlrc->debugPreEnd();
			}

		} else if (type==INTEGER_DATA) {

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("	INTEGER output bind\n");
				pvt->sqlrc->debugPreEnd();
			}

			// get the value
			if (getLongLong((uint64_t *)&(*pvt->outbindvars)[count].
					value.integerval)!=sizeof(uint64_t)) {
				setError("Failed to get integer value.\n "
					"A network error may have occurred.");
				return false;
			}

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("		");
				pvt->sqlrc->debugPrint("done fetching\n");
				pvt->sqlrc->debugPreEnd();
			}

		} else if (type==DOUBLE_DATA) {

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("	DOUBLE output bind\n");
				pvt->sqlrc->debugPreEnd();
			}

			// get the value
			if (getDouble(&(*pvt->outbindvars)[count].value.
						doubleval.value)!=
						sizeof(double)) {
				setError("Failed to get double value.\n "
					"A network error may have occurred.");
				return false;
			}

			// get the precision
			if (getLong(&(*pvt->outbindvars)[count].value.
						doubleval.precision)!=
						sizeof(uint32_t)) {
				setError("Failed to get precision.\n "
					"A network error may have occurred.");
				return false;
			}

			// get the scale
			if (getLong(&(*pvt->outbindvars)[count].value.
						doubleval.scale)!=
						sizeof(uint32_t)) {
				setError("Failed to get scale.\n "
					"A network error may have occurred.");
				return false;
			}

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("		");
				pvt->sqlrc->debugPrint("done fetching\n");
				pvt->sqlrc->debugPreEnd();
			}

		} else if (type==DATE_DATA) {

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("	DATE output bind\n");
				pvt->sqlrc->debugPreEnd();
			}

			uint16_t	temp;

			// get the year
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->outbindvars)[count].value.dateval.year=(int16_t)temp;

			// get the month
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->outbindvars)[count].value.dateval.month=(int16_t)temp;

			// get the day
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->outbindvars)[count].value.dateval.day=(int16_t)temp;

			// get the hour
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->outbindvars)[count].value.dateval.hour=(int16_t)temp;

			// get the minute
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->outbindvars)[count].value.dateval.minute=(int16_t)temp;

			// get the second
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->outbindvars)[count].value.dateval.second=(int16_t)temp;

			// get the microsecond
			uint32_t	temp32;
			if (getLong(&temp32)!=sizeof(uint32_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->outbindvars)[count].value.
					dateval.microsecond=(int32_t)temp32;

			// get the timezone length
			uint16_t	length;
			if (getShort(&length)!=sizeof(uint16_t)) {
				setError("Failed to get timezone length.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->outbindvars)[count].value.dateval.tz=new char[length+1];

			// get the timezone
			if ((uint16_t)getString((*pvt->outbindvars)[count].value.
						dateval.tz,length)!=length) {
				setError("Failed to get timezone.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->outbindvars)[count].value. dateval.tz[length]='\0';

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("		");
				pvt->sqlrc->debugPrint("done fetching\n");
				pvt->sqlrc->debugPreEnd();
			}

		} else if (type==CURSOR_DATA) {

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("	CURSOR output bind\n");
				pvt->sqlrc->debugPreEnd();
			}

			// get the cursor id
			if (getShort((uint16_t *)
				&((*pvt->outbindvars)[count].value.cursorid))!=
				sizeof(uint16_t)) {
				setError("Failed to get cursor id.\n "
					"A network error may have occurred.");
				return false;
			}

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("		");
				pvt->sqlrc->debugPrint("done fetching\n");
				pvt->sqlrc->debugPreEnd();
			}

		} else {

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("	LOB/CLOB ");
				pvt->sqlrc->debugPrint("output bind\n");
				pvt->sqlrc->debugPreEnd();
			}

			// must be START_LONG_DATA...
			// get the total length of the long data
			uint64_t	totallength;
			if (getLongLong(&totallength)!=sizeof(uint64_t)) {
				setError("Failed to get total length.\n "
					"A network error may have occurred.");
				return false;
			}

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("		length=");
				pvt->sqlrc->debugPrint((int64_t)totallength);
				pvt->sqlrc->debugPrint("\n");
				pvt->sqlrc->debugPreEnd();
			}

			// create a buffer to hold the data
			char	*buffer=new char[totallength+1];

			uint64_t	offset=0;
			uint32_t	length;
			for (;;) {

				if (pvt->sqlrc->debug()) {
					pvt->sqlrc->debugPreStart();
					pvt->sqlrc->debugPrint("		");
					pvt->sqlrc->debugPrint("fetching...\n");
					pvt->sqlrc->debugPreEnd();
				}

				// get the type of the chunk
				if (getShort(&type)!=sizeof(uint16_t)) {
					delete[] buffer;
					setError("Failed to get chunk type.\n "
						"A network error may have "
						"occurred.");
					return false;
				}

				// check to see if we're done
				if (type==END_LONG_DATA) {
					break;
				}

				// get the length of the chunk
				if (getLong(&length)!=sizeof(uint32_t)) {
					delete[] buffer;
					setError("Failed to get chunk length.\n"
						" A network error may have "
						"occurred.");
					return false;
				}

				// get the chunk of data
				if ((uint32_t)getString(buffer+offset,
							length)!=length) {
					delete[] buffer;
					setError("Failed to get chunk data.\n "
						"A network error may have "
						"occurred.");
					return false;
				}

				offset=offset+length;
			}

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("		");
				pvt->sqlrc->debugPrint("done fetching.\n");
				pvt->sqlrc->debugPreEnd();
			}

			// NULL terminate the buffer.  This makes 
			// certain operations safer and won't hurt
			// since the actual length (which doesn't
			// include the NULL) is available from
			// getOutputBindLength.
			buffer[totallength]='\0';
			(*pvt->outbindvars)[count].value.lobval=buffer;
			(*pvt->outbindvars)[count].resultvaluesize=totallength;
		}

		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint((*pvt->outbindvars)[count].variable);
			pvt->sqlrc->debugPrint("=");
			if ((*pvt->outbindvars)[count].type==
						BINDVARTYPE_BLOB) {
				pvt->sqlrc->debugPrintBlob(
					(*pvt->outbindvars)[count].value.lobval,
					(*pvt->outbindvars)[count].resultvaluesize);
			} else if ((*pvt->outbindvars)[count].type==
						BINDVARTYPE_CLOB) {
				pvt->sqlrc->debugPrintClob(
					(*pvt->outbindvars)[count].value.lobval,
					(*pvt->outbindvars)[count].resultvaluesize);
			} else if ((*pvt->outbindvars)[count].type==
						BINDVARTYPE_CURSOR) {
				pvt->sqlrc->debugPrint((int64_t)(*pvt->outbindvars)[count].
								value.cursorid);
			} else if ((*pvt->outbindvars)[count].type==
						BINDVARTYPE_INTEGER) {
				pvt->sqlrc->debugPrint((*pvt->outbindvars)[count].
							value.integerval);
			} else if ((*pvt->outbindvars)[count].type==
						BINDVARTYPE_DOUBLE) {
				pvt->sqlrc->debugPrint((*pvt->outbindvars)[count].
						value.doubleval.value);
				pvt->sqlrc->debugPrint("(");
				pvt->sqlrc->debugPrint((int64_t)(*pvt->outbindvars)[count].
						value.doubleval.precision);
				pvt->sqlrc->debugPrint(",");
				pvt->sqlrc->debugPrint((int64_t)(*pvt->outbindvars)[count].
						value.doubleval.scale);
				pvt->sqlrc->debugPrint(")");
			} else if ((*pvt->outbindvars)[count].type==
						BINDVARTYPE_DATE) {
				pvt->sqlrc->debugPrint((int64_t)(*pvt->outbindvars)[count].
							value.dateval.year);
				pvt->sqlrc->debugPrint("-");
				pvt->sqlrc->debugPrint((int64_t)(*pvt->outbindvars)[count].
							value.dateval.month);
				pvt->sqlrc->debugPrint("-");
				pvt->sqlrc->debugPrint((int64_t)(*pvt->outbindvars)[count].
							value.dateval.day);
				pvt->sqlrc->debugPrint(" ");
				pvt->sqlrc->debugPrint((int64_t)(*pvt->outbindvars)[count].
							value.dateval.hour);
				pvt->sqlrc->debugPrint(":");
				pvt->sqlrc->debugPrint((int64_t)(*pvt->outbindvars)[count].
							value.dateval.minute);
				pvt->sqlrc->debugPrint(":");
				pvt->sqlrc->debugPrint((int64_t)(*pvt->outbindvars)[count].
							value.dateval.second);
				pvt->sqlrc->debugPrint(" ");
				pvt->sqlrc->debugPrint((*pvt->outbindvars)[count].
							value.dateval.tz);
			} else {
				pvt->sqlrc->debugPrint((*pvt->outbindvars)[count].
							value.stringval);
			}
			pvt->sqlrc->debugPrint("\n");
			pvt->sqlrc->debugPreEnd();
		}

		count++;
	}

	// cache the output binds
	cacheOutputBinds(count);

	return true;
}

bool sqlrcursor::parseData() {

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Parsing Data\n");
		pvt->sqlrc->debugPreEnd();
	}

	// if we're already at the end of the result set, then just return
	if (pvt->endofresultset) {
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Already at the end of the result set\n");
			pvt->sqlrc->debugPreEnd();
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
	pvt->firstrowindex=pvt->rowcount;

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

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("Got end of result set.\n");
				pvt->sqlrc->debugPreEnd();
			}
			pvt->endofresultset=true;

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
				if (!pvt->rows) {
					createRowBuffers();
				}
				currentrow=pvt->rows[rowbuffercount];
			} else {
				if (pvt->sqlrc->debug()) {
					pvt->sqlrc->debugPreStart();
					pvt->sqlrc->debugPrint("Creating extra rows.\n");
					pvt->sqlrc->debugPreEnd();
				}
				if (!pvt->firstextrarow) {
					currentrow=new row(pvt->colcount);
					pvt->firstextrarow=currentrow;
				} else {
					currentrow->next=new row(pvt->colcount);
					currentrow=currentrow->next;
				}
			}
			if (pvt->colcount>currentrow->colcount) {
				currentrow->resize(pvt->colcount);
			}

			rowbuffercount++;
			pvt->rowcount++;
		}

		if (type==NULL_DATA) {

			// handle null data
			if (pvt->returnnulls) {
				buffer=NULL;
			} else {
				buffer=(char *)pvt->rowstorage->allocate(1);
				buffer[0]='\0';
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
			buffer=(char *)pvt->rowstorage->allocate(length+1);
			if ((uint32_t)getString(buffer,length)!=length) {
				setError("Failed to get the field data.\n A network error may have occurred");
				return false;
			}
			buffer[length]='\0';

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
					bytestring::copy(
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
			buffer[totallength]='\0';
			length=totallength;
		}

		// add the buffer to the current row
		currentrow->addField(colindex,buffer,length);
	
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			if (buffer) {
				if (type==END_LONG_DATA) {
					pvt->sqlrc->debugPrint("\nLOB data:");
					pvt->sqlrc->debugPrintBlob(buffer,length);
				} else {
					pvt->sqlrc->debugPrint("\"");
					pvt->sqlrc->debugPrint(buffer);
					pvt->sqlrc->debugPrint("\",");
				}
			} else {
				pvt->sqlrc->debugPrint(buffer);
				pvt->sqlrc->debugPrint(",");
			}
			pvt->sqlrc->debugPreEnd();
		}

		// tag the column as a long data type or not
		currentcol=getColumnInternal(colindex);

		// set whether this column is a "long type" or not
		currentcol->longdatatype=(type==END_LONG_DATA)?1:0;

		if (pvt->sendcolumninfo==SEND_COLUMN_INFO && 
				pvt->sentcolumninfo==SEND_COLUMN_INFO) {

			// keep track of the longest field
			if (length>currentcol->longest) {
				currentcol->longest=length;
			}
		}

		// move to the next column, handle end of row 
		colindex++;
		if (colindex==pvt->colcount) {

			colindex=0;

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("\n");
				pvt->sqlrc->debugPreEnd();
			}

			// check to see if we've gotten enough rows
			if (pvt->rsbuffersize && rowbuffercount==pvt->rsbuffersize) {
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
	pvt->rows=new row *[OPTIMISTIC_ROW_COUNT];
	for (uint64_t i=0; i<OPTIMISTIC_ROW_COUNT; i++) {
		pvt->rows[i]=new row(pvt->colcount);
	}
}

void sqlrcursor::createExtraRowArray() {

	// create the arrays
	uint64_t	howmany=pvt->rowcount-pvt->firstrowindex-OPTIMISTIC_ROW_COUNT;
	pvt->extrarows=new row *[howmany];
	
	// populate the arrays
	row	*currentrow=pvt->firstextrarow;
	for (uint64_t i=0; i<howmany; i++) {
		pvt->extrarows[i]=currentrow;
		currentrow=currentrow->next;
	}
}

void sqlrcursor::getErrorFromServer() {

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Getting Error From Server\n");
		pvt->sqlrc->debugPreEnd();
	}

	bool	networkerror=true;

	// get the error code
	if (getLongLong((uint64_t *)&pvt->errorno)==sizeof(uint64_t)) {

		// get the length of the error string
		uint16_t	length;
		if (getShort(&length)==sizeof(uint16_t)) {

			// get the error string
			pvt->error=new char[length+1];
			pvt->sqlrc->cs()->read(pvt->error,length);
			pvt->error[length]='\0';

			networkerror=false;
		}
	}

	if (networkerror) {
		setError("There was an error, but the connection"
				" died trying to retrieve it.  Sorry.");
	}
	
	handleError();
}

void sqlrcursor::setError(const char *err) {

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Setting Error\n");
		pvt->sqlrc->debugPreEnd();
	}
	pvt->error=charstring::duplicate(err);
	handleError();
}

void sqlrcursor::handleError() {

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint((int64_t)pvt->errorno);
		pvt->sqlrc->debugPrint(":\n");
		pvt->sqlrc->debugPrint(pvt->error);
		pvt->sqlrc->debugPrint("\n");
		pvt->sqlrc->debugPreEnd();
	}

	pvt->endofresultset=true;

	cacheError();
	finishCaching();
}

bool sqlrcursor::fetchRowIntoBuffer(bool getallrows, uint64_t row,
						uint64_t *rowbufferindex) {

	// if we getting the entire result set at once, then the result set 
	// buffer index is the requested row-pvt->firstrowindex
	if (!pvt->rsbuffersize) {
		if (row<pvt->rowcount && row>=pvt->firstrowindex) {
			*rowbufferindex=row-pvt->firstrowindex;
			return true;
		}
		return false;
	}

	// but, if we're not getting the entire result set at once
	// and if the requested row is not in the current range, 
	// fetch more data from the connection
	while (row>=(pvt->firstrowindex+pvt->rsbuffersize) && !pvt->endofresultset) {

		if (pvt->sqlrc->connected() || (pvt->cachesource && pvt->cachesourceind)) {

			clearRows();

			// if we're not fetching from a cached result set,
			// tell the server to send one 
			if (!pvt->cachesource && !pvt->cachesourceind) {
				pvt->sqlrc->cs()->write((uint16_t)FETCH_RESULT_SET);
				pvt->sqlrc->cs()->write(pvt->cursorid);
			}

			if (!skipAndFetch(getallrows,row) || !parseData()) {
				return false;
			}

		} else {
			return false;
		}
	}

	// return the buffer index corresponding to the requested row
	// or -1 if the requested row is past the end of the result set
	if (row<pvt->rowcount) {
		*rowbufferindex=row%pvt->rsbuffersize;
		return true;
	}
	return false;
}

int32_t sqlrcursor::getShort(uint16_t *integer,
				int32_t timeoutsec, int32_t timeoutusec) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (pvt->cachesource && pvt->cachesourceind) {
		return pvt->cachesource->read(integer);
	} else {
		return pvt->sqlrc->cs()->read(integer,timeoutsec,timeoutusec);
	}
}

int32_t sqlrcursor::getShort(uint16_t *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (pvt->cachesource && pvt->cachesourceind) {
		return pvt->cachesource->read(integer);
	} else {
		return pvt->sqlrc->cs()->read(integer);
	}
}

int32_t sqlrcursor::getLong(uint32_t *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (pvt->cachesource && pvt->cachesourceind) {
		return pvt->cachesource->read(integer);
	} else {
		return pvt->sqlrc->cs()->read(integer);
	}
}

int32_t sqlrcursor::getLongLong(uint64_t *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (pvt->cachesource && pvt->cachesourceind) {
		return pvt->cachesource->read(integer);
	} else {
		return pvt->sqlrc->cs()->read(integer);
	}
}

int32_t sqlrcursor::getString(char *string, int32_t size) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (pvt->cachesource && pvt->cachesourceind) {
		return pvt->cachesource->read(string,size);
	} else {
		return pvt->sqlrc->cs()->read(string,size);
	}
}

int32_t sqlrcursor::getDouble(double *value) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (pvt->cachesource && pvt->cachesourceind) {
		return pvt->cachesource->read(value);
	} else {
		return pvt->sqlrc->cs()->read(value);
	}
}

bool sqlrcursor::fetchFromBindCursor() {

	if (!pvt->endofresultset || !pvt->sqlrc->connected()) {
		return false;
	}

	// FIXME: should these be here?
	clearVariables();
	clearResultSet();

	pvt->cached=false;
	pvt->endofresultset=false;

	// tell the server we're fetching from a bind cursor
	pvt->sqlrc->cs()->write((uint16_t)FETCH_FROM_BIND_CURSOR);

	// send the cursor id to the server
	pvt->sqlrc->cs()->write((uint16_t)pvt->cursorid);

	sendGetColumnInfo();

	pvt->sqlrc->flushWriteBuffer();

	if (pvt->rsbuffersize) {
		return processResultSet(false,pvt->rsbuffersize-1);
	} else {
		return processResultSet(true,0);
	}
}

bool sqlrcursor::openCachedResultSet(const char *filename) {

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Opening cached result set: ");
		pvt->sqlrc->debugPrint(filename);
		pvt->sqlrc->debugPrint("\n");
		pvt->sqlrc->debugPreEnd();
	}

	if (!pvt->endofresultset) {
		closeResultSet(true);
	}
	clearResultSet();

	pvt->cached=true;
	pvt->endofresultset=false;

	// create the index file name
	size_t	indexfilenamelen=charstring::length(filename)+5;
	char	*indexfilename=new char[indexfilenamelen];
	charstring::copy(indexfilename,filename);
	charstring::append(indexfilename,".ind");

	// open the file
	pvt->cachesource=new file();
	pvt->cachesourceind=new file();
	if ((pvt->cachesource->open(filename,O_RDWR)) &&
		(pvt->cachesourceind->open(indexfilename,O_RDWR))) {

		delete[] indexfilename;

		// initialize firstrowindex and rowcount
		pvt->firstrowindex=0;
		pvt->rowcount=pvt->firstrowindex;

		// make sure it's a cache file and skip the ttl
		char		magicid[13];
		uint64_t	ttl;
		if (getString(magicid,13)==13 &&
			!charstring::compare(magicid,"SQLRELAYCACHE",13) &&
			getLongLong(&ttl)==sizeof(uint64_t)) {

			// process the result set
			if (pvt->rsbuffersize) {
				return processResultSet(false,pvt->firstrowindex+
								pvt->rsbuffersize-1);
			} else {
				return processResultSet(true,0);
			}
		} else {

			// if the test above failed, the file is either not
			// a cache file or is corrupt
			stringbuffer	errstr;
			errstr.append("File ");
			errstr.append(filename);
			errstr.append(" is either corrupt");
			errstr.append(" or not a cache file.");
			setError(errstr.getString());
		}

	} else {

		// if we couldn't open the file, set the error message
		stringbuffer	errstr;
		errstr.append("Couldn't open ");
		errstr.append(filename);
		errstr.append(" and ");
		errstr.append(indexfilename);
		setError(errstr.getString());

		delete[] indexfilename;
	}

	// if we fell through to here, then an error has ocurred
	clearCacheSource();
	return false;
}

void sqlrcursor::clearCacheSource() {
	if (pvt->cachesource) {
		pvt->cachesource->close();
		delete pvt->cachesource;
		pvt->cachesource=NULL;
	}
	if (pvt->cachesourceind) {
		pvt->cachesourceind->close();
		delete pvt->cachesourceind;
		pvt->cachesourceind=NULL;
	}
}

uint32_t sqlrcursor::colCount() {
	return pvt->colcount;
}

column *sqlrcursor::getColumn(uint32_t index) {
	if (pvt->sendcolumninfo==SEND_COLUMN_INFO && 
			pvt->sentcolumninfo==SEND_COLUMN_INFO &&
			pvt->colcount && index<pvt->colcount) {
		return getColumnInternal(index);
	}
	return NULL;
}

column *sqlrcursor::getColumn(const char *name) {
	if (pvt->sendcolumninfo==SEND_COLUMN_INFO && 
			pvt->sentcolumninfo==SEND_COLUMN_INFO) {
		column	*whichcolumn;
		for (uint32_t i=0; i<pvt->colcount; i++) {
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
		return &pvt->columns[index];
	}
	return &pvt->extracolumns[index-OPTIMISTIC_COLUMN_COUNT];
}

const char * const *sqlrcursor::getColumnNames() {

	if (pvt->sendcolumninfo==DONT_SEND_COLUMN_INFO ||
			pvt->sentcolumninfo==DONT_SEND_COLUMN_INFO) {
		return NULL;
	}

	if (!pvt->columnnamearray) {
		if (pvt->sqlrc->debug()) {
			pvt->sqlrc->debugPreStart();
			pvt->sqlrc->debugPrint("Creating Column Arrays...\n");
			pvt->sqlrc->debugPreEnd();
		}
	
		// build a 2d array of pointers to the column names
		pvt->columnnamearray=new char *[pvt->colcount+1];
		pvt->columnnamearray[pvt->colcount]=NULL;
		for (uint32_t i=0; i<pvt->colcount; i++) {
			pvt->columnnamearray[i]=getColumnInternal(i)->name;
		}
	}
	return pvt->columnnamearray;
}

const char *sqlrcursor::getColumnName(uint32_t col) {
	column	*whichcol=getColumn(col);
	return (whichcol)?whichcol->name:NULL;
}

const char *sqlrcursor::getColumnType(uint32_t col) {
	column	*whichcol=getColumn(col);
	if (whichcol) {
		if (pvt->columntypeformat!=COLUMN_TYPE_IDS) {
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
		if (pvt->columntypeformat!=COLUMN_TYPE_IDS) {
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
	return (whichcol)?(whichcol->nullable!=0):false;
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

uint64_t sqlrcursor::firstRowIndex() {
	return pvt->firstrowindex;
}

bool sqlrcursor::endOfResultSet() {
	return pvt->endofresultset;
}

uint64_t sqlrcursor::rowCount() {
	return pvt->rowcount;
}

uint64_t sqlrcursor::affectedRows() {
	if (pvt->knowsaffectedrows==AFFECTED_ROWS) {
		return pvt->affectedrows;
	}
	return 0;
}

uint64_t sqlrcursor::totalRows() {
	if (pvt->knowsactualrows==ACTUAL_ROWS) {
		return pvt->actualrows;
	}
	return 0;
}

int64_t sqlrcursor::errorNumber() {
	// if we have a code then we should have a message too,
	// the codes could be any number, including 0, so check
	// the message to see which code to return
	if (pvt->error) {
		return pvt->errorno;
	} else if (pvt->sqlrc->error()) {
		return pvt->sqlrc->errorno();
	}
	return 0;
}

const char *sqlrcursor::errorMessage() {
	if (pvt->error) {
		return pvt->error;
	} else if (pvt->sqlrc->error()) {
		return pvt->sqlrc->error();
	}
	return NULL;
}

void sqlrcursor::getNullsAsEmptyStrings() {
	pvt->returnnulls=false;
}

void sqlrcursor::getNullsAsNulls() {
	pvt->returnnulls=true;
}

char *sqlrcursor::getFieldInternal(uint64_t row, uint32_t col) {
	if (row<OPTIMISTIC_ROW_COUNT) {
		return pvt->rows[row]->getField(col);
	}
	return pvt->extrarows[row-OPTIMISTIC_ROW_COUNT]->getField(col);
}

uint32_t sqlrcursor::getFieldLengthInternal(uint64_t row, uint32_t col) {
	if (row<OPTIMISTIC_ROW_COUNT) {
		return pvt->rows[row]->getFieldLength(col);
	}
	return pvt->extrarows[row-OPTIMISTIC_ROW_COUNT]->getFieldLength(col);
}

const char *sqlrcursor::getField(uint64_t row, uint32_t col) {

	if (pvt->rowcount && row>=pvt->firstrowindex && col<pvt->colcount) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		uint64_t	rowbufferindex;
		if (fetchRowIntoBuffer(false,row,&rowbufferindex)) {
			return getFieldInternal(rowbufferindex,col);
		}
	}
	return NULL;
}

int64_t sqlrcursor::getFieldAsInteger(uint64_t row, uint32_t col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toInteger(field):0;
}

double sqlrcursor::getFieldAsDouble(uint64_t row, uint32_t col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toFloat(field):0.0;
}

const char *sqlrcursor::getField(uint64_t row, const char *col) {

	if (pvt->sendcolumninfo==SEND_COLUMN_INFO && 
			pvt->sentcolumninfo==SEND_COLUMN_INFO &&
			pvt->rowcount && row>=pvt->firstrowindex) {
		for (uint32_t i=0; i<pvt->colcount; i++) {
			if (!charstring::compareIgnoringCase(
					getColumnInternal(i)->name,col)) {

				// in the event that we're stepping through the
				// result set instead of buffering the entire 
				// thing, the requested row may have to be 
				// fetched into the buffer...
				uint64_t	rowbufferindex;
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

int64_t sqlrcursor::getFieldAsInteger(uint64_t row, const char *col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toInteger(field):0;
}

double sqlrcursor::getFieldAsDouble(uint64_t row, const char *col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toFloat(field):0.0;
}

uint32_t sqlrcursor::getFieldLength(uint64_t row, uint32_t col) {

	if (pvt->rowcount && row>=pvt->firstrowindex && col<pvt->colcount) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		uint64_t	rowbufferindex;
		if (fetchRowIntoBuffer(false,row,&rowbufferindex)) {
			return getFieldLengthInternal(rowbufferindex,col);
		}
	}
	return 0;
}

uint32_t sqlrcursor::getFieldLength(uint64_t row, const char *col) {

	if (pvt->sendcolumninfo==SEND_COLUMN_INFO && 
			pvt->sentcolumninfo==SEND_COLUMN_INFO &&
			pvt->rowcount && row>=pvt->firstrowindex) {

		for (uint32_t i=0; i<pvt->colcount; i++) {
			if (!charstring::compareIgnoringCase(
					getColumnInternal(i)->name,col)) {

				// in the event that we're stepping through the
				// result set instead of buffering the entire 
				// thing, the requested row may have to be 
				// fetched into the buffer...
				uint64_t	rowbufferindex;
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

const char * const *sqlrcursor::getRow(uint64_t row) {

	if (pvt->rowcount && row>=pvt->firstrowindex) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		uint64_t	rowbufferindex;
		if (fetchRowIntoBuffer(false,row,&rowbufferindex)) {
			if (!pvt->fields) {
				createFields();
			}
			return pvt->fields[rowbufferindex];
		}
	}
	return NULL;
}

void sqlrcursor::createFields() {
	// lets say that rowcount=5 and firstrowindex=3,
	// the fields array will contain 2 elements:
	// 	fields[0] (corresponding to row 3) and
	// 	fields[1] (corresponding to row 4)
	uint64_t	rowbuffercount=pvt->rowcount-pvt->firstrowindex;
	pvt->fields=new char **[rowbuffercount+1];
	pvt->fields[rowbuffercount]=(char **)NULL;
	for (uint64_t i=0; i<rowbuffercount; i++) {
		pvt->fields[i]=new char *[pvt->colcount+1];
		pvt->fields[i][pvt->colcount]=(char *)NULL;
		for (uint32_t j=0; j<pvt->colcount; j++) {
			pvt->fields[i][j]=getFieldInternal(i,j);
		}
	}
}

uint32_t *sqlrcursor::getRowLengths(uint64_t row) {

	if (pvt->rowcount && row>=pvt->firstrowindex) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		uint64_t	rowbufferindex;
		if (fetchRowIntoBuffer(false,row,&rowbufferindex)) {
			if (!pvt->fieldlengths) {
				createFieldLengths();
			}
			return pvt->fieldlengths[rowbufferindex];
		}
	}
	return NULL;
}

void sqlrcursor::createFieldLengths() {
	// lets say that rowcount=5 and firstrowindex=3,
	// the fieldlengths array will contain 2 elements:
	// 	fieldlengths[0] (corresponding to row 3) and
	// 	fieldlengths[1] (corresponding to row 4)
	uint64_t	rowbuffercount=pvt->rowcount-pvt->firstrowindex;
	pvt->fieldlengths=new uint32_t *[rowbuffercount+1];
	pvt->fieldlengths[rowbuffercount]=0;
	for (uint64_t i=0; i<rowbuffercount; i++) {
		pvt->fieldlengths[i]=new uint32_t[pvt->colcount+1];
		pvt->fieldlengths[i][pvt->colcount]=0;
		for (uint32_t j=0; j<pvt->colcount; j++) {
			pvt->fieldlengths[i][j]=getFieldLengthInternal(i,j);
		}
	}
}

void sqlrcursor::suspendResultSet() {

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Suspending Result Set\n");
		pvt->sqlrc->debugPreEnd();
	}

	if (pvt->sqlrc->connected() && !pvt->cached) {

		pvt->sqlrc->cs()->write((uint16_t)SUSPEND_RESULT_SET);
		pvt->sqlrc->cs()->write(pvt->cursorid);

		pvt->sqlrc->flushWriteBuffer();
	}

	clearCacheDest();
	pvt->suspendresultsetsent=1;
}

uint16_t sqlrcursor::getResultSetId() {
	return pvt->cursorid;
}

bool sqlrcursor::resumeResultSet(uint16_t id) {
	return resumeCachedResultSet(id,NULL);
}

bool sqlrcursor::resumeCachedResultSet(uint16_t id, const char *filename) {

	if (!pvt->endofresultset && !pvt->suspendresultsetsent) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!pvt->sqlrc->connected()) {
		return false;
	}

	pvt->cached=false;
	pvt->resumed=true;
	pvt->endofresultset=false;

	if (pvt->sqlrc->debug()) {
		pvt->sqlrc->debugPreStart();
		pvt->sqlrc->debugPrint("Resuming Result Set of Cursor: ");
		pvt->sqlrc->debugPrint((int64_t)id);
		pvt->sqlrc->debugPrint("\n");
		pvt->sqlrc->debugPreEnd();
	}

	// tell the server we want to resume the result set
	pvt->sqlrc->cs()->write((uint16_t)RESUME_RESULT_SET);

	// send the id of the cursor we want to 
	// resume the result set of to the server
	pvt->sqlrc->cs()->write(id);

	// process the result set
	if (!charstring::isNullOrEmpty(filename)) {
		cacheToFile(filename);
	}
	if (pvt->rsbuffersize) {
		if (processResultSet(true,pvt->firstrowindex+pvt->rsbuffersize-1)) {
			return true;
		}
	} else {
		if (processResultSet(false,0)) {
			return true;
		}
	}
	return false;
}

void sqlrcursor::closeResultSet() {
	closeResultSet(true);
}

void sqlrcursor::closeResultSet(bool closeremote) {

	// If the end of the previous result set was never reached, abort it.
	// If we're caching data to a local file, get the rest of the data; we
	// won't have to abort the result set in that case, the server will
	// do it.

	if (pvt->sqlrc->connected() || pvt->cached) {
		if (pvt->cachedest && pvt->cachedestind) {
			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("Getting the rest of the result set, since this is a cached result set.\n");
				pvt->sqlrc->debugPreEnd();
			}
			while (!pvt->endofresultset) {
				clearRows();

				// if we're not fetching from a cached result 
				// set tell the server to send one 
				if (!pvt->cachesource && !pvt->cachesourceind) {
					pvt->sqlrc->cs()->write((uint16_t)
							FETCH_RESULT_SET);
					pvt->sqlrc->cs()->write(pvt->cursorid);
				}

				// parseData should call finishCaching when
				// it hits the end of the result set, but
				// if it or skipAndFetch return a -1 (network
				// error) we'll have to call it ourselves.
				if (!skipAndFetch(true,0) || !parseData()) {
					finishCaching();
					return;
				}
			}
		} else if (closeremote && pvt->havecursorid) {

			if (pvt->sqlrc->debug()) {
				pvt->sqlrc->debugPreStart();
				pvt->sqlrc->debugPrint("Aborting Result "
							"Set For Cursor: ");
				pvt->sqlrc->debugPrint((int64_t)pvt->cursorid);
				pvt->sqlrc->debugPrint("\n");
				pvt->sqlrc->debugPreEnd();
			}

			pvt->sqlrc->cs()->write((uint16_t)ABORT_RESULT_SET);
			pvt->sqlrc->cs()->write((uint16_t)DONT_NEED_NEW_CURSOR);
			pvt->sqlrc->cs()->write(pvt->cursorid);
			pvt->sqlrc->flushWriteBuffer();
		}
	}
}

void sqlrcursor::clearResultSet() {

	clearCacheDest();
	clearCacheSource();
	clearError();

	// columns is cleared after rows because colcount is used in 
	// clearRows() and set to 0 in clearColumns()
	clearRows();
	clearColumns();

	// clear row counters, since fetchRowIntoBuffer() and clearResultSet()
	// are the only methods that call clearRows() and fetchRowIntoBuffer()
	// needs these values not to be cleared, we'll clear them here...
	pvt->firstrowindex=0;
	pvt->previousrowcount=pvt->rowcount;
	pvt->rowcount=0;
	pvt->actualrows=0;
	pvt->affectedrows=0;
	pvt->endofresultset=true;
	pvt->suspendresultsetsent=0;
}

void sqlrcursor::clearError() {
	delete[] pvt->error;
	pvt->error=NULL;
	pvt->errorno=0;
	if (pvt->sqlrc) {
		pvt->sqlrc->clearError();
	}
}

void sqlrcursor::clearRows() {

	// delete data in rows for long datatypes
	uint32_t	rowbuffercount=pvt->rowcount-pvt->firstrowindex;
	for (uint32_t i=0; i<rowbuffercount; i++) {
	        for (uint32_t j=0; j<pvt->colcount; j++) {
			if (getColumnInternal(j)->longdatatype) {
				// don't delete null columns
				// (who's lengths will be 0)
				if (getFieldLengthInternal(i,j)) {
					delete[] getFieldInternal(i,j);
				}
			}
		}
	}

	// delete linked list storing extra result set fields
	row	*currentrow;
	if (pvt->firstextrarow) {
		currentrow=pvt->firstextrarow;
		while (currentrow) {
			pvt->firstextrarow=currentrow->next;
			delete currentrow;
			currentrow=pvt->firstextrarow;
		}
		pvt->firstextrarow=NULL;
	}
	currentrow=NULL;

	// delete array pointing to linked list items
	delete[] pvt->extrarows;
	pvt->extrarows=NULL;

	// delete arrays of fields and field lengths
	if (pvt->fields) {
		for (uint32_t i=0; i<rowbuffercount; i++) {
			delete[] pvt->fields[i];
		}
		delete[] pvt->fields;
		pvt->fields=NULL;
	}
	if (pvt->fieldlengths) {
		for (uint32_t i=0; i<rowbuffercount; i++) {
			delete[] pvt->fieldlengths[i];
		}
		delete[] pvt->fieldlengths;
		pvt->fieldlengths=NULL;
	}

	// reset the row storage pool
	pvt->rowstorage->deallocate();
}

void sqlrcursor::clearColumns() {

	// delete the column type strings (if necessary)
	if (pvt->sentcolumninfo==SEND_COLUMN_INFO &&
				pvt->columntypeformat!=COLUMN_TYPE_IDS) {
		for (uint32_t i=0; i<pvt->colcount; i++) {
			delete[] getColumnInternal(i)->typestring;
		}
	}

	// reset the column storage pool
	pvt->colstorage->deallocate();

	// reset the column count
	pvt->previouscolcount=pvt->colcount;
	pvt->colcount=0;

	// delete array pointing to each column name
	delete[] pvt->columnnamearray;
	pvt->columnnamearray=NULL;
}

char *sqlrcursor::getQueryTree() {

	pvt->reexecute=false;
	pvt->validatebinds=false;
	pvt->resumed=false;
	clearVariables();

	if (!pvt->endofresultset) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!pvt->sqlrc->openSession()) {
		return NULL;
	}

	pvt->cached=false;
	pvt->endofresultset=false;

	// tell the server we want to get a db list
	pvt->sqlrc->cs()->write((uint16_t)GET_QUERY_TREE);

	// tell the server whether we'll need a cursor or not
	sendCursorStatus();

	pvt->sqlrc->flushWriteBuffer();

	uint16_t	err=getErrorStatus();
	if (err!=NO_ERROR_OCCURRED) {
		if (err==TIMEOUT_GETTING_ERROR_STATUS) {
			pvt->sqlrc->endSession();
			return NULL;
		}
		getErrorFromServer();
		if (err==ERROR_OCCURRED_DISCONNECT) {
			pvt->sqlrc->endSession();
		}
		return NULL;
	}

	// get the size of the tree
	uint64_t	querytreelen;
	if (pvt->sqlrc->cs()->read(&querytreelen)!=sizeof(uint64_t)) {
		return NULL;
	}

	// get the tree itself
	char	*querytree=new char[querytreelen+1];
	if ((uint64_t)pvt->sqlrc->cs()->read(querytree,querytreelen)!=querytreelen) {
		delete[] querytree;
		return NULL;
	}
	querytree[querytreelen]='\0';

	return querytree;
}

bool sqlrcursor::endofresultset() {
	return pvt->endofresultset;
}

void sqlrcursor::sqlrc(sqlrconnection *sqlrc) {
	pvt->sqlrc=sqlrc;
}

sqlrcursor *sqlrcursor::next() {
	return pvt->next;
}

void sqlrcursor::havecursorid(bool havecursorid) {
	pvt->havecursorid=havecursorid;
}
