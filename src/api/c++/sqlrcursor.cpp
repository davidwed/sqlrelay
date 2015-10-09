// Copyright (c) 1999-2001  David Muse
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

sqlrcursor::sqlrcursor(sqlrconnection *sqlrc, bool copyreferences) {
	init(sqlrc,copyreferences);
}

sqlrcursor::sqlrcursor(sqlrconnection *sqlrc) {
	init(sqlrc,false);
}

void sqlrcursor::init(sqlrconnection *sqlrc, bool copyreferences) {

	// copy references
	copyrefs=copyreferences;

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

	errorno=0;
	error=NULL;

	rows=NULL;
	extrarows=NULL;
	firstextrarow=NULL;
	rowstorage=new memorypool(OPTIMISTIC_RESULT_SET_SIZE,
			OPTIMISTIC_RESULT_SET_SIZE/OPTIMISTIC_ROW_COUNT,5);
	fields=NULL;
	fieldlengths=NULL;

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
	subvars=new dynamicarray<bindvar>(OPTIMISTIC_BIND_COUNT,16);
	inbindvars=new dynamicarray<bindvar>(OPTIMISTIC_BIND_COUNT,16);
	outbindvars=new dynamicarray<bindvar>(OPTIMISTIC_BIND_COUNT,16);
	clearVariables();
}

sqlrcursor::~sqlrcursor() {

	// abort result set if necessary
	if (sqlrc && !sqlrc->endsessionsent && !sqlrc->suspendsessionsent) {
		closeResultSet(true);
	}

	// deallocate copied references
	deleteVariables();
	delete outbindvars;
	delete inbindvars;
	delete subvars;

	// deallocate the query buffer
	delete[] querybuffer;

	// deallocate the fullpath (used for file queries)
	delete[] fullpath;

	clearResultSet();
	delete[] columns;
	delete[] extracolumns;
	delete colstorage;
	if (rows) {
		for (uint32_t i=0; i<OPTIMISTIC_ROW_COUNT; i++) {
			delete rows[i];
		}
		delete[] rows;
	}
	delete rowstorage;

	// it's possible for the connection to be deleted before the 
	// cursor is, in that case, don't do any of this stuff
	if (sqlrc) {

		// remove self from connection's cursor list
		if (!next && !prev) {
			sqlrc->firstcursor=NULL;
			sqlrc->lastcursor=NULL;
		} else {
			sqlrcursor	*temp=next;
			if (next) {
				next->prev=prev;
			} else {
				sqlrc->lastcursor=prev;
			}
			if (prev) {
				prev->next=temp;
			} else {
				sqlrc->firstcursor=next;
			}
		}

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Deallocated cursor\n");
			sqlrc->debugPreEnd();
		}
	}

	if (copyrefs && cachedestname) {
		delete[] cachedestname;
	}
	delete[] cachedestindname;
}

void sqlrcursor::setResultSetBufferSize(uint64_t rows) {
	rsbuffersize=rows;
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Result Set Buffer Size: ");
		sqlrc->debugPrint((int64_t)rows);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}
}

uint64_t sqlrcursor::getResultSetBufferSize() {
	return rsbuffersize;
}

void sqlrcursor::dontGetColumnInfo() {
	sendcolumninfo=DONT_SEND_COLUMN_INFO;
}

void sqlrcursor::getColumnInfo() {
	sendcolumninfo=SEND_COLUMN_INFO;
}

void sqlrcursor::mixedCaseColumnNames() {
	colcase=MIXED_CASE;
}

void sqlrcursor::upperCaseColumnNames() {
	colcase=UPPER_CASE;
}

void sqlrcursor::lowerCaseColumnNames() {
	colcase=LOWER_CASE;
}

void sqlrcursor::cacheToFile(const char *filename) {

	cacheon=true;
	cachettl=600;
	if (copyrefs) {
		delete[] cachedestname;
		cachedestname=charstring::duplicate(filename);
	} else {
		cachedestname=(char *)filename;
	}

	// create the index name
	delete[] cachedestindname;
	size_t	cachedestindnamelen=charstring::length(filename)+5;
	cachedestindname=new char[cachedestindnamelen];
	charstring::copy(cachedestindname,filename);
	charstring::append(cachedestindname,".ind");
}

void sqlrcursor::setCacheTtl(uint32_t ttl) {
	cachettl=ttl;
}

const char *sqlrcursor::getCacheFileName() {
	return cachedestname;
}

void sqlrcursor::cacheOff() {
	cacheon=false;
}

void sqlrcursor::startCaching() {

	if (!resumed) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Caching data to ");
			sqlrc->debugPrint(cachedestname);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}
	} else {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Resuming caching data to ");
			sqlrc->debugPrint(cachedestname);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}
	}

	// create the cache file, truncate it unless we're 
	// resuming a previous session
	cachedest=new file();
	cachedestind=new file();
	if (!resumed) {
		cachedest->open(cachedestname,O_RDWR|O_TRUNC|O_CREAT,
					permissions::ownerReadWrite());
		cachedestind->open(cachedestindname,O_RDWR|O_TRUNC|O_CREAT,
					permissions::ownerReadWrite());
	} else {
		cachedest->open(cachedestname,O_RDWR|O_CREAT|O_APPEND);
		cachedestind->open(cachedestindname,O_RDWR|O_CREAT|O_APPEND);
	}

	if (cachedest && cachedestind) {

		// calculate and set write buffer size
		// FIXME: I think rudiments bugs keep this from working...
		/*filesystem	fs;
		if (fs.initialize(cachedestname)) {
			off64_t	optblocksize=fs.getOptimumTransferBlockSize();
			cachedest->setWriteBufferSize(
					(optblocksize)?optblocksize:1024);
			cachedestind->setWriteBufferSize(
					(optblocksize)?optblocksize:1024);
		}*/

		if (!resumed) {

			// write "magic" identifier to head of files
			cachedest->write("SQLRELAYCACHE",13);
			cachedestind->write("SQLRELAYCACHE",13);
			
			// write ttl to files
			datetime	dt;
			dt.getSystemDateAndTime();
			int64_t	expiration=dt.getEpoch()+cachettl;
			cachedest->write(expiration);
			cachedestind->write(expiration);
		}

	} else {

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Error caching data to ");
			sqlrc->debugPrint(cachedestname);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		// in case of an error, clean up
		clearCacheDest();
	}
}

void sqlrcursor::cacheError() {

	if (resumed || !cachedest) {
		return;
	}

	// write the number of returned rows, affected rows 
	// and a zero to terminate the column descriptions
	cachedest->write((uint16_t)NO_ACTUAL_ROWS);
	cachedest->write((uint16_t)NO_AFFECTED_ROWS);
	cachedest->write((uint16_t)END_COLUMN_INFO);
}

void sqlrcursor::cacheNoError() {

	if (resumed || !cachedest) {
		return;
	}

	cachedest->write((uint16_t)NO_ERROR_OCCURRED);
}

void sqlrcursor::cacheColumnInfo() {

	if (resumed || !cachedest) {
		return;
	}

	// write the number of returned rows
	cachedest->write(knowsactualrows);
	if (knowsactualrows==ACTUAL_ROWS) {
		cachedest->write(actualrows);
	}

	// write the number of affected rows
	cachedest->write(knowsaffectedrows);
	if (knowsaffectedrows==AFFECTED_ROWS) {
		cachedest->write(affectedrows);
	}

	// write whether or not the column info is is cached
	cachedest->write(sentcolumninfo);

	// write the column count
	cachedest->write(colcount);

	// write column descriptions to the cache file
	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO) {

		// write column type format
		cachedest->write(columntypeformat);

		// write the columns themselves
		uint16_t	namelen;
		column		*whichcolumn;
		for (uint32_t i=0; i<colcount; i++) {

			// get the column
			whichcolumn=getColumnInternal(i);

			// write the name
			namelen=charstring::length(whichcolumn->name);
			cachedest->write(namelen);
			cachedest->write(whichcolumn->name,namelen);

			// write the type
			if (columntypeformat==COLUMN_TYPE_IDS) {
				cachedest->write(whichcolumn->type);
			} else {
				cachedest->write(whichcolumn->typestringlength);
				cachedest->write(whichcolumn->typestring,
						whichcolumn->typestringlength);
			}

			// write the length, precision and scale
			cachedest->write(whichcolumn->length);
			cachedest->write(whichcolumn->precision);
			cachedest->write(whichcolumn->scale);

			// write the flags
			cachedest->write(whichcolumn->nullable);
			cachedest->write(whichcolumn->primarykey);
			cachedest->write(whichcolumn->unique);
			cachedest->write(whichcolumn->partofkey);
			cachedest->write(whichcolumn->unsignednumber);
			cachedest->write(whichcolumn->zerofill);
			cachedest->write(whichcolumn->binary);
			cachedest->write(whichcolumn->autoincrement);
		}
	}
}

void sqlrcursor::cacheOutputBinds(uint32_t count) {

	if (resumed || !cachedest) {
		return;
	}

	// write the variable/value pairs to the cache file
	uint16_t	len;
	for (uint32_t i=0; i<count; i++) {

		cachedest->write((uint16_t)(*outbindvars)[i].type);

		len=charstring::length((*outbindvars)[i].variable);
		cachedest->write(len);
		cachedest->write((*outbindvars)[i].variable,len);

		len=(*outbindvars)[i].resultvaluesize;
		cachedest->write(len);
		if ((*outbindvars)[i].type==BINDVARTYPE_STRING ||
				(*outbindvars)[i].type==BINDVARTYPE_BLOB ||
				(*outbindvars)[i].type==BINDVARTYPE_CLOB) {
			cachedest->write((*outbindvars)[i].value.stringval,len);
			cachedest->write((*outbindvars)[i].value.lobval,len);
		} else if ((*outbindvars)[i].type==BINDVARTYPE_INTEGER) {
			cachedest->write((*outbindvars)[i].value.integerval);
		} else if ((*outbindvars)[i].type==BINDVARTYPE_DOUBLE) {
			cachedest->write((*outbindvars)[i].value.
						doubleval.value);
			cachedest->write((*outbindvars)[i].value.
						doubleval.precision);
			cachedest->write((*outbindvars)[i].value.
						doubleval.scale);
		}
	}

	// terminate the list of output binds
	cachedest->write((uint16_t)END_BIND_VARS);
}

void sqlrcursor::cacheData() {

	if (!cachedest) {
		return;
	}

	// write the data to the cache file
	uint32_t	rowbuffercount=rowcount-firstrowindex;
	for (uint32_t i=0; i<rowbuffercount; i++) {

		// get the current offset in the cache destination file
		int64_t	position=cachedest->getCurrentPosition();

		// seek to the right place in the index file and write the
		// destination file offset
		cachedestind->setPositionRelativeToBeginning(
			13+sizeof(int64_t)+((firstrowindex+i)*sizeof(int64_t)));
		cachedestind->write(position);

		// write the row to the cache file
		for (uint32_t j=0; j<colcount; j++) {
			uint16_t	type;
			int32_t		len;
			char		*field=getFieldInternal(i,j);
			if (field) {
				type=STRING_DATA;
				len=charstring::length(field);
				cachedest->write(type);
				cachedest->write(len);
				if (len>0) {
					cachedest->write(field);
				}
			} else {
				type=NULL_DATA;
				cachedest->write(type);
			}
		}
	}

	if (endofresultset) {
		finishCaching();
	}
}

void sqlrcursor::finishCaching() {

	if (!cachedest) {
		return;
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Finishing caching.\n");
		sqlrc->debugPreEnd();
	}

	// terminate the result set
	cachedest->write((uint16_t)END_RESULT_SET);
	// FIXME: I think rudiments bugs keep this from working...
	/*cachedest->flushWriteBuffer(-1,-1);
	cachedestind->flushWriteBuffer(-1,-1);*/

	// close the cache file and clean up
	clearCacheDest();
}

void sqlrcursor::clearCacheDest() {

	// close the cache file and clean up
	if (cachedest) {
		cachedest->close();
		delete cachedest;
		cachedest=NULL;
		cachedestind->close();
		delete cachedestind;
		cachedestind=NULL;
		cacheon=false;
	}
}

bool sqlrcursor::getDatabaseList(const char *wild) {
	return getDatabaseList(wild,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getDatabaseList(const char *wild,
					sqlrclientlistformat_t listformat) {
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("getting database list");
		if (wild) {
			sqlrc->debugPrint("\"");
			sqlrc->debugPrint(wild);
			sqlrc->debugPrint("\"");
		}
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}
	return getList(GETDBLIST,listformat,NULL,wild);
}

bool sqlrcursor::getTableList(const char *wild) {
	return getTableList(wild,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getTableList(const char *wild,
					sqlrclientlistformat_t listformat) {
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("getting table list");
		if (wild) {
			sqlrc->debugPrint("\"");
			sqlrc->debugPrint(wild);
			sqlrc->debugPrint("\"");
		}
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}
	return getList(GETTABLELIST,listformat,NULL,wild);
}

bool sqlrcursor::getColumnList(const char *table, const char *wild) {
	return getColumnList(table,wild,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getColumnList(const char *table,
				const char *wild,
				sqlrclientlistformat_t listformat) {
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("getting column list for: \"");
		sqlrc->debugPrint(table);
		sqlrc->debugPrint("\"");
		if (wild) {
			sqlrc->debugPrint(" - \"");
			sqlrc->debugPrint(wild);
			sqlrc->debugPrint("\"");
		}
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}
	return getList(GETCOLUMNLIST,listformat,(table)?table:"",wild);
}

bool sqlrcursor::getList(uint16_t command, sqlrclientlistformat_t listformat,
					const char *table, const char *wild) {

	reexecute=false;
	validatebinds=false;
	resumed=false;
	clearVariables();

	if (!endofresultset) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!sqlrc->openSession()) {
		return false;
	}

	cached=false;
	endofresultset=false;

	// tell the server we want to get a db list
	sqlrc->cs->write(command);

	// tell the server whether we'll need a cursor or not
	sendCursorStatus();

	// send the list format
	sqlrc->cs->write((uint16_t)listformat);

	// send the wild parameter
	uint32_t	len=charstring::length(wild);
	sqlrc->cs->write(len);
	if (len) {
		sqlrc->cs->write(wild,len);
	}

	// send the table parameter
	if (table) {
		len=charstring::length(table);
		sqlrc->cs->write(len);
		if (len) {
			sqlrc->cs->write(table,len);
		}
	}

	sqlrc->flushWriteBuffer();

	// process the result set
	bool	retval=true;
	if (rsbuffersize) {
		if (!processResultSet(false,rsbuffersize-1)) {
			retval=false;
		}
	} else {
		if (!processResultSet(true,0)) {
			retval=false;
		}
	}

	// set up not to re-execute the same query if executeQuery is called
	// again before calling prepareQuery on a new query
	reexecute=false;

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
	reexecute=false;
	validatebinds=false;
	resumed=false;
	clearVariables();
	querylen=length;
	if (copyrefs) {
		initQueryBuffer(querylen);
		charstring::copy(querybuffer,query,querylen);
		querybuffer[querylen]='\0';
	} else {
		queryptr=query;
	}
}

bool sqlrcursor::prepareFileQuery(const char *path, const char *filename) {

	// init some variables
	reexecute=false;
	validatebinds=false;
	resumed=false;
	clearVariables();

	// init the fullpath buffer
	if (!fullpath) {
		fullpath=new char[MAXPATHLEN+1];
	}

	// add the path to the fullpath
	uint32_t	index=0;
	uint32_t	counter=0;
	if (path) {
		while (path[index] && counter<MAXPATHLEN) {
			fullpath[counter]=path[index];
			index++;
			counter++;
		}

		// add the "/" to the fullpath
		if (counter<=MAXPATHLEN) {
			fullpath[counter]='/';
			counter++;
		}
	}

	// add the file to the fullpath
	index=0;
	while (filename[index] && counter<MAXPATHLEN) {
		fullpath[counter]=filename[index];
		index++;
		counter++;
	}

	// handle a filename that's too long
	if (counter>MAXPATHLEN) {

		// sabotage the file name so it can't be opened
		fullpath[0]='\0';

		// debug info
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("File name ");
			if (path) {
				sqlrc->debugPrint((char *)path);
				sqlrc->debugPrint("/");
			}
			sqlrc->debugPrint((char *)filename);
			sqlrc->debugPrint(" is too long.");
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

	} else {

		// terminate the string
		fullpath[counter]='\0';

		// debug info
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("File: ");
			sqlrc->debugPrint(fullpath);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}
	}

	// open the file
	file	queryfile;
	if (!queryfile.open(fullpath,O_RDONLY)) {

		// set the error
		char	*err=new char[32+charstring::length(fullpath)];
		charstring::append(err,"The file ");
		charstring::append(err,fullpath);
		charstring::append(err," could not be opened.\n");
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint(err);
			sqlrc->debugPreEnd();
		}
		setError(err);

		// set queryptr to NULL so executeQuery won't try to do
		// anything with it in the event that it gets called
		queryptr=NULL;

		delete[] err;

		return false;
	}

	initQueryBuffer(queryfile.getSize());

	// read the file into the query buffer
	querylen=queryfile.getSize();
	queryfile.read((unsigned char *)querybuffer,querylen);
	querybuffer[querylen]='\0';

	queryfile.close();

	return true;
}

void sqlrcursor::initQueryBuffer(uint32_t querylength) {
	delete[] querybuffer;
	querybuffer=new char[querylength+1];
	queryptr=querybuffer;
}

void sqlrcursor::attachToBindCursor(uint16_t bindcursorid) {
	prepareQuery("");
	reexecute=true;
	cursorid=bindcursorid;
}

uint16_t sqlrcursor::countBindVariables() const {

	if (!queryptr) {
		return 0;
	}

	char	lastchar='\0';
	bool	inquotes=false;

	uint16_t	questionmarkcount=0;
	uint16_t	coloncount=0;
	uint16_t	atsigncount=0;
	uint16_t	dollarsigncount=0;

	for (const char *ptr=queryptr; *ptr; ptr++) {

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
	subvars->clear();
	dirtysubs=false;
	dirtybinds=false;
	clearBinds();
}

void sqlrcursor::deleteVariables() {
	deleteSubstitutionVariables();
	deleteInputBindVariables();
	deleteOutputBindVariables();
}

void sqlrcursor::deleteSubstitutionVariables() {

	if (copyrefs) {
		for (uint64_t i=0; i<subvars->getLength(); i++) {
			delete[] (*subvars)[i].variable;
			if ((*subvars)[i].type==BINDVARTYPE_STRING) {
				delete[] (*subvars)[i].value.stringval;
			}
			if ((*subvars)[i].type==BINDVARTYPE_DATE) {
				delete[] (*subvars)[i].value.dateval.tz;
			}
		}
	}
}

void sqlrcursor::deleteInputBindVariables() {

	if (copyrefs) {
		for (uint64_t i=0; i<inbindvars->getLength(); i++) {
			delete[] (*inbindvars)[i].variable;
			if ((*inbindvars)[i].type==BINDVARTYPE_STRING) {
				delete[] (*inbindvars)[i].value.stringval;
			}
			if ((*inbindvars)[i].type==BINDVARTYPE_BLOB ||
				(*inbindvars)[i].type==BINDVARTYPE_CLOB) {
				delete[] (*inbindvars)[i].value.lobval;
			}
		}
	}
}

void sqlrcursor::deleteOutputBindVariables() {

	for (uint64_t i=0; i<outbindvars->getLength(); i++) {
		if (copyrefs) {
			delete[] (*outbindvars)[i].variable;
		}
		if ((*outbindvars)[i].type==BINDVARTYPE_STRING) {
			delete[] (*outbindvars)[i].value.stringval;
		}
		if ((*outbindvars)[i].type==BINDVARTYPE_BLOB ||
			(*outbindvars)[i].type==BINDVARTYPE_CLOB) {
			delete[] (*outbindvars)[i].value.lobval;
		}
	}
}

void sqlrcursor::substitution(const char *variable, const char *value) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,subvars);
	if (!bv) {
		bv=&(*subvars)[subvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	stringVar(bv,variable,value);
	dirtysubs=true;
}

void sqlrcursor::substitution(const char *variable, int64_t value) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,subvars);
	if (!bv) {
		bv=&(*subvars)[subvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	integerVar(bv,variable,value);
	dirtysubs=true;
}

void sqlrcursor::substitution(const char *variable, double value, 
				uint32_t precision, uint32_t scale) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,subvars);
	if (!bv) {
		bv=&(*subvars)[subvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	doubleVar(bv,variable,value,precision,scale);
	dirtysubs=true;
}

void sqlrcursor::clearBinds() {

	deleteInputBindVariables();
	inbindvars->clear();

	deleteOutputBindVariables();
	outbindvars->clear();
}

void sqlrcursor::inputBindBlob(const char *variable, const char *value,
							uint32_t size) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,inbindvars);
	if (!bv) {
		bv=&(*inbindvars)[inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	lobVar(bv,variable,value,size,BINDVARTYPE_BLOB);
	bv->send=true;
	dirtybinds=true;
}

void sqlrcursor::inputBindClob(const char *variable, const char *value,
							uint32_t size) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,inbindvars);
	if (!bv) {
		bv=&(*inbindvars)[inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	lobVar(bv,variable,value,size,BINDVARTYPE_CLOB);
	bv->send=true;
	dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable, const char *value) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,inbindvars);
	if (!bv) {
		bv=&(*inbindvars)[inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	stringVar(bv,variable,value);
	bv->send=true;
	dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable, const char *value,
						uint32_t valuesize) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,inbindvars);
	if (!bv) {
		bv=&(*inbindvars)[inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	stringVar(bv,variable,value,valuesize);
	bv->send=true;
	dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable, int64_t value) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,inbindvars);
	if (!bv) {
		bv=&(*inbindvars)[inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	integerVar(bv,variable,value);
	bv->send=true;
	dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable, double value, 
				uint32_t precision, uint32_t scale) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,inbindvars);
	if (!bv) {
		bv=&(*inbindvars)[inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	doubleVar(bv,variable,value,precision,scale);
	bv->send=true;
	dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable,
				int16_t year, int16_t month, int16_t day,
				int16_t hour, int16_t minute, int16_t second,
				int32_t microsecond, const char *tz) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool	preexisting=true;
	bindvar	*bv=findVar(variable,inbindvars);
	if (!bv) {
		bv=&(*inbindvars)[inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	dateVar(bv,variable,year,month,day,hour,minute,second,microsecond,tz);
	bv->send=true;
	dirtybinds=true;
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
		if (copyrefs) {
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
	if (copyrefs) {
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
		if (copyrefs) {
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
	if (copyrefs) {
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

	bindvar	*bv=findVar(variable,outbindvars);
	bool	preexisting=true;
	if (!bv) {
		bv=&(*outbindvars)[outbindvars->getLength()];
		preexisting=false;
		dirtybinds=true;
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
	if (copyrefs) {
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
		for (uint64_t i=0; i<outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*outbindvars)[i].variable,variable) &&
				(*outbindvars)[i].type==BINDVARTYPE_STRING) {
				return (*outbindvars)[i].value.stringval;
			}
		}
	}
	return NULL;
}

uint32_t sqlrcursor::getOutputBindLength(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*outbindvars)[i].variable,variable)) {
				return (*outbindvars)[i].resultvaluesize;
			}
		}
	}
	return 0;
}

const char *sqlrcursor::getOutputBindBlob(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*outbindvars)[i].variable,variable) &&
				(*outbindvars)[i].type==BINDVARTYPE_BLOB) {
				return (*outbindvars)[i].value.lobval;
			}
		}
	}
	return NULL;
}

const char *sqlrcursor::getOutputBindClob(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*outbindvars)[i].variable,variable) &&
				(*outbindvars)[i].type==BINDVARTYPE_CLOB) {
				return (*outbindvars)[i].value.lobval;
			}
		}
	}
	return NULL;
}

int64_t sqlrcursor::getOutputBindInteger(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*outbindvars)[i].variable,variable) &&
				(*outbindvars)[i].type==BINDVARTYPE_INTEGER) {
				return (*outbindvars)[i].value.integerval;
			}
		}
	}
	return -1;
}

double sqlrcursor::getOutputBindDouble(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*outbindvars)[i].variable,variable) &&
				(*outbindvars)[i].type==BINDVARTYPE_DOUBLE) {
				return (*outbindvars)[i].value.doubleval.value;
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
		for (uint64_t i=0; i<outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*outbindvars)[i].variable,variable) &&
				(*outbindvars)[i].type==BINDVARTYPE_DATE) {
				*year=(*outbindvars)[i].value.dateval.year;
				*month=(*outbindvars)[i].value.dateval.month;
				*day=(*outbindvars)[i].value.dateval.day;
				*hour=(*outbindvars)[i].value.dateval.hour;
				*minute=(*outbindvars)[i].value.dateval.minute;
				*second=(*outbindvars)[i].value.dateval.second;
				*microsecond=(*outbindvars)[i].
						value.dateval.microsecond;
				*tz=(*outbindvars)[i].value.dateval.tz;
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
	sqlrcursor	*bindcursor=new sqlrcursor(sqlrc,copyrefs);
	bindcursor->attachToBindCursor(bindcursorid);
	return bindcursor;
}

bool sqlrcursor::outputBindCursorIdIsValid(const char *variable) {
	if (variable) {
		for (uint64_t i=0; i<outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*outbindvars)[i].variable,variable)) {
				return true;
			}
		}
	}
	return false;
}

uint16_t sqlrcursor::getOutputBindCursorId(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*outbindvars)[i].variable,variable)) {
				return (*outbindvars)[i].value.cursorid;
			}
		}
	}
	return 0;
}

void sqlrcursor::validateBinds() {
	validatebinds=true;
}

bool sqlrcursor::validBind(const char *variable) {
	performSubstitutions();
	validateBindsInternal();
	for (uint64_t in=0; in<inbindvars->getLength(); in++) {
		if (!charstring::compare(
			(*inbindvars)[in].variable,variable)) {
			return (*inbindvars)[in].send;
		}
	}
	for (uint64_t out=0; out<outbindvars->getLength(); out++) {
		if (!charstring::compare(
			(*outbindvars)[out].variable,variable)) {
			return (*outbindvars)[out].send;
		}
	}
	return false;
}

bool sqlrcursor::executeQuery() {

	if (!queryptr) {
		setError("No query to execute.");
		return false;
	}

	performSubstitutions();

	// validate the bind variables
	if (validatebinds) {
		validateBindsInternal();
	}
		
	// run the query
	bool	retval=runQuery(queryptr);

	// set up to re-execute the same query if executeQuery is called
	// again before calling prepareQuery
	reexecute=true;

	return retval;
}

void sqlrcursor::performSubstitutions() {

	if (!subvars->getLength() || !dirtysubs) {
		return;
	}

	// perform substitutions
	stringbuffer	container;
	const char	*ptr=queryptr;
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
				i<subvars->getLength() && !found; i++) {

	
				// if we find a match, write the 
				// value to the container and skip 
				// past the $(variable)
				len=charstring::length(
						(*subvars)[i].variable);
				if (!(*subvars)[i].donesubstituting &&
					!charstring::compare((ptr+2),
						(*subvars)[i].variable,len) &&
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
	for (uint64_t i=0; i<subvars->getLength(); i++) {
		(*subvars)[i].donesubstituting=(*subvars)[i].substituted;
	}

	delete[] querybuffer;
	querylen=container.getStringLength();
	querybuffer=container.detachString();
	queryptr=querybuffer;

	dirtysubs=false;
}

void sqlrcursor::validateBindsInternal() {

	if (!dirtybinds) {
		return;
	}

	// some useful variables
	const char	*ptr;
	const char	*start;
	const char	*after;
	bool		found;
	int		len;

	// check each input bind
	for (uint64_t in=0; in<inbindvars->getLength(); in++) {

		// don't check bind-by-position variables
		len=charstring::length((*inbindvars)[in].variable);
		if (charstring::isInteger((*inbindvars)[in].variable,len)) {
			continue;
		}

		found=false;
		start=queryptr+1;

		// there may be more than 1 match for the variable name as in
		// "select * from table where table_name=:table_name", both
		// table_name's would match, but only the second is a bind
		// variable
		while ((ptr=charstring::findFirst(start,
					(*inbindvars)[in].variable))) {

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

		(*inbindvars)[in].send=found;
	}

	// check each output bind
	for (uint64_t out=0; out<outbindvars->getLength(); out++) {

		// don't check bind-by-position variables
		len=charstring::length((*outbindvars)[out].variable);
		if (charstring::isInteger((*outbindvars)[out].variable,len)) {
			continue;
		}

		found=false;
		start=queryptr+1;

		// there may be more than 1 match for the variable name as in
		// "select * from table where table_name=:table_name", both
		// table_name's would match, but only 1 is correct
		while ((ptr=charstring::findFirst(start,
					(*outbindvars)[out].variable))) {

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

		(*outbindvars)[out].send=found;
	}
}

void sqlrcursor::performSubstitution(stringbuffer *buffer, uint16_t which) {

	if ((*subvars)[which].type==BINDVARTYPE_STRING) {
		buffer->append((*subvars)[which].value.stringval);
	} else if ((*subvars)[which].type==BINDVARTYPE_INTEGER) {
		buffer->append((*subvars)[which].value.integerval);
	} else if ((*subvars)[which].type==BINDVARTYPE_DOUBLE) {
		buffer->append((*subvars)[which].value.doubleval.value,
				(*subvars)[which].value.doubleval.precision,
				(*subvars)[which].value.doubleval.scale);
	}
	(*subvars)[which].substituted=true;
}

bool sqlrcursor::runQuery(const char *query) {

	// send the query
	if (sendQueryInternal(query)) {

		sendInputBinds();
		sendOutputBinds();
		sendGetColumnInfo();

		sqlrc->flushWriteBuffer();

		if (rsbuffersize) {
			if (processResultSet(false,rsbuffersize-1)) {
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
		sqlrc->debugOn();
	}

	if (!endofresultset) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!sqlrc->openSession()) {
		return false;
	}

	cached=false;
	endofresultset=false;

	// send the query to the server.
	if (!reexecute) {

		// tell the server we're sending a query
		sqlrc->cs->write((uint16_t)NEW_QUERY);

		// tell the server whether we'll need a cursor or not
		sendCursorStatus();

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Sending Client Info:");
			sqlrc->debugPrint("\n");
			sqlrc->debugPrint("Length: ");
			sqlrc->debugPrint((int64_t)sqlrc->clientinfolen);
			sqlrc->debugPrint("\n");
			sqlrc->debugPrint(sqlrc->clientinfo);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		// send the client info
		// FIXME: arguably this should be its own command
		sqlrc->cs->write(sqlrc->clientinfolen);
		sqlrc->cs->write(sqlrc->clientinfo,sqlrc->clientinfolen);

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Sending Query:");
			sqlrc->debugPrint("\n");
			sqlrc->debugPrint("Length: ");
			sqlrc->debugPrint((int64_t)querylen);
			sqlrc->debugPrint("\n");
			sqlrc->debugPrint(query);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		// send the query
		sqlrc->cs->write(querylen);
		sqlrc->cs->write(query,querylen);

	} else {

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Requesting re-execution of ");
			sqlrc->debugPrint("previous query.");
			sqlrc->debugPrint("\n");
			sqlrc->debugPrint("Requesting Cursor: ");
			sqlrc->debugPrint((int64_t)cursorid);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		// tell the server we're sending a query
		sqlrc->cs->write((uint16_t)REEXECUTE_QUERY);

		// send the cursor id to the server
		sqlrc->cs->write(cursorid);
	}

	return true;
}

void sqlrcursor::sendCursorStatus() {

	if (havecursorid) {

		// tell the server we already have a cursor
		sqlrc->cs->write((uint16_t)DONT_NEED_NEW_CURSOR);

		// send the cursor id to the server
		sqlrc->cs->write(cursorid);

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Requesting Cursor: ");
			sqlrc->debugPrint((int64_t)cursorid);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

	} else {

		// tell the server we need a cursor
		sqlrc->cs->write((uint16_t)NEED_NEW_CURSOR);

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Requesting a new cursor.\n");
			sqlrc->debugPreEnd();
		}
	}
}

void sqlrcursor::sendInputBinds() {

	// index
	uint16_t	i=0;

	// count number of vars to send
	uint16_t	count=inbindvars->getLength();
	for (i=0; i<count; i++) {
		if (!(*inbindvars)[i].send) {
			count--;
		}
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Sending ");
		sqlrc->debugPrint((int64_t)count);
		sqlrc->debugPrint(" Input Bind Variables:\n");
		sqlrc->debugPreEnd();
	}

	// write the input bind variables/values to the server.
	sqlrc->cs->write(count);
	uint16_t	size;
	i=0;
	while (i<count) {

		// don't send anything if the send flag is turned off
		if (!(*inbindvars)[i].send) {
			continue;
		}

		// send the variable
		size=charstring::length((*inbindvars)[i].variable);
		sqlrc->cs->write(size);
		sqlrc->cs->write((*inbindvars)[i].variable,(size_t)size);
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint((*inbindvars)[i].variable);
			sqlrc->debugPrint("(");
			sqlrc->debugPrint((int64_t)size);
		}

		// send the type
		sqlrc->cs->write((uint16_t)(*inbindvars)[i].type);

		// send the value
		if ((*inbindvars)[i].type==BINDVARTYPE_NULL) {

			if (sqlrc->debug) {
				sqlrc->debugPrint(":NULL)\n");
				sqlrc->debugPreEnd();
			}

		} else if ((*inbindvars)[i].type==BINDVARTYPE_STRING) {

			sqlrc->cs->write((*inbindvars)[i].valuesize);
			if ((*inbindvars)[i].valuesize>0) {
				sqlrc->cs->write((*inbindvars)[i].
							value.stringval,
					(size_t)(*inbindvars)[i].valuesize);
			}

			if (sqlrc->debug) {
				sqlrc->debugPrint(":STRING)=");
				sqlrc->debugPrint((*inbindvars)[i].
							value.stringval);
				sqlrc->debugPrint("(");
				sqlrc->debugPrint((int64_t)(*inbindvars)[i].
								valuesize);
				sqlrc->debugPrint(")");
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		} else if ((*inbindvars)[i].type==BINDVARTYPE_INTEGER) {

			sqlrc->cs->write((uint64_t)(*inbindvars)[i].
							value.integerval);

			if (sqlrc->debug) {
				sqlrc->debugPrint(":LONG)=");
				sqlrc->debugPrint((int64_t)(*inbindvars)[i].
							value.integerval);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		} else if ((*inbindvars)[i].type==BINDVARTYPE_DOUBLE) {

			sqlrc->cs->write((double)(*inbindvars)[i].value.
							doubleval.value);
			sqlrc->cs->write((*inbindvars)[i].value.
							doubleval.precision);
			sqlrc->cs->write((*inbindvars)[i].value.
							doubleval.scale);

			if (sqlrc->debug) {
				sqlrc->debugPrint(":DOUBLE)=");
				sqlrc->debugPrint((*inbindvars)[i].value.
							doubleval.value);
				sqlrc->debugPrint(":");
				sqlrc->debugPrint((int64_t)(*inbindvars)[i].
						value.doubleval.precision);
				sqlrc->debugPrint(",");
				sqlrc->debugPrint((int64_t)(*inbindvars)[i].
						value.doubleval.scale);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		} else if ((*inbindvars)[i].type==BINDVARTYPE_DATE) {

			sqlrc->cs->write((uint16_t)
					(*inbindvars)[i].value.dateval.year);
			sqlrc->cs->write((uint16_t)
					(*inbindvars)[i].value.dateval.month);
			sqlrc->cs->write((uint16_t)
					(*inbindvars)[i].value.dateval.day);
			sqlrc->cs->write((uint16_t)
					(*inbindvars)[i].value.dateval.hour);
			sqlrc->cs->write((uint16_t)
					(*inbindvars)[i].value.dateval.minute);
			sqlrc->cs->write((uint16_t)
					(*inbindvars)[i].value.dateval.second);
			sqlrc->cs->write((uint32_t)
					(*inbindvars)[i].value.
							dateval.microsecond);
			sqlrc->cs->write((uint16_t)
					charstring::length(
					(*inbindvars)[i].value.dateval.tz));
			sqlrc->cs->write((*inbindvars)[i].value.dateval.tz);

			if (sqlrc->debug) {
				sqlrc->debugPrint(":DATE)=");
				sqlrc->debugPrint((int64_t)
					(*inbindvars)[i].value.dateval.year);
				sqlrc->debugPrint("-");
				sqlrc->debugPrint((int64_t)
					(*inbindvars)[i].value.dateval.month);
				sqlrc->debugPrint("-");
				sqlrc->debugPrint((int64_t)
					(*inbindvars)[i].value.dateval.day);
				sqlrc->debugPrint(" ");
				sqlrc->debugPrint((int64_t)
					(*inbindvars)[i].value.dateval.hour);
				sqlrc->debugPrint(":");
				sqlrc->debugPrint((int64_t)
					(*inbindvars)[i].value.dateval.minute);
				sqlrc->debugPrint(":");
				sqlrc->debugPrint((int64_t)
					(*inbindvars)[i].value.dateval.second);
				sqlrc->debugPrint(":");
				sqlrc->debugPrint((int64_t)
					(*inbindvars)[i].value.
						dateval.microsecond);
				sqlrc->debugPrint(" ");
				sqlrc->debugPrint(
					(*inbindvars)[i].value.dateval.tz);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		} else if ((*inbindvars)[i].type==BINDVARTYPE_BLOB ||
				(*inbindvars)[i].type==BINDVARTYPE_CLOB) {

			sqlrc->cs->write((*inbindvars)[i].valuesize);
			if ((*inbindvars)[i].valuesize>0) {
				sqlrc->cs->write((*inbindvars)[i].
					value.lobval,
					(size_t)(*inbindvars)[i].valuesize);
			}

			if (sqlrc->debug) {
				if ((*inbindvars)[i].type==
							BINDVARTYPE_BLOB) {
					sqlrc->debugPrint(":BLOB)=");
					sqlrc->debugPrintBlob(
						(*inbindvars)[i].value.lobval,
						(*inbindvars)[i].valuesize);
				} else if ((*inbindvars)[i].type==
							BINDVARTYPE_CLOB) {
					sqlrc->debugPrint(":CLOB)=");
					sqlrc->debugPrintClob(
						(*inbindvars)[i].value.lobval,
						(*inbindvars)[i].valuesize);
				}
				sqlrc->debugPrint("(");
				sqlrc->debugPrint((int64_t)(*inbindvars)[i].
								valuesize);
				sqlrc->debugPrint(")");
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}
		}

		i++;
	}
}

void sqlrcursor::sendOutputBinds() {

	// index
	uint16_t	i=0;

	// count number of vars to send
	uint16_t	count=outbindvars->getLength();
	for (i=0; i<count; i++) {
		if (!(*outbindvars)[i].send) {
			count--;
		}
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Sending ");
		sqlrc->debugPrint((int64_t)count);
		sqlrc->debugPrint(" Output Bind Variables:\n");
		sqlrc->debugPreEnd();
	}

	// write the output bind variables to the server.
	sqlrc->cs->write(count);
	uint16_t	size;
	i=0;
	while (i<count) {

		// don't send anything if the send flag is turned off
		if (!(*outbindvars)[i].send) {
			continue;
		}

		// send the variable, type and size that the buffer needs to be
		size=charstring::length((*outbindvars)[i].variable);
		sqlrc->cs->write(size);
		sqlrc->cs->write((*outbindvars)[i].variable,(size_t)size);
		sqlrc->cs->write((uint16_t)(*outbindvars)[i].type);
		if ((*outbindvars)[i].type==BINDVARTYPE_STRING ||
			(*outbindvars)[i].type==BINDVARTYPE_BLOB ||
			(*outbindvars)[i].type==BINDVARTYPE_CLOB ||
			(*outbindvars)[i].type==BINDVARTYPE_NULL) {
			sqlrc->cs->write((*outbindvars)[i].valuesize);
		}

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint((*outbindvars)[i].variable);
			const char	*bindtype=NULL;
			switch ((*outbindvars)[i].type) {
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
			sqlrc->debugPrint(bindtype);
			if ((*outbindvars)[i].type==BINDVARTYPE_STRING ||
				(*outbindvars)[i].type==BINDVARTYPE_BLOB ||
				(*outbindvars)[i].type==BINDVARTYPE_CLOB ||
				(*outbindvars)[i].type==BINDVARTYPE_NULL) {
				sqlrc->debugPrint("(");
				sqlrc->debugPrint((int64_t)(*outbindvars)[i].
								valuesize);
				sqlrc->debugPrint(")");
			}
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		i++;
	}
}

void sqlrcursor::sendGetColumnInfo() {

	if (sendcolumninfo==SEND_COLUMN_INFO) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Send Column Info: yes\n");
			sqlrc->debugPreEnd();
		}
		sqlrc->cs->write((uint16_t)SEND_COLUMN_INFO);
	} else {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Send Column Info: no\n");
			sqlrc->debugPreEnd();
		}
		sqlrc->cs->write((uint16_t)DONT_SEND_COLUMN_INFO);
	}
}

bool sqlrcursor::processResultSet(bool getallrows, uint64_t rowtoget) {

	// start caching the result set
	if (cacheon) {
		startCaching();
	}

	// parse the columninfo and data
	bool	success=true;

	// skip and fetch here if we're not reading from a cached result set
	// this way, everything gets done in 1 round trip
	if (!cachesource) {
		success=skipAndFetch(getallrows,firstrowindex+rowtoget);
	}

	// check for an error
	if (success) {

		uint16_t	err=getErrorStatus();
		if (err!=NO_ERROR_OCCURRED) {

			// if there was a timeout, then end
			// the session and bail immediately
			if (err==TIMEOUT_GETTING_ERROR_STATUS) {
				sqlrc->endSession();
				return false;
			}

			// otherwise, get the error from the server
			getErrorFromServer();

			// don't get the cursor if the error was that there
			// were no cursors available
			if (errorno!=SQLR_ERROR_NOCURSORS) {
				getCursorId();
			}

			// if we need to disconnect then end the session
			if (err==ERROR_OCCURRED_DISCONNECT) {
				sqlrc->endSession();
			}
			return false;
		}
	}

	// get data back from the server
	if (success && ((cachesource && cachesourceind) ||
			((!cachesource && !cachesourceind)  && 
				(success=getCursorId()) && 
				(success=getSuspended()))) &&
			(success=parseColumnInfo()) && 
			(success=parseOutputBinds())) {

		// skip and fetch here if we're reading from a cached result set
		if (cachesource) {
			success=skipAndFetch(getallrows,firstrowindex+rowtoget);
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
		sqlrc->endSession();
	}
	return success;
}

bool sqlrcursor::skipAndFetch(bool getallrows, uint64_t rowtoget) {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Skipping and Fetching\n");
		if (!getallrows) {
			sqlrc->debugPrint("	row to get: ");
			sqlrc->debugPrint((int64_t)rowtoget);
			sqlrc->debugPrint("\n");
		}
		sqlrc->debugPreEnd();
	}

	// if we're stepping through the result set, we can possibly 
	// skip a big chunk of it...
	if (!skipRows(getallrows,rowtoget)) {
		return false;
	}

	// tell the connection how many rows to send
	fetchRows();

	sqlrc->flushWriteBuffer();
	return true;
}

void sqlrcursor::fetchRows() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Fetching ");
		sqlrc->debugPrint((int64_t)rsbuffersize);
		sqlrc->debugPrint(" rows\n");
		sqlrc->debugPreEnd();
	}

	// if we're reading from a cached result set, do nothing
	if (cachesource && cachesourceind) {
		return;
	}

	// otherwise, send to the connection the number of rows to send back
	sqlrc->cs->write(rsbuffersize);
}

bool sqlrcursor::skipRows(bool getallrows, uint64_t rowtoget) {

	// if we're reading from a cached result set we have to manually skip
	if (cachesource && cachesourceind) {

		// skip to the next block of rows
		if (getallrows) {
			return true;
		} else {
			rowcount=rowtoget-(rowtoget%rsbuffersize);
		}

		// get the row offset from the index
		cachesourceind->setPositionRelativeToBeginning(
				13+sizeof(int64_t)+(rowcount*sizeof(int64_t)));
		int64_t	rowoffset;
		if (cachesourceind->read(&rowoffset)!=sizeof(int64_t)) {
			setError("The cache file index appears to be corrupt.");
			return false;
		}

		// skip to that offset in the cache file
		cachesource->setPositionRelativeToBeginning(rowoffset);
		return true;
	}

	// calculate how many rows to skip unless we're buffering the entire
	// result set or caching the result set
	uint64_t	skip=0;
	if (rsbuffersize && !cachedest && !getallrows) {
		skip=(rowtoget-(rowtoget%rsbuffersize))-rowcount; 
		rowcount=rowcount+skip;
	}
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Skipping ");
		sqlrc->debugPrint((int64_t)skip);
		sqlrc->debugPrint(" rows\n");
		sqlrc->debugPreEnd();
	}

	// if we're reading from a connection, send the connection the 
	// number of rows to skip
	sqlrc->cs->write(skip);
	return true;
}

uint16_t sqlrcursor::getErrorStatus() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Checking For An Error...\n");
		sqlrc->debugPreEnd();
	}

	// get a flag indicating whether there's been an error or not
	uint16_t	err;
	int32_t	result=getShort(&err,sqlrc->responsetimeoutsec,
					sqlrc->responsetimeoutusec);
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
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("none.\n");
			sqlrc->debugPreEnd();
		}
		cacheNoError();
		return NO_ERROR_OCCURRED;
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("error!!!\n");
		sqlrc->debugPreEnd();
	}
	return err;
}

bool sqlrcursor::getCursorId() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Getting Cursor ID...\n");
		sqlrc->debugPreEnd();
	}
	if (sqlrc->cs->read(&cursorid)!=sizeof(uint16_t)) {
		if (!error) {
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
	havecursorid=true;
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Cursor ID: ");
		sqlrc->debugPrint((int64_t)cursorid);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}
	return true;
}

bool sqlrcursor::getSuspended() {

	// see if the result set of that cursor is actually suspended
	uint16_t	suspendedresultset;
	if (sqlrc->cs->read(&suspendedresultset)!=sizeof(uint16_t)) {
		setError("Failed to determine whether "
			"the session was suspended or not.\n "
			"A network error may have ocurred.");
		return false;
	}

	if (suspendedresultset==SUSPENDED_RESULT_SET) {

		// If it was suspended the server will send the index of the 
		// last row from the previous result set.
		// Initialize firstrowindex and rowcount from this index.
		if (sqlrc->cs->read(&firstrowindex)!=sizeof(uint64_t)) {
			setError("Failed to get the index of the "
				"last row of a previously suspended result "
				"set.\n A network error may have ocurred.");
			return false;
		}
		rowcount=firstrowindex+1;
	
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Previous result set was ");
	       		sqlrc->debugPrint("suspended at row index: ");
			sqlrc->debugPrint((int64_t)firstrowindex);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

	} else {

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Previous result set was ");
	       		sqlrc->debugPrint("not suspended.\n");
			sqlrc->debugPreEnd();
		}
	}
	return true;
}

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
			currentcol->name=(char *)colstorage->allocate(length+1);
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

bool sqlrcursor::parseOutputBinds() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Receiving Output Bind Values: \n");
		sqlrc->debugPreEnd();
	}

	// useful variables
	uint16_t	type;
	uint32_t	length;
	uint16_t	count=0;

	// get the bind values
	for (;;) {

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("	getting type...\n");
			sqlrc->debugPreEnd();
		}

		// get the data type
		if (getShort(&type)!=sizeof(uint16_t)) {
			setError("Failed to get data type.\n "
				"A network error may have occurred.");

			return false;
		}

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("	done getting type: ");
			sqlrc->debugPrint((int64_t)type);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		// check for end of bind values
		if (type==END_BIND_VARS) {

			break;

		} else if (type==NULL_DATA) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	NULL output bind\n");
				sqlrc->debugPreEnd();
			}

			// handle a null value
			(*outbindvars)[count].resultvaluesize=0;
			if ((*outbindvars)[count].type==BINDVARTYPE_STRING) {
				if (returnnulls) {
					(*outbindvars)[count].value.
							stringval=NULL;
				} else {
					(*outbindvars)[count].value.
							stringval=new char[1];
					(*outbindvars)[count].value.
							stringval[0]='\0';
				}
			} else if ((*outbindvars)[count].type==
						BINDVARTYPE_INTEGER) {
				(*outbindvars)[count].value.integerval=0;
			} else if ((*outbindvars)[count].type==
						BINDVARTYPE_DOUBLE) {
				(*outbindvars)[count].value.doubleval.value=0;
				(*outbindvars)[count].value.doubleval.precision=0;
				(*outbindvars)[count].value.doubleval.scale=0;
			} else if ((*outbindvars)[count].type==
						BINDVARTYPE_DATE) {
				(*outbindvars)[count].value.dateval.year=0;
				(*outbindvars)[count].value.dateval.month=0;
				(*outbindvars)[count].value.dateval.day=0;
				(*outbindvars)[count].value.dateval.hour=0;
				(*outbindvars)[count].value.dateval.minute=0;
				(*outbindvars)[count].value.dateval.second=0;
				(*outbindvars)[count].value.dateval.microsecond=0;
				if (returnnulls) {
					(*outbindvars)[count].
						value.dateval.tz=NULL;
				} else {
					(*outbindvars)[count].
						value.dateval.tz=new char[1];
					(*outbindvars)[count].
						value.dateval.tz[0]='\0';
				}
			} 

			if (sqlrc->debug) {
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching.\n");
			}

		} else if (type==STRING_DATA) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	STRING output bind\n");
				sqlrc->debugPreEnd();
			}

			// get the value length
			if (getLong(&length)!=sizeof(uint32_t)) {
				setError("Failed to get string value length.\n "
					"A network error may have occurred.");
				return false;
			}
			(*outbindvars)[count].resultvaluesize=length;
			(*outbindvars)[count].value.stringval=new char[length+1];

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		length=");
				sqlrc->debugPrint((int64_t)length);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

			// get the value
			if ((uint32_t)getString((*outbindvars)[count].value.
						stringval,length)!=length) {
				setError("Failed to get string value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*outbindvars)[count].value.stringval[length]='\0';

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching\n");
				sqlrc->debugPreEnd();
			}

		} else if (type==INTEGER_DATA) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	INTEGER output bind\n");
				sqlrc->debugPreEnd();
			}

			// get the value
			if (getLongLong((uint64_t *)&(*outbindvars)[count].
					value.integerval)!=sizeof(uint64_t)) {
				setError("Failed to get integer value.\n "
					"A network error may have occurred.");
				return false;
			}

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching\n");
				sqlrc->debugPreEnd();
			}

		} else if (type==DOUBLE_DATA) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	DOUBLE output bind\n");
				sqlrc->debugPreEnd();
			}

			// get the value
			if (getDouble(&(*outbindvars)[count].value.
						doubleval.value)!=
						sizeof(double)) {
				setError("Failed to get double value.\n "
					"A network error may have occurred.");
				return false;
			}

			// get the precision
			if (getLong(&(*outbindvars)[count].value.
						doubleval.precision)!=
						sizeof(uint32_t)) {
				setError("Failed to get precision.\n "
					"A network error may have occurred.");
				return false;
			}

			// get the scale
			if (getLong(&(*outbindvars)[count].value.
						doubleval.scale)!=
						sizeof(uint32_t)) {
				setError("Failed to get scale.\n "
					"A network error may have occurred.");
				return false;
			}

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching\n");
				sqlrc->debugPreEnd();
			}

		} else if (type==DATE_DATA) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	DATE output bind\n");
				sqlrc->debugPreEnd();
			}

			uint16_t	temp;

			// get the year
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*outbindvars)[count].value.dateval.year=(int16_t)temp;

			// get the month
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*outbindvars)[count].value.dateval.month=(int16_t)temp;

			// get the day
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*outbindvars)[count].value.dateval.day=(int16_t)temp;

			// get the hour
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*outbindvars)[count].value.dateval.hour=(int16_t)temp;

			// get the minute
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*outbindvars)[count].value.dateval.minute=(int16_t)temp;

			// get the second
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*outbindvars)[count].value.dateval.second=(int16_t)temp;

			// get the microsecond
			uint32_t	temp32;
			if (getLong(&temp32)!=sizeof(uint32_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*outbindvars)[count].value.
					dateval.microsecond=(int32_t)temp32;

			// get the timezone length
			uint16_t	length;
			if (getShort(&length)!=sizeof(uint16_t)) {
				setError("Failed to get timezone length.\n "
					"A network error may have occurred.");
				return false;
			}
			(*outbindvars)[count].value.dateval.tz=new char[length+1];

			// get the timezone
			if ((uint16_t)getString((*outbindvars)[count].value.
						dateval.tz,length)!=length) {
				setError("Failed to get timezone.\n "
					"A network error may have occurred.");
				return false;
			}
			(*outbindvars)[count].value. dateval.tz[length]='\0';

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching\n");
				sqlrc->debugPreEnd();
			}

		} else if (type==CURSOR_DATA) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	CURSOR output bind\n");
				sqlrc->debugPreEnd();
			}

			// get the cursor id
			if (getShort((uint16_t *)
				&((*outbindvars)[count].value.cursorid))!=
				sizeof(uint16_t)) {
				setError("Failed to get cursor id.\n "
					"A network error may have occurred.");
				return false;
			}

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching\n");
				sqlrc->debugPreEnd();
			}

		} else {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	LOB/CLOB ");
				sqlrc->debugPrint("output bind\n");
				sqlrc->debugPreEnd();
			}

			// must be START_LONG_DATA...
			// get the total length of the long data
			uint64_t	totallength;
			if (getLongLong(&totallength)!=sizeof(uint64_t)) {
				setError("Failed to get total length.\n "
					"A network error may have occurred.");
				return false;
			}

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		length=");
				sqlrc->debugPrint((int64_t)totallength);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

			// create a buffer to hold the data
			char	*buffer=new char[totallength+1];

			uint64_t	offset=0;
			uint32_t	length;
			for (;;) {

				if (sqlrc->debug) {
					sqlrc->debugPreStart();
					sqlrc->debugPrint("		");
					sqlrc->debugPrint("fetching...\n");
					sqlrc->debugPreEnd();
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

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching.\n");
				sqlrc->debugPreEnd();
			}

			// NULL terminate the buffer.  This makes 
			// certain operations safer and won't hurt
			// since the actual length (which doesn't
			// include the NULL) is available from
			// getOutputBindLength.
			buffer[totallength]='\0';
			(*outbindvars)[count].value.lobval=buffer;
			(*outbindvars)[count].resultvaluesize=totallength;
		}

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint((*outbindvars)[count].variable);
			sqlrc->debugPrint("=");
			if ((*outbindvars)[count].type==
						BINDVARTYPE_BLOB) {
				sqlrc->debugPrintBlob(
					(*outbindvars)[count].value.lobval,
					(*outbindvars)[count].resultvaluesize);
			} else if ((*outbindvars)[count].type==
						BINDVARTYPE_CLOB) {
				sqlrc->debugPrintClob(
					(*outbindvars)[count].value.lobval,
					(*outbindvars)[count].resultvaluesize);
			} else if ((*outbindvars)[count].type==
						BINDVARTYPE_CURSOR) {
				sqlrc->debugPrint((int64_t)(*outbindvars)[count].
								value.cursorid);
			} else if ((*outbindvars)[count].type==
						BINDVARTYPE_INTEGER) {
				sqlrc->debugPrint((*outbindvars)[count].
							value.integerval);
			} else if ((*outbindvars)[count].type==
						BINDVARTYPE_DOUBLE) {
				sqlrc->debugPrint((*outbindvars)[count].
						value.doubleval.value);
				sqlrc->debugPrint("(");
				sqlrc->debugPrint((int64_t)(*outbindvars)[count].
						value.doubleval.precision);
				sqlrc->debugPrint(",");
				sqlrc->debugPrint((int64_t)(*outbindvars)[count].
						value.doubleval.scale);
				sqlrc->debugPrint(")");
			} else if ((*outbindvars)[count].type==
						BINDVARTYPE_DATE) {
				sqlrc->debugPrint((int64_t)(*outbindvars)[count].
							value.dateval.year);
				sqlrc->debugPrint("-");
				sqlrc->debugPrint((int64_t)(*outbindvars)[count].
							value.dateval.month);
				sqlrc->debugPrint("-");
				sqlrc->debugPrint((int64_t)(*outbindvars)[count].
							value.dateval.day);
				sqlrc->debugPrint(" ");
				sqlrc->debugPrint((int64_t)(*outbindvars)[count].
							value.dateval.hour);
				sqlrc->debugPrint(":");
				sqlrc->debugPrint((int64_t)(*outbindvars)[count].
							value.dateval.minute);
				sqlrc->debugPrint(":");
				sqlrc->debugPrint((int64_t)(*outbindvars)[count].
							value.dateval.second);
				sqlrc->debugPrint(" ");
				sqlrc->debugPrint((*outbindvars)[count].
							value.dateval.tz);
			} else {
				sqlrc->debugPrint((*outbindvars)[count].
							value.stringval);
			}
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		count++;
	}

	// cache the output binds
	cacheOutputBinds(count);

	return true;
}

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
			if (colcount>currentrow->colcount) {
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
				buffer=(char *)rowstorage->allocate(1);
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
			buffer=(char *)rowstorage->allocate(length+1);
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
	
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			if (buffer) {
				if (type==END_LONG_DATA) {
					sqlrc->debugPrint("\nLOB data:");
					sqlrc->debugPrintBlob(buffer,length);
				} else {
					sqlrc->debugPrint("\"");
					sqlrc->debugPrint(buffer);
					sqlrc->debugPrint("\",");
				}
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

void sqlrcursor::getErrorFromServer() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Getting Error From Server\n");
		sqlrc->debugPreEnd();
	}

	bool	networkerror=true;

	// get the error code
	if (getLongLong((uint64_t *)&errorno)==sizeof(uint64_t)) {

		// get the length of the error string
		uint16_t	length;
		if (getShort(&length)==sizeof(uint16_t)) {

			// get the error string
			error=new char[length+1];
			sqlrc->cs->read(error,length);
			error[length]='\0';

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

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Setting Error\n");
		sqlrc->debugPreEnd();
	}
	error=charstring::duplicate(err);
	handleError();
}

void sqlrcursor::handleError() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint((int64_t)errorno);
		sqlrc->debugPrint(":\n");
		sqlrc->debugPrint(error);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	endofresultset=true;

	cacheError();
	finishCaching();
}

bool sqlrcursor::fetchRowIntoBuffer(bool getallrows, uint64_t row,
						uint64_t *rowbufferindex) {

	// if we getting the entire result set at once, then the result set 
	// buffer index is the requested row-firstrowindex
	if (!rsbuffersize) {
		if (row<rowcount && row>=firstrowindex) {
			*rowbufferindex=row-firstrowindex;
			return true;
		}
		return false;
	}

	// but, if we're not getting the entire result set at once
	// and if the requested row is not in the current range, 
	// fetch more data from the connection
	while (row>=(firstrowindex+rsbuffersize) && !endofresultset) {

		if (sqlrc->connected || (cachesource && cachesourceind)) {

			clearRows();

			// if we're not fetching from a cached result set,
			// tell the server to send one 
			if (!cachesource && !cachesourceind) {
				sqlrc->cs->write((uint16_t)FETCH_RESULT_SET);
				sqlrc->cs->write(cursorid);
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
	if (row<rowcount) {
		*rowbufferindex=row%rsbuffersize;
		return true;
	}
	return false;
}

int32_t sqlrcursor::getShort(uint16_t *integer,
				int32_t timeoutsec, int32_t timeoutusec) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(integer);
	} else {
		return sqlrc->cs->read(integer,timeoutsec,timeoutusec);
	}
}

int32_t sqlrcursor::getShort(uint16_t *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(integer);
	} else {
		return sqlrc->cs->read(integer);
	}
}

int32_t sqlrcursor::getLong(uint32_t *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(integer);
	} else {
		return sqlrc->cs->read(integer);
	}
}

int32_t sqlrcursor::getLongLong(uint64_t *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(integer);
	} else {
		return sqlrc->cs->read(integer);
	}
}

int32_t sqlrcursor::getString(char *string, int32_t size) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(string,size);
	} else {
		return sqlrc->cs->read(string,size);
	}
}

int32_t sqlrcursor::getDouble(double *value) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(value);
	} else {
		return sqlrc->cs->read(value);
	}
}

bool sqlrcursor::fetchFromBindCursor() {

	if (!endofresultset || !sqlrc->connected) {
		return false;
	}

	// FIXME: should these be here?
	clearVariables();
	clearResultSet();

	cached=false;
	endofresultset=false;

	// tell the server we're fetching from a bind cursor
	sqlrc->cs->write((uint16_t)FETCH_FROM_BIND_CURSOR);

	// send the cursor id to the server
	sqlrc->cs->write((uint16_t)cursorid);

	sendGetColumnInfo();

	sqlrc->flushWriteBuffer();

	if (rsbuffersize) {
		return processResultSet(false,rsbuffersize-1);
	} else {
		return processResultSet(true,0);
	}
}

bool sqlrcursor::openCachedResultSet(const char *filename) {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Opening cached result set: ");
		sqlrc->debugPrint(filename);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	if (!endofresultset) {
		closeResultSet(true);
	}
	clearResultSet();

	cached=true;
	endofresultset=false;

	// create the index file name
	size_t	indexfilenamelen=charstring::length(filename)+5;
	char	*indexfilename=new char[indexfilenamelen];
	charstring::copy(indexfilename,filename);
	charstring::append(indexfilename,".ind");

	// open the file
	cachesource=new file();
	cachesourceind=new file();
	if ((cachesource->open(filename,O_RDWR)) &&
		(cachesourceind->open(indexfilename,O_RDWR))) {

		delete[] indexfilename;

		// initialize firstrowindex and rowcount
		firstrowindex=0;
		rowcount=firstrowindex;

		// make sure it's a cache file and skip the ttl
		char		magicid[13];
		uint64_t	ttl;
		if (getString(magicid,13)==13 &&
			!charstring::compare(magicid,"SQLRELAYCACHE",13) &&
			getLongLong(&ttl)==sizeof(uint64_t)) {

			// process the result set
			if (rsbuffersize) {
				return processResultSet(false,firstrowindex+
								rsbuffersize-1);
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
	if (cachesource) {
		cachesource->close();
		delete cachesource;
		cachesource=NULL;
	}
	if (cachesourceind) {
		cachesourceind->close();
		delete cachesourceind;
		cachesourceind=NULL;
	}
}

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
	return firstrowindex;
}

bool sqlrcursor::endOfResultSet() {
	return endofresultset;
}

uint64_t sqlrcursor::rowCount() {
	return rowcount;
}

uint64_t sqlrcursor::affectedRows() {
	if (knowsaffectedrows==AFFECTED_ROWS) {
		return affectedrows;
	}
	return 0;
}

uint64_t sqlrcursor::totalRows() {
	if (knowsactualrows==ACTUAL_ROWS) {
		return actualrows;
	}
	return 0;
}

int64_t sqlrcursor::errorNumber() {
	// if we have a code then we should have a message too,
	// the codes could be any number, including 0, so check
	// the message to see which code to return
	if (error) {
		return errorno;
	} else if (sqlrc->error) {
		return sqlrc->errorno;
	}
	return 0;
}

const char *sqlrcursor::errorMessage() {
	if (error) {
		return error;
	} else if (sqlrc->error) {
		return sqlrc->error;
	}
	return NULL;
}

void sqlrcursor::getNullsAsEmptyStrings() {
	returnnulls=false;
}

void sqlrcursor::getNullsAsNulls() {
	returnnulls=true;
}

char *sqlrcursor::getFieldInternal(uint64_t row, uint32_t col) {
	if (row<OPTIMISTIC_ROW_COUNT) {
		return rows[row]->getField(col);
	}
	return extrarows[row-OPTIMISTIC_ROW_COUNT]->getField(col);
}

uint32_t sqlrcursor::getFieldLengthInternal(uint64_t row, uint32_t col) {
	if (row<OPTIMISTIC_ROW_COUNT) {
		return rows[row]->getFieldLength(col);
	}
	return extrarows[row-OPTIMISTIC_ROW_COUNT]->getFieldLength(col);
}

const char *sqlrcursor::getField(uint64_t row, uint32_t col) {

	if (rowcount && row>=firstrowindex && col<colcount) {

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

	if (rowcount && row>=firstrowindex && col<colcount) {

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

	if (rowcount && row>=firstrowindex) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		uint64_t	rowbufferindex;
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
	uint64_t	rowbuffercount=rowcount-firstrowindex;
	fields=new char **[rowbuffercount+1];
	fields[rowbuffercount]=(char **)NULL;
	for (uint64_t i=0; i<rowbuffercount; i++) {
		fields[i]=new char *[colcount+1];
		fields[i][colcount]=(char *)NULL;
		for (uint32_t j=0; j<colcount; j++) {
			fields[i][j]=getFieldInternal(i,j);
		}
	}
}

uint32_t *sqlrcursor::getRowLengths(uint64_t row) {

	if (rowcount && row>=firstrowindex) {

		// in the event that we're stepping through the result set 
		// instead of buffering the entire thing, the requested row
		// may have to be fetched into the buffer...
		uint64_t	rowbufferindex;
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
	uint64_t	rowbuffercount=rowcount-firstrowindex;
	fieldlengths=new uint32_t *[rowbuffercount+1];
	fieldlengths[rowbuffercount]=0;
	for (uint64_t i=0; i<rowbuffercount; i++) {
		fieldlengths[i]=new uint32_t[colcount+1];
		fieldlengths[i][colcount]=0;
		for (uint32_t j=0; j<colcount; j++) {
			fieldlengths[i][j]=getFieldLengthInternal(i,j);
		}
	}
}

void sqlrcursor::suspendResultSet() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Suspending Result Set\n");
		sqlrc->debugPreEnd();
	}

	if (sqlrc->connected && !cached) {

		sqlrc->cs->write((uint16_t)SUSPEND_RESULT_SET);
		sqlrc->cs->write(cursorid);

		sqlrc->flushWriteBuffer();
	}

	clearCacheDest();
	suspendresultsetsent=1;
}

uint16_t sqlrcursor::getResultSetId() {
	return cursorid;
}

bool sqlrcursor::resumeResultSet(uint16_t id) {
	return resumeCachedResultSet(id,NULL);
}

bool sqlrcursor::resumeCachedResultSet(uint16_t id, const char *filename) {

	if (!endofresultset && !suspendresultsetsent) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!sqlrc->connected) {
		return false;
	}

	cached=false;
	resumed=true;
	endofresultset=false;

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Resuming Result Set of Cursor: ");
		sqlrc->debugPrint((int64_t)id);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	// tell the server we want to resume the result set
	sqlrc->cs->write((uint16_t)RESUME_RESULT_SET);

	// send the id of the cursor we want to 
	// resume the result set of to the server
	sqlrc->cs->write(id);

	// process the result set
	if (!charstring::isNullOrEmpty(filename)) {
		cacheToFile(filename);
	}
	if (rsbuffersize) {
		if (processResultSet(true,firstrowindex+rsbuffersize-1)) {
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

	if (sqlrc->connected || cached) {
		if (cachedest && cachedestind) {
			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("Getting the rest of the result set, since this is a cached result set.\n");
				sqlrc->debugPreEnd();
			}
			while (!endofresultset) {
				clearRows();

				// if we're not fetching from a cached result 
				// set tell the server to send one 
				if (!cachesource && !cachesourceind) {
					sqlrc->cs->write((uint16_t)
							FETCH_RESULT_SET);
					sqlrc->cs->write(cursorid);
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
		} else if (closeremote && havecursorid) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("Aborting Result "
							"Set For Cursor: ");
				sqlrc->debugPrint((int64_t)cursorid);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

			sqlrc->cs->write((uint16_t)ABORT_RESULT_SET);
			sqlrc->cs->write((uint16_t)DONT_NEED_NEW_CURSOR);
			sqlrc->cs->write(cursorid);
			sqlrc->flushWriteBuffer();
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
	firstrowindex=0;
	previousrowcount=rowcount;
	rowcount=0;
	actualrows=0;
	affectedrows=0;
	endofresultset=true;
	suspendresultsetsent=0;
}

void sqlrcursor::clearError() {
	delete[] error;
	error=NULL;
	errorno=0;
	if (sqlrc) {
		sqlrc->clearError();
	}
}

void sqlrcursor::clearRows() {

	// delete data in rows for long datatypes
	uint32_t	rowbuffercount=rowcount-firstrowindex;
	for (uint32_t i=0; i<rowbuffercount; i++) {
	        for (uint32_t j=0; j<colcount; j++) {
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
	if (firstextrarow) {
		currentrow=firstextrarow;
		while (currentrow) {
			firstextrarow=currentrow->next;
			delete currentrow;
			currentrow=firstextrarow;
		}
		firstextrarow=NULL;
	}
	currentrow=NULL;

	// delete array pointing to linked list items
	delete[] extrarows;
	extrarows=NULL;

	// delete arrays of fields and field lengths
	if (fields) {
		for (uint32_t i=0; i<rowbuffercount; i++) {
			delete[] fields[i];
		}
		delete[] fields;
		fields=NULL;
	}
	if (fieldlengths) {
		for (uint32_t i=0; i<rowbuffercount; i++) {
			delete[] fieldlengths[i];
		}
		delete[] fieldlengths;
		fieldlengths=NULL;
	}

	// reset the row storage pool
	rowstorage->deallocate();
}

void sqlrcursor::clearColumns() {

	// delete the column type strings (if necessary)
	if (sentcolumninfo==SEND_COLUMN_INFO &&
				columntypeformat!=COLUMN_TYPE_IDS) {
		for (uint32_t i=0; i<colcount; i++) {
			delete[] getColumnInternal(i)->typestring;
		}
	}

	// reset the column storage pool
	colstorage->deallocate();

	// reset the column count
	previouscolcount=colcount;
	colcount=0;

	// delete array pointing to each column name
	delete[] columnnamearray;
	columnnamearray=NULL;
}

char *sqlrcursor::getQueryTree() {

	reexecute=false;
	validatebinds=false;
	resumed=false;
	clearVariables();

	if (!endofresultset) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!sqlrc->openSession()) {
		return NULL;
	}

	cached=false;
	endofresultset=false;

	// tell the server we want to get a db list
	sqlrc->cs->write((uint16_t)GET_QUERY_TREE);

	// tell the server whether we'll need a cursor or not
	sendCursorStatus();

	sqlrc->flushWriteBuffer();

	uint16_t	err=getErrorStatus();
	if (err!=NO_ERROR_OCCURRED) {
		if (err==TIMEOUT_GETTING_ERROR_STATUS) {
			sqlrc->endSession();
			return NULL;
		}
		getErrorFromServer();
		if (err==ERROR_OCCURRED_DISCONNECT) {
			sqlrc->endSession();
		}
		return NULL;
	}

	// get the size of the tree
	uint64_t	querytreelen;
	if (sqlrc->cs->read(&querytreelen)!=sizeof(uint64_t)) {
		return NULL;
	}

	// get the tree itself
	char	*querytree=new char[querytreelen+1];
	if ((uint64_t)sqlrc->cs->read(querytree,querytreelen)!=querytreelen) {
		delete[] querytree;
		return NULL;
	}
	querytree[querytreelen]='\0';

	return querytree;
}
