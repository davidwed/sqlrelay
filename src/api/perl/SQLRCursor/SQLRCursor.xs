/* Copyright (c) 2001  David Muse
   See the file COPYING for more information */

#include <config.h>

#include "perlincludes.h"

#ifndef na
	#define na PL_na
#endif

typedef class sqlrcursor sqlrcursor;

MODULE = Firstworks::SQLRCursor		PACKAGE = Firstworks::SQLRCursor
REQUIRE: 1.925

sqlrcursor *
sqlrcursor::new(sqlrc)
		sqlrconnection *sqlrc
	CODE:
		RETVAL=new sqlrcursor(sqlrc);
		RETVAL->copyReferences();
	OUTPUT:
		RETVAL

void
sqlrcursor::DESTROY()

void
sqlrcursor::setResultSetBufferSize(rows)
		int rows

int
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
		char *filename

void
sqlrcursor::setCacheTtl(ttl)
		int ttl

char *
sqlrcursor::getCacheFileName()

void
sqlrcursor::cacheOff()

int
sqlrcursor::sendQuery(query)
		char *query

int
sqlrcursor::sendQueryWithLength(query,length)
		char *query
		int length
	CODE:
		RETVAL=THIS->sendQuery(query,length);
	OUTPUT:
		RETVAL

int
sqlrcursor::sendFileQuery(path,file)
		char *path
		char *file

void
sqlrcursor::prepareQuery(query)
		char *query

void
sqlrcursor::prepareQueryWithLength(query,length)
		char *query
		int length
	CODE:
		THIS->prepareQuery(query,length);
		

int
sqlrcursor::prepareFileQuery(path,file)
		char *path
		char *file

void
sqlrcursor::substitution(variable,...)
		char *variable
	CODE:
		if (SvIOK(ST(2))) {
			THIS->substitution(variable,(long)SvIV(ST(2)));
		} else if (SvNOK(ST(2))) {
			THIS->substitution(variable,(double)SvNV(ST(2)),
						(unsigned short)SvIV(ST(3)),
						(unsigned short)SvIV(ST(4)));
		} else if (SvPOK(ST(2))) {
			THIS->substitution(variable,SvPV(ST(2),na));
		} else {
			THIS->substitution(variable,(char *)NULL);
		}

void
sqlrcursor::clearBinds()

void
sqlrcursor::inputBind(variable,...)
		char *variable
	CODE:
		if (SvIOK(ST(2))) {
			THIS->inputBind(variable,(long)SvIV(ST(2)));
		} else if (SvNOK(ST(2))) {
			THIS->inputBind(variable,(double)SvNV(ST(2)),
						(unsigned short)SvIV(ST(3)),
						(unsigned short)SvIV(ST(4)));
		} else if (SvPOK(ST(2))) {
			THIS->inputBind(variable,SvPV(ST(2),na));
		} else {
			THIS->inputBind(variable,(char *)NULL);
		}

void
sqlrcursor::inputBindBlob(variable,value,size)
		char *variable
		char *value
		unsigned long size

void
sqlrcursor::inputBindClob(variable,value,size)
		char *variable
		char *value
		unsigned long size

void
sqlrcursor::validateBinds()

int
sqlrcursor::executeQuery()

int
sqlrcursor::fetchFromBindCursor()

void
sqlrcursor::defineOutputBind(variable,length)
		char *variable
		int length

void
sqlrcursor::defineOutputBindBlob(variable)
		char *variable

void
sqlrcursor::defineOutputBindClob(variable)
		char *variable

void
sqlrcursor::defineOutputBindCursor(variable)
		char *variable

char *
sqlrcursor::getOutputBind(variable)
		char *variable
	CODE:
		char	*value=THIS->getOutputBind(variable);
		long	length=THIS->getOutputBindLength(variable);
		ST(0)=sv_newmortal();
		if (value) {
			sv_setpvn(ST(0),value,length);
		} else {
			ST(0)=&PL_sv_undef;
		}

long
sqlrcursor::getOutputBindLength(variable)
		char *variable

sqlrcursor *
sqlrcursor::getOutputBindCursor(variable)
		char *variable
	PREINIT:
		char *	CLASS = "Firstworks::SQLRCursor";
	CODE:
		RETVAL=THIS->getOutputBindCursor(variable);
		RETVAL->copyReferences();
	OUTPUT:
		RETVAL

int
sqlrcursor::openCachedResultSet(filename)
	char	*filename

int
sqlrcursor::colCount()

int
sqlrcursor::rowCount()

int
sqlrcursor::totalRows()

int
sqlrcursor::affectedRows()

int
sqlrcursor::firstRowIndex()

int
sqlrcursor::endOfResultSet()

char *
sqlrcursor::errorMessage()

void
sqlrcursor::getNullsAsEmptyStrings()

void
sqlrcursor::getNullsAsUndefined()
	CODE:
		THIS->getNullsAsNulls();

int
sqlrcursor::validRow(row)
		int	row
	CODE:
		RETVAL=1;
		if (!THIS->getRow(row)) {
			RETVAL=0;
		}
	OUTPUT:
		RETVAL

char *
sqlrcursor::getField(row,...)
		int	row
	CODE:
		char	*field;
		long	length;
		ST(0)=sv_newmortal();
		if (SvIOK(ST(2)) || SvNOK(ST(2))) {
			field=THIS->getField(row,(int)SvIV(ST(2)));
			length=THIS->getFieldLength(row,(int)SvIV(ST(2)));
		} else if (SvPOK(ST(2))) {
			field=THIS->getField(row,SvPV(ST(2),na));
			length=THIS->getFieldLength(row,SvPV(ST(2),na));
		} 
		if (field) {
			sv_setpvn(ST(0),field,length);
		} else {
			ST(0)=&PL_sv_undef;
		}

long
sqlrcursor::getFieldLength(row,...)
		int	row
	CODE:
		RETVAL=0;
		if (SvIOK(ST(2)) || SvNOK(ST(2))) {
			RETVAL=THIS->getFieldLength(row,(int)SvIV(ST(2)));
		} else if (SvPOK(ST(2))) {
			RETVAL=THIS->getFieldLength(row,SvPV(ST(2),na));
		}
	OUTPUT:
		RETVAL

char **
sqlrcursor::getColumnNames()
	PPCODE:
		int	index=0;
		char	**namesptr=THIS->getColumnNames();
		EXTEND(SP,THIS->colCount());
		if (namesptr) {
			for (index=0; index<THIS->colCount(); index++) {
				PUSHs(sv_2mortal(newSVpv(namesptr[index],0)));
			}
		}

char *
sqlrcursor::getColumnName(col)
		int col

char *
sqlrcursor::getColumnType(...)
	CODE:
		RETVAL=NULL;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnType((int)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnType(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

int
sqlrcursor::getColumnLength(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getColumnLength((int)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getColumnLength(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

int
sqlrcursor::getLongest(...)
	CODE:
		RETVAL=0;
		if (SvIOK(ST(1)) || SvNOK(ST(1))) {
			RETVAL=THIS->getLongest((int)SvIV(ST(1)));
		} else if (SvPOK(ST(1))) {
			RETVAL=THIS->getLongest(SvPV(ST(1),na));
		}
	OUTPUT:
		RETVAL

int
sqlrcursor::getResultSetId()

void
sqlrcursor::suspendResultSet()

int
sqlrcursor::resumeResultSet(id)
		int id

int
sqlrcursor::resumeCachedResultSet(id,filename)
		int id
		char *filename
