// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information.

#ifndef DATATYPES_H
#define DATATYPES_H
#include <string.h>

extern "C" {

#define COLUMN_TYPE_IDS		0
#define COLUMN_TYPE_NAMES	1

typedef	enum {
	UNKNOWN_DATATYPE=0,
	// addded by freetds
	CHAR_DATATYPE,		// 1
	INT_DATATYPE,
	SMALLINT_DATATYPE,
	TINYINT_DATATYPE,
	MONEY_DATATYPE,
	DATETIME_DATATYPE,
	NUMERIC_DATATYPE,
	DECIMAL_DATATYPE,
	SMALLDATETIME_DATATYPE,
	SMALLMONEY_DATATYPE,
	IMAGE_DATATYPE,
	BINARY_DATATYPE,
	BIT_DATATYPE,
	REAL_DATATYPE,
	FLOAT_DATATYPE,
	TEXT_DATATYPE,
	VARCHAR_DATATYPE,
	VARBINARY_DATATYPE,
	LONGCHAR_DATATYPE,
	LONGBINARY_DATATYPE,
	LONG_DATATYPE,
	ILLEGAL_DATATYPE,
	SENSITIVITY_DATATYPE,
	BOUNDARY_DATATYPE,
	VOID_DATATYPE,
	USHORT_DATATYPE,
	// added by lago
	UNDEFINED_DATATYPE,	// 27
	DOUBLE_DATATYPE,
	DATE_DATATYPE,
	TIME_DATATYPE,
	TIMESTAMP_DATATYPE,
	// added by msql
	UINT_DATATYPE,		// 32
	LASTREAL_DATATYPE,
	// added by mysql
	STRING_DATATYPE,	// 34
	VARSTRING_DATATYPE,
	LONGLONG_DATATYPE,
	MEDIUMINT_DATATYPE,
	YEAR_DATATYPE,
	NEWDATE_DATATYPE,
	NULL_DATATYPE,
	ENUM_DATATYPE,
	SET_DATATYPE,
	TINY_BLOB_DATATYPE,
	MEDIUM_BLOB_DATATYPE,
	LONG_BLOB_DATATYPE,
	BLOB_DATATYPE,
	// added by oracle
	VARCHAR2_DATATYPE,	// 47
	NUMBER_DATATYPE,
	ROWID_DATATYPE,
	RAW_DATATYPE,
	LONG_RAW_DATATYPE,
	MLSLABEL_DATATYPE,
	CLOB_DATATYPE,
	BFILE_DATATYPE,
	// added by odbc
	BIGINT_DATATYPE,	// 55
	INTEGER_DATATYPE,
	LONGVARBINARY_DATATYPE,
	LONGVARCHAR_DATATYPE,
	// added by db2
	GRAPHIC_DATATYPE,	// 59
	VARGRAPHIC_DATATYPE,
	LONGVARGRAPHIC_DATATYPE,
	DBCLOB_DATATYPE,
	DATALINK_DATATYPE,
	USER_DEFINED_TYPE_DATATYPE,
	SHORT_DATATYPE,
	TINY_DATATYPE,
	// added by interbase
	D_FLOAT_DATATYPE,	// 67
	ARRAY_DATATYPE,
	QUAD_DATATYPE,
	INT64_DATATYPE,
	DOUBLE_PRECISION_DATATYPE
} datatype;

#ifdef NEED_DATATYPESTRING
static char	*datatypestring[] = {
	"UNKNOWN",
	// addded by freetds
	"CHAR",		// 1
	"INT",
	"SMALLINT",
	"TINYINT",
	"MONEY",
	"DATETIME",
	"NUMERIC",
	"DECIMAL",
	"SMALLDATETIME",
	"SMALLMONEY",
	"IMAGE",
	"BINARY",
	"BIT",
	"REAL",
	"FLOAT",
	"TEXT",
	"VARCHAR",
	"VARBINARY",
	"LONGCHAR",
	"LONGBINARY",
	"LONG",
	"ILLEGAL",
	"SENSITIVITY",
	"BOUNDARY",
	"VOID",
	"USHORT",
	// added by lago
	"UNDEFINED",	// 27
	"DOUBLE",
	"DATE",
	"TIME",
	"TIMESTAMP",
	// added by msql
	"UINT",		// 32
	"LASTREAL",
	// added by mysql
	"STRING",	// 34
	"VARSTRING",
	"LONGLONG",
	"MEDIUMINT",
	"YEAR",
	"NEWDATE",
	"NULL",
	"ENUM",
	"SET",
	"TINYBLOB",
	"MEDIUMBLOB",
	"LONGBLOB",
	"BLOB",
	// added by oracle
	"VARCHAR2",	// 47
	"NUMBER",
	"ROWID",
	"RAW",
	"LONG_RAW",
	"MLSLABEL",
	"CLOB",
	"BFILE",
	// added by odbc
	"BIGINT",	// 55
	"INTEGER",
	"LONGVARBINARY",
	"LONGVARCHAR",
	// added by db2
	"GRAPHIC",	// 59
	"VARGRAPHIC",
	"LONGVARGRAPHIC",
	"DBCLOB",
	"DATALINK",
	"USER_DEFINED_TYPE",
	"SHORT_DATATYPE",
	"TINY_DATATYPE",
	// added by interbase
	"D_FLOAT",	// 67
	"ARRAY",
	"QUAD",
	"INT64",
	"DOUBLE PRECISION"
	// none added by postgresql
	// none added by sqlite
};
#endif

#ifdef NEED_IS_NUMBER_TYPE_CHAR
static int isNumberTypeChar(const char *type) { 
	return (!strcmp(type,"NUMBER") || !strcmp(type,"INT") ||
		!strcmp(type,"SMALLINT") || !strcmp(type,"TINYINT") ||
		!strcmp(type,"NUMERIC") || !strcmp(type,"BIT") ||
		!strcmp(type,"REAL") || !strcmp(type,"FLOAT") ||
		!strcmp(type,"USHORT") || !strcmp(type,"DOUBLE") ||
		!strcmp(type,"UINT") || !strcmp(type,"LASTREAL") ||
		!strcmp(type,"TINY") || !strcmp(type,"SHORT") ||
		!strcmp(type,"LONGLONG") || !strcmp(type,"MEDIUMINT") ||
		!strcmp(type,"YEAR") || !strcmp(type,"BIGINT") ||
		!strcmp(type,"INTEGER") || !strcmp(type,"D_FLOAT") ||
		!strcmp(type,"DECIMAL") || !strcmp(type,"INT64") ||
		!strcmp(type,"MONEY") || !strcmp(type,"SMALLMONEY") ||
		!strcmp(type,"DOUBLE PRECISION") ||
		!strcmp(type,"int2") || !strcmp(type,"_int2") ||
		!strcmp(type,"int4") || !strcmp(type,"_int4") ||
		!strcmp(type,"int8") || !strcmp(type,"_int8") ||
		!strcmp(type,"oid") || !strcmp(type,"_oid") ||
		!strcmp(type,"tid") || !strcmp(type,"_tid") ||
		!strcmp(type,"xid") || !strcmp(type,"_xid") ||
		!strcmp(type,"cid") || !strcmp(type,"_cid") ||
		!strcmp(type,"float4") || !strcmp(type,"_float4") ||
		!strcmp(type,"float8") || !strcmp(type,"_float8"));
}
#endif

#ifdef NEED_IS_BLOB_TYPE_CHAR
static int isBlobTypeChar(const char *type) { 
	return (!strcmp(type,"IMAGE") || !strcmp(type,"BINARY") ||
		!strcmp(type,"VARBINARY") || !strcmp(type,"LONGCHAR") ||
		!strcmp(type,"LONGBINARY") || !strcmp(type,"LONG") ||
		!strcmp(type,"TINYBLOB") || !strcmp(type,"MEDIUMBLOB") ||
		!strcmp(type,"LONGBLOB") || !strcmp(type,"BLOB") ||
		!strcmp(type,"RAW") || !strcmp(type,"LONG_RAW") ||
		!strcmp(type,"CLOB") || !strcmp(type,"BFILE") ||
		!strcmp(type,"DBCLOB"));
}
#endif

#ifdef NEED_IS_UNSIGNED_TYPE_CHAR
static int isUnsignedTypeChar(const char *type) { 
	return (!strcmp(type,"USHORT") || !strcmp(type,"UINT"));
}
#endif

#ifdef NEED_IS_BINARY_TYPE_CHAR
static int isBinaryTypeChar(const char *type) { 
	return (!strcmp(type,"IMAGE") || !strcmp(type,"BINARY") ||
		!strcmp(type,"VARBINARY") || !strcmp(type,"LONGBINARY") ||
		!strcmp(type,"TINYBLOB") || !strcmp(type,"MEDIUMBLOB") ||
		!strcmp(type,"LONGBLOB") || !strcmp(type,"BLOB") ||
		!strcmp(type,"BFILE") || !strcmp(type,"LONGVARBINARY") ||
		!strcmp(type,"GRAPHIC") || !strcmp(type,"VARGRAPHIC") ||
		!strcmp(type,"LONGVARGRAPHIC"));
}
#endif

#ifdef NEED_IS_NUMBER_TYPE_INT
static int isNumberTypeInt(int type) {
	return (type==NUMBER_DATATYPE || type==INT_DATATYPE ||
		type==SMALLINT_DATATYPE || type==TINYINT_DATATYPE ||
		type==NUMERIC_DATATYPE || type==BIT_DATATYPE ||
		type==REAL_DATATYPE || type==FLOAT_DATATYPE ||
		type==USHORT_DATATYPE || type==DOUBLE_DATATYPE ||
		type==UINT_DATATYPE || type==LASTREAL_DATATYPE ||
		type==TINY_DATATYPE || type==SHORT_DATATYPE ||
		type==LONGLONG_DATATYPE || type==MEDIUMINT_DATATYPE ||
		type==YEAR_DATATYPE || type==BIGINT_DATATYPE ||
		type==INTEGER_DATATYPE || type==D_FLOAT_DATATYPE || 
		type==DECIMAL_DATATYPE || type==INT64_DATATYPE ||
		type==MONEY_DATATYPE || type==SMALLMONEY_DATATYPE ||
		type==DOUBLE_PRECISION_DATATYPE);
}
#endif

}

#endif
