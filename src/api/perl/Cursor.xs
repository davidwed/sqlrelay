/* Copyright (c) 1999-2018 David Muse
   See the file COPYING for more information */

#include <config.h>

// SCO OSR6 requires this
#ifdef SQLRELAY_HAVE_SYS_VNODE_H
	#include <sys/vnode.h>
#endif

#include "../c++/sqlrelay/sqlrclient.h"
#include <EXTERN.h>
#define explicit

#if defined(WIN32)

	// some versions of active perl erroneously try to
	// use __inline__ which isn't valid for MSVC
	#ifdef _MSC_VER
		#define __inline__ __inline
	#endif

	// msvc < 2013 (version 18.00) don't have stdbool.h
	// active perl 5.20 and up require either stdbool.h or this workaround
	#if _MSC_VER<1800
		#include <patchlevel.h>
		#if PERL_REVISION>5 || (PERL_REVISION==5 && PERL_VERSION>=20)
			#define PERL_BOOL_AS_CHAR
			#define __inline__ inline
		#endif
	#endif
#endif

#ifndef _SCO_DS
extern "C" {
#endif
	#include <perl.h>
#ifndef _SCO_DS
}
#endif

#include <XSUB.h>
#ifdef CLASS
	#undef CLASS
#endif

#ifdef THIS
	#undef THIS
#endif

#ifdef PERL500
        #ifndef SvUV
                #define SvUV SvIV
        #endif
        #ifndef sv_setuv
                #define sv_setuv sv_setiv
        #endif
	#ifndef PERLREALLYOLD
        	#undef sv_setpv
        	#define sv_setpv(a,b) Perl_sv_setpv(a,(char *)b)
        	#undef sv_setpvn
        	#define sv_setpvn(a,b,c) Perl_sv_setpvn(a,(char *)b,c)
	#else
		#define CLASS "SQLRelay::Cursor"
        #endif
#endif

#ifdef WIN32
	#undef XS_EXTERNAL
	#undef XS_INTERNAL
	#define XS_EXTERNAL(name) __declspec(dllexport) XSPROTO(name)
	#define XS_INTERNAL(name) STATIC XSPROTO(name)
#endif

#ifndef PERLREALLYOLD
	#ifndef na
		#define na PL_na
	#endif

	#ifndef sv_undef
		#define sv_undef PL_sv_undef
	#endif
#endif

/* xsubpp outputs __attribute__((noreturn)) this isn't
 * understood by gcc < 3.0. */
#ifdef __GNUC__
	#if __GNUC__ < 3
		#define __attribute__(x)
	#endif
#endif

#ifdef WIN32
	#undef XS_EXTERNAL
	#undef XS_INTERNAL
	#define XS_EXTERNAL(name) __declspec(dllexport) XSPROTO(name)
	#define XS_INTERNAL(name) STATIC XSPROTO(name)
#endif

typedef class sqlrcursor sqlrcursor;

MODULE = SQLRelay::Cursor		PACKAGE = SQLRelay::Cursor

sqlrcursor *
sqlrcursor::new(sqlrc)
		sqlrconnection *sqlrc
	CODE:
		RETVAL=new sqlrcursor(sqlrc,true);
	OUTPUT:
		RETVAL

void
sqlrcursor::DESTROY()

void
sqlrcursor::setResultSetBufferSize(rows)
		uint64_t rows

uint64_t
sqlrcursor::getResultSetBufferSize()

void
sqlrcursor::dontGetColumnInfo()

void
sqlrcursor::getColumnInfo()

void
sqlrcursor::mixedCaseColumnNames()

void
sqlrcursor::upperCaseColumnNames()

void
sqlrcursor::lowerCaseColumnNames()

void
sqlrcursor::cacheToFile(filename)
		const char *filename

void
sqlrcursor::setCacheTtl(ttl)
		uint32_t ttl

const char *
sqlrcursor::getCacheFileName()

void
sqlrcursor::cacheOff()

bool
sqlrcursor::getDatabaseList(wild)
		const char *wild

bool
sqlrcursor::getTableList(wild)
		const char *wild

bool
sqlrcursor::getColumnList(table,wild)
		const char *table
		const char *wild

bool
sqlrcursor::sendQuery(query)
		const char *query

bool
sqlrcursor::sendQueryWithLength(query,length)
		const char *query
		uint32_t length
	CODE:
		RETVAL=THIS->sendQuery(query,length);
	OUTPUT:
		RETVAL

bool
sqlrcursor::sendFileQuery(path,file)
		const char *path
		const char *file

void
sqlrcursor::prepareQuery(query)
		const char *query

void
sqlrcursor::prepareQueryWithLength(query,length)
		const char *query
		uint32_t length
	CODE:
		THIS->prepareQuery(query,length);
		

bool
sqlrcursor::prepareFileQuery(path,file)
		const char *path
		const char *file

bool
sqlrcursor::substitution(variable,...)
		const char *variable
	CODE:
		RETVAL=1;
		if (SvIOK(ST(2))) {
			THIS->substitution(variable,(int64_t)SvIV(ST(2)));
		} else if (SvNOK(ST(2))) {
			THIS->substitution(variable,(double)SvNV(ST(2)),
						(uint32_t)SvIV(ST(3)),
						(uint32_t)SvIV(ST(4)));
		} else if (SvPOK(ST(2))) {
			THIS->substitution(variable,SvPV(ST(2),na));
		} else if (!SvOK(ST(2))) {
			THIS->substitution(variable,(const char *)NULL);
		} else {
			RETVAL=0;
		}
	OUTPUT:
		RETVAL

void
sqlrcursor::clearBinds()

uint16_t
sqlrcursor::countBindVariables()

bool
sqlrcursor::inputBind(variable,...)
		const char *variable
	CODE:
		RETVAL=1;
		if (SvIOK(ST(2))) {
			THIS->inputBind(variable,(int64_t)SvIV(ST(2)));
		} else if (SvNOK(ST(2))) {
			THIS->inputBind(variable,(double)SvNV(ST(2)),
						(uint32_t)SvIV(ST(3)),
						(uint32_t)SvIV(ST(4)));
		} else if (SvPOK(ST(2))) {
			if (SvIOK(ST(3)) && SvIV(ST(3))>0) {
				THIS->inputBind(variable,SvPV(ST(2),na),
							(uint32_t)SvIV(ST(3)));
			} else {
				THIS->inputBind(variable,SvPV(ST(2),na));
			}
		} else if (!SvOK(ST(2))) {
			THIS->inputBind(variable,(const char *)NULL);
		} else {
			RETVAL=0;
		}
	OUTPUT:
		RETVAL

bool
sqlrcursor::inputBindBlob(variable,value,size)
		const char *variable
		const char *value
		uint32_t size
	CODE:
		RETVAL=0;
		if (SvPOK(ST(2))) {
			THIS->inputBindBlob(variable,value,size);
			RETVAL=1;
		} else if (!SvOK(ST(2))) {
			THIS->inputBindBlob(variable,NULL,0);
		}
	OUTPUT:
		RETVAL

bool
sqlrcursor::inputBindClob(variable,value,size)
		const char *variable
		const char *value
		uint32_t size
	CODE:
		RETVAL=0;
		if (SvPOK(ST(2))) {
			THIS->inputBindClob(variable,value,size);
			RETVAL=1;
		} else if (!SvOK(ST(2))) {
			THIS->inputBindClob(variable,NULL,0);
		}
	OUTPUT:
		RETVAL

void
sqlrcursor::validateBinds()

bool
sqlrcursor::validBind(variable)
	const char	*variable

bool
sqlrcursor::executeQuery()

bool
sqlrcursor::fetchFromBindCursor()

void
sqlrcursor::defineOutputBindString(variable,length)
		const char *variable
		uint32_t length

void
sqlrcursor::defineOutputBindInteger(variable)
		const char *variable

void
sqlrcursor::defineOutputBindDouble(variable)
		const char *variable

void
sqlrcursor::defineOutputBindBlob(variable)
		const char *variable

void
sqlrcursor::defineOutputBindClob(variable)
		const char *variable

void
sqlrcursor::defineOutputBindCursor(variable)
		const char *variable

const char *
sqlrcursor::getOutputBindString(variable)
		const char *variable
	CODE:
		const char	*value=THIS->getOutputBindString(variable);
		uint32_t	length=THIS->getOutputBindLength(variable);
		ST(0)=sv_newmortal();
		if (value) {
			sv_setpvn(ST(0),value,length);
		} else {
			ST(0)=&sv_undef;
		}

const char *
sqlrcursor::getOutputBindBlob(variable)
		const char *variable
	CODE:
		const char	*value=THIS->getOutputBindBlob(variable);
		uint32_t	length=THIS->getOutputBindLength(variable);
		ST(0)=sv_newmortal();
		if (value) {
			sv_setpvn(ST(0),value,length);
		} else {
			ST(0)=&sv_undef;
		}

const char *
sqlrcursor::getOutputBindClob(variable)
		const char *variable
	CODE:
		const char	*value=THIS->getOutputBindClob(variable);
		uint32_t	length=THIS->getOutputBindLength(variable);
		ST(0)=sv_newmortal();
		if (value) {
			sv_setpvn(ST(0),value,length);
		} else {
			ST(0)=&sv_undef;
		}

int64_t
sqlrcursor::getOutputBindInteger(variable)
		const char *variable
	CODE:
		int64_t	value=THIS->getOutputBindInteger(variable);
		ST(0)=sv_newmortal();
		sv_setiv(ST(0),value);

double
sqlrcursor::getOutputBindDouble(variable)
		const char *variable
	CODE:
		double	value=THIS->getOutputBindDouble(variable);
		ST(0)=sv_newmortal();
		sv_setnv(ST(0),value);

uint32_t
sqlrcursor::getOutputBindLength(variable)
		const char *variable

sqlrcursor *
sqlrcursor::getOutputBindCursor(variable)
		const char *variable
	CODE:
#ifndef PERLREALLYOLD
		char *	CLASS = "SQLRelay::Cursor";
#endif
		RETVAL=THIS->getOutputBindCursor(variable,true);
	OUTPUT:
		RETVAL

bool
sqlrcursor::openCachedResultSet(filename)
	const char	*filename

uint32_t
sqlrcursor::colCount()

uint64_t
sqlrcursor::rowCount()

uint64_t
sqlrcursor::totalRows()

uint64_t
sqlrcursor::affectedRows()

uint64_t
sqlrcursor::firstRowIndex()

bool
sqlrcursor::endOfResultSet()

const char *
sqlrcursor::errorMessage()

int64_t
sqlrcursor::errorNumber()

void
sqlrcursor::getNullsAsEmptyStrings()

void
sqlrcursor::getNullsAsUndefined()
	CODE:
		THIS->getNullsAsNulls();

bool
sqlrcursor::validRow(row)
		uint64_t	row
	CODE:
		RETVAL=1;
		if (!THIS->getRow(row)) {
			RETVAL=0;
		}
	OUTPUT:
		RETVAL

const char *
sqlrcursor::getField(row,...)
		uint64_t	row
	CODE:
		const char	*field=NULL;
		uint32_t	length=0;
		ST(0)=sv_newmortal();
		if (SvIOK(ST(2)) || SvNOK(ST(2))) {
			field=THIS->getField(row,(uint32_t)SvIV(ST(2)));
			length=THIS->getFieldLength(row,(uint32_t)SvIV(ST(2)));
		} else if (SvPOK(ST(2))) {
			field=THIS->getField(row,SvPV(ST(2),na));
			length=THIS->getFieldLength(row,SvPV(ST(2),na));
		} 
		if (field) {
			sv_setpvn(ST(0),field,length);
		} else {
			ST(0)=&sv_undef;
		}

int64_t
sqlrcursor::getFieldAsInteger(row,...)
		uint64_t	row
	CODE:
		int64_t	field=0;
		ST(0)=sv_newmortal();
		if (SvIOK(ST(2)) || SvNOK(ST(2))) {
			field=THIS->getFieldAsInteger(row,
						(uint32_t)SvIV(ST(2)));
		} else if (SvPOK(ST(2))) {
			field=THIS->getFieldAsInteger(row,SvPV(ST(2),na));
		} 
		sv_setiv(ST(0),field);

double
sqlrcursor::getFieldAsDouble(row,...)
		uint64_t	row
	CODE:
		double	field=0.0;
		ST(0)=sv_newmortal();
		if (SvIOK(ST(2)) || SvNOK(ST(2))) {
			field=THIS->getFieldAsDouble(row,(uint32_t)SvIV(ST(2)));
		} else if (SvPOK(ST(2))) {
			field=THIS->getFieldAsDouble(row,SvPV(ST(2),na));
		} 
		sv_setnv(ST(0),field);

uint32_t
sqlrcursor::getFieldLength(row,...)
		uint64_t	row
	CODE:
		RETVAL=0;
		if (SvIOK(ST(2)) || SvNOK(ST(2))) {
			RETVAL=THIS->getFieldLength(row,(uint32_t)SvIV(ST(2)));
		} else if (SvPOK(ST(2))) {
			RETVAL=THIS->getFieldLength(row,SvPV(ST(2),na));
		}
	OUTPUT:
		RETVAL

const char * const *
sqlrcursor::getColumnNames()
	PPCODE:
		uint32_t	index=0;
		const char * const *namesptr=THIS->getColumnNames();
		EXTEND(SP,THIS->colCount());
		if (namesptr) {
			for (index=0; index<THIS->colCount(); index++) {
				// On some platforms newSVpv takes a char *
				// argument rather than a const char *
				// argument.
				PUSHs(sv_2mortal(
					newSVpv((char *)namesptr[index],0)));
			}
		}

const char *
sqlrcursor::getColumnName(col)
		uint32_t col

const char *
sqlrcursor::getColumnType(...)
	CODE:
		RETVAL=NULL;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnType((uint32_t)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnType(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

uint32_t
sqlrcursor::getColumnLength(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnLength((uint32_t)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnLength(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

uint32_t
sqlrcursor::getColumnPrecision(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnPrecision((uint32_t)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnPrecision(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

uint32_t
sqlrcursor::getColumnScale(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnScale((uint32_t)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnScale(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

bool
sqlrcursor::getColumnIsNullable(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnIsNullable((uint32_t)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnIsNullable(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

bool
sqlrcursor::getColumnIsPrimaryKey(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnIsPrimaryKey((uint32_t)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnIsPrimaryKey(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

bool
sqlrcursor::getColumnIsUnique(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnIsUnique((uint32_t)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnIsUnique(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

bool
sqlrcursor::getColumnIsPartOfKey(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnIsPartOfKey((uint32_t)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnIsPartOfKey(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

bool
sqlrcursor::getColumnIsUnsigned(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnIsUnsigned((uint32_t)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnIsUnsigned(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

bool
sqlrcursor::getColumnIsZeroFilled(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnIsZeroFilled((uint32_t)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnIsZeroFilled(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

bool
sqlrcursor::getColumnIsBinary(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnIsBinary((uint32_t)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnIsBinary(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

bool
sqlrcursor::getColumnIsAutoIncrement(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnIsAutoIncrement((uint32_t)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnIsAutoIncrement(SvPV(ST(1),na));
		}
	OUTPUT:

uint32_t
sqlrcursor::getLongest(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getLongest((uint32_t)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getLongest(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

uint16_t
sqlrcursor::getResultSetId()

void
sqlrcursor::suspendResultSet()

bool
sqlrcursor::resumeResultSet(id)
		uint16_t id

bool
sqlrcursor::resumeCachedResultSet(id,filename)
		uint16_t id
		const char *filename
