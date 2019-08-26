// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/memorypool.h>
#include <rudiments/file.h>
#include <rudiments/charstring.h>
#include <rudiments/permissions.h>
#include <rudiments/datetime.h>
#include <rudiments/bytestring.h>
#include <rudiments/character.h>
#include <rudiments/filesystem.h>
#include <rudiments/error.h>
#include <defines.h>
#define NEED_DATATYPESTRING 1
#include <datatypes.h>
#define NEED_BEFORE_BIND_VARIABLE 1
#define NEED_IS_BIND_DELIMITER 1
#define NEED_AFTER_BIND_VARIABLE 1
#define NEED_COUNT_BIND_VARIABLES 1
#include <bindvariables.h>

#ifndef MAXPATHLEN
	#define MAXPATHLEN 256
#endif

// we're optimistic that the average query will contain 16 bind variables
#define OPTIMISTIC_BIND_COUNT 16

// we're optimistic that the average query will contain 16 columns whose data
// requires an average of 16 bytes to store
#define OPTIMISTIC_COLUMN_COUNT 16
#define OPTIMISTIC_AVERAGE_COLUMN_DATA_LENGTH 16
#define OPTIMISTIC_COLUMN_DATA_SIZE OPTIMISTIC_COLUMN_COUNT*\
					OPTIMISTIC_AVERAGE_COLUMN_DATA_LENGTH

// we're also optimistic that if we need more space, that growing at a rate of
// 1 column at a time will work out well
#define OPTIMISTIC_COLUMN_DATA_GROWTH_SIZE OPTIMISTIC_COLUMN_DATA_SIZE

// we're optimistic that the average query will contain 16 rows whose fields
// average 16 characters in length
#define OPTIMISTIC_ROW_COUNT 16
#define OPTIMISTIC_AVERAGE_FIELD_LENGTH 16
#define OPTIMISTIC_RESULT_SET_SIZE OPTIMISTIC_COLUMN_COUNT*\
					OPTIMISTIC_ROW_COUNT*\
					OPTIMISTIC_AVERAGE_FIELD_LENGTH

// we're also optimistic that if we need more space, that growing at a rate of
// 4 rows at a time will work out well
#define OPTIMISTIC_RESULT_SET_GROWTH_SIZE OPTIMISTIC_COLUMN_COUNT*4*\
					OPTIMISTIC_AVERAGE_FIELD_LENGTH



class sqlrclientrow {
	friend class sqlrcursor;
	private:
			sqlrclientrow(uint32_t colcount);
			~sqlrclientrow();
		void	resize(uint32_t colcount);
		void	addField(uint32_t column, 
				const char *buffer, uint32_t length);

		char		*getField(uint32_t column) const;
		uint32_t	getFieldLength(uint32_t column) const;

		sqlrclientrow	*next;

		char		*fields[OPTIMISTIC_COLUMN_COUNT];
		uint32_t	fieldlengths[OPTIMISTIC_COLUMN_COUNT];
		char		**extrafields;
		uint32_t	*extrafieldlengths;

		uint32_t	colcount;
};

sqlrclientrow::sqlrclientrow(uint32_t colcount) {
	this->colcount=colcount;
	if (colcount>=OPTIMISTIC_COLUMN_COUNT) {
		extrafields=new char *[colcount-OPTIMISTIC_COLUMN_COUNT];
		extrafieldlengths=new uint32_t
					[colcount-OPTIMISTIC_COLUMN_COUNT];
	} else {
		extrafields=NULL;
		extrafieldlengths=NULL;
	}
}

sqlrclientrow::~sqlrclientrow() {
	delete[] extrafields;
	delete[] extrafieldlengths;
}

void sqlrclientrow::resize(uint32_t colcount) {
	this->colcount=colcount;
	if (colcount>=OPTIMISTIC_COLUMN_COUNT) {
		delete[] extrafields;
		delete[] extrafieldlengths;
		extrafields=new char *[colcount-OPTIMISTIC_COLUMN_COUNT];
		extrafieldlengths=new uint32_t
					[colcount-OPTIMISTIC_COLUMN_COUNT];
	}
}

void sqlrclientrow::addField(uint32_t column,
				const char *buffer, uint32_t length) {
	if (column<OPTIMISTIC_COLUMN_COUNT) {
		fields[column]=(char *)buffer;
		fieldlengths[column]=length;
	} else {
		extrafields[column-OPTIMISTIC_COLUMN_COUNT]=(char *)buffer;
		extrafieldlengths[column-OPTIMISTIC_COLUMN_COUNT]=length;
	}
}

char *sqlrclientrow::getField(uint32_t column) const {
	if (column<OPTIMISTIC_COLUMN_COUNT) {
		return fields[column];
	} else {
		return extrafields[column-OPTIMISTIC_COLUMN_COUNT];
	}
}

uint32_t sqlrclientrow::getFieldLength(uint32_t column) const {
	if (column<OPTIMISTIC_COLUMN_COUNT) {
		return fieldlengths[column];
	} else {
		return extrafieldlengths[column-OPTIMISTIC_COLUMN_COUNT];
	}
}


class sqlrclientcolumn {
	public:
		char		*name;
		uint16_t	type;
		char		*typestring;
		uint16_t	typestringlength;
		uint32_t	length;
		uint32_t	longest;
		unsigned char	longdatatype;
		uint32_t	precision;
		uint32_t	scale;
		uint16_t	nullable;
		uint16_t	primarykey;
		uint16_t	unique;
		uint16_t	partofkey;
		uint16_t	unsignednumber;
		uint16_t	zerofill;
		uint16_t	binary;
		uint16_t	autoincrement;
		char		*table;
};

enum columncase {
	MIXED_CASE,
	UPPER_CASE,
	LOWER_CASE
};


class sqlrclientbindvar {
	friend class sqlrcursor;
	friend class sqlrcursorprivate;
	private:
		char	*variable;
		union {
			char	*stringval;
			int64_t	integerval;
			struct {
				double		value;
				uint32_t	precision;
				uint32_t	scale;
			} doubleval;
			struct {
				int16_t	year;
				int16_t	month;
				int16_t	day;
				int16_t	hour;
				int16_t	minute;
				int16_t	second;
				int32_t	microsecond;
				char	*tz;
				bool	isnegative;
			} dateval;
			char		*lobval;
			uint16_t	cursorid;
		} value;
		uint32_t	valuesize;
		uint32_t	resultvaluesize;

		sqlrclientbindvartype_t 	type;

		bool		send;

		bool		substituted;
		bool		donesubstituting;
};


class sqlrcursorprivate {
	friend class sqlrcursor;
	private:
		sqlrclientbindvar	*findVar(const char *variable,
					dynamicarray<sqlrclientbindvar> *vars);

		bool		_resumed;
		bool		_cached;

		// query
		char		*_querybuffer;
		const char	*_queryptr;
		uint32_t	_querylen;
		char		*_fullpath;
		bool		_reexecute;

		// substitution variables
		dynamicarray<sqlrclientbindvar>	*_subvars;
		bool				_dirtysubs;

		// bind variables
		dynamicarray<sqlrclientbindvar>	*_inbindvars;
		dynamicarray<sqlrclientbindvar>	*_outbindvars;
		dynamicarray<sqlrclientbindvar>	*_inoutbindvars;
		bool				_validatebinds;
		bool				_dirtybinds;
		bool				_clearbindsduringprepare;

		// result set
		bool		_lazyfetch;
		uint64_t	_rsbuffersize;
		uint16_t	_sendcolumninfo;
		uint16_t	_sentcolumninfo;

		uint16_t	_suspendresultsetsent;
		bool		_endofresultset;

		uint16_t	_columntypeformat;
		uint32_t	_colcount;
		uint32_t	_previouscolcount;

		columncase	_colcase;

		sqlrclientcolumn	*_columns;
		sqlrclientcolumn	*_extracolumns;
		memorypool		*_colstorage;
		char			**_columnnamearray;

		uint64_t	_firstrowindex;
		uint64_t	_resumedlastrowindex;
		uint64_t	_rowcount;
		uint16_t	_knowsactualrows;
		uint64_t	_actualrows;
		uint16_t	_knowsaffectedrows;
		uint64_t	_affectedrows;

		sqlrclientrow	**_rows;
		sqlrclientrow	**_extrarows;
		memorypool	*_rowstorage;
		sqlrclientrow	*_firstextrarow;
		char		***_fields;
		uint32_t	**_fieldlengths;

		bool		_returnnulls;

		// result set caching
		bool		_cacheon;
		int32_t		_cachettl;
		char		*_cachedestname;
		char		*_cachedestindname;
		file		*_cachedest;
		file		*_cachedestind;
		file		*_cachesource;
		file		*_cachesourceind;

		// error
		int64_t		_errorno;
		char		*_error;

		// copy references flag
		bool		_copyrefs;

		// parent connection
		sqlrconnection	*_sqlrc;

		// next/previous pointers
		sqlrcursor	*_next;
		sqlrcursor	*_prev;

		// cursor id
		uint16_t	_cursorid;
		bool		_havecursorid;

		// query translation
		char		*_querytree;
		char		*_translatedquery;

		// socket client
		socketclient	*_cs;
};

// This method is a member of sqlrcursorprivate, rather than sqlrcuror, because
// if it were a member of sqlrcursor, then it would have to be defined in the
// header file.  If it were, then since it references a
// dynamicarray<sqlrclientbindvar>, older compilers would also require that
// sqlrclientbindvar be defined in the header file as well.  To avoid all of
// that, it's part of sqlrcursorprivate.
sqlrclientbindvar *sqlrcursorprivate::findVar(const char *variable,
					dynamicarray<sqlrclientbindvar> *vars) {
	for (uint16_t i=0; i<vars->getLength(); i++) {
		if (!charstring::compare((*vars)[i].variable,variable)) {
			return &((*vars)[i]);
		}
	}
	return NULL;
}


sqlrcursor::sqlrcursor(sqlrconnection *sqlrc, bool copyreferences) {
	init(sqlrc,copyreferences);
}

sqlrcursor::sqlrcursor(sqlrconnection *sqlrc) {
	init(sqlrc,false);
}

void sqlrcursor::init(sqlrconnection *sqlrc, bool copyreferences) {

	pvt=new sqlrcursorprivate;

	// copy references
	pvt->_copyrefs=copyreferences;

	pvt->_sqlrc=sqlrc;

	// put self in connection's cursor list
	if (pvt->_sqlrc->lastcursor()) {
		pvt->_sqlrc->lastcursor()->pvt->_next=this;
		pvt->_prev=pvt->_sqlrc->lastcursor();
	} else {
		pvt->_sqlrc->firstcursor(this);
		pvt->_prev=NULL;
	}
	pvt->_sqlrc->lastcursor(this);
	pvt->_next=NULL;

	// session state
	pvt->_cached=false;

	// query
	pvt->_querybuffer=NULL;
	pvt->_fullpath=NULL;

	// result set
	pvt->_lazyfetch=false;
	pvt->_rsbuffersize=0;

	pvt->_firstrowindex=0;
	pvt->_resumedlastrowindex=0;
	pvt->_rowcount=0;
	pvt->_actualrows=0;
	pvt->_affectedrows=0;
	pvt->_endofresultset=true;

	pvt->_errorno=0;
	pvt->_error=NULL;

	pvt->_rows=NULL;
	pvt->_extrarows=NULL;
	pvt->_firstextrarow=NULL;
	pvt->_rowstorage=new memorypool(OPTIMISTIC_RESULT_SET_SIZE,
					OPTIMISTIC_RESULT_SET_GROWTH_SIZE,
					5);
	pvt->_fields=NULL;
	pvt->_fieldlengths=NULL;

	pvt->_colcount=0;
	pvt->_previouscolcount=0;
	pvt->_columns=NULL;
	pvt->_extracolumns=NULL;
	pvt->_colstorage=new memorypool(OPTIMISTIC_COLUMN_DATA_SIZE,
					OPTIMISTIC_COLUMN_DATA_GROWTH_SIZE,
					5);
	pvt->_columnnamearray=NULL;

	pvt->_returnnulls=false;

	// cache file
	pvt->_cachesource=NULL;
	pvt->_cachesourceind=NULL;
	pvt->_cachedestname=NULL;
	pvt->_cachedestindname=NULL;
	pvt->_cachedest=NULL;
	pvt->_cachedestind=NULL;
	pvt->_cacheon=false;

	// options...
	pvt->_sendcolumninfo=SEND_COLUMN_INFO;
	pvt->_sentcolumninfo=SEND_COLUMN_INFO;
	pvt->_columntypeformat=COLUMN_TYPE_IDS;
	pvt->_colcase=MIXED_CASE;

	// cursor id
	pvt->_cursorid=0;
	pvt->_havecursorid=false;

	// query translation
	pvt->_querytree=NULL;
	pvt->_translatedquery=NULL;

	// socket client
	pvt->_cs=NULL;

	// initialize all bind/substitution-related variables
	pvt->_subvars=new dynamicarray<sqlrclientbindvar>(
					OPTIMISTIC_BIND_COUNT,16);
	pvt->_inbindvars=new dynamicarray<sqlrclientbindvar>(
					OPTIMISTIC_BIND_COUNT,16);
	pvt->_outbindvars=new dynamicarray<sqlrclientbindvar>(
					OPTIMISTIC_BIND_COUNT,16);
	pvt->_inoutbindvars=new dynamicarray<sqlrclientbindvar>(
					OPTIMISTIC_BIND_COUNT,16);
	pvt->_clearbindsduringprepare=true;
	clearVariables();
}

sqlrcursor::~sqlrcursor() {

	// abort result set if necessary
	if (pvt->_sqlrc && !pvt->_sqlrc->endsessionsent() &&
				!pvt->_sqlrc->suspendsessionsent()) {
		closeResultSet(true);
	}

	// deallocate copied references
	deleteVariables();
	delete pvt->_inoutbindvars;
	delete pvt->_outbindvars;
	delete pvt->_inbindvars;
	delete pvt->_subvars;

	// deallocate the query buffer
	delete[] pvt->_querybuffer;

	// deallocate the fullpath (used for file queries)
	delete[] pvt->_fullpath;

	clearResultSet();
	delete[] pvt->_columns;
	delete[] pvt->_extracolumns;
	delete pvt->_colstorage;
	if (pvt->_rows) {
		for (uint32_t i=0; i<OPTIMISTIC_ROW_COUNT; i++) {
			delete pvt->_rows[i];
		}
		delete[] pvt->_rows;
	}
	delete pvt->_rowstorage;

	// it's possible for the connection to be deleted before the 
	// cursor is, in that case, don't do any of this stuff
	if (pvt->_sqlrc) {

		// remove self from connection's cursor list
		if (!pvt->_next && !pvt->_prev) {
			pvt->_sqlrc->firstcursor(NULL);
			pvt->_sqlrc->lastcursor(NULL);
		} else {
			sqlrcursor	*temp=pvt->_next;
			if (pvt->_next) {
				pvt->_next->pvt->_prev=pvt->_prev;
			} else {
				pvt->_sqlrc->lastcursor(pvt->_prev);
			}
			if (pvt->_prev) {
				pvt->_prev->pvt->_next=temp;
			} else {
				pvt->_sqlrc->firstcursor(pvt->_next);
			}
		}

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Deallocated cursor\n");
			pvt->_sqlrc->debugPreEnd();
		}
	}

	if (pvt->_copyrefs && pvt->_cachedestname) {
		delete[] pvt->_cachedestname;
	}
	delete[] pvt->_cachedestindname;

	// query translation
	delete[] pvt->_querytree;
	delete[] pvt->_translatedquery;

	delete pvt;
}

void sqlrcursor::setResultSetBufferSize(uint64_t rows) {
	pvt->_rsbuffersize=rows;
	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Result Set Buffer Size: ");
		pvt->_sqlrc->debugPrint((int64_t)rows);
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}
}

uint64_t sqlrcursor::getResultSetBufferSize() {
	return pvt->_rsbuffersize;
}

void sqlrcursor::lazyFetch() {
	pvt->_lazyfetch=true;
}

void sqlrcursor::dontLazyFetch() {
	pvt->_lazyfetch=false;
}

void sqlrcursor::dontGetColumnInfo() {
	pvt->_sendcolumninfo=DONT_SEND_COLUMN_INFO;
}

void sqlrcursor::getColumnInfo() {
	pvt->_sendcolumninfo=SEND_COLUMN_INFO;
}

void sqlrcursor::mixedCaseColumnNames() {
	pvt->_colcase=MIXED_CASE;
}

void sqlrcursor::upperCaseColumnNames() {
	pvt->_colcase=UPPER_CASE;
}

void sqlrcursor::lowerCaseColumnNames() {
	pvt->_colcase=LOWER_CASE;
}

void sqlrcursor::cacheToFile(const char *filename) {

	pvt->_cacheon=true;
	pvt->_cachettl=600;
	if (pvt->_copyrefs) {
		delete[] pvt->_cachedestname;
		pvt->_cachedestname=charstring::duplicate(filename);
	} else {
		pvt->_cachedestname=(char *)filename;
	}

	// create the index name
	delete[] pvt->_cachedestindname;
	size_t	cachedestindnamelen=charstring::length(filename)+5;
	pvt->_cachedestindname=new char[cachedestindnamelen];
	charstring::copy(pvt->_cachedestindname,filename);
	charstring::append(pvt->_cachedestindname,".ind");
}

void sqlrcursor::setCacheTtl(uint32_t ttl) {
	pvt->_cachettl=ttl;
}

const char *sqlrcursor::getCacheFileName() {
	return pvt->_cachedestname;
}

void sqlrcursor::cacheOff() {
	pvt->_cacheon=false;
}

void sqlrcursor::startCaching() {

	if (!pvt->_resumed) {
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Caching data to ");
			pvt->_sqlrc->debugPrint(pvt->_cachedestname);
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}
	} else {
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Resuming caching data to ");
			pvt->_sqlrc->debugPrint(pvt->_cachedestname);
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}
	}

	// create the cache file, truncate it unless we're 
	// resuming a previous session
	pvt->_cachedest=new file();
	pvt->_cachedestind=new file();
	if (!pvt->_resumed) {
		pvt->_cachedest->open(pvt->_cachedestname,
					O_RDWR|O_TRUNC|O_CREAT,
					permissions::ownerReadWrite());
		pvt->_cachedestind->open(pvt->_cachedestindname,
					O_RDWR|O_TRUNC|O_CREAT,
					permissions::ownerReadWrite());
	} else {
		pvt->_cachedest->open(pvt->_cachedestname,
					O_RDWR|O_CREAT|O_APPEND,
					permissions::ownerReadWrite());
		pvt->_cachedestind->open(pvt->_cachedestindname,
					O_RDWR|O_CREAT|O_APPEND,
					permissions::ownerReadWrite());
	}

	if (pvt->_cachedest && pvt->_cachedestind) {

		// calculate and set write buffer size
		// FIXME: I think rudiments bugs keep this from working...
		/*filesystem	fs;
		if (fs.open(pvt->_cachedestname)) {
			off64_t	optblocksize=fs.getOptimumTransferBlockSize();
			pvt->_cachedest->setWriteBufferSize(
					(optblocksize)?optblocksize:1024);
			pvt->_cachedestind->setWriteBufferSize(
					(optblocksize)?optblocksize:1024);
		}*/

		if (!pvt->_resumed) {

			// write "magic" identifier to head of files
			pvt->_cachedest->write("SQLRELAYCACHE",13);
			pvt->_cachedestind->write("SQLRELAYCACHE",13);
			
			// write ttl to files
			datetime	dt;
			dt.getSystemDateAndTime();
			int64_t	expiration=dt.getEpoch()+pvt->_cachettl;
			pvt->_cachedest->write(expiration);
			pvt->_cachedestind->write(expiration);
		}

	} else {

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Error caching data to ");
			pvt->_sqlrc->debugPrint(pvt->_cachedestname);
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}

		// in case of an error, clean up
		clearCacheDest();
	}
}

void sqlrcursor::cacheError() {

	if (pvt->_resumed || !pvt->_cachedest) {
		return;
	}

	// write the number of returned rows, affected rows 
	// and a zero to terminate the column descriptions
	pvt->_cachedest->write((uint16_t)NO_ACTUAL_ROWS);
	pvt->_cachedest->write((uint16_t)NO_AFFECTED_ROWS);
	pvt->_cachedest->write((uint16_t)END_COLUMN_INFO);
}

void sqlrcursor::cacheNoError() {

	if (pvt->_resumed || !pvt->_cachedest) {
		return;
	}

	pvt->_cachedest->write((uint16_t)NO_ERROR_OCCURRED);
}

void sqlrcursor::cacheColumnInfo() {

	if (pvt->_resumed || !pvt->_cachedest) {
		return;
	}

	// write the number of returned rows
	pvt->_cachedest->write(pvt->_knowsactualrows);
	if (pvt->_knowsactualrows==ACTUAL_ROWS) {
		pvt->_cachedest->write(pvt->_actualrows);
	}

	// write the number of affected rows
	pvt->_cachedest->write(pvt->_knowsaffectedrows);
	if (pvt->_knowsaffectedrows==AFFECTED_ROWS) {
		pvt->_cachedest->write(pvt->_affectedrows);
	}

	// write whether or not the column info is is cached
	pvt->_cachedest->write(pvt->_sentcolumninfo);

	// write the column count
	pvt->_cachedest->write(pvt->_colcount);

	// write column descriptions to the cache file
	if (pvt->_sendcolumninfo==SEND_COLUMN_INFO && 
			pvt->_sentcolumninfo==SEND_COLUMN_INFO) {

		// write column type format
		pvt->_cachedest->write(pvt->_columntypeformat);

		// write the columns themselves
		uint16_t			namelen;
		uint16_t			tablelen;
		sqlrclientcolumn		*whichcolumn;
		for (uint32_t i=0; i<pvt->_colcount; i++) {

			// get the column
			whichcolumn=getColumnInternal(i);

			// write the name
			namelen=charstring::length(whichcolumn->name);
			pvt->_cachedest->write(namelen);
			pvt->_cachedest->write(whichcolumn->name,namelen);

			// write the type
			if (pvt->_columntypeformat==COLUMN_TYPE_IDS) {
				pvt->_cachedest->write(whichcolumn->type);
			} else {
				pvt->_cachedest->write(
						whichcolumn->typestringlength);
				pvt->_cachedest->write(
						whichcolumn->typestring,
						whichcolumn->typestringlength);
			}

			// write the length, precision and scale
			pvt->_cachedest->write(whichcolumn->length);
			pvt->_cachedest->write(whichcolumn->precision);
			pvt->_cachedest->write(whichcolumn->scale);

			// write the flags
			pvt->_cachedest->write(whichcolumn->nullable);
			pvt->_cachedest->write(whichcolumn->primarykey);
			pvt->_cachedest->write(whichcolumn->unique);
			pvt->_cachedest->write(whichcolumn->partofkey);
			pvt->_cachedest->write(whichcolumn->unsignednumber);
			pvt->_cachedest->write(whichcolumn->zerofill);
			pvt->_cachedest->write(whichcolumn->binary);
			pvt->_cachedest->write(whichcolumn->autoincrement);

			// write the table
			tablelen=charstring::length(whichcolumn->table);
			pvt->_cachedest->write(tablelen);
			pvt->_cachedest->write(whichcolumn->table,tablelen);
		}
	}
}

void sqlrcursor::cacheOutputBinds(uint32_t count) {

	if (pvt->_resumed || !pvt->_cachedest) {
		return;
	}

	// write the variable/value pairs to the cache file
	uint16_t	len;
	for (uint32_t i=0; i<count; i++) {

		pvt->_cachedest->write((uint16_t)(*pvt->_outbindvars)[i].type);

		len=charstring::length((*pvt->_outbindvars)[i].variable);
		pvt->_cachedest->write(len);
		pvt->_cachedest->write((*pvt->_outbindvars)[i].variable,len);

		len=(*pvt->_outbindvars)[i].resultvaluesize;
		pvt->_cachedest->write(len);
		if ((*pvt->_outbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_STRING ||
			(*pvt->_outbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_BLOB ||
			(*pvt->_outbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_CLOB) {
			pvt->_cachedest->write(
				(*pvt->_outbindvars)[i].value.stringval,len);
			pvt->_cachedest->write(
				(*pvt->_outbindvars)[i].value.lobval,len);
		} else if ((*pvt->_outbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_INTEGER) {
			pvt->_cachedest->write(
				(*pvt->_outbindvars)[i].value.integerval);
		} else if ((*pvt->_outbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_DOUBLE) {
			pvt->_cachedest->write(
				(*pvt->_outbindvars)[i].value.
							doubleval.value);
			pvt->_cachedest->write(
				(*pvt->_outbindvars)[i].value.
							doubleval.precision);
			pvt->_cachedest->write(
				(*pvt->_outbindvars)[i].value.
							doubleval.scale);
		}
	}

	// terminate the list of output binds
	pvt->_cachedest->write((uint16_t)END_BIND_VARS);
}

void sqlrcursor::cacheInputOutputBinds(uint32_t count) {

	if (pvt->_resumed || !pvt->_cachedest) {
		return;
	}

	// FIXME: implement this

	// terminate the list of output binds
	pvt->_cachedest->write((uint16_t)END_BIND_VARS);
}

void sqlrcursor::cacheData() {

	if (!pvt->_cachedest) {
		return;
	}

	// write the data to the cache file
	uint32_t	rowbuffercount=pvt->_rowcount-pvt->_firstrowindex;
	for (uint32_t i=0; i<rowbuffercount; i++) {

		// get the current offset in the cache destination file
		int64_t	position=pvt->_cachedest->getCurrentPosition();

		// seek to the right place in the index file and write the
		// destination file offset
		pvt->_cachedestind->setPositionRelativeToBeginning(
				13+sizeof(int64_t)+
				((pvt->_firstrowindex+i)*sizeof(int64_t)));
		pvt->_cachedestind->write(position);

		// write the row to the cache file
		for (uint32_t j=0; j<pvt->_colcount; j++) {
			uint16_t	type;
			int32_t		len;
			char		*field=getFieldInternal(i,j);
			if (field) {
				type=STRING_DATA;
				len=charstring::length(field);
				pvt->_cachedest->write(type);
				pvt->_cachedest->write(len);
				if (len>0) {
					pvt->_cachedest->write(field);
				}
			} else {
				type=NULL_DATA;
				pvt->_cachedest->write(type);
			}
		}
	}

	if (pvt->_endofresultset) {
		finishCaching();
	}
}

void sqlrcursor::finishCaching() {

	if (!pvt->_cachedest) {
		return;
	}

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Finishing caching.\n");
		pvt->_sqlrc->debugPreEnd();
	}

	// terminate the result set
	pvt->_cachedest->write((uint16_t)END_RESULT_SET);
	// FIXME: I think rudiments bugs keep this from working...
	/*pvt->_cachedest->flushWriteBuffer(-1,-1);
	pvt->_cachedestind->flushWriteBuffer(-1,-1);*/

	// close the cache file and clean up
	clearCacheDest();
}

void sqlrcursor::clearCacheDest() {

	// close the cache file and clean up
	if (pvt->_cachedest) {
		pvt->_cachedest->close();
		delete pvt->_cachedest;
		pvt->_cachedest=NULL;
		pvt->_cachedestind->close();
		delete pvt->_cachedestind;
		pvt->_cachedestind=NULL;
		pvt->_cacheon=false;
	}
}

bool sqlrcursor::getDatabaseList(const char *wild) {
	return getDatabaseList(wild,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getDatabaseList(const char *wild,
					sqlrclientlistformat_t listformat) {
	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("getting database list");
		if (wild) {
			pvt->_sqlrc->debugPrint("\"");
			pvt->_sqlrc->debugPrint(wild);
			pvt->_sqlrc->debugPrint("\"");
		}
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}
	return getList(GETDBLIST,listformat,NULL,wild,0);
}

bool sqlrcursor::getSchemaList(const char *wild) {
	return getSchemaList(wild,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getSchemaList(const char *wild,
					sqlrclientlistformat_t listformat) {
	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("getting schema list");
		if (wild) {
			pvt->_sqlrc->debugPrint("\"");
			pvt->_sqlrc->debugPrint(wild);
			pvt->_sqlrc->debugPrint("\"");
		}
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}
	return getList(GETSCHEMALIST,listformat,NULL,wild,0);
}

bool sqlrcursor::getTableList(const char *wild) {
	return getTableList(wild,SQLRCLIENTLISTFORMAT_MYSQL,
				DB_OBJECT_TABLE|DB_OBJECT_VIEW|
				DB_OBJECT_ALIAS|DB_OBJECT_SYNONYM);
}

bool sqlrcursor::getTableList(const char *wild,
					sqlrclientlistformat_t listformat,
					uint16_t objecttypes) {
	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("getting table list ");
		if (wild) {
			pvt->_sqlrc->debugPrint("\"");
			pvt->_sqlrc->debugPrint(wild);
			pvt->_sqlrc->debugPrint("\"");
		}
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}
	return getList(GETTABLELIST2,listformat,NULL,wild,objecttypes);
}

bool sqlrcursor::getTableTypeList(const char *wild) {
	return getTableTypeList(wild,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getTableTypeList(const char *wild,
					sqlrclientlistformat_t listformat) {
	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("getting table type list ");
		if (wild) {
			pvt->_sqlrc->debugPrint("\"");
			pvt->_sqlrc->debugPrint(wild);
			pvt->_sqlrc->debugPrint("\"");
		}
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}
	return getList(GETTABLETYPELIST,listformat,NULL,wild,0);
}

bool sqlrcursor::getColumnList(const char *table, const char *wild) {
	return getColumnList(table,wild,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getColumnList(const char *table,
				const char *wild,
				sqlrclientlistformat_t listformat) {
	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("getting column list for: \"");
		pvt->_sqlrc->debugPrint(table);
		pvt->_sqlrc->debugPrint("\"");
		if (wild) {
			pvt->_sqlrc->debugPrint(" - \"");
			pvt->_sqlrc->debugPrint(wild);
			pvt->_sqlrc->debugPrint("\"");
		}
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}
	return getList(GETCOLUMNLIST,listformat,(table)?table:"",wild,0);
}

bool sqlrcursor::getPrimaryKeysList(const char *table, const char *wild) {
	return getPrimaryKeysList(table,wild,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getPrimaryKeysList(const char *table,
					const char *wild,
					sqlrclientlistformat_t listformat) {
	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("getting primary keys for: \"");
		pvt->_sqlrc->debugPrint(table);
		pvt->_sqlrc->debugPrint("\"");
		if (wild) {
			pvt->_sqlrc->debugPrint(" - \"");
			pvt->_sqlrc->debugPrint(wild);
			pvt->_sqlrc->debugPrint("\"");
		}
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}
	return getList(GETPRIMARYKEYLIST,listformat,(table)?table:"",wild,0);
}

bool sqlrcursor::getKeyAndIndexList(const char *table, const char *qualifier) {
	return getKeyAndIndexList(table,qualifier,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getKeyAndIndexList(const char *table,
					const char *qualifier,
					sqlrclientlistformat_t listformat) {
	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("getting keys and indexes for: \"");
		pvt->_sqlrc->debugPrint(table);
		pvt->_sqlrc->debugPrint("\"");
		if (qualifier) {
			pvt->_sqlrc->debugPrint(" - \"");
			pvt->_sqlrc->debugPrint(qualifier);
			pvt->_sqlrc->debugPrint("\"");
		}
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}
	return getList(GETKEYANDINDEXLIST,listformat,
					(table)?table:"",qualifier,0);
}

bool sqlrcursor::getProcedureBindAndColumnList(
				const char *procedure,
				const char *wild) {
	return getProcedureBindAndColumnList(procedure,wild,
					SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getProcedureBindAndColumnList(
				const char *procedure,
				const char *wild,
				sqlrclientlistformat_t listformat) {
	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("getting procedure bind "
					"and column list for: \"");
		pvt->_sqlrc->debugPrint(procedure);
		pvt->_sqlrc->debugPrint("\"");
		if (wild) {
			pvt->_sqlrc->debugPrint(" - \"");
			pvt->_sqlrc->debugPrint(wild);
			pvt->_sqlrc->debugPrint("\"");
		}
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}
	return getList(GETPROCEDUREBINDANDCOLUMNLIST,
				listformat,(procedure)?procedure:"",wild,0);
}

bool sqlrcursor::getTypeInfoList(const char *type, const char *wild) {
	return getTypeInfoList(type,wild,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getTypeInfoList(const char *type,
				const char *wild,
				sqlrclientlistformat_t listformat) {
	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("getting type info for: \"");
		pvt->_sqlrc->debugPrint(type);
		pvt->_sqlrc->debugPrint("\"");
		if (wild) {
			pvt->_sqlrc->debugPrint(" - \"");
			pvt->_sqlrc->debugPrint(wild);
			pvt->_sqlrc->debugPrint("\"");
		}
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}
	return getList(GETTYPEINFOLIST,listformat,(type)?type:"",wild,0);
}

bool sqlrcursor::getProcedureList(const char *wild) {
	return getProcedureList(wild,SQLRCLIENTLISTFORMAT_MYSQL);
}

bool sqlrcursor::getProcedureList(const char *wild,
					sqlrclientlistformat_t listformat) {
	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("getting procedure list ");
		if (wild) {
			pvt->_sqlrc->debugPrint("\"");
			pvt->_sqlrc->debugPrint(wild);
			pvt->_sqlrc->debugPrint("\"");
		}
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}
	return getList(GETPROCEDURELIST,listformat,NULL,wild,0);
}

bool sqlrcursor::getList(uint16_t command, sqlrclientlistformat_t listformat,
					const char *table, const char *wild,
					uint16_t objecttypes) {

	pvt->_reexecute=false;
	pvt->_validatebinds=false;
	pvt->_resumed=false;
	clearVariables();

	if (!pvt->_endofresultset) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!pvt->_sqlrc->openSession()) {
		return false;
	}

	pvt->_cached=false;
	pvt->_endofresultset=false;

	// refresh socket client
	pvt->_cs=pvt->_sqlrc->cs();

	// tell the server we want to get a list
	pvt->_cs->write(command);

	// tell the server whether we'll need a cursor or not
	sendCursorStatus();

	// send the list format
	pvt->_cs->write((uint16_t)listformat);

	// send the wild parameter
	uint32_t	len=charstring::length(wild);
	pvt->_cs->write(len);
	if (len) {
		pvt->_cs->write(wild,len);
	}

	// send the table parameter
	if (table) {
		len=charstring::length(table);
		pvt->_cs->write(len);
		if (len) {
			pvt->_cs->write(table,len);
		}
	}

	// send the objecttypes parameter
	if (command==GETTABLELIST2) {
		pvt->_cs->write(objecttypes);
	}

	pvt->_sqlrc->flushWriteBuffer();

	// process the initial result set
	bool	retval=processInitialResultSet();

	// set up not to re-execute the same query if executeQuery is called
	// again before calling prepareQuery on a new query
	pvt->_reexecute=false;

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
	pvt->_reexecute=false;
	pvt->_validatebinds=false;
	pvt->_resumed=false;
	clearVariables(pvt->_clearbindsduringprepare);
	pvt->_querylen=length;
	if (pvt->_copyrefs) {
		initQueryBuffer(pvt->_querylen);
		charstring::copy(pvt->_querybuffer,query,pvt->_querylen);
		pvt->_querybuffer[pvt->_querylen]='\0';
	} else {
		pvt->_queryptr=query;
	}
}

bool sqlrcursor::prepareFileQuery(const char *path, const char *filename) {

	// init some variables
	pvt->_reexecute=false;
	pvt->_validatebinds=false;
	pvt->_resumed=false;
	clearVariables();

	// init the fullpath buffer
	if (!pvt->_fullpath) {
		pvt->_fullpath=new char[MAXPATHLEN+1];
	}

	// add the path to the fullpath
	uint32_t	index=0;
	uint32_t	counter=0;
	if (path) {
		while (path[index] && counter<MAXPATHLEN) {
			pvt->_fullpath[counter]=path[index];
			index++;
			counter++;
		}

		// add the "/" to the fullpath
		if (counter<=MAXPATHLEN) {
			pvt->_fullpath[counter]='/';
			counter++;
		}
	}

	// add the file to the fullpath
	index=0;
	while (filename[index] && counter<MAXPATHLEN) {
		pvt->_fullpath[counter]=filename[index];
		index++;
		counter++;
	}

	// handle a filename that's too long
	if (counter>MAXPATHLEN) {

		// sabotage the file name so it can't be opened
		pvt->_fullpath[0]='\0';

		// debug info
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("File name ");
			if (path) {
				pvt->_sqlrc->debugPrint((char *)path);
				pvt->_sqlrc->debugPrint("/");
			}
			pvt->_sqlrc->debugPrint((char *)filename);
			pvt->_sqlrc->debugPrint(" is too long.");
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}

	} else {

		// terminate the string
		pvt->_fullpath[counter]='\0';

		// debug info
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("File: ");
			pvt->_sqlrc->debugPrint(pvt->_fullpath);
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}
	}

	// open the file
	file	queryfile;
	if (!queryfile.open(pvt->_fullpath,O_RDONLY)) {

		// set the error
		char	*err=new char[32+charstring::length(pvt->_fullpath)];
		charstring::append(err,"The file ");
		charstring::append(err,pvt->_fullpath);
		charstring::append(err," could not be opened.\n");
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint(err);
			pvt->_sqlrc->debugPreEnd();
		}
		setError(err);

		// set queryptr to NULL so executeQuery won't try to do
		// anything with it in the event that it gets called
		pvt->_queryptr=NULL;

		delete[] err;

		return false;
	}

	initQueryBuffer(queryfile.getSize());

	// read the file into the query buffer
	pvt->_querylen=queryfile.getSize();
	queryfile.read((unsigned char *)pvt->_querybuffer,pvt->_querylen);
	pvt->_querybuffer[pvt->_querylen]='\0';

	queryfile.close();

	return true;
}

void sqlrcursor::initQueryBuffer(uint32_t querylength) {
	delete[] pvt->_querybuffer;
	pvt->_querybuffer=new char[querylength+1];
	pvt->_queryptr=pvt->_querybuffer;
}

void sqlrcursor::attachToBindCursor(uint16_t bindcursorid) {
	prepareQuery("");
	pvt->_reexecute=true;
	pvt->_cursorid=bindcursorid;
}

uint16_t sqlrcursor::countBindVariables() const {
	return ::countBindVariables(pvt->_queryptr,pvt->_querylen,
		pvt->_sqlrc->getBindVariableDelimiterQuestionMarkSupported(),
		pvt->_sqlrc->getBindVariableDelimiterColonSupported(),
		pvt->_sqlrc->getBindVariableDelimiterAtSignSupported(),
		pvt->_sqlrc->getBindVariableDelimiterDollarSignSupported());
}

void sqlrcursor::clearVariables() {
	clearVariables(true);
}

void sqlrcursor::clearVariables(bool clearbinds) {
	deleteSubstitutionVariables();
	pvt->_subvars->clear();
	pvt->_dirtysubs=false;
	if (clearbinds) {
		pvt->_dirtybinds=false;
		clearBinds();
	}
}

void sqlrcursor::deleteVariables() {
	deleteSubstitutionVariables();
	deleteInputBindVariables();
	deleteOutputBindVariables();
	deleteInputOutputBindVariables();
}

void sqlrcursor::deleteSubstitutionVariables() {

	if (pvt->_copyrefs) {
		for (uint64_t i=0; i<pvt->_subvars->getLength(); i++) {
			delete[] (*pvt->_subvars)[i].variable;
			if ((*pvt->_subvars)[i].type==
					SQLRCLIENTBINDVARTYPE_STRING) {
				delete[] (*pvt->_subvars)[i].value.stringval;
			}
			if ((*pvt->_subvars)[i].type==
					SQLRCLIENTBINDVARTYPE_DATE) {
				delete[] (*pvt->_subvars)[i].value.dateval.tz;
			}
		}
	}
}

void sqlrcursor::deleteInputBindVariables() {

	if (pvt->_copyrefs) {
		for (uint64_t i=0; i<pvt->_inbindvars->getLength(); i++) {
			delete[] (*pvt->_inbindvars)[i].variable;
			if ((*pvt->_inbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_STRING) {
				delete[] (*pvt->_inbindvars)[i].value.stringval;
			}
			if ((*pvt->_inbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_BLOB ||
				(*pvt->_inbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_CLOB) {
				delete[] (*pvt->_inbindvars)[i].value.lobval;
			}
			if ((*pvt->_inbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_DATE) {
				delete[] (*pvt->_inbindvars)[i].
							value.dateval.tz;
			}
		}
	}
}

void sqlrcursor::deleteOutputBindVariables() {

	for (uint64_t i=0; i<pvt->_outbindvars->getLength(); i++) {
		if (pvt->_copyrefs) {
			delete[] (*pvt->_outbindvars)[i].variable;
		}
		if ((*pvt->_outbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_STRING) {
			delete[] (*pvt->_outbindvars)[i].value.stringval;
		}
		if ((*pvt->_outbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_BLOB ||
			(*pvt->_outbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_CLOB) {
			delete[] (*pvt->_outbindvars)[i].value.lobval;
		}
		if ((*pvt->_outbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_DATE) {
			delete[] (*pvt->_outbindvars)[i].value.dateval.tz;
		}
	}
}

void sqlrcursor::deleteInputOutputBindVariables() {

	for (uint64_t i=0; i<pvt->_inoutbindvars->getLength(); i++) {
		if (pvt->_copyrefs) {
			delete[] (*pvt->_inoutbindvars)[i].variable;
		}
		if ((*pvt->_inoutbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_STRING) {
			delete[] (*pvt->_inoutbindvars)[i].value.stringval;
		}
		if ((*pvt->_inoutbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_BLOB ||
			(*pvt->_inoutbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_CLOB) {
			delete[] (*pvt->_inoutbindvars)[i].value.lobval;
		}
		if ((*pvt->_inoutbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_DATE) {
			delete[] (*pvt->_inoutbindvars)[i].value.dateval.tz;
		}
	}
}

void sqlrcursor::substitution(const char *variable, const char *value) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool			preexisting=true;
	sqlrclientbindvar	*bv=pvt->findVar(variable,pvt->_subvars);
	if (!bv) {
		bv=&(*pvt->_subvars)[pvt->_subvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	stringVar(bv,variable,value);
	pvt->_dirtysubs=true;
}

void sqlrcursor::substitution(const char *variable, int64_t value) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool			preexisting=true;
	sqlrclientbindvar	*bv=pvt->findVar(variable,pvt->_subvars);
	if (!bv) {
		bv=&(*pvt->_subvars)[pvt->_subvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	integerVar(bv,variable,value);
	pvt->_dirtysubs=true;
}

void sqlrcursor::substitution(const char *variable, double value, 
				uint32_t precision, uint32_t scale) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool			preexisting=true;
	sqlrclientbindvar	*bv=pvt->findVar(variable,pvt->_subvars);
	if (!bv) {
		bv=&(*pvt->_subvars)[pvt->_subvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	doubleVar(bv,variable,value,precision,scale);
	pvt->_dirtysubs=true;
}

void sqlrcursor::clearBinds() {

	deleteInputBindVariables();
	pvt->_inbindvars->clear();

	deleteOutputBindVariables();
	pvt->_outbindvars->clear();

	deleteInputOutputBindVariables();
	pvt->_inoutbindvars->clear();
}

void sqlrcursor::inputBindBlob(const char *variable, const char *value,
							uint32_t size) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool			preexisting=true;
	sqlrclientbindvar	*bv=pvt->findVar(variable,pvt->_inbindvars);
	if (!bv) {
		bv=&(*pvt->_inbindvars)[pvt->_inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	lobVar(bv,variable,value,size,SQLRCLIENTBINDVARTYPE_BLOB);
	bv->send=true;
	pvt->_dirtybinds=true;
}

void sqlrcursor::inputBindClob(const char *variable, const char *value,
							uint32_t size) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool			preexisting=true;
	sqlrclientbindvar	*bv=pvt->findVar(variable,pvt->_inbindvars);
	if (!bv) {
		bv=&(*pvt->_inbindvars)[pvt->_inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	lobVar(bv,variable,value,size,SQLRCLIENTBINDVARTYPE_CLOB);
	bv->send=true;
	pvt->_dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable, const char *value) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool			preexisting=true;
	sqlrclientbindvar	*bv=pvt->findVar(variable,pvt->_inbindvars);
	if (!bv) {
		bv=&(*pvt->_inbindvars)[pvt->_inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	stringVar(bv,variable,value);
	bv->send=true;
	pvt->_dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable, const char *value,
						uint32_t valuesize) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool			preexisting=true;
	sqlrclientbindvar	*bv=pvt->findVar(variable,pvt->_inbindvars);
	if (!bv) {
		bv=&(*pvt->_inbindvars)[pvt->_inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	stringVar(bv,variable,value,valuesize);
	bv->send=true;
	pvt->_dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable, int64_t value) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool			preexisting=true;
	sqlrclientbindvar	*bv=pvt->findVar(variable,pvt->_inbindvars);
	if (!bv) {
		bv=&(*pvt->_inbindvars)[pvt->_inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	integerVar(bv,variable,value);
	bv->send=true;
	pvt->_dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable, double value, 
				uint32_t precision, uint32_t scale) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool			preexisting=true;
	sqlrclientbindvar	*bv=pvt->findVar(variable,pvt->_inbindvars);
	if (!bv) {
		bv=&(*pvt->_inbindvars)[pvt->_inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	doubleVar(bv,variable,value,precision,scale);
	bv->send=true;
	pvt->_dirtybinds=true;
}

void sqlrcursor::inputBind(const char *variable,
				int16_t year, int16_t month, int16_t day,
				int16_t hour, int16_t minute, int16_t second,
				int32_t microsecond, const char *tz,
				bool isnegative) {
	if (charstring::isNullOrEmpty(variable)) {
		return;
	}
	bool			preexisting=true;
	sqlrclientbindvar	*bv=pvt->findVar(variable,pvt->_inbindvars);
	if (!bv) {
		bv=&(*pvt->_inbindvars)[pvt->_inbindvars->getLength()];
		preexisting=false;
	}
	initVar(bv,variable,preexisting);
	dateVar(bv,variable,year,month,day,hour,
		minute,second,microsecond,tz,isnegative);
	bv->send=true;
	pvt->_dirtybinds=true;
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

void sqlrcursor::stringVar(sqlrclientbindvar *var,
					const char *variable,
					const char *value) {
	stringVar(var,variable,value,charstring::length(value));
}

void sqlrcursor::stringVar(sqlrclientbindvar *var,
					const char *variable,
					const char *value,
					uint32_t valuesize) {

	// store the value, handle NULL values too
	if (value) {
		if (pvt->_copyrefs) {
			var->value.stringval=charstring::duplicate(value);
		} else {
			var->value.stringval=(char *)value;
		}
		var->valuesize=valuesize;
		var->type=SQLRCLIENTBINDVARTYPE_STRING;
	} else {
		var->type=SQLRCLIENTBINDVARTYPE_NULL;
	}
}

void sqlrcursor::integerVar(sqlrclientbindvar *var,
					const char *variable,
					int64_t value) {
	var->type=SQLRCLIENTBINDVARTYPE_INTEGER;
	var->value.integerval=value;
}

void sqlrcursor::doubleVar(sqlrclientbindvar *var,
					const char *variable,
					double value,
					uint32_t precision,
					uint32_t scale) {
	var->type=SQLRCLIENTBINDVARTYPE_DOUBLE;
	var->value.doubleval.value=value;
	var->value.doubleval.precision=precision;
	var->value.doubleval.scale=scale;
}

void sqlrcursor::dateVar(sqlrclientbindvar *var,
					const char *variable,
					int16_t year,
					int16_t month,
					int16_t day,
					int16_t hour,
					int16_t minute,
					int16_t second,
					int32_t microsecond,
					const char *tz,
					bool isnegative) {
	var->type=SQLRCLIENTBINDVARTYPE_DATE;
	var->value.dateval.year=year;
	var->value.dateval.month=month;
	var->value.dateval.day=day;
	var->value.dateval.hour=hour;
	var->value.dateval.minute=minute;
	var->value.dateval.second=second;
	var->value.dateval.microsecond=microsecond;
	var->value.dateval.isnegative=isnegative;
	if (pvt->_copyrefs) {
		var->value.dateval.tz=charstring::duplicate(tz);
	} else {
		var->value.dateval.tz=(char *)tz;
	}
}

void sqlrcursor::lobVar(sqlrclientbindvar *var,
					const char *variable,
					const char *value,
					uint32_t size,
					sqlrclientbindvartype_t type) {

	// Store the value, handle NULL values too.
	// For LOB's empty strings are handled as NULL's as well, this is
	// probably not right, but I can't get empty string lob binds to work.
	if (value && size>0) {
		if (pvt->_copyrefs) {
			var->value.lobval=new char[size];
			bytestring::copy(var->value.lobval,value,size);
		} else {
			var->value.lobval=(char *)value;
		}
		var->valuesize=size;
		var->type=type;
	} else {
		var->type=SQLRCLIENTBINDVARTYPE_NULL;
	}
}

void sqlrcursor::initVar(sqlrclientbindvar *var,
				const char *variable,
				bool preexisting) {

	// clear any old variable name that was stored and assign the new 
	// variable name also clear any old value that was stored in this 
	// variable
	if (pvt->_copyrefs) {
		if (preexisting) {
			delete[] var->variable;
			if (var->type==SQLRCLIENTBINDVARTYPE_STRING) {
				delete[] var->value.stringval;
			} else if (var->type==SQLRCLIENTBINDVARTYPE_BLOB ||
					var->type==SQLRCLIENTBINDVARTYPE_CLOB) {
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
	defineOutputBindGeneric(variable,
				SQLRCLIENTBINDVARTYPE_STRING,length);
}

void sqlrcursor::defineOutputBindInteger(const char *variable) {
	defineOutputBindGeneric(variable,
				SQLRCLIENTBINDVARTYPE_INTEGER,sizeof(int64_t));
}

void sqlrcursor::defineOutputBindDouble(const char *variable) {
	defineOutputBindGeneric(variable,
				SQLRCLIENTBINDVARTYPE_DOUBLE,sizeof(double));
}

void sqlrcursor::defineOutputBindDate(const char *variable) {
	defineOutputBindGeneric(variable,
				SQLRCLIENTBINDVARTYPE_DATE,0);
}

void sqlrcursor::defineOutputBindBlob(const char *variable) {
	defineOutputBindGeneric(variable,
				SQLRCLIENTBINDVARTYPE_BLOB,0);
}

void sqlrcursor::defineOutputBindClob(const char *variable) {
	defineOutputBindGeneric(variable,
				SQLRCLIENTBINDVARTYPE_CLOB,0);
}

void sqlrcursor::defineOutputBindCursor(const char *variable) {
	defineOutputBindGeneric(variable,
				SQLRCLIENTBINDVARTYPE_CURSOR,0);
}

void sqlrcursor::defineOutputBindGeneric(const char *variable,
						sqlrclientbindvartype_t type,
						uint32_t valuesize) {

	if (charstring::isNullOrEmpty(variable)) {
		return;
	}

	bool			preexisting=true;
	sqlrclientbindvar	*bv=pvt->findVar(variable,pvt->_outbindvars);
	if (!bv) {
		bv=&(*pvt->_outbindvars)[pvt->_outbindvars->getLength()];
		preexisting=false;
		pvt->_dirtybinds=true;
	}

	// clean up old values and set new values
	if (preexisting) {
		if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
			delete[] bv->value.stringval;
		} else if (bv->type==SQLRCLIENTBINDVARTYPE_BLOB ||
				bv->type==SQLRCLIENTBINDVARTYPE_CLOB) {
			delete[] bv->value.lobval;
		}
	}
	if (pvt->_copyrefs) {
		if (preexisting) {
			delete[] bv->variable;
		}
		bv->variable=charstring::duplicate(variable);
	} else {
		bv->variable=(char *)variable;
	}
	bv->type=type;
	if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
		bv->value.stringval=NULL;
	} else if (bv->type==SQLRCLIENTBINDVARTYPE_BLOB ||
				bv->type==SQLRCLIENTBINDVARTYPE_CLOB) {
		bv->value.lobval=NULL;
	}
	bv->valuesize=valuesize;
	bv->resultvaluesize=0;
	bv->send=true;
}

void sqlrcursor::defineInputOutputBindString(const char *variable,
							const char *value,
							uint32_t length) {
	defineInputOutputBindGeneric(variable,
				SQLRCLIENTBINDVARTYPE_STRING,
				value,0,0.0,0,0,
				0,0,0,0,0,0,0,NULL,false,
				length);
}

void sqlrcursor::defineInputOutputBindInteger(const char *variable,
							int64_t value) {
	defineInputOutputBindGeneric(variable,
				SQLRCLIENTBINDVARTYPE_INTEGER,
				NULL,value,0.0,0,0,
				0,0,0,0,0,0,0,NULL,false,
				sizeof(int64_t));
}

void sqlrcursor::defineInputOutputBindDouble(const char *variable,
							double value,
							uint32_t precision,
							uint32_t scale) {
	defineInputOutputBindGeneric(variable,
				SQLRCLIENTBINDVARTYPE_DOUBLE,
				NULL,0,value,precision,scale,
				0,0,0,0,0,0,0,NULL,false,
				sizeof(double));
}

void sqlrcursor::defineInputOutputBindDate(const char *variable,
						int16_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						bool isnegative) {
	defineInputOutputBindGeneric(variable,
				SQLRCLIENTBINDVARTYPE_DATE,
				NULL,0,0.0,0,0,
				year,month,day,minute,
				hour,second,microsecond,
				tz,isnegative,
				0);
}

void sqlrcursor::defineOutputBindBlob(const char *variable,
						const char *value,
						uint32_t size) {
	// FIXME: implement this...
}

void sqlrcursor::defineOutputBindClob(const char *variable,
						const char *value,
						uint32_t size) {
	// FIXME: implement this...
}

void sqlrcursor::defineInputOutputBindGeneric(const char *variable,
						sqlrclientbindvartype_t type,
						const char *strvalue,
						int64_t intvalue,
						double doublevalue,
						uint32_t precision,
						uint32_t scale,
						int16_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						bool isnegative,
						uint32_t valuesize) {

	if (charstring::isNullOrEmpty(variable)) {
		return;
	}

	bool			preexisting=true;
	sqlrclientbindvar	*bv=pvt->findVar(variable,pvt->_inoutbindvars);
	if (!bv) {
		bv=&(*pvt->_inoutbindvars)[pvt->_inoutbindvars->getLength()];
		preexisting=false;
		pvt->_dirtybinds=true;
	}

	// clean up old values and set new values
	if (preexisting) {
		if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
			delete[] bv->value.stringval;
		} else if (bv->type==SQLRCLIENTBINDVARTYPE_BLOB ||
				bv->type==SQLRCLIENTBINDVARTYPE_CLOB) {
			delete[] bv->value.lobval;
		}
	}
	if (pvt->_copyrefs) {
		if (preexisting) {
			delete[] bv->variable;
		}
		bv->variable=charstring::duplicate(variable);
	} else {
		bv->variable=(char *)variable;
	}
	bv->type=type;
	if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
		bv->value.stringval=new char[valuesize+1];
		if (strvalue) {
			charstring::copy(bv->value.stringval,
						strvalue,valuesize);
		} else {
			bv->value.stringval[0]='\0';
			bv->type=SQLRCLIENTBINDVARTYPE_NULL;
		}
	} else if (bv->type==SQLRCLIENTBINDVARTYPE_INTEGER) {
		bv->value.integerval=intvalue;
	} else if (bv->type==SQLRCLIENTBINDVARTYPE_DOUBLE) {
		bv->value.doubleval.value=doublevalue;
		bv->value.doubleval.precision=precision;
		bv->value.doubleval.scale=scale;
	} else if (bv->type==SQLRCLIENTBINDVARTYPE_DATE) {
		bv->value.dateval.year=year;
		bv->value.dateval.month=month;
		bv->value.dateval.day=day;
		bv->value.dateval.hour=hour;
		bv->value.dateval.minute=minute;
		bv->value.dateval.second=second;
		bv->value.dateval.microsecond=microsecond;
		bv->value.dateval.tz=(char *)tz;
		bv->value.dateval.isnegative=isnegative;
	} else if (bv->type==SQLRCLIENTBINDVARTYPE_BLOB ||
				bv->type==SQLRCLIENTBINDVARTYPE_CLOB) {
		// FIXME: initialize....
		bv->value.lobval=NULL;
	}
	bv->valuesize=valuesize;
	bv->resultvaluesize=0;
	bv->send=true;
}

const char *sqlrcursor::getOutputBindString(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->_outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_outbindvars)[i].variable,variable) &&
				((*pvt->_outbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_STRING ||
				(*pvt->_outbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_NULL)) {
				return (*pvt->_outbindvars)[i].value.stringval;
			}
		}
	}
	return NULL;
}

uint32_t sqlrcursor::getOutputBindLength(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->_outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_outbindvars)[i].variable,variable)) {
				return (*pvt->_outbindvars)[i].resultvaluesize;
			}
		}
	}
	return 0;
}

const char *sqlrcursor::getOutputBindBlob(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->_outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_outbindvars)[i].variable,variable) &&
				(*pvt->_outbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_BLOB) {
				return (*pvt->_outbindvars)[i].value.lobval;
			}
		}
	}
	return NULL;
}

const char *sqlrcursor::getOutputBindClob(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->_outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_outbindvars)[i].variable,variable) &&
				(*pvt->_outbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_CLOB) {
				return (*pvt->_outbindvars)[i].value.lobval;
			}
		}
	}
	return NULL;
}

int64_t sqlrcursor::getOutputBindInteger(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->_outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_outbindvars)[i].variable,variable) &&
				(*pvt->_outbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_INTEGER) {
				return (*pvt->_outbindvars)[i].value.integerval;
			}
		}
	}
	return -1;
}

double sqlrcursor::getOutputBindDouble(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->_outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_outbindvars)[i].variable,variable) &&
				(*pvt->_outbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_DOUBLE) {
				return (*pvt->_outbindvars)[i].
							value.doubleval.value;
			}
		}
	}
	return -1.0;
}

bool sqlrcursor::getOutputBindDate(const char *variable,
			int16_t *year, int16_t *month, int16_t *day,
			int16_t *hour, int16_t *minute, int16_t *second,
			int32_t *microsecond, const char **tz,
			bool *isnegative) {

	if (variable) {
		for (uint64_t i=0; i<pvt->_outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_outbindvars)[i].variable,variable) &&
				(*pvt->_outbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_DATE) {
				*year=(*pvt->_outbindvars)[i].
						value.dateval.year;
				*month=(*pvt->_outbindvars)[i].
						value.dateval.month;
				*day=(*pvt->_outbindvars)[i].
						value.dateval.day;
				*hour=(*pvt->_outbindvars)[i].
						value.dateval.hour;
				*minute=(*pvt->_outbindvars)[i].
						value.dateval.minute;
				*second=(*pvt->_outbindvars)[i].
						value.dateval.second;
				*microsecond=(*pvt->_outbindvars)[i].
						value.dateval.microsecond;
				*tz=(*pvt->_outbindvars)[i].
						value.dateval.tz;
				*isnegative=(*pvt->_outbindvars)[i].
						value.dateval.isnegative;
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
	sqlrcursor	*bindcursor=new sqlrcursor(pvt->_sqlrc,copyrefs);
	bindcursor->attachToBindCursor(bindcursorid);
	return bindcursor;
}

const char *sqlrcursor::getInputOutputBindString(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->_inoutbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_inoutbindvars)[i].variable,variable) &&
				((*pvt->_inoutbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_STRING ||
				(*pvt->_inoutbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_NULL)) {
				return (*pvt->_inoutbindvars)[i].
							value.stringval;
			}
		}
	}
	return NULL;
}

uint32_t sqlrcursor::getInputOutputBindLength(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->_inoutbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_inoutbindvars)[i].variable,variable)) {
				return (*pvt->_inoutbindvars)[i].
							resultvaluesize;
			}
		}
	}
	return 0;
}

int64_t sqlrcursor::getInputOutputBindInteger(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->_inoutbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_inoutbindvars)[i].variable,variable) &&
				(*pvt->_inoutbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_INTEGER) {
				return (*pvt->_inoutbindvars)[i].
							value.integerval;
			}
		}
	}
	return -1;
}

double sqlrcursor::getInputOutputBindDouble(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->_inoutbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_inoutbindvars)[i].variable,variable) &&
				(*pvt->_inoutbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_DOUBLE) {
				return (*pvt->_inoutbindvars)[i].
							value.doubleval.value;
			}
		}
	}
	return -1.0;
}

bool sqlrcursor::getInputOutputBindDate(const char *variable,
			int16_t *year, int16_t *month, int16_t *day,
			int16_t *hour, int16_t *minute, int16_t *second,
			int32_t *microsecond, const char **tz,
			bool *isnegative) {

	if (variable) {
		for (uint64_t i=0; i<pvt->_inoutbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_inoutbindvars)[i].variable,variable) &&
				(*pvt->_inoutbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_DATE) {
				*year=(*pvt->_inoutbindvars)[i].
						value.dateval.year;
				*month=(*pvt->_inoutbindvars)[i].
						value.dateval.month;
				*day=(*pvt->_inoutbindvars)[i].
						value.dateval.day;
				*hour=(*pvt->_inoutbindvars)[i].
						value.dateval.hour;
				*minute=(*pvt->_inoutbindvars)[i].
						value.dateval.minute;
				*second=(*pvt->_inoutbindvars)[i].
						value.dateval.second;
				*microsecond=(*pvt->_inoutbindvars)[i].
						value.dateval.microsecond;
				*tz=(*pvt->_inoutbindvars)[i].
						value.dateval.tz;
				*isnegative=(*pvt->_inoutbindvars)[i].
						value.dateval.isnegative;
				return true;
			}
		}
	}
	return false;
}

const char *sqlrcursor::getInputOutputBindBlob(const char *variable) {
	// FIXME: implement this
	return NULL;
}

const char *sqlrcursor::getInputOutputBindClob(const char *variable) {
	// FIXME: implement this
	return NULL;
}

bool sqlrcursor::outputBindCursorIdIsValid(const char *variable) {
	if (variable) {
		for (uint64_t i=0; i<pvt->_outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_outbindvars)[i].variable,variable)) {
				return true;
			}
		}
	}
	return false;
}

uint16_t sqlrcursor::getOutputBindCursorId(const char *variable) {

	if (variable) {
		for (uint64_t i=0; i<pvt->_outbindvars->getLength(); i++) {
			if (!charstring::compare(
				(*pvt->_outbindvars)[i].variable,variable)) {
				return (*pvt->_outbindvars)[i].value.cursorid;
			}
		}
	}
	return 0;
}

void sqlrcursor::validateBinds() {
	pvt->_validatebinds=true;
}

bool sqlrcursor::validBind(const char *variable) {
	performSubstitutions();
	validateBindsInternal();
	for (uint64_t in=0; in<pvt->_inbindvars->getLength(); in++) {
		if (!charstring::compare(
			(*pvt->_inbindvars)[in].variable,variable)) {
			return (*pvt->_inbindvars)[in].send;
		}
	}
	for (uint64_t out=0; out<pvt->_outbindvars->getLength(); out++) {
		if (!charstring::compare(
			(*pvt->_outbindvars)[out].variable,variable)) {
			return (*pvt->_outbindvars)[out].send;
		}
	}
	for (uint64_t out=0; out<pvt->_inoutbindvars->getLength(); out++) {
		if (!charstring::compare(
			(*pvt->_inoutbindvars)[out].variable,variable)) {
			return (*pvt->_inoutbindvars)[out].send;
		}
	}
	return false;
}

bool sqlrcursor::executeQuery() {

	if (!pvt->_queryptr) {
		setError("No query to execute.");
		return false;
	}

	performSubstitutions();

	// validate the bind variables
	if (pvt->_validatebinds) {
		validateBindsInternal();
	}
		
	// run the query
	bool	retval=runQuery();

	// set up to re-execute the same query if executeQuery is called
	// again before calling prepareQuery
	pvt->_reexecute=true;

	return retval;
}

void sqlrcursor::performSubstitutions() {

	if (!pvt->_subvars->getLength() || !pvt->_dirtysubs) {
		return;
	}

	// perform substitutions
	stringbuffer	container;
	const char	*ptr=pvt->_queryptr;
	const char	*endptr=pvt->_queryptr+pvt->_querylen;
	bool		found=false;
	bool		inquotes=false;
	bool		inbraces=false;
	int		len=0;
	stringbuffer	*braces=NULL;

	// iterate through the string
	while (ptr<endptr) {
	
		// figure out whether we're inside a quoted 
		// string or not
		if (*ptr=='\'' && *(ptr-1)!='\\') {
			inquotes=!inquotes;
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
				i<pvt->_subvars->getLength() && !found; i++) {

	
				// if we find a match, write the 
				// value to the container and skip 
				// past the $(variable)
				len=charstring::length(
						(*pvt->_subvars)[i].variable);
				if (!(*pvt->_subvars)[i].donesubstituting &&
					!charstring::compare((ptr+2),
						(*pvt->_subvars)[i].
							variable,len) &&
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
	for (uint64_t i=0; i<pvt->_subvars->getLength(); i++) {
		(*pvt->_subvars)[i].donesubstituting=
					(*pvt->_subvars)[i].substituted;
	}

	delete[] pvt->_querybuffer;
	pvt->_querylen=container.getSize();
	pvt->_querybuffer=container.detachString();
	pvt->_queryptr=pvt->_querybuffer;

	pvt->_dirtysubs=false;
}

void sqlrcursor::validateBindsInternal() {

	if (!pvt->_dirtybinds) {
		return;
	}

	// check each input bind
	for (uint64_t in=0; in<pvt->_inbindvars->getLength(); in++) {

		// don't check bind-by-position variables
		if (charstring::isInteger(
				(*pvt->_inbindvars)[in].variable)) {
			continue;
		}

		(*pvt->_inbindvars)[in].send=
			validateBind((*pvt->_inbindvars)[in].variable);
	}

	// check each output bind
	for (uint64_t out=0; out<pvt->_outbindvars->getLength(); out++) {

		// don't check bind-by-position variables
		if (charstring::isInteger(
				(*pvt->_outbindvars)[out].variable)) {
			continue;
		}

		(*pvt->_outbindvars)[out].send=
			validateBind((*pvt->_outbindvars)[out].variable);
	}

	// check each input/output bind
	for (uint64_t inout=0;
		inout<pvt->_inoutbindvars->getLength(); inout++) {

		// don't check bind-by-position variables
		if (charstring::isInteger(
				(*pvt->_inoutbindvars)[inout].variable)) {
			continue;
		}

		(*pvt->_inoutbindvars)[inout].send=
			validateBind((*pvt->_inoutbindvars)[inout].variable);
	}
}

bool sqlrcursor::validateBind(const char *variable) {

	queryparsestate_t	parsestate=IN_QUERY;
	stringbuffer		currentbind;

	size_t	len=charstring::length(variable);

	// run through the querybuffer...
	const char	*ptr=pvt->_queryptr;
	const char	*endptr=pvt->_queryptr+pvt->_querylen;
	char		prev='\0';
	do {

		// if we're in the query...
		if (parsestate==IN_QUERY) {

			// if we find a quote, we're in quotes
			if (*ptr=='\'') {
				parsestate=IN_QUOTES;
			}

			// if we find whitespace or a couple of other things
			// then the next thing could be a bind variable
			if (beforeBindVariable(ptr)) {
				parsestate=BEFORE_BIND;
			}

			// move on
			if (*ptr=='\\' && prev=='\\') {
				prev='\0';
			} else {
				prev=*ptr;
			}
			ptr++;
			continue;
		}

		// copy anything in quotes verbatim
		if (parsestate==IN_QUOTES) {

			// if we find a quote, but not an escaped quote,
			// then we're back in the query
			// (or we're in between one of these: '...''...'
			// which is functionally the same)
			if (*ptr=='\'' && prev!='\\') {
				parsestate=IN_QUERY;
			}

			// move on
			if (*ptr=='\\' && prev=='\\') {
				prev='\0';
			} else {
				prev=*ptr;
			}
			ptr++;
			continue;
		}

		if (parsestate==BEFORE_BIND) {

			// if we find a bind variable...
			if (isBindDelimiter(ptr,
		pvt->_sqlrc->getBindVariableDelimiterQuestionMarkSupported(),
		pvt->_sqlrc->getBindVariableDelimiterColonSupported(),
		pvt->_sqlrc->getBindVariableDelimiterAtSignSupported(),
		pvt->_sqlrc->getBindVariableDelimiterDollarSignSupported())) {
				parsestate=IN_BIND;
				currentbind.clear();
				continue;
			}

			// if we didn't find a bind variable then we're just
			// back in the query
			parsestate=IN_QUERY;
			continue;
		}

		// if we're in a bind variable...
		if (parsestate==IN_BIND) {

			// If we find whitespace or a few other things
			// then we're done with the bind variable.  Process it.
			// Otherwise get the variable itself in another buffer.
			bool	endofbind=afterBindVariable(ptr);
			if (endofbind || ptr==endptr-1) {

				// special case...
				// last character in the query
				if (!endofbind && ptr==endptr-1) {
					currentbind.append(*ptr);
					if (*ptr=='\\' && prev=='\\') {
						prev='\0';
					} else {
						prev=*ptr;
					}
					ptr++;
				}

				// check variable against currentbind
				if (len==currentbind.getStringLength()-1 &&
					!charstring::compare(
						variable,
						currentbind.getString()+1,
						len)) {
					return true;
				}

				parsestate=IN_QUERY;

			} else {

				// move on
				currentbind.append(*ptr);
				if (*ptr=='\\' && prev=='\\') {
					prev='\0';
				} else {
					prev=*ptr;
				}
				ptr++;
			}
			continue;
		}

	} while (ptr<endptr);

	return false;
}

void sqlrcursor::performSubstitution(stringbuffer *buffer, uint16_t which) {

	if ((*pvt->_subvars)[which].type==
				SQLRCLIENTBINDVARTYPE_STRING) {
		buffer->append((*pvt->_subvars)[which].value.stringval);
	} else if ((*pvt->_subvars)[which].type==
				SQLRCLIENTBINDVARTYPE_INTEGER) {
		buffer->append((*pvt->_subvars)[which].value.integerval);
	} else if ((*pvt->_subvars)[which].type==
				SQLRCLIENTBINDVARTYPE_DOUBLE) {
		buffer->append((*pvt->_subvars)[which].value.doubleval.value,
			(*pvt->_subvars)[which].value.doubleval.precision,
			(*pvt->_subvars)[which].value.doubleval.scale);
	}
	(*pvt->_subvars)[which].substituted=true;
}

bool sqlrcursor::runQuery() {

	// send the query
	if (sendQueryInternal()) {

		sendInputBinds();
		sendOutputBinds();
		sendInputOutputBinds();
		sendGetColumnInfo();

		pvt->_sqlrc->flushWriteBuffer();

		if (processInitialResultSet()) {
			return true;
		}
	}
	return false;
}

bool sqlrcursor::sendQueryInternal() {

	// if the first 8 characters of the query are "-- debug" followed
	// by a return, then set debugging on
	if (!charstring::compare(pvt->_queryptr,"-- debug\n",9)) {
		pvt->_sqlrc->debugOn();
	}

	if (!pvt->_endofresultset) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!pvt->_sqlrc->openSession()) {
		return false;
	}

	pvt->_cached=false;
	pvt->_endofresultset=false;

	// refresh socket client
	pvt->_cs=pvt->_sqlrc->cs();

	// send the query to the server.
	if (!pvt->_reexecute) {

		// tell the server we're sending a query
		pvt->_cs->write((uint16_t)NEW_QUERY);

		// tell the server whether we'll need a cursor or not
		sendCursorStatus();

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Sending Client Info:");
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPrint("Length: ");
			pvt->_sqlrc->debugPrint(
					(int64_t)pvt->_sqlrc->clientinfolen());
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPrint(pvt->_sqlrc->clientinfo());
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}

		// send the client info
		// FIXME: arguably this should be its own command
		pvt->_cs->write(pvt->_sqlrc->clientinfolen());
		pvt->_cs->write(pvt->_sqlrc->clientinfo(),
					pvt->_sqlrc->clientinfolen());

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Sending Query:");
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPrint("Length: ");
			pvt->_sqlrc->debugPrint((int64_t)pvt->_querylen);
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPrint(pvt->_queryptr);
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}

		// send the query
		pvt->_cs->write(pvt->_querylen);
		pvt->_cs->write(pvt->_queryptr,pvt->_querylen);

	} else {

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Requesting re-execution of ");
			pvt->_sqlrc->debugPrint("previous query.");
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPrint("Requesting Cursor: ");
			pvt->_sqlrc->debugPrint((int64_t)pvt->_cursorid);
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}

		// tell the server we're sending a query
		pvt->_cs->write((uint16_t)REEXECUTE_QUERY);

		// send the cursor id to the server
		pvt->_cs->write(pvt->_cursorid);
	}

	return true;
}

void sqlrcursor::sendCursorStatus() {

	if (pvt->_havecursorid) {

		// tell the server we already have a cursor
		pvt->_cs->write((uint16_t)DONT_NEED_NEW_CURSOR);

		// send the cursor id to the server
		pvt->_cs->write(pvt->_cursorid);

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Requesting Cursor: ");
			pvt->_sqlrc->debugPrint((int64_t)pvt->_cursorid);
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}

	} else {

		// tell the server we need a cursor
		pvt->_cs->write((uint16_t)NEED_NEW_CURSOR);

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Requesting a new cursor.\n");
			pvt->_sqlrc->debugPreEnd();
		}
	}
}

void sqlrcursor::sendInputBinds() {

	// index
	uint16_t	i=0;

	// count number of vars to send
	uint16_t	count=pvt->_inbindvars->getLength();
	for (i=0; i<count; i++) {
		if (!(*pvt->_inbindvars)[i].send) {
			count--;
		}
	}

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Sending ");
		pvt->_sqlrc->debugPrint((int64_t)count);
		pvt->_sqlrc->debugPrint(" Input Bind Variables:\n");
		pvt->_sqlrc->debugPreEnd();
	}

	// write the input bind variables/values to the server.
	pvt->_cs->write(count);
	uint16_t	size;
	i=0;
	while (i<count) {

		// don't send anything if the send flag is turned off
		if (!(*pvt->_inbindvars)[i].send) {
			continue;
		}

		// send the variable
		size=charstring::length((*pvt->_inbindvars)[i].variable);
		pvt->_cs->write(size);
		pvt->_cs->write((*pvt->_inbindvars)[i].variable,(size_t)size);
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint(
					(*pvt->_inbindvars)[i].variable);
			pvt->_sqlrc->debugPrint("(");
			pvt->_sqlrc->debugPrint((int64_t)size);
		}

		// send the type
		pvt->_cs->write((uint16_t)(*pvt->_inbindvars)[i].type);

		// send the value
		if ((*pvt->_inbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_NULL) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPrint(":NULL)\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else if ((*pvt->_inbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_STRING) {

			pvt->_cs->write((*pvt->_inbindvars)[i].valuesize);
			if ((*pvt->_inbindvars)[i].valuesize>0) {
				pvt->_cs->write(
					(*pvt->_inbindvars)[i].value.stringval,
					(size_t)(*pvt->_inbindvars)[i].
								valuesize);
			}

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPrint(":STRING)=");
				pvt->_sqlrc->debugPrint(
					(*pvt->_inbindvars)[i].value.stringval);
				pvt->_sqlrc->debugPrint("(");
				pvt->_sqlrc->debugPrint(
					(int64_t)(*pvt->_inbindvars)[i].
								valuesize);
				pvt->_sqlrc->debugPrint(")");
				pvt->_sqlrc->debugPrint("\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else if ((*pvt->_inbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_INTEGER) {

			pvt->_cs->write((uint64_t)(*pvt->_inbindvars)[i].
							value.integerval);

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPrint(":LONG)=");
				pvt->_sqlrc->debugPrint(
					(int64_t)(*pvt->_inbindvars)[i].
							value.integerval);
				pvt->_sqlrc->debugPrint("\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else if ((*pvt->_inbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_DOUBLE) {

			pvt->_cs->write((*pvt->_inbindvars)[i].value.
							doubleval.value);
			pvt->_cs->write((*pvt->_inbindvars)[i].value.
							doubleval.precision);
			pvt->_cs->write((*pvt->_inbindvars)[i].value.
							doubleval.scale);

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPrint(":DOUBLE)=");
				pvt->_sqlrc->debugPrint(
					(*pvt->_inbindvars)[i].value.
							doubleval.value);
				pvt->_sqlrc->debugPrint(":");
				pvt->_sqlrc->debugPrint(
					(int64_t)(*pvt->_inbindvars)[i].
						value.doubleval.precision);
				pvt->_sqlrc->debugPrint(",");
				pvt->_sqlrc->debugPrint(
					(int64_t)(*pvt->_inbindvars)[i].
						value.doubleval.scale);
				pvt->_sqlrc->debugPrint("\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else if ((*pvt->_inbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_DATE) {

			pvt->_cs->write((uint16_t)(*pvt->_inbindvars)[i].
						value.dateval.year);
			pvt->_cs->write((uint16_t)(*pvt->_inbindvars)[i].
						value.dateval.month);
			pvt->_cs->write((uint16_t)(*pvt->_inbindvars)[i].
						value.dateval.day);
			pvt->_cs->write((uint16_t)(*pvt->_inbindvars)[i].
						value.dateval.hour);
			pvt->_cs->write((uint16_t)(*pvt->_inbindvars)[i].
						value.dateval.minute);
			pvt->_cs->write((uint16_t)(*pvt->_inbindvars)[i].
						value.dateval.second);
			pvt->_cs->write((uint32_t)(*pvt->_inbindvars)[i].
						value.dateval.microsecond);
			pvt->_cs->write((uint16_t)charstring::length(
						(*pvt->_inbindvars)[i].
							value.dateval.tz));
			pvt->_cs->write((*pvt->_inbindvars)[i].
						value.dateval.tz);
			pvt->_cs->write((*pvt->_inbindvars)[i].
						value.dateval.isnegative);

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPrint(":DATE)=");
				pvt->_sqlrc->debugPrint((int64_t)
					(*pvt->_inbindvars)[i].
						value.dateval.year);
				pvt->_sqlrc->debugPrint("-");
				pvt->_sqlrc->debugPrint((int64_t)
					(*pvt->_inbindvars)[i].
						value.dateval.month);
				pvt->_sqlrc->debugPrint("-");
				pvt->_sqlrc->debugPrint((int64_t)
					(*pvt->_inbindvars)[i].
						value.dateval.day);
				pvt->_sqlrc->debugPrint(" ");
				if ((*pvt->_inbindvars)[i].
						value.dateval.isnegative) {
					pvt->_sqlrc->debugPrint("-");
				}
				pvt->_sqlrc->debugPrint((int64_t)
					(*pvt->_inbindvars)[i].
						value.dateval.hour);
				pvt->_sqlrc->debugPrint(":");
				pvt->_sqlrc->debugPrint((int64_t)
					(*pvt->_inbindvars)[i].
						value.dateval.minute);
				pvt->_sqlrc->debugPrint(":");
				pvt->_sqlrc->debugPrint((int64_t)
					(*pvt->_inbindvars)[i].
						value.dateval.second);
				pvt->_sqlrc->debugPrint(".");
				pvt->_sqlrc->debugPrint((int64_t)
					(*pvt->_inbindvars)[i].value.
						dateval.microsecond);
				pvt->_sqlrc->debugPrint(" ");
				pvt->_sqlrc->debugPrint(
					(*pvt->_inbindvars)[i].
						value.dateval.tz);
				pvt->_sqlrc->debugPrint("\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else if ((*pvt->_inbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_BLOB ||
				(*pvt->_inbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_CLOB) {

			pvt->_cs->write((*pvt->_inbindvars)[i].valuesize);
			if ((*pvt->_inbindvars)[i].valuesize>0) {
				pvt->_cs->write((*pvt->_inbindvars)[i].value.lobval,
						(size_t)(*pvt->_inbindvars)[i].
								valuesize);
			}

			if (pvt->_sqlrc->debug()) {
				if ((*pvt->_inbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_BLOB) {
					pvt->_sqlrc->debugPrint(":BLOB)=");
					pvt->_sqlrc->debugPrintBlob(
						(*pvt->_inbindvars)[i].
								value.lobval,
						(*pvt->_inbindvars)[i].
								valuesize);
				} else if ((*pvt->_inbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_CLOB) {
					pvt->_sqlrc->debugPrint(":CLOB)=");
					pvt->_sqlrc->debugPrintClob(
						(*pvt->_inbindvars)[i].
								value.lobval,
						(*pvt->_inbindvars)[i].
								valuesize);
				}
				pvt->_sqlrc->debugPrint("(");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inbindvars)[i].
								valuesize);
				pvt->_sqlrc->debugPrint(")");
				pvt->_sqlrc->debugPrint("\n");
				pvt->_sqlrc->debugPreEnd();
			}
		}

		i++;
	}
}

void sqlrcursor::sendOutputBinds() {

	// index
	uint16_t	i=0;

	// count number of vars to send
	uint16_t	count=pvt->_outbindvars->getLength();
	for (i=0; i<count; i++) {
		if (!(*pvt->_outbindvars)[i].send) {
			count--;
		}
	}

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Sending ");
		pvt->_sqlrc->debugPrint((int64_t)count);
		pvt->_sqlrc->debugPrint(" Output Bind Variables:\n");
		pvt->_sqlrc->debugPreEnd();
	}

	// write the output bind variables to the server.
	pvt->_cs->write(count);
	uint16_t	size;
	i=0;
	while (i<count) {

		// don't send anything if the send flag is turned off
		if (!(*pvt->_outbindvars)[i].send) {
			continue;
		}

		// send the variable, type and size that the buffer needs to be
		size=charstring::length((*pvt->_outbindvars)[i].variable);
		pvt->_cs->write(size);
		pvt->_cs->write((*pvt->_outbindvars)[i].variable,(size_t)size);
		pvt->_cs->write((uint16_t)(*pvt->_outbindvars)[i].type);
		if ((*pvt->_outbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_STRING ||
			(*pvt->_outbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_BLOB ||
			(*pvt->_outbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_CLOB ||
			(*pvt->_outbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_NULL) {
			pvt->_cs->write((*pvt->_outbindvars)[i].valuesize);
		}

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint(
				(*pvt->_outbindvars)[i].variable);
			const char	*bindtype=NULL;
			switch ((*pvt->_outbindvars)[i].type) {
				case SQLRCLIENTBINDVARTYPE_NULL:
					bindtype="(NULL)";
					break;
				case SQLRCLIENTBINDVARTYPE_STRING:
					bindtype="(STRING)";
					break;
				case SQLRCLIENTBINDVARTYPE_INTEGER:
					bindtype="(INTEGER)";
					break;
				case SQLRCLIENTBINDVARTYPE_DOUBLE:
					bindtype="(DOUBLE)";
					break;
				case SQLRCLIENTBINDVARTYPE_DATE:
					bindtype="(DATE)";
					break;
				case SQLRCLIENTBINDVARTYPE_BLOB:
					bindtype="(BLOB)";
					break;
				case SQLRCLIENTBINDVARTYPE_CLOB:
					bindtype="(CLOB)";
					break;
				case SQLRCLIENTBINDVARTYPE_CURSOR:
					bindtype="(CURSOR)";
					break;
			}
			pvt->_sqlrc->debugPrint(bindtype);
			if ((*pvt->_outbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_STRING ||
				(*pvt->_outbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_BLOB ||
				(*pvt->_outbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_CLOB ||
				(*pvt->_outbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_NULL) {
				pvt->_sqlrc->debugPrint("(");
				pvt->_sqlrc->debugPrint((int64_t)
					(*pvt->_outbindvars)[i].valuesize);
				pvt->_sqlrc->debugPrint(")");
			}
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}

		i++;
	}
}

void sqlrcursor::sendInputOutputBinds() {

	// index
	uint16_t	i=0;

	// count number of vars to send
	uint16_t	count=pvt->_inoutbindvars->getLength();
	for (i=0; i<count; i++) {
		if (!(*pvt->_inoutbindvars)[i].send) {
			count--;
		}
	}

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Sending ");
		pvt->_sqlrc->debugPrint((int64_t)count);
		pvt->_sqlrc->debugPrint(" Input/Output Bind Variables:\n");
		pvt->_sqlrc->debugPreEnd();
	}

	// write the input/output bind variables to the server.
	pvt->_cs->write(count);
	uint16_t	size;
	i=0;
	while (i<count) {

		// don't send anything if the send flag is turned off
		if (!(*pvt->_inoutbindvars)[i].send) {
			continue;
		}

		// send the variable, type, size that the buffer needs to be,
		// and value
		size=charstring::length((*pvt->_inoutbindvars)[i].variable);
		pvt->_cs->write(size);
		pvt->_cs->write((*pvt->_inoutbindvars)[i].variable,
							(size_t)size);
		pvt->_cs->write((uint16_t)(*pvt->_inoutbindvars)[i].type);
		if ((*pvt->_inoutbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_NULL) {
			pvt->_cs->write((*pvt->_inoutbindvars)[i].valuesize);
		} else if ((*pvt->_inoutbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_STRING ||
			(*pvt->_inoutbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_BLOB ||
			(*pvt->_inoutbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_CLOB) {
			pvt->_cs->write((*pvt->_inoutbindvars)[i].valuesize);
			if ((*pvt->_inoutbindvars)[i].valuesize>0) {
				pvt->_cs->write(
					(*pvt->_inoutbindvars)[i].
							value.stringval,
					(size_t)(*pvt->_inoutbindvars)[i].
								valuesize);
			}
		} else if ((*pvt->_inoutbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_INTEGER) {
			pvt->_cs->write((uint64_t)(*pvt->_inoutbindvars)[i].
							value.integerval);
		} else if ((*pvt->_inoutbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_DOUBLE) {
			pvt->_cs->write((*pvt->_inoutbindvars)[i].
						value.doubleval.value);
			pvt->_cs->write((*pvt->_inoutbindvars)[i].
						value.doubleval.precision);
			pvt->_cs->write((*pvt->_inoutbindvars)[i].
						value.doubleval.scale);
		} else if ((*pvt->_inoutbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_DATE) {
			pvt->_cs->write((uint16_t)(*pvt->_inoutbindvars)[i].
							value.dateval.year);
			pvt->_cs->write((uint16_t)(*pvt->_inoutbindvars)[i].
							value.dateval.month);
			pvt->_cs->write((uint16_t)(*pvt->_inoutbindvars)[i].
							value.dateval.day);
			pvt->_cs->write((uint16_t)(*pvt->_inoutbindvars)[i].
							value.dateval.hour);
			pvt->_cs->write((uint16_t)(*pvt->_inoutbindvars)[i].
							value.dateval.minute);
			pvt->_cs->write((uint16_t)(*pvt->_inoutbindvars)[i].
							value.dateval.second);
			pvt->_cs->write((uint32_t)(*pvt->_inoutbindvars)[i].
						value.dateval.microsecond);
			pvt->_cs->write((uint16_t)charstring::length(
						(*pvt->_inoutbindvars)[i].
						value.dateval.tz));
			pvt->_cs->write((*pvt->_inoutbindvars)[i].
						value.dateval.tz);
			pvt->_cs->write((*pvt->_inoutbindvars)[i].
						value.dateval.isnegative);
		}

		if (pvt->_sqlrc->debug()) {

			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint(
				(*pvt->_inoutbindvars)[i].variable);
			const char	*bindtype=NULL;
			switch ((*pvt->_inoutbindvars)[i].type) {
				case SQLRCLIENTBINDVARTYPE_NULL:
					bindtype="(NULL)";
					break;
				case SQLRCLIENTBINDVARTYPE_STRING:
					bindtype="(STRING)";
					break;
				case SQLRCLIENTBINDVARTYPE_INTEGER:
					bindtype="(INTEGER)";
					break;
				case SQLRCLIENTBINDVARTYPE_DOUBLE:
					bindtype="(DOUBLE)";
					break;
				case SQLRCLIENTBINDVARTYPE_DATE:
					bindtype="(DATE)";
					break;
				case SQLRCLIENTBINDVARTYPE_BLOB:
					bindtype="(BLOB)";
					break;
				case SQLRCLIENTBINDVARTYPE_CLOB:
					bindtype="(CLOB)";
					break;
				case SQLRCLIENTBINDVARTYPE_CURSOR:
					bindtype="(CURSOR)";
					break;
			}
			pvt->_sqlrc->debugPrint(bindtype);

			if ((*pvt->_inoutbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_STRING ||
				(*pvt->_inoutbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_BLOB ||
				(*pvt->_inoutbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_CLOB ||
				(*pvt->_inoutbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_NULL) {

				pvt->_sqlrc->debugPrint("=");
				pvt->_sqlrc->debugPrint(
					(*pvt->_inoutbindvars)[i].
							value.stringval);
				pvt->_sqlrc->debugPrint("(");
				pvt->_sqlrc->debugPrint((int64_t)
					(*pvt->_inoutbindvars)[i].valuesize);
				pvt->_sqlrc->debugPrint(")");

			} else if ((*pvt->_inoutbindvars)[i].type==
						SQLRCLIENTBINDVARTYPE_INTEGER) {

				pvt->_sqlrc->debugPrint("=");
				pvt->_sqlrc->debugPrint(
					(int64_t)(*pvt->_inoutbindvars)[i].
							value.integerval);
				pvt->_sqlrc->debugPrint("\n");
				pvt->_sqlrc->debugPreEnd();

			} else if ((*pvt->_inoutbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_DOUBLE) {

				if (pvt->_sqlrc->debug()) {
					pvt->_sqlrc->debugPrint("=");
					pvt->_sqlrc->debugPrint(
						(*pvt->_inoutbindvars)[i].
						value.doubleval.value);
					pvt->_sqlrc->debugPrint(":");
					pvt->_sqlrc->debugPrint(
						(int64_t)
						(*pvt->_inoutbindvars)[i].
						value.doubleval.precision);
					pvt->_sqlrc->debugPrint(",");
					pvt->_sqlrc->debugPrint(
						(int64_t)
						(*pvt->_inoutbindvars)[i].
						value.doubleval.scale);
					pvt->_sqlrc->debugPrint("\n");
					pvt->_sqlrc->debugPreEnd();
				}

			} else if ((*pvt->_inoutbindvars)[i].type==
					SQLRCLIENTBINDVARTYPE_DATE) {

				if (pvt->_sqlrc->debug()) {
					pvt->_sqlrc->debugPrint("=");
					pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[i].
							value.dateval.year);
					pvt->_sqlrc->debugPrint("-");
					pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[i].
							value.dateval.month);
					pvt->_sqlrc->debugPrint("-");
					pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[i].
							value.dateval.day);
					pvt->_sqlrc->debugPrint(" ");
					if ((*pvt->_inoutbindvars)[i].
						value.dateval.isnegative) {
						pvt->_sqlrc->debugPrint("-");
					}
					pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[i].
							value.dateval.hour);
					pvt->_sqlrc->debugPrint(":");
					pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[i].
							value.dateval.minute);
					pvt->_sqlrc->debugPrint(":");
					pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[i].
							value.dateval.second);
					pvt->_sqlrc->debugPrint(".");
					pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[i].value.
							dateval.microsecond);
					pvt->_sqlrc->debugPrint(" ");
					pvt->_sqlrc->debugPrint(
						(*pvt->_inoutbindvars)[i].
							value.dateval.tz);
					pvt->_sqlrc->debugPrint("\n");
					pvt->_sqlrc->debugPreEnd();
				}
			}
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}

		// set this to NULL here because it will be deleted in
		// deleteInputOutputVariables, and it needs to be NULL in case
		// the query fails and parseInputOutputVariables (which
		// allocates a buffer for it) is never called
		(*pvt->_inoutbindvars)[i].value.dateval.tz=NULL;

		i++;
	}
}

void sqlrcursor::sendGetColumnInfo() {

	if (pvt->_sendcolumninfo==SEND_COLUMN_INFO) {
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Send Column Info: yes\n");
			pvt->_sqlrc->debugPreEnd();
		}
		pvt->_cs->write((uint16_t)SEND_COLUMN_INFO);
	} else {
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Send Column Info: no\n");
			pvt->_sqlrc->debugPreEnd();
		}
		pvt->_cs->write((uint16_t)DONT_SEND_COLUMN_INFO);
	}
}

bool sqlrcursor::processInitialResultSet() {

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Fetching initial rows...\n");
		pvt->_sqlrc->debugPreEnd();
	}

	// start caching the result set
	if (pvt->_cacheon) {
		startCaching();
	}

	// parse the columninfo and data
	bool	success=true;

	// Skip and fetch here if we're not reading from a cached result set.
	// This way, everything gets done in 1 round trip.
	if (!pvt->_cachesource) {
		success=skipAndFetch(true,0);
	}

	// check for an error
	if (success) {

		uint16_t	err=getErrorStatus();
		if (err!=NO_ERROR_OCCURRED) {

			// if there was a timeout, then end
			// the session and bail immediately
			if (err==TIMEOUT_GETTING_ERROR_STATUS) {
				pvt->_sqlrc->endSession();
				return false;
			}

			// otherwise, get the error from the server
			getErrorFromServer();

			// don't get the cursor if the error was that there
			// were no cursors available
			if (pvt->_errorno!=SQLR_ERROR_NOCURSORS) {
				getCursorId();
			}

			// if we need to disconnect then end the session
			if (err==ERROR_OCCURRED_DISCONNECT) {
				pvt->_sqlrc->endSession();
			}
			return false;
		}
	}

	// get data from the server/cache
	if (success && ((pvt->_cachesource && pvt->_cachesourceind) ||
			((!pvt->_cachesource && !pvt->_cachesourceind)  && 
				(success=getCursorId()) && 
				(success=getSuspended()))) &&
			(success=parseColumnInfo()) && 
			(success=parseOutputBinds()) &&
			(success=parseInputOutputBinds())) {

		// skip and fetch here if we're reading from a cached result set
		if (pvt->_cachesource) {
			success=skipAndFetch(true,0);
		}

		// parse the data
		if (success) {

			if (!pvt->_lazyfetch) {

				success=parseResults();

			} else {

				// If we just resumed a result set, (or more
				// precisely, if we did, and any rows had been
				// fetched from it prior to suspension) then
				// rowcount and firstrowindex need to be set
				// here.  If we were't lazy-fetching then they
				// would have been set inside of parseResults().
				if (pvt->_resumedlastrowindex) {
					pvt->_rowcount=
						pvt->_resumedlastrowindex+1;
					pvt->_firstrowindex=pvt->_rowcount;
				}
			}
		}
	}

	if (!success) {
		// some kind of network error occurred, end the session
		clearResultSet();
		pvt->_sqlrc->endSession();
	}
	return success;
}

bool sqlrcursor::skipAndFetch(bool initial, uint64_t rowstoskip) {

	if (!skipRows(initial,rowstoskip)) {
		return false;
	}

	fetchRows();

	pvt->_sqlrc->flushWriteBuffer();
	return true;
}

bool sqlrcursor::skipRows(bool initial, uint64_t rowstoskip) {

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Skipping ");
		pvt->_sqlrc->debugPrint((int64_t)rowstoskip);
		pvt->_sqlrc->debugPrint(" rows\n");
		pvt->_sqlrc->debugPreEnd();
	}

	// bump the rowcount
	pvt->_rowcount+=rowstoskip;

	// skip manually if we're reading from a cached result set
	if (pvt->_cachesource && pvt->_cachesourceind) {

		// bail if we don't need to skip
		if (!rowstoskip) {
			return true;
		}

		// get the row offset from the index
		pvt->_cachesourceind->setPositionRelativeToBeginning(
					13+sizeof(int64_t)+
					(pvt->_rowcount*sizeof(int64_t)));
		int64_t	rowoffset;
		if (pvt->_cachesourceind->read(&rowoffset)!=sizeof(int64_t)) {
			setError("The cache file index appears to be corrupt.");
			return false;
		}

		// skip to that offset in the cache file
		pvt->_cachesource->setPositionRelativeToBeginning(rowoffset);
		return true;
	}

	if (initial) {

		// If this is the initial fetch, then rowstoskip will always
		// be 0, and prior to 1.2, we would always send a 0 here.
		//
		// In 1.2+ we need a way to send some flags to the server, so
		// we'll repurpose these 8 bytes as flags.
		//
		// For now, the only flag is whether or not to do lazy fetches.

		uint64_t	flags=0;
		if (pvt->_lazyfetch) {
			flags=1;
		}

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			if (pvt->_lazyfetch) {
				pvt->_sqlrc->debugPrint("Lazy fetching\n");
			} else {
				pvt->_sqlrc->debugPrint("Eager fetching\n");
			}
			pvt->_sqlrc->debugPreEnd();
		}

		pvt->_cs->write(flags);

	} else {
		// otherwise send the server the number of rows to skip
		pvt->_cs->write(rowstoskip);
	}
	return true;
}

void sqlrcursor::fetchRows() {

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Fetching ");
		pvt->_sqlrc->debugPrint((int64_t)pvt->_rsbuffersize);
		pvt->_sqlrc->debugPrint(" rows\n");
		pvt->_sqlrc->debugPreEnd();
	}

	// bail if we're reading from a cached result set
	if (pvt->_cachesource && pvt->_cachesourceind) {
		return;
	}

	// otherwise send the server the number of rows to fetch
	pvt->_cs->write(pvt->_rsbuffersize);
}

uint16_t sqlrcursor::getErrorStatus() {

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Checking For An Error...\n");
		pvt->_sqlrc->debugPreEnd();
	}

	// get a flag indicating whether there's been an error or not
	uint16_t	err;
	int32_t	result=getShort(&err,pvt->_sqlrc->responsetimeoutsec(),
					pvt->_sqlrc->responsetimeoutusec());
	if (result==RESULT_TIMEOUT) {
		setError("Timeout while determining whether "
				"an error occurred or not.\n");
		return TIMEOUT_GETTING_ERROR_STATUS;
	} else if (result!=sizeof(uint16_t)) {
		setError("Failed to determine whether an "
				"error occurred or not.\n "
				"A network error may have occurred.");
		return ERROR_OCCURRED;
	}

	if (err==NO_ERROR_OCCURRED) {
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("	none.\n");
			pvt->_sqlrc->debugPreEnd();
		}
		cacheNoError();
		return NO_ERROR_OCCURRED;
	}

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("	error!!!\n");
		pvt->_sqlrc->debugPreEnd();
	}
	return err;
}

bool sqlrcursor::getCursorId() {

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Getting Cursor ID...\n");
		pvt->_sqlrc->debugPreEnd();
	}
	if (pvt->_cs->read(&pvt->_cursorid)!=sizeof(uint16_t)) {
		if (!pvt->_error) {
			char	*err=error::getErrorString();
			stringbuffer	errstr;
			errstr.append("Failed to get a cursor id.\n "
					"A network error may have occurred. ");
			errstr.append(err);
			setError(errstr.getString());
			delete[] err;
		}
		return false;
	}
	pvt->_havecursorid=true;
	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Cursor ID: ");
		pvt->_sqlrc->debugPrint((int64_t)pvt->_cursorid);
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}
	return true;
}

bool sqlrcursor::getSuspended() {

	// see if the result set of that cursor is actually suspended
	uint16_t	suspendedresultset;
	if (pvt->_cs->read(&suspendedresultset)!=sizeof(uint16_t)) {
		setError("Failed to determine whether "
			"the session was suspended or not.\n "
			"A network error may have occurred.");
		return false;
	}

	if (suspendedresultset==SUSPENDED_RESULT_SET) {

		// If it was suspended the server will send the index of the 
		// last row from the previous result set.
		if (pvt->_cs->read(&pvt->_resumedlastrowindex)!=
						sizeof(uint64_t)) {
			setError("Failed to get the index of the "
				"last row of a previously suspended result "
				"set.\n A network error may have occurred.");
			return false;
		}
	
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Result set was ");
	       		pvt->_sqlrc->debugPrint("suspended at row index: ");
			pvt->_sqlrc->debugPrint(
					(int64_t)pvt->_resumedlastrowindex);
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}

	} else {

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Previous result set was ");
	       		pvt->_sqlrc->debugPrint("not suspended.\n");
			pvt->_sqlrc->debugPreEnd();
		}
	}
	return true;
}

bool sqlrcursor::parseColumnInfo() {

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Parsing Column Info\n");
		pvt->_sqlrc->debugPrint("	Actual row count: ");
		pvt->_sqlrc->debugPreEnd();
	}

	// first get whether the server knows the total number of rows or not
	if (getShort(&pvt->_knowsactualrows)!=sizeof(uint16_t)) {
		setError("Failed to get whether the server knows "
				"the number of actual rows or not.\n"
				"A network error may have occurred.");
		return false;
	}

	// get the number of rows returned by the query
	if (pvt->_knowsactualrows==ACTUAL_ROWS) {
		if (getLongLong(&pvt->_actualrows)!=sizeof(uint64_t)) {
			setError("Failed to get the number of actual rows.\n"
					"A network error may have occurred.");
			return false;
		}
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint((int64_t)pvt->_actualrows);
			pvt->_sqlrc->debugPreEnd();
		}
	} else {
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("unknown");
			pvt->_sqlrc->debugPreEnd();
		}
	}

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPrint("	Affected row count: ");
		pvt->_sqlrc->debugPreEnd();
	}

	// get whether the server knows the number of affected rows or not
	if (getShort(&pvt->_knowsaffectedrows)!=sizeof(uint16_t)) {
		setError("Failed to get whether the server knows "
				"the number of affected rows or not.\n"
				"A network error may have occurred.");
		return false;
	}

	// get the number of rows affected by the query
	if (pvt->_knowsaffectedrows==AFFECTED_ROWS) {
		if (getLongLong(&pvt->_affectedrows)!=sizeof(uint64_t)) {
			setError("Failed to get the number of affected rows.\n"
				"A network error may have occurred.");
			return false;
		}
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint((int64_t)pvt->_affectedrows);
			pvt->_sqlrc->debugPreEnd();
		}
	} else {
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("unknown");
			pvt->_sqlrc->debugPreEnd();
		}
	}

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}

	// get whether the server is sending column info or not
	if (getShort(&pvt->_sentcolumninfo)!=sizeof(uint16_t)) {
		setError("Failed to get whether the server "
				"is sending column info or not.\n"
				"A network error may have occurred.");
		return false;
	}

	// get column count
	if (getLong(&pvt->_colcount)!=sizeof(uint32_t)) {
		setError("Failed to get the column count.\n"
				"A network error may have occurred.");
		return false;
	}
	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("	Column count: ");
		pvt->_sqlrc->debugPrint((int64_t)pvt->_colcount);
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}

	// we have to do this here even if we're not getting the column
	// descriptions because we are going to use the longdatatype member
	// variable no matter what
	createColumnBuffers();

	if (pvt->_sendcolumninfo==SEND_COLUMN_INFO && 
			pvt->_sentcolumninfo==SEND_COLUMN_INFO) {

		// get whether column types will be predefined id's or strings
		if (getShort(&pvt->_columntypeformat)!=sizeof(uint16_t)) {
			setError("Failed to whether column types will be "
					"predefined id's or strings.\n"
					"A network error may have occurred.");
			return false;
		}

		// some useful variables
		uint16_t			length;
		sqlrclientcolumn		*currentcol;

		// get the columninfo segment
		for (uint32_t i=0; i<pvt->_colcount; i++) {
	
			// get the column name length
			if (getShort(&length)!=sizeof(uint16_t)) {
				setError("Failed to get the column name "
					"length.\n"
					"A network error may have occurred.");
				return false;
			}
	
			// which column to use
			currentcol=getColumnInternal(i);
	
			// get the column name
			currentcol->name=
				(char *)pvt->_colstorage->allocate(length+1);
			if (getString(currentcol->name,length)!=length) {
				setError("Failed to get the column name.\n "
					"A network error may have occurred.");
				return false;
			}
			currentcol->name[length]='\0';

			// upper/lowercase column name if necessary
			if (pvt->_colcase==UPPER_CASE) {
				charstring::upper(currentcol->name);
			} else if (pvt->_colcase==LOWER_CASE) {
				charstring::lower(currentcol->name);
			}

			if (pvt->_columntypeformat==COLUMN_TYPE_IDS) {

				// get the column type
				if (getShort(&currentcol->type)!=
						sizeof(uint16_t)) {
					setError("Failed to get the column "
						"type.\n"
						"A network error may have "
						"occurred.");
					return false;
				}

				// sanity check
				if (currentcol->type>=END_DATATYPE) {
					currentcol->type=UNKNOWN_DATATYPE;
				}

			} else {

				// get the column type length
				if (getShort(&currentcol->typestringlength)!=
						sizeof(uint16_t)) {
					setError("Failed to get the column "
						"type length.\n"
						"A network error may have "
						"occurred.");
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
					setError("Failed to get the column "
						"type.\n"
						"A network error may have "
						"occurred.");
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
				setError("Failed to get column info.\n"
					"A network error may have occurred.");
				return false;
			}
	
			// get the table length
			if (getShort(&length)!=sizeof(uint16_t)) {
				setError("Failed to get the table length.\n"
					"A network error may have occurred.");
				return false;
			}
	
			// get the table
			currentcol->table=
				(char *)pvt->_colstorage->allocate(length+1);
			if (getString(currentcol->table,length)!=length) {
				setError("Failed to get the table.\n "
					"A network error may have occurred.");
				return false;
			}
			currentcol->table[length]='\0';


			// initialize the longest value
			currentcol->longest=0;
	
			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("	");
				pvt->_sqlrc->debugPrint("\"");
				pvt->_sqlrc->debugPrint(currentcol->table);
				pvt->_sqlrc->debugPrint("\".");
				pvt->_sqlrc->debugPrint("\"");
				pvt->_sqlrc->debugPrint(currentcol->name);
				pvt->_sqlrc->debugPrint("\",");
				pvt->_sqlrc->debugPrint("\"");
				if (pvt->_columntypeformat!=COLUMN_TYPE_IDS) {
					pvt->_sqlrc->debugPrint(
						currentcol->typestring);
				} else {
					pvt->_sqlrc->debugPrint(
						datatypestring[
							currentcol->type]);
				}
				pvt->_sqlrc->debugPrint("\", ");
				pvt->_sqlrc->debugPrint((int64_t)
							currentcol->length);
				pvt->_sqlrc->debugPrint(" (");
				pvt->_sqlrc->debugPrint((int64_t)
							currentcol->precision);
				pvt->_sqlrc->debugPrint(",");
				pvt->_sqlrc->debugPrint((int64_t)
							currentcol->scale);
				pvt->_sqlrc->debugPrint(") ");
				if (!currentcol->nullable) {
					pvt->_sqlrc->debugPrint(
							"NOT NULL ");
				}
				if (currentcol->primarykey) {
					pvt->_sqlrc->debugPrint(
							"Primary Key ");
				}
				if (currentcol->unique) {
					pvt->_sqlrc->debugPrint(
							"Unique ");
				}
				if (currentcol->partofkey) {
					pvt->_sqlrc->debugPrint(
							"Part of a Key ");
				}
				if (currentcol->unsignednumber) {
					pvt->_sqlrc->debugPrint(
							"Unsigned ");
				}
				if (currentcol->zerofill) {
					pvt->_sqlrc->debugPrint(
							"Zero Filled ");
				}
				if (currentcol->binary) {
					pvt->_sqlrc->debugPrint(
							"Binary ");
				}
				if (currentcol->autoincrement) {
					pvt->_sqlrc->debugPrint(
							"Auto-Increment ");
				}
				pvt->_sqlrc->debugPrint("\n");
				pvt->_sqlrc->debugPreEnd();
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
	if (!pvt->_columns) {
		pvt->_columns=new sqlrclientcolumn[OPTIMISTIC_COLUMN_COUNT];
	}

	// if there are more columns than our static column buffer
	// can handle, create extra columns, these will be deleted after each
	// query
	if (pvt->_colcount>OPTIMISTIC_COLUMN_COUNT &&
			pvt->_colcount>pvt->_previouscolcount) {
		delete[] pvt->_extracolumns;
		pvt->_extracolumns=
			new sqlrclientcolumn[
				pvt->_colcount-OPTIMISTIC_COLUMN_COUNT];
	}
}

bool sqlrcursor::parseOutputBinds() {

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Receiving Output Bind Values: \n");
		pvt->_sqlrc->debugPreEnd();
	}

	// useful variables
	uint16_t	type;
	uint32_t	length;
	uint16_t	count=0;

	// get the bind values
	for (;;) {

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("	getting type...\n");
			pvt->_sqlrc->debugPreEnd();
		}

		// get the data type
		if (getShort(&type)!=sizeof(uint16_t)) {
			setError("Failed to get data type.\n "
				"A network error may have occurred.");

			return false;
		}

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("	done getting type: ");
			pvt->_sqlrc->debugPrint((int64_t)type);
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}

		// check for end of bind values
		if (type==END_BIND_VARS) {

			break;

		} else if (type==NULL_DATA) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
						"	NULL output bind\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// handle a null value
			(*pvt->_outbindvars)[count].resultvaluesize=0;
			if ((*pvt->_outbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_STRING) {
				if (pvt->_returnnulls) {
					(*pvt->_outbindvars)[count].value.
							stringval=NULL;
				} else {
					(*pvt->_outbindvars)[count].value.
							stringval=new char[1];
					(*pvt->_outbindvars)[count].value.
							stringval[0]='\0';
				}
			} else if ((*pvt->_outbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_INTEGER) {
				(*pvt->_outbindvars)[count].value.integerval=0;
			} else if ((*pvt->_outbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_DOUBLE) {
				(*pvt->_outbindvars)[count].
						value.doubleval.value=0;
				(*pvt->_outbindvars)[count].
						value.doubleval.precision=0;
				(*pvt->_outbindvars)[count].
						value.doubleval.scale=0;
			} else if ((*pvt->_outbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_DATE) {
				(*pvt->_outbindvars)[count].
						value.dateval.year=0;
				(*pvt->_outbindvars)[count].
						value.dateval.month=0;
				(*pvt->_outbindvars)[count].
						value.dateval.day=0;
				(*pvt->_outbindvars)[count].
						value.dateval.hour=0;
				(*pvt->_outbindvars)[count].
						value.dateval.minute=0;
				(*pvt->_outbindvars)[count].
						value.dateval.second=0;
				(*pvt->_outbindvars)[count].
						value.dateval.microsecond=0;
				if (pvt->_returnnulls) {
					(*pvt->_outbindvars)[count].
						value.dateval.tz=NULL;
				} else {
					(*pvt->_outbindvars)[count].
						value.dateval.tz=new char[1];
					(*pvt->_outbindvars)[count].
						value.dateval.tz[0]='\0';
				}
			} 

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPrint("		");
				pvt->_sqlrc->debugPrint("done fetching.\n");
			}

		} else if (type==STRING_DATA) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
						"	STRING output bind\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// get the value length
			if (getLong(&length)!=sizeof(uint32_t)) {
				setError("Failed to get string value length.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_outbindvars)[count].resultvaluesize=length;
			(*pvt->_outbindvars)[count].value.stringval=
							new char[length+1];

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
						"		length=");
				pvt->_sqlrc->debugPrint((int64_t)length);
				pvt->_sqlrc->debugPrint("\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// get the value
			if ((uint32_t)getString(
					(*pvt->_outbindvars)[count].value.
						stringval,length)!=length) {
				setError("Failed to get string value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_outbindvars)[count].
					value.stringval[length]='\0';

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("		");
				pvt->_sqlrc->debugPrint("done fetching\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else if (type==INTEGER_DATA) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
						"	INTEGER output bind\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// get the value
			if (getLongLong((uint64_t *)
					&(*pvt->_outbindvars)[count].value.
						integerval)!=sizeof(uint64_t)) {
				setError("Failed to get integer value.\n "
					"A network error may have occurred.");
				return false;
			}

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("		");
				pvt->_sqlrc->debugPrint("done fetching\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else if (type==DOUBLE_DATA) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
					"	DOUBLE output bind\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// get the value
			if (getDouble(&(*pvt->_outbindvars)[count].value.
						doubleval.value)!=
						sizeof(double)) {
				setError("Failed to get double value.\n "
					"A network error may have occurred.");
				return false;
			}

			// get the precision
			if (getLong(&(*pvt->_outbindvars)[count].value.
						doubleval.precision)!=
						sizeof(uint32_t)) {
				setError("Failed to get precision.\n "
					"A network error may have occurred.");
				return false;
			}

			// get the scale
			if (getLong(&(*pvt->_outbindvars)[count].value.
						doubleval.scale)!=
						sizeof(uint32_t)) {
				setError("Failed to get scale.\n "
					"A network error may have occurred.");
				return false;
			}

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("		");
				pvt->_sqlrc->debugPrint("done fetching\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else if (type==DATE_DATA) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
					"	DATE output bind\n");
				pvt->_sqlrc->debugPreEnd();
			}

			uint16_t	temp;

			// get the year
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_outbindvars)[count].
					value.dateval.year=(int16_t)temp;

			// get the month
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_outbindvars)[count].
					value.dateval.month=(int16_t)temp;

			// get the day
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_outbindvars)[count].
					value.dateval.day=(int16_t)temp;

			// get the hour
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_outbindvars)[count].
					value.dateval.hour=(int16_t)temp;

			// get the minute
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_outbindvars)[count].
					value.dateval.minute=(int16_t)temp;

			// get the second
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_outbindvars)[count].
					value.dateval.second=(int16_t)temp;

			// get the microsecond
			uint32_t	temp32;
			if (getLong(&temp32)!=sizeof(uint32_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_outbindvars)[count].value.
					dateval.microsecond=(int32_t)temp32;

			// get the timezone length
			uint16_t	length;
			if (getShort(&length)!=sizeof(uint16_t)) {
				setError("Failed to get timezone length.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_outbindvars)[count].
					value.dateval.tz=new char[length+1];

			// get the timezone
			if ((uint16_t)getString(
					(*pvt->_outbindvars)[count].value.
						dateval.tz,length)!=length) {
				setError("Failed to get timezone.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_outbindvars)[count].
					value.dateval.tz[length]='\0';

			// get the isnegative flag
			bool	tempbool;
			if (getBool(&tempbool)!=sizeof(bool)) {
				setError("Failed to get bool value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_outbindvars)[count].value.
					dateval.isnegative=tempbool;

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("		");
				pvt->_sqlrc->debugPrint("done fetching\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else if (type==CURSOR_DATA) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
					"	CURSOR output bind\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// get the cursor id
			if (getShort((uint16_t *)
				&((*pvt->_outbindvars)[count].value.cursorid))!=
				sizeof(uint16_t)) {
				setError("Failed to get cursor id.\n "
					"A network error may have occurred.");
				return false;
			}

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("		");
				pvt->_sqlrc->debugPrint("done fetching\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("	LOB/CLOB ");
				pvt->_sqlrc->debugPrint("output bind\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// must be START_LONG_DATA...
			// get the total length of the long data
			uint64_t	totallength;
			if (getLongLong(&totallength)!=sizeof(uint64_t)) {
				setError("Failed to get total length.\n "
					"A network error may have occurred.");
				return false;
			}

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
					"		length=");
				pvt->_sqlrc->debugPrint((int64_t)totallength);
				pvt->_sqlrc->debugPrint("\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// create a buffer to hold the data
			char	*buffer=new char[totallength+1];

			uint64_t	offset=0;
			uint32_t	length;
			for (;;) {

				if (pvt->_sqlrc->debug()) {
					pvt->_sqlrc->debugPreStart();
					pvt->_sqlrc->debugPrint(
							"		");
					pvt->_sqlrc->debugPrint(
							"fetching...\n");
					pvt->_sqlrc->debugPreEnd();
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

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("		");
				pvt->_sqlrc->debugPrint("done fetching.\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// NULL terminate the buffer.  This makes 
			// certain operations safer and won't hurt
			// since the actual length (which doesn't
			// include the NULL) is available from
			// getOutputBindLength.
			buffer[totallength]='\0';
			(*pvt->_outbindvars)[count].value.lobval=buffer;
			(*pvt->_outbindvars)[count].resultvaluesize=totallength;
		}

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint(
					(*pvt->_outbindvars)[count].variable);
			pvt->_sqlrc->debugPrint("=");
			if ((*pvt->_outbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_BLOB) {
				pvt->_sqlrc->debugPrintBlob(
					(*pvt->_outbindvars)[count].
							value.lobval,
					(*pvt->_outbindvars)[count].
							resultvaluesize);
			} else if ((*pvt->_outbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_CLOB) {
				pvt->_sqlrc->debugPrintClob(
					(*pvt->_outbindvars)[count].
							value.lobval,
					(*pvt->_outbindvars)[count].
							resultvaluesize);
			} else if ((*pvt->_outbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_CURSOR) {
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_outbindvars)[count].
							value.cursorid);
			} else if ((*pvt->_outbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_INTEGER) {
				pvt->_sqlrc->debugPrint(
						(*pvt->_outbindvars)[count].
							value.integerval);
			} else if ((*pvt->_outbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_DOUBLE) {
				pvt->_sqlrc->debugPrint(
						(*pvt->_outbindvars)[count].
							value.doubleval.value);
				pvt->_sqlrc->debugPrint("(");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_outbindvars)[count].
							value.doubleval.
							precision);
				pvt->_sqlrc->debugPrint(",");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_outbindvars)[count].
							value.doubleval.
							scale);
				pvt->_sqlrc->debugPrint(")");
			} else if ((*pvt->_outbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_DATE) {
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_outbindvars)[count].
							value.dateval.year);
				pvt->_sqlrc->debugPrint("-");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_outbindvars)[count].
							value.dateval.month);
				pvt->_sqlrc->debugPrint("-");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_outbindvars)[count].
							value.dateval.day);
				pvt->_sqlrc->debugPrint(" ");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_outbindvars)[count].
							value.dateval.hour);
				pvt->_sqlrc->debugPrint(":");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_outbindvars)[count].
							value.dateval.minute);
				pvt->_sqlrc->debugPrint(":");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_outbindvars)[count].
							value.dateval.second);
				pvt->_sqlrc->debugPrint(".");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[count].
						value.dateval.microsecond);
				pvt->_sqlrc->debugPrint(" ");
				pvt->_sqlrc->debugPrint(
						(*pvt->_outbindvars)[count].
							value.dateval.tz);
			} else {
				pvt->_sqlrc->debugPrint(
						(*pvt->_outbindvars)[count].
							value.stringval);
			}
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}

		count++;
	}

	// cache the output binds
	cacheOutputBinds(count);

	return true;
}

bool sqlrcursor::parseInputOutputBinds() {

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Receiving Input/Output "
						"Bind Values: \n");
		pvt->_sqlrc->debugPreEnd();
	}

	// useful variables
	uint16_t	type;
	uint32_t	length;
	uint16_t	count=0;

	// get the bind values
	for (;;) {

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("	getting type...\n");
			pvt->_sqlrc->debugPreEnd();
		}

		// get the data type
		if (getShort(&type)!=sizeof(uint16_t)) {
			setError("Failed to get data type.\n "
				"A network error may have occurred.");

			return false;
		}

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("	done getting type: ");
			pvt->_sqlrc->debugPrint((int64_t)type);
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}

		// check for end of bind values
		if (type==END_BIND_VARS) {

			break;

		} else if (type==NULL_DATA) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
						"	NULL output bind\n");
				pvt->_sqlrc->debugPreEnd();
			}

			if ((*pvt->_inoutbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_NULL) {
				(*pvt->_inoutbindvars)[count].type=
						SQLRCLIENTBINDVARTYPE_STRING;
			}

			// handle a null value
			(*pvt->_inoutbindvars)[count].resultvaluesize=0;
			if ((*pvt->_inoutbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_STRING) {
				if (pvt->_returnnulls) {
					delete[] (*pvt->_inoutbindvars)[count].
								value.stringval;
					(*pvt->_inoutbindvars)[count].value.
							stringval=NULL;
				} else {
					(*pvt->_inoutbindvars)[count].value.
							stringval[0]='\0';
				}
			} else if ((*pvt->_inoutbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_INTEGER) {
				(*pvt->_inoutbindvars)[count].value.
							integerval=0;
			} else if ((*pvt->_inoutbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_DOUBLE) {
				(*pvt->_inoutbindvars)[count].
						value.doubleval.value=0;
				(*pvt->_inoutbindvars)[count].
						value.doubleval.precision=0;
				(*pvt->_inoutbindvars)[count].
						value.doubleval.scale=0;
			} else if ((*pvt->_inoutbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_DATE) {
				(*pvt->_inoutbindvars)[count].
						value.dateval.year=0;
				(*pvt->_inoutbindvars)[count].
						value.dateval.month=0;
				(*pvt->_inoutbindvars)[count].
						value.dateval.day=0;
				(*pvt->_inoutbindvars)[count].
						value.dateval.hour=0;
				(*pvt->_inoutbindvars)[count].
						value.dateval.minute=0;
				(*pvt->_inoutbindvars)[count].
						value.dateval.second=0;
				(*pvt->_inoutbindvars)[count].
						value.dateval.microsecond=0;
				(*pvt->_inoutbindvars)[count].
						value.dateval.tz=new char[1];
				(*pvt->_inoutbindvars)[count].
						value.dateval.tz[0]='\0';
			} 

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPrint("		");
				pvt->_sqlrc->debugPrint("done fetching.\n");
			}

		} else if (type==STRING_DATA) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
						"	STRING input/output "
						"bind\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// get the value length
			if (getLong(&length)!=sizeof(uint32_t)) {
				setError("Failed to get string value length.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_inoutbindvars)[count].resultvaluesize=length;
			// FIXME: verify that resultvaluesize <= size of buffer

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
						"		length=");
				pvt->_sqlrc->debugPrint((int64_t)length);
				pvt->_sqlrc->debugPrint("\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// get the value
			if ((uint32_t)getString(
					(*pvt->_inoutbindvars)[count].value.
						stringval,length)!=length) {
				setError("Failed to get string value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_inoutbindvars)[count].
					value.stringval[length]='\0';

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("		");
				pvt->_sqlrc->debugPrint("done fetching\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else if (type==INTEGER_DATA) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
						"	INTEGER input/output "
						"bind\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// get the value
			if (getLongLong((uint64_t *)
					&(*pvt->_inoutbindvars)[count].value.
						integerval)!=sizeof(uint64_t)) {
				setError("Failed to get integer value.\n "
					"A network error may have occurred.");
				return false;
			}

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("		");
				pvt->_sqlrc->debugPrint("done fetching\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else if (type==DOUBLE_DATA) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
					"	DOUBLE input/output bind\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// get the value
			if (getDouble(&(*pvt->_inoutbindvars)[count].value.
						doubleval.value)!=
						sizeof(double)) {
				setError("Failed to get double value.\n "
					"A network error may have occurred.");
				return false;
			}

			// get the precision
			if (getLong(&(*pvt->_inoutbindvars)[count].value.
						doubleval.precision)!=
						sizeof(uint32_t)) {
				setError("Failed to get precision.\n "
					"A network error may have occurred.");
				return false;
			}

			// get the scale
			if (getLong(&(*pvt->_inoutbindvars)[count].value.
						doubleval.scale)!=
						sizeof(uint32_t)) {
				setError("Failed to get scale.\n "
					"A network error may have occurred.");
				return false;
			}

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("		");
				pvt->_sqlrc->debugPrint("done fetching\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else if (type==DATE_DATA) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
					"	DATE input/output bind\n");
				pvt->_sqlrc->debugPreEnd();
			}

			uint16_t	temp;

			// get the year
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_inoutbindvars)[count].
					value.dateval.year=(int16_t)temp;

			// get the month
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_inoutbindvars)[count].
					value.dateval.month=(int16_t)temp;

			// get the day
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_inoutbindvars)[count].
					value.dateval.day=(int16_t)temp;

			// get the hour
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_inoutbindvars)[count].
					value.dateval.hour=(int16_t)temp;

			// get the minute
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_inoutbindvars)[count].
					value.dateval.minute=(int16_t)temp;

			// get the second
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_inoutbindvars)[count].
					value.dateval.second=(int16_t)temp;

			// get the microsecond
			uint32_t	temp32;
			if (getLong(&temp32)!=sizeof(uint32_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_inoutbindvars)[count].value.
					dateval.microsecond=(int32_t)temp32;

			// get the timezone length
			uint16_t	length;
			if (getShort(&length)!=sizeof(uint16_t)) {
				setError("Failed to get timezone length.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_inoutbindvars)[count].value.
					dateval.tz=new char[length+1];

			// get the timezone
			if ((uint16_t)getString(
					(*pvt->_inoutbindvars)[count].value.
						dateval.tz,length)!=length) {
				setError("Failed to get timezone.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_inoutbindvars)[count].
					value.dateval.tz[length]='\0';

			// get the isnegative flag
			bool	tempbool;
			if (getBool(&tempbool)!=sizeof(bool)) {
				setError("Failed to get bool value.\n "
					"A network error may have occurred.");
				return false;
			}
			(*pvt->_inoutbindvars)[count].value.
					dateval.isnegative=tempbool;

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("		");
				pvt->_sqlrc->debugPrint("done fetching\n");
				pvt->_sqlrc->debugPreEnd();
			}

		} else {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("	LOB/CLOB ");
				pvt->_sqlrc->debugPrint("input/output bind\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// must be START_LONG_DATA...
			// get the total length of the long data
			uint64_t	totallength;
			if (getLongLong(&totallength)!=sizeof(uint64_t)) {
				setError("Failed to get total length.\n "
					"A network error may have occurred.");
				return false;
			}

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
					"		length=");
				pvt->_sqlrc->debugPrint((int64_t)totallength);
				pvt->_sqlrc->debugPrint("\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// create a buffer to hold the data
			// FIXME: this should be pre-allocated or something...
			char	*buffer=new char[totallength+1];

			uint64_t	offset=0;
			uint32_t	length;
			for (;;) {

				if (pvt->_sqlrc->debug()) {
					pvt->_sqlrc->debugPreStart();
					pvt->_sqlrc->debugPrint(
							"		");
					pvt->_sqlrc->debugPrint(
							"fetching...\n");
					pvt->_sqlrc->debugPreEnd();
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

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("		");
				pvt->_sqlrc->debugPrint("done fetching.\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// NULL terminate the buffer.  This makes 
			// certain operations safer and won't hurt
			// since the actual length (which doesn't
			// include the NULL) is available from
			// getOutputBindLength.
			buffer[totallength]='\0';
			(*pvt->_inoutbindvars)[count].value.lobval=buffer;
			(*pvt->_inoutbindvars)[count].resultvaluesize=
								totallength;
		}

		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint(
					(*pvt->_inoutbindvars)[count].variable);
			pvt->_sqlrc->debugPrint("=");
			if ((*pvt->_inoutbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_BLOB) {
				pvt->_sqlrc->debugPrintBlob(
					(*pvt->_inoutbindvars)[count].
							value.lobval,
					(*pvt->_inoutbindvars)[count].
							resultvaluesize);
			} else if ((*pvt->_inoutbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_CLOB) {
				pvt->_sqlrc->debugPrintClob(
					(*pvt->_inoutbindvars)[count].
							value.lobval,
					(*pvt->_inoutbindvars)[count].
							resultvaluesize);
			} else if ((*pvt->_inoutbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_CURSOR) {
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[count].
							value.cursorid);
			} else if ((*pvt->_inoutbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_INTEGER) {
				pvt->_sqlrc->debugPrint(
						(*pvt->_inoutbindvars)[count].
							value.integerval);
			} else if ((*pvt->_inoutbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_DOUBLE) {
				pvt->_sqlrc->debugPrint(
						(*pvt->_inoutbindvars)[count].
							value.doubleval.value);
				pvt->_sqlrc->debugPrint("(");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[count].
							value.doubleval.
							precision);
				pvt->_sqlrc->debugPrint(",");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[count].
							value.doubleval.
							scale);
				pvt->_sqlrc->debugPrint(")");
			} else if ((*pvt->_inoutbindvars)[count].type==
						SQLRCLIENTBINDVARTYPE_DATE) {
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[count].
							value.dateval.year);
				pvt->_sqlrc->debugPrint("-");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[count].
							value.dateval.month);
				pvt->_sqlrc->debugPrint("-");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[count].
							value.dateval.day);
				pvt->_sqlrc->debugPrint(" ");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[count].
							value.dateval.hour);
				pvt->_sqlrc->debugPrint(":");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[count].
							value.dateval.minute);
				pvt->_sqlrc->debugPrint(":");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[count].
							value.dateval.second);
				pvt->_sqlrc->debugPrint(".");
				pvt->_sqlrc->debugPrint((int64_t)
						(*pvt->_inoutbindvars)[count].
						value.dateval.microsecond);
				pvt->_sqlrc->debugPrint(" ");
				pvt->_sqlrc->debugPrint(
						(*pvt->_inoutbindvars)[count].
							value.dateval.tz);
			} else {
				pvt->_sqlrc->debugPrint(
						(*pvt->_inoutbindvars)[count].
							value.stringval);
			}
			pvt->_sqlrc->debugPrint("\n");
			pvt->_sqlrc->debugPreEnd();
		}

		count++;
	}

	// cache the input/output binds
	cacheInputOutputBinds(count);

	return true;
}

bool sqlrcursor::parseResults() {

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Parsing Results\n");
		pvt->_sqlrc->debugPreEnd();
	}

	// if we're already at the end of the result set, then just return
	if (pvt->_endofresultset) {
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint(
				"Already at the end of the result set\n");
			pvt->_sqlrc->debugPreEnd();
		}
		return true;
	}

	// if we just resumed a result set, then reset the rowcount
	if (!pvt->_rowcount && pvt->_resumedlastrowindex) {
		pvt->_rowcount=pvt->_resumedlastrowindex+1;
	}

	// set firstrowindex to the index of the first row in the block of rows
	pvt->_firstrowindex=pvt->_rowcount;

	// useful variables
	uint16_t		type;
	uint32_t		length;
	char			*buffer=NULL;
	uint32_t		colindex=0;
	sqlrclientcolumn	*currentcol;
	sqlrclientrow		*currentrow=NULL;
	bool			firstrow=true;

	// in the block of rows, keep track of
	// how many rows are actually populated
	uint64_t	rowblockcount=0;

	// get rows
	for (;;) {

		// get the type of the field
		if (getShort(&type)!=sizeof(uint16_t)) {
			setError("Failed to get the field type.\n"
				"A network error may have occurred");
			return false;
		}

		// check for an error
		if (type==FETCH_ERROR) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
						"Got fetch error.\n");
				pvt->_sqlrc->debugPreEnd();
			}
			pvt->_endofresultset=true;

			// if we were stepping through a cached result set
			// then we need to close the file
			clearCacheSource();

			uint16_t err=getErrorStatus();
			if (err==TIMEOUT_GETTING_ERROR_STATUS) {
				// The pattern here is that we bail
				// immediately.  Error status has
				// already been set.
				pvt->_sqlrc->endSession();
				return false;
			}
			getErrorFromServer();
			if (err==ERROR_OCCURRED_DISCONNECT) {
				pvt->_sqlrc->endSession();
				return false;
			}
			break;
		}

		// check for the end of the result set
		if (type==END_RESULT_SET) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
						"Got end of result set.\n");
				pvt->_sqlrc->debugPreEnd();
			}
			pvt->_endofresultset=true;

			// if we were stepping through a cached result set
			// then we need to close the file
			clearCacheSource();
			break;
		} 

		// if we're on the first column, start a new row,
		// reset the column pointer, and increment the
		// buffer counter and total row counter
		if (colindex==0) {

			if (rowblockcount<OPTIMISTIC_ROW_COUNT) {
				if (!pvt->_rows) {
					createRowBuffers();
				}
				currentrow=pvt->_rows[rowblockcount];
			} else {
				if (pvt->_sqlrc->debug()) {
					pvt->_sqlrc->debugPreStart();
					pvt->_sqlrc->debugPrint(
						"Creating extra rows.\n");
					pvt->_sqlrc->debugPreEnd();
				}
				if (!pvt->_firstextrarow) {
					currentrow=new sqlrclientrow(
								pvt->_colcount);
					pvt->_firstextrarow=currentrow;
				} else {
					currentrow->next=new sqlrclientrow(
								pvt->_colcount);
					currentrow=currentrow->next;
				}
			}
			if (pvt->_colcount>currentrow->colcount) {
				currentrow->resize(pvt->_colcount);
			}

			rowblockcount++;
			pvt->_rowcount++;
		}

		if (type==NULL_DATA) {

			// handle null data
			if (pvt->_returnnulls) {
				buffer=NULL;
			} else {
				buffer=(char *)pvt->_rowstorage->allocate(1);
				buffer[0]='\0';
			}
			length=0;

		} else if (type==STRING_DATA) {
		
			// handle non-null data
			if (getLong(&length)!=sizeof(uint32_t)) {
				setError("Failed to get the field length.\n"
					"A network error may have occurred");
				return false;
			}

			// for non-long, non-NULL datatypes...
			// get the field into a buffer
			buffer=(char *)pvt->_rowstorage->allocate(length+1);
			if ((uint32_t)getString(buffer,length)!=length) {
				setError("Failed to get the field data.\n"
					"A network error may have occurred");
				return false;
			}
			buffer[length]='\0';

		} else if (type==START_LONG_DATA) {

			uint64_t	totallength;
			if (getLongLong(&totallength)!=sizeof(uint64_t)) {
				setError("Failed to get total length.\n"
					"A network error may have occurred");
				return false;
			}

			// create a buffer to hold the data
			if (totallength) {
				buffer=new char[totallength+1];
			} else {
				buffer=(char *)pvt->_rowstorage->allocate(1);
			}

			// handle a long datatype
			uint64_t	offset=0;
			for (;;) {

				// get the type of the chunk
				if (getShort(&type)!=sizeof(uint16_t)) {
					delete[] buffer;
					setError("Failed to get chunk type.\n"
						"A network error may have "
						"occurred");
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
						"A network error may have "
						"occurred");
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
					if (totallength) {
						delete[] buffer;
					}
					buffer=newbuffer;
					totallength=offset+length;
				}

				// get the chunk of data
				if ((uint32_t)getString(buffer+offset,
							length)!=length) {
					delete[] buffer;
					setError("Failed to get chunk data.\n"
						"A network error may have "
						"occurred");
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
	
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			if (buffer) {
				if (type==END_LONG_DATA) {
					pvt->_sqlrc->debugPrint("\nLOB data:");
					pvt->_sqlrc->debugPrintBlob(
							buffer,length);
				} else {
					pvt->_sqlrc->debugPrint("\"");
					pvt->_sqlrc->debugPrint(buffer);
					pvt->_sqlrc->debugPrint("\",");
				}
			} else {
				pvt->_sqlrc->debugPrint(buffer);
				pvt->_sqlrc->debugPrint(",");
			}
			pvt->_sqlrc->debugPreEnd();
		}

		// tag the column as a long data type or not
		currentcol=getColumnInternal(colindex);

		// set whether this column is a "long type" or not
		// (unless it's already set)
		if (firstrow || !currentcol->longdatatype) {
			currentcol->longdatatype=(type==END_LONG_DATA)?1:0;
		}

		if (pvt->_sendcolumninfo==SEND_COLUMN_INFO && 
				pvt->_sentcolumninfo==SEND_COLUMN_INFO) {

			// keep track of the longest field
			if (length>currentcol->longest) {
				currentcol->longest=length;
			}
		}

		// move to the next column, handle end of row 
		colindex++;
		if (colindex==pvt->_colcount) {

			colindex=0;

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("\n");
				pvt->_sqlrc->debugPreEnd();
			}

			// check to see if we've gotten enough rows
			if (pvt->_rsbuffersize &&
				rowblockcount==pvt->_rsbuffersize) {
				break;
			}

			firstrow=false;
		}
	}

	// terminate the row list
	if (rowblockcount>=OPTIMISTIC_ROW_COUNT && currentrow) {
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
	pvt->_rows=new sqlrclientrow *[OPTIMISTIC_ROW_COUNT];
	for (uint64_t i=0; i<OPTIMISTIC_ROW_COUNT; i++) {
		pvt->_rows[i]=new sqlrclientrow(pvt->_colcount);
	}
}

void sqlrcursor::createExtraRowArray() {

	// create the arrays
	uint64_t	howmany=pvt->_rowcount-
				pvt->_firstrowindex-
				OPTIMISTIC_ROW_COUNT;
	pvt->_extrarows=new sqlrclientrow *[howmany];
	
	// populate the arrays
	sqlrclientrow	*currentrow=pvt->_firstextrarow;
	for (uint64_t i=0; i<howmany; i++) {
		pvt->_extrarows[i]=currentrow;
		currentrow=currentrow->next;
	}
}

void sqlrcursor::getErrorFromServer() {

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Getting Error From Server\n");
		pvt->_sqlrc->debugPreEnd();
	}

	bool	networkerror=true;

	// get the error code
	if (getLongLong((uint64_t *)&pvt->_errorno)==sizeof(uint64_t)) {

		// get the length of the error string
		uint16_t	length;
		if (getShort(&length)==sizeof(uint16_t)) {

			// get the error string
			pvt->_error=new char[length+1];
			pvt->_cs->read(pvt->_error,length);
			pvt->_error[length]='\0';

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

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Setting Error\n");
		pvt->_sqlrc->debugPreEnd();
	}
	pvt->_error=charstring::duplicate(err);
	handleError();
}

void sqlrcursor::handleError() {

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint((int64_t)pvt->_errorno);
		pvt->_sqlrc->debugPrint(":\n");
		pvt->_sqlrc->debugPrint(pvt->_error);
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}

	pvt->_endofresultset=true;

	cacheError();
	finishCaching();
}

bool sqlrcursor::fetchRowIntoBuffer(uint64_t row, uint64_t *rowbufferindex) {

	// bail if the requested row is before the current block of rows
	if (row<pvt->_firstrowindex) {
		return false;
	}

	// if the requested row is in the current block of rows
	// then return the row buffer index
	if (row>=pvt->_firstrowindex && row<pvt->_rowcount) {
		*rowbufferindex=row-pvt->_firstrowindex;
		return true;
	}

	// bail if the requested row is past the end of the result set
	if (pvt->_endofresultset) {
		return false;
	}

	// bail if we lost our data source
	if (!pvt->_sqlrc->connected() &&
		!(pvt->_cachesource && pvt->_cachesourceind)) {
		return false;
	}

	// otherwise, fetch additional rows...

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Fetching additional rows...\n");
		pvt->_sqlrc->debugPreEnd();
	}

	// skip to the block of rows containing the requested row,
	// fetch that block of rows, and process what we get back...
	do {

		// clear the row buffers
		clearRows();

		// if we're not fetching from a cached result set,
		// then tell the server to send some rows
		if (!pvt->_cachesource && !pvt->_cachesourceind) {
			pvt->_cs->write((uint16_t)FETCH_RESULT_SET);
			pvt->_cs->write(pvt->_cursorid);
		}

		// calculate how many rows to actually skip
		// if we're caching the result set, then we shouldn't skip any
		// (thus the outer loop)
		// also, if we're fetching all rows then we shouldn't skip any
		uint64_t	rowstoskip=0;
		if (!pvt->_cachedest && pvt->_rsbuffersize) {
			rowstoskip=
			row-((pvt->_rsbuffersize)?(row%pvt->_rsbuffersize):0)-
			pvt->_rowcount;
		}

		if (!skipAndFetch(false,rowstoskip) || !parseResults()) {
			return false;
		}

	} while (!pvt->_endofresultset && pvt->_rowcount<=row);

	// bail if the requested row is still past the end of the result set
	if (row>=pvt->_rowcount) {
		return false;
	}

	// the requested row must be in the current block of rows,
	// return the row buffer index
	*rowbufferindex=(pvt->_rsbuffersize)?(row%pvt->_rsbuffersize):row;
	return true;
}

int32_t sqlrcursor::getBool(bool *boolean) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (pvt->_cachesource && pvt->_cachesourceind) {
		return pvt->_cachesource->read(boolean);
	} else {
		return pvt->_cs->read(boolean);
	}
}

int32_t sqlrcursor::getShort(uint16_t *integer,
				int32_t timeoutsec, int32_t timeoutusec) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (pvt->_cachesource && pvt->_cachesourceind) {
		return pvt->_cachesource->read(integer);
	} else {
		return pvt->_cs->read(integer,timeoutsec,timeoutusec);
	}
}

int32_t sqlrcursor::getShort(uint16_t *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (pvt->_cachesource && pvt->_cachesourceind) {
		return pvt->_cachesource->read(integer);
	} else {
		return pvt->_cs->read(integer);
	}
}

int32_t sqlrcursor::getLong(uint32_t *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (pvt->_cachesource && pvt->_cachesourceind) {
		return pvt->_cachesource->read(integer);
	} else {
		return pvt->_cs->read(integer);
	}
}

int32_t sqlrcursor::getLongLong(uint64_t *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (pvt->_cachesource && pvt->_cachesourceind) {
		return pvt->_cachesource->read(integer);
	} else {
		return pvt->_cs->read(integer);
	}
}

int32_t sqlrcursor::getString(char *string, int32_t size) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (pvt->_cachesource && pvt->_cachesourceind) {
		return pvt->_cachesource->read(string,size);
	} else {
		return pvt->_cs->read(string,size);
	}
}

int32_t sqlrcursor::getDouble(double *value) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (pvt->_cachesource && pvt->_cachesourceind) {
		return pvt->_cachesource->read(value);
	} else {
		return pvt->_cs->read(value);
	}
}

bool sqlrcursor::fetchFromBindCursor() {

	if (!pvt->_endofresultset || !pvt->_sqlrc->connected()) {
		return false;
	}

	// FIXME: should these be here?
	clearVariables();
	clearResultSet();

	pvt->_cached=false;
	pvt->_endofresultset=false;

	// refresh socket client
	pvt->_cs=pvt->_sqlrc->cs();

	// tell the server we're fetching from a bind cursor
	pvt->_cs->write((uint16_t)FETCH_FROM_BIND_CURSOR);

	// send the cursor id to the server
	pvt->_cs->write((uint16_t)pvt->_cursorid);

	sendGetColumnInfo();

	pvt->_sqlrc->flushWriteBuffer();

	return processInitialResultSet();
}

bool sqlrcursor::openCachedResultSet(const char *filename) {

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Opening cached result set: ");
		pvt->_sqlrc->debugPrint(filename);
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}

	if (!pvt->_endofresultset) {
		closeResultSet(true);
	}
	clearResultSet();

	pvt->_cached=true;
	pvt->_endofresultset=false;

	// create the index file name
	size_t	indexfilenamelen=charstring::length(filename)+5;
	char	*indexfilename=new char[indexfilenamelen];
	charstring::copy(indexfilename,filename);
	charstring::append(indexfilename,".ind");

	// open the file
	pvt->_cachesource=new file();
	pvt->_cachesourceind=new file();
	if ((pvt->_cachesource->open(filename,O_RDWR)) &&
		(pvt->_cachesourceind->open(indexfilename,O_RDWR))) {

		delete[] indexfilename;

		// make sure it's a cache file and skip the ttl
		char		magicid[13];
		uint64_t	ttl;
		if (getString(magicid,13)==13 &&
			!charstring::compare(magicid,"SQLRELAYCACHE",13) &&
			getLongLong(&ttl)==sizeof(uint64_t)) {

			// process the result set
			return processInitialResultSet();
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

	// if we fell through to here, then an error has occurred
	clearCacheSource();
	return false;
}

void sqlrcursor::clearCacheSource() {
	if (pvt->_cachesource) {
		pvt->_cachesource->close();
		delete pvt->_cachesource;
		pvt->_cachesource=NULL;
	}
	if (pvt->_cachesourceind) {
		pvt->_cachesourceind->close();
		delete pvt->_cachesourceind;
		pvt->_cachesourceind=NULL;
	}
}

uint32_t sqlrcursor::colCount() {
	return pvt->_colcount;
}

sqlrclientcolumn *sqlrcursor::getColumn(uint32_t index) {
	if (pvt->_sendcolumninfo==SEND_COLUMN_INFO && 
			pvt->_sentcolumninfo==SEND_COLUMN_INFO &&
			pvt->_colcount && index<pvt->_colcount) {
		return getColumnInternal(index);
	}
	return NULL;
}

sqlrclientcolumn *sqlrcursor::getColumn(const char *name) {
	if (pvt->_sendcolumninfo==SEND_COLUMN_INFO && 
			pvt->_sentcolumninfo==SEND_COLUMN_INFO) {
		sqlrclientcolumn	*whichcolumn;
		for (uint32_t i=0; i<pvt->_colcount; i++) {
			whichcolumn=getColumnInternal(i);
			if (!charstring::compare(whichcolumn->name,name)) {
				return whichcolumn;
			}
		}
	}
	return NULL;
}

sqlrclientcolumn *sqlrcursor::getColumnInternal(uint32_t index) {
	if (index<OPTIMISTIC_COLUMN_COUNT) {
		return &pvt->_columns[index];
	}
	return &pvt->_extracolumns[index-OPTIMISTIC_COLUMN_COUNT];
}

const char * const *sqlrcursor::getColumnNames() {

	if (pvt->_sendcolumninfo==DONT_SEND_COLUMN_INFO ||
			pvt->_sentcolumninfo==DONT_SEND_COLUMN_INFO) {
		return NULL;
	}

	if (!pvt->_columnnamearray) {
		if (pvt->_sqlrc->debug()) {
			pvt->_sqlrc->debugPreStart();
			pvt->_sqlrc->debugPrint("Creating Column Arrays...\n");
			pvt->_sqlrc->debugPreEnd();
		}
	
		// build a 2d array of pointers to the column names
		pvt->_columnnamearray=new char *[pvt->_colcount+1];
		pvt->_columnnamearray[pvt->_colcount]=NULL;
		for (uint32_t i=0; i<pvt->_colcount; i++) {
			pvt->_columnnamearray[i]=getColumnInternal(i)->name;
		}
	}
	return pvt->_columnnamearray;
}

const char *sqlrcursor::getColumnName(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?whichcol->name:NULL;
}

const char *sqlrcursor::getColumnType(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	if (whichcol) {
		if (pvt->_columntypeformat!=COLUMN_TYPE_IDS) {
			return whichcol->typestring;
		} else {
			return datatypestring[whichcol->type];
		}
	}
	return NULL;
}

uint32_t sqlrcursor::getColumnLength(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?whichcol->length:0;
}

uint32_t sqlrcursor::getColumnPrecision(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?whichcol->precision:0;
}

uint32_t sqlrcursor::getColumnScale(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?whichcol->scale:0;
}

bool sqlrcursor::getColumnIsNullable(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->nullable!=0):false;
}

bool sqlrcursor::getColumnIsPrimaryKey(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->primarykey!=0):false;
}

bool sqlrcursor::getColumnIsUnique(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->unique!=0):false;
}

bool sqlrcursor::getColumnIsPartOfKey(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->partofkey!=0):false;
}

bool sqlrcursor::getColumnIsUnsigned(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->unsignednumber!=0):false;
}

bool sqlrcursor::getColumnIsZeroFilled(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->zerofill!=0):false;
}

bool sqlrcursor::getColumnIsBinary(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->binary!=0):false;
}

bool sqlrcursor::getColumnIsAutoIncrement(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->autoincrement!=0):false;
}

const char *sqlrcursor::getColumnTable(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?whichcol->table:NULL;
}

uint32_t sqlrcursor::getLongest(uint32_t col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?whichcol->longest:0;
}

const char *sqlrcursor::getColumnType(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	if (whichcol) {
		if (pvt->_columntypeformat!=COLUMN_TYPE_IDS) {
			return whichcol->typestring;
		} else {
			return datatypestring[whichcol->type];
		}
	}
	return NULL;
}

uint32_t sqlrcursor::getColumnLength(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?whichcol->length:0;
}

uint32_t sqlrcursor::getColumnPrecision(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?whichcol->precision:0;
}

uint32_t sqlrcursor::getColumnScale(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?whichcol->scale:0;
}

bool sqlrcursor::getColumnIsNullable(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->nullable!=0):false;
}

bool sqlrcursor::getColumnIsPrimaryKey(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->primarykey!=0):false;
}

bool sqlrcursor::getColumnIsUnique(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->unique!=0):false;
}

bool sqlrcursor::getColumnIsPartOfKey(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->partofkey!=0):false;
}

bool sqlrcursor::getColumnIsUnsigned(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->unsignednumber!=0):false;
}

bool sqlrcursor::getColumnIsZeroFilled(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->zerofill!=0):false;
}

bool sqlrcursor::getColumnIsBinary(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->binary!=0):false;
}

bool sqlrcursor::getColumnIsAutoIncrement(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?(whichcol->autoincrement!=0):false;
}

const char *sqlrcursor::getColumnTable(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?whichcol->table:NULL;
}


uint32_t sqlrcursor::getLongest(const char *col) {
	sqlrclientcolumn	*whichcol=getColumn(col);
	return (whichcol)?whichcol->longest:0;
}

uint64_t sqlrcursor::firstRowIndex() {
	return pvt->_firstrowindex;
}

bool sqlrcursor::endOfResultSet() {
	return pvt->_endofresultset;
}

uint64_t sqlrcursor::rowCount() {
	return pvt->_rowcount;
}

uint64_t sqlrcursor::affectedRows() {
	if (pvt->_knowsaffectedrows==AFFECTED_ROWS) {
		return pvt->_affectedrows;
	}
	return 0;
}

uint64_t sqlrcursor::totalRows() {
	if (pvt->_knowsactualrows==ACTUAL_ROWS) {
		return pvt->_actualrows;
	}
	return 0;
}

int64_t sqlrcursor::errorNumber() {
	// if we have a code then we should have a message too,
	// the codes could be any number, including 0, so check
	// the message to see which code to return
	if (pvt->_error) {
		return pvt->_errorno;
	} else if (pvt->_sqlrc->error()) {
		return pvt->_sqlrc->errorno();
	}
	return 0;
}

const char *sqlrcursor::errorMessage() {
	if (pvt->_error) {
		return pvt->_error;
	} else if (pvt->_sqlrc->error()) {
		return pvt->_sqlrc->error();
	}
	return NULL;
}

void sqlrcursor::getNullsAsEmptyStrings() {
	pvt->_returnnulls=false;
}

void sqlrcursor::getNullsAsNulls() {
	pvt->_returnnulls=true;
}

char *sqlrcursor::getFieldInternal(uint64_t row, uint32_t col) {
	if (row<OPTIMISTIC_ROW_COUNT) {
		return pvt->_rows[row]->getField(col);
	}
	return pvt->_extrarows[row-OPTIMISTIC_ROW_COUNT]->getField(col);
}

uint32_t sqlrcursor::getFieldLengthInternal(uint64_t row, uint32_t col) {
	if (row<OPTIMISTIC_ROW_COUNT) {
		return pvt->_rows[row]->getFieldLength(col);
	}
	return pvt->_extrarows[row-OPTIMISTIC_ROW_COUNT]->getFieldLength(col);
}

const char *sqlrcursor::getField(uint64_t row, uint32_t col) {

	// bail if the requested column is invalid
	if (col>=pvt->_colcount) {
		return NULL;
	}

	// fetch and return the field
	uint64_t	rowbufferindex;
	if (fetchRowIntoBuffer(row,&rowbufferindex)) {
		return getFieldInternal(rowbufferindex,col);
	}
	return NULL;
}

int64_t sqlrcursor::getFieldAsInteger(uint64_t row, uint32_t col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toInteger(field):0;
}

double sqlrcursor::getFieldAsDouble(uint64_t row, uint32_t col) {
	const char	*field=getField(row,col);
	return (field)?charstring::toFloatC(field):0.0;
}

const char *sqlrcursor::getField(uint64_t row, const char *col) {

	// bail if no column info was sent
	if (pvt->_sendcolumninfo!=SEND_COLUMN_INFO || 
			pvt->_sentcolumninfo!=SEND_COLUMN_INFO) {
		return NULL;
	}

	// get the column index, by name
	for (uint32_t i=0; i<pvt->_colcount; i++) {
		if (!charstring::compare(getColumnInternal(i)->name,col)) {

			// fetch and return the field
			uint64_t	rowbufferindex;
			if (fetchRowIntoBuffer(row,&rowbufferindex)) {
				return getFieldInternal(rowbufferindex,i);
			}
			return NULL;
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
	return (field)?charstring::toFloatC(field):0.0;
}

uint32_t sqlrcursor::getFieldLength(uint64_t row, uint32_t col) {

	// bail if the requested column is invalid
	if (col>=pvt->_colcount) {
		return 0;
	}

	// fetch and return the field length
	uint64_t	rowbufferindex;
	if (fetchRowIntoBuffer(row,&rowbufferindex)) {
		return getFieldLengthInternal(rowbufferindex,col);
	}
	return 0;
}

uint32_t sqlrcursor::getFieldLength(uint64_t row, const char *col) {

	// bail if no column info was sent
	if (pvt->_sendcolumninfo!=SEND_COLUMN_INFO ||
		pvt->_sentcolumninfo!=SEND_COLUMN_INFO) {
		return 0;
	}

	// get the column index, by name
	for (uint32_t i=0; i<pvt->_colcount; i++) {
		if (!charstring::compare(getColumnInternal(i)->name,col)) {

			// fetch and return the field length
			uint64_t	rowbufferindex;
			if (fetchRowIntoBuffer(row,&rowbufferindex)) {
				return getFieldLengthInternal(rowbufferindex,i);
			}
			return 0;
		}
	}
	return 0;
}

const char * const *sqlrcursor::getRow(uint64_t row) {

	// fetch and return the row
	uint64_t	rowbufferindex;
	if (fetchRowIntoBuffer(row,&rowbufferindex)) {
		if (!pvt->_fields) {
			createFields();
		}
		return pvt->_fields[rowbufferindex];
	}
	return NULL;
}

void sqlrcursor::createFields() {
	// lets say that rowcount=5 and firstrowindex=3,
	// the fields array will contain 2 elements:
	// 	fields[0] (corresponding to row 3) and
	// 	fields[1] (corresponding to row 4)
	uint64_t	rowbuffercount=pvt->_rowcount-pvt->_firstrowindex;
	pvt->_fields=new char **[rowbuffercount+1];
	pvt->_fields[rowbuffercount]=(char **)NULL;
	for (uint64_t i=0; i<rowbuffercount; i++) {
		pvt->_fields[i]=new char *[pvt->_colcount+1];
		pvt->_fields[i][pvt->_colcount]=(char *)NULL;
		for (uint32_t j=0; j<pvt->_colcount; j++) {
			pvt->_fields[i][j]=getFieldInternal(i,j);
		}
	}
}

uint32_t *sqlrcursor::getRowLengths(uint64_t row) {

	// fetch and return the row lengths
	uint64_t	rowbufferindex;
	if (fetchRowIntoBuffer(row,&rowbufferindex)) {
		if (!pvt->_fieldlengths) {
			createFieldLengths();
		}
		return pvt->_fieldlengths[rowbufferindex];
	}
	return NULL;
}

void sqlrcursor::createFieldLengths() {
	// lets say that rowcount=5 and firstrowindex=3,
	// the fieldlengths array will contain 2 elements:
	// 	fieldlengths[0] (corresponding to row 3) and
	// 	fieldlengths[1] (corresponding to row 4)
	uint64_t	rowbuffercount=pvt->_rowcount-pvt->_firstrowindex;
	pvt->_fieldlengths=new uint32_t *[rowbuffercount+1];
	pvt->_fieldlengths[rowbuffercount]=0;
	for (uint64_t i=0; i<rowbuffercount; i++) {
		pvt->_fieldlengths[i]=new uint32_t[pvt->_colcount+1];
		pvt->_fieldlengths[i][pvt->_colcount]=0;
		for (uint32_t j=0; j<pvt->_colcount; j++) {
			pvt->_fieldlengths[i][j]=getFieldLengthInternal(i,j);
		}
	}
}

void sqlrcursor::suspendResultSet() {

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Suspending Result Set\n");
		pvt->_sqlrc->debugPreEnd();
	}

	if (pvt->_sqlrc->connected() && !pvt->_cached) {

		// refresh socket client
		pvt->_cs=pvt->_sqlrc->cs();

		pvt->_cs->write((uint16_t)SUSPEND_RESULT_SET);
		pvt->_cs->write(pvt->_cursorid);

		pvt->_sqlrc->flushWriteBuffer();
	}

	clearCacheDest();
	pvt->_suspendresultsetsent=1;
}

uint16_t sqlrcursor::getResultSetId() {
	return pvt->_cursorid;
}

bool sqlrcursor::resumeResultSet(uint16_t id) {
	return resumeCachedResultSet(id,NULL);
}

bool sqlrcursor::resumeCachedResultSet(uint16_t id, const char *filename) {

	if (!pvt->_endofresultset && !pvt->_suspendresultsetsent) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!pvt->_sqlrc->connected()) {
		return false;
	}

	pvt->_cached=false;
	pvt->_resumed=true;
	pvt->_endofresultset=false;

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Resuming Result Set of Cursor: ");
		pvt->_sqlrc->debugPrint((int64_t)id);
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}

	// refresh socket client
	pvt->_cs=pvt->_sqlrc->cs();

	// tell the server we want to resume the result set
	pvt->_cs->write((uint16_t)RESUME_RESULT_SET);

	// send the id of the cursor we want to 
	// resume the result set of to the server
	pvt->_cs->write(id);

	// process the result set
	if (!charstring::isNullOrEmpty(filename)) {
		cacheToFile(filename);
	}
	return processInitialResultSet();
}

bool sqlrcursor::nextResultSet() {

	// One could imagine an optimization where the availability of a next
	// result set could be returned along with every result set..

	if (!pvt->_queryptr) {
		setError("No query being executed: _queryptr");
		return false;
	}
	if (!pvt->_reexecute) {
		setError("No query being executed: _rexecute");
		return false;
	}

	if (!pvt->_sqlrc->openSession()) {
		return false;
	}

	// refresh socket client
	pvt->_cs=pvt->_sqlrc->cs();

	if (pvt->_sqlrc->debug()) {
		pvt->_sqlrc->debugPreStart();
		pvt->_sqlrc->debugPrint("Requesting nextResultSet");
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPrint("Requesting Cursor: ");
		pvt->_sqlrc->debugPrint((int64_t)pvt->_cursorid);
		pvt->_sqlrc->debugPrint("\n");
		pvt->_sqlrc->debugPreEnd();
	}
	pvt->_cs->write((uint16_t)NEXT_RESULT_SET);
	pvt->_cs->write(pvt->_cursorid);
	pvt->_sqlrc->flushWriteBuffer();

	uint16_t err=getErrorStatus();
	if (err!=NO_ERROR_OCCURRED) {
		if (err==TIMEOUT_GETTING_ERROR_STATUS) {
			// the pattern here is that we bail immediately.
			// error status has already been set.
			pvt->_sqlrc->endSession();
			return false;
		}
		getErrorFromServer();
		if (err==ERROR_OCCURRED_DISCONNECT) {
			pvt->_sqlrc->endSession();
		}
		return false;
	}
	bool resultbool;
	if (getBool(&resultbool)!=sizeof(bool)) {
		setError("Failed to get bool value.\n "
			 "A network error may have occurred.");
		return false;
	}
	return resultbool;
}

void sqlrcursor::closeResultSet() {
	closeResultSet(true);
}

void sqlrcursor::closeResultSet(bool closeremote) {

	// If the end of the previous result set was never reached, abort it.
	// If we're caching data to a local file, get the rest of the data; we
	// won't have to abort the result set in that case, the server will
	// do it.

	// refresh socket client
	pvt->_cs=pvt->_sqlrc->cs();

	if (pvt->_sqlrc->connected() || pvt->_cached) {
		if (pvt->_cachedest && pvt->_cachedestind) {
			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint(
					"Getting the rest of the result set, "
					"since this is a cached result set.\n");
				pvt->_sqlrc->debugPreEnd();
			}
			while (!pvt->_endofresultset) {
				clearRows();

				// if we're not fetching from a cached result 
				// set, then tell the server to send one 
				if (!pvt->_cachesource &&
						!pvt->_cachesourceind) {
					pvt->_cs->write(
						(uint16_t)FETCH_RESULT_SET);
					pvt->_cs->write(pvt->_cursorid);
				}

				// parseResults should call finishCaching when
				// it hits the end of the result set, but
				// if it or skipAndFetch return a -1 (network
				// error) we'll have to call it ourselves.
				if (!skipAndFetch(false,0) || !parseResults()) {
					finishCaching();
					return;
				}
			}
		} else if (closeremote && pvt->_havecursorid) {

			if (pvt->_sqlrc->debug()) {
				pvt->_sqlrc->debugPreStart();
				pvt->_sqlrc->debugPrint("Aborting Result "
							"Set For Cursor: ");
				pvt->_sqlrc->debugPrint(
						(int64_t)pvt->_cursorid);
				pvt->_sqlrc->debugPrint("\n");
				pvt->_sqlrc->debugPreEnd();
			}

			pvt->_cs->write((uint16_t)ABORT_RESULT_SET);
			pvt->_cs->write((uint16_t)DONT_NEED_NEW_CURSOR);
			pvt->_cs->write(pvt->_cursorid);
			pvt->_sqlrc->flushWriteBuffer();
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
	pvt->_firstrowindex=0;
	pvt->_resumedlastrowindex=0;
	pvt->_rowcount=0;
	pvt->_actualrows=0;
	pvt->_affectedrows=0;
	pvt->_endofresultset=true;
	pvt->_suspendresultsetsent=0;
}

void sqlrcursor::clearError() {
	delete[] pvt->_error;
	pvt->_error=NULL;
	pvt->_errorno=0;
	if (pvt->_sqlrc) {
		pvt->_sqlrc->clearError();
	}
}

void sqlrcursor::clearRows() {

	// delete data in rows for long datatypes
	uint32_t	rowbuffercount=pvt->_rowcount-pvt->_firstrowindex;
	for (uint32_t i=0; i<rowbuffercount; i++) {
		for (uint32_t j=0; j<pvt->_colcount; j++) {
			if (getColumnInternal(j)->longdatatype) {
				char		*field=getFieldInternal(i,j);
				uint32_t	len=getFieldLengthInternal(i,j);
				// Null lobs might be stored as a NULL or as
				// an empty string.  In either case (and in no
				// other case) the length will be 0.  In the
				// case of a NULL there's nothing to delete.
				// In the case of an empty string, the memory
				// will be allocated from the rowstorage pool
				// and shouldn't be deallocated here.
				if (len) {
					delete[] field;
				}
			}
		}
	}

	// delete linked list storing extra result set fields
	sqlrclientrow	*currentrow;
	if (pvt->_firstextrarow) {
		currentrow=pvt->_firstextrarow;
		while (currentrow) {
			pvt->_firstextrarow=currentrow->next;
			delete currentrow;
			currentrow=pvt->_firstextrarow;
		}
		pvt->_firstextrarow=NULL;
	}
	currentrow=NULL;

	// delete array pointing to linked list items
	delete[] pvt->_extrarows;
	pvt->_extrarows=NULL;

	// delete arrays of fields and field lengths
	if (pvt->_fields) {
		for (uint32_t i=0; i<rowbuffercount; i++) {
			delete[] pvt->_fields[i];
		}
		delete[] pvt->_fields;
		pvt->_fields=NULL;
	}
	if (pvt->_fieldlengths) {
		for (uint32_t i=0; i<rowbuffercount; i++) {
			delete[] pvt->_fieldlengths[i];
		}
		delete[] pvt->_fieldlengths;
		pvt->_fieldlengths=NULL;
	}

	// reset the row storage pool
	pvt->_rowstorage->clear();
}

void sqlrcursor::clearColumns() {

	// delete the column type strings (if necessary)
	if (pvt->_sentcolumninfo==SEND_COLUMN_INFO &&
				pvt->_columntypeformat!=COLUMN_TYPE_IDS) {
		for (uint32_t i=0; i<pvt->_colcount; i++) {
			delete[] getColumnInternal(i)->typestring;
		}
	}

	// reset the column storage pool
	pvt->_colstorage->clear();

	// reset the column count
	pvt->_previouscolcount=pvt->_colcount;
	pvt->_colcount=0;

	// delete array pointing to each column name
	delete[] pvt->_columnnamearray;
	pvt->_columnnamearray=NULL;
}

const char *sqlrcursor::getQueryTree() {

	delete[] pvt->_querytree;
	pvt->_querytree=NULL;

	pvt->_reexecute=false;
	pvt->_validatebinds=false;
	pvt->_resumed=false;
	clearVariables();

	if (!pvt->_endofresultset) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!pvt->_sqlrc->openSession()) {
		return NULL;
	}

	pvt->_cached=false;
	pvt->_endofresultset=false;

	// refresh socket client
	pvt->_cs=pvt->_sqlrc->cs();

	// tell the server we want to get a query tree
	pvt->_cs->write((uint16_t)GET_QUERY_TREE);

	// tell the server whether we'll need a cursor or not
	sendCursorStatus();

	pvt->_sqlrc->flushWriteBuffer();

	uint16_t	err=getErrorStatus();
	if (err!=NO_ERROR_OCCURRED) {
		if (err==TIMEOUT_GETTING_ERROR_STATUS) {
			pvt->_sqlrc->endSession();
			return NULL;
		}
		getErrorFromServer();
		if (err==ERROR_OCCURRED_DISCONNECT) {
			pvt->_sqlrc->endSession();
		}
		return NULL;
	}

	// get the size of the tree
	uint64_t	len;
	if (pvt->_cs->read(&len)!=sizeof(uint64_t)) {
		return NULL;
	}

	// get the tree itself
	pvt->_querytree=new char[len+1];
	if ((uint64_t)pvt->_cs->read(pvt->_querytree,len)!=len) {
		delete[] pvt->_querytree;
		pvt->_querytree=NULL;
		return NULL;
	}
	pvt->_querytree[len]='\0';

	return pvt->_querytree;
}

const char *sqlrcursor::getTranslatedQuery() {

	delete[] pvt->_translatedquery;
	pvt->_translatedquery=NULL;

	pvt->_reexecute=false;
	pvt->_validatebinds=false;
	pvt->_resumed=false;
	clearVariables();

	if (!pvt->_endofresultset) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!pvt->_sqlrc->openSession()) {
		return NULL;
	}

	pvt->_cached=false;
	pvt->_endofresultset=false;

	// refresh socket client
	pvt->_cs=pvt->_sqlrc->cs();

	// tell the server we want to get a translated query
	pvt->_cs->write((uint16_t)GET_TRANSLATED_QUERY);

	// tell the server whether we'll need a cursor or not
	sendCursorStatus();

	pvt->_sqlrc->flushWriteBuffer();

	uint16_t	err=getErrorStatus();
	if (err!=NO_ERROR_OCCURRED) {
		if (err==TIMEOUT_GETTING_ERROR_STATUS) {
			pvt->_sqlrc->endSession();
			return NULL;
		}
		getErrorFromServer();
		if (err==ERROR_OCCURRED_DISCONNECT) {
			pvt->_sqlrc->endSession();
		}
		return NULL;
	}

	// get the size of the query
	uint64_t	len;
	if (pvt->_cs->read(&len)!=sizeof(uint64_t)) {
		return NULL;
	}

	// get the query itself
	pvt->_translatedquery=new char[len+1];
	if ((uint64_t)pvt->_cs->read(pvt->_translatedquery,len)!=len) {
		delete[] pvt->_translatedquery;
		pvt->_translatedquery=NULL;
		return NULL;
	}
	pvt->_translatedquery[len]='\0';

	return pvt->_translatedquery;
}

bool sqlrcursor::endofresultset() {
	return pvt->_endofresultset;
}

void sqlrcursor::sqlrc(sqlrconnection *sqlrc) {
	pvt->_sqlrc=sqlrc;
}

sqlrcursor *sqlrcursor::next() {
	return pvt->_next;
}

void sqlrcursor::havecursorid(bool havecursorid) {
	pvt->_havecursorid=havecursorid;
}

void sqlrcursor::clearBindsDuringPrepare() {
	pvt->_clearbindsduringprepare=true;
}

void sqlrcursor::dontClearBindsDuringPrepare() {
	pvt->_clearbindsduringprepare=false;
}
