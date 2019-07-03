// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#ifndef DATATYPES_H
#define DATATYPES_H
#include <rudiments/charstring.h>

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
	// added by firebird
	D_FLOAT_DATATYPE,	// 67
	ARRAY_DATATYPE,
	QUAD_DATATYPE,
	INT64_DATATYPE,
	DOUBLE_PRECISION_DATATYPE,
	// added by postgresql
	BOOL_DATATYPE,
	BYTEA_DATATYPE,
	NAME_DATATYPE,
	INT8_DATATYPE,
	INT2_DATATYPE,
	INT2VECTOR_DATATYPE,
	INT4_DATATYPE,
	REGPROC_DATATYPE,
	OID_DATATYPE,
	TID_DATATYPE,
	XID_DATATYPE,
	CID_DATATYPE,
	OIDVECTOR_DATATYPE,
	SMGR_DATATYPE,
	POINT_DATATYPE,
	LSEG_DATATYPE,
	PATH_DATATYPE,
	BOX_DATATYPE,
	POLYGON_DATATYPE,
	LINE_DATATYPE,
	_LINE_DATATYPE,
	FLOAT4_DATATYPE,
	FLOAT8_DATATYPE,
	ABSTIME_DATATYPE,
	RELTIME_DATATYPE,
	TINTERVAL_DATATYPE,
	CIRCLE_DATATYPE,
	_CIRCLE_DATATYPE,
	_MONEY_DATATYPE,
	MACADDR_DATATYPE,
	INET_DATATYPE,
	CIDR_DATATYPE,
	_BOOL_DATATYPE,
	_BYTEA_DATATYPE,
	_CHAR_DATATYPE,
	_NAME_DATATYPE,
	_INT2_DATATYPE,
	_INT2VECTOR_DATATYPE,
	_INT4_DATATYPE,
	_REGPROC_DATATYPE,
	_TEXT_DATATYPE,
	_OID_DATATYPE,
	_TID_DATATYPE,
	_XID_DATATYPE,
	_CID_DATATYPE,
	_OIDVECTOR_DATATYPE,
	_BPCHAR_DATATYPE,
	_VARCHAR_DATATYPE,
	_INT8_DATATYPE,
	_POINT_DATATYPE,
	_LSEG_DATATYPE,
	_PATH_DATATYPE,
	_BOX_DATATYPE,
	_FLOAT4_DATATYPE,
	_FLOAT8_DATATYPE,
	_ABSTIME_DATATYPE,
	_RELTIME_DATATYPE,
	_TINTERVAL_DATATYPE,
	_POLYGON_DATATYPE,
	ACLITEM_DATATYPE,
	_ACLITEM_DATATYPE,
	_MACADDR_DATATYPE,
	_INET_DATATYPE,
	_CIDR_DATATYPE,
	BPCHAR_DATATYPE,
	_TIMESTAMP_DATATYPE,
	_DATE_DATATYPE,
	_TIME_DATATYPE,
	TIMESTAMPTZ_DATATYPE,
	_TIMESTAMPTZ_DATATYPE,
	INTERVAL_DATATYPE,
	_INTERVAL_DATATYPE,
	_NUMERIC_DATATYPE,
	TIMETZ_DATATYPE,
	_TIMETZ_DATATYPE,
	_BIT_DATATYPE,
	VARBIT_DATATYPE,
	_VARBIT_DATATYPE,
	REFCURSOR_DATATYPE,
	_REFCURSOR_DATATYPE,
	REGPROCEDURE_DATATYPE,
	REGOPER_DATATYPE,
	REGOPERATOR_DATATYPE,
	REGCLASS_DATATYPE,
	REGTYPE_DATATYPE,
	_REGPROCEDURE_DATATYPE,
	_REGOPER_DATATYPE,
	_REGOPERATOR_DATATYPE,
	_REGCLASS_DATATYPE,
	_REGTYPE_DATATYPE,
	RECORD_DATATYPE,
	CSTRING_DATATYPE,
	ANY_DATATYPE,
	ANYARRAY_DATATYPE,
	TRIGGER_DATATYPE,
	LANGUAGE_HANDLER_DATATYPE,
	INTERNAL_DATATYPE,
	OPAQUE_DATATYPE,
	ANYELEMENT_DATATYPE,
	PG_TYPE_DATATYPE,
	PG_ATTRIBUTE_DATATYPE,
	PG_PROC_DATATYPE,
	PG_CLASS_DATATYPE,
	// none added by sqlite
	// added by sqlserver
	UBIGINT_DATATYPE,
	UNIQUEIDENTIFIER_DATATYPE,
	// added by informix
	SMALLFLOAT_DATATYPE,
	BYTE_DATATYPE,
	BOOLEAN_DATATYPE,
	// also added by mysql
	TINYTEXT_DATATYPE,
	MEDIUMTEXT_DATATYPE,
	LONGTEXT_DATATYPE,
	JSON_DATATYPE,
	GEOMETRY_DATATYPE,
	// also added by oracle
	SDO_GEOMETRY_DATATYPE,
	// added by mssql
	NCHAR_DATATYPE,
	NVARCHAR_DATATYPE,
	NTEXT_DATATYPE,
	XML_DATATYPE,
	DATETIMEOFFSET_DATATYPE,
	END_DATATYPE
} datatype;

#ifdef NEED_DATATYPESTRING
static const char	*datatypestring[] = {
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
	"SHORT",
	"TINY",
	// added by firebird
	"D_FLOAT",	// 67
	"ARRAY",
	"QUAD",
	"INT64",
	"DOUBLE PRECISION",
	// added by postgresql
	"BOOL",
	"BYTEA",
	"NAME",
	"INT8",
	"INT2",
	"INT2VECTOR",
	"INT4",
	"REGPROC",
	"OID",
	"TID",
	"XID",
	"CID",
	"OIDVECTOR",
	"SMGR",
	"POINT",
	"LSEG",
	"PATH",
	"BOX",
	"POLYGON",
	"LINE",
	"LINE_ARRAY",
	"FLOAT4",
	"FLOAT8",
	"ABSTIME",
	"RELTIME",
	"TINTERVAL",
	"CIRCLE",
	"CIRCLE_ARRAY",
	"MONEY_ARRAY",
	"MACADDR",
	"INET",
	"CIDR",
	"BOOL_ARRAY",
	"BYTEA_ARRAY",
	"CHAR_ARRAY",
	"NAME_ARRAY",
	"INT2_ARRAY",
	"INT2VECTOR_ARRAY",
	"INT4_ARRAY",
	"REGPROC_ARRAY",
	"TEXT_ARRAY",
	"OID_ARRAY",
	"TID_ARRAY",
	"XID_ARRAY",
	"CID_ARRAY",
	"OIDVECTOR_ARRAY",
	"BPCHAR_ARRAY",
	"VARCHAR_ARRAY",
	"INT8_ARRAY",
	"POINT_ARRAY",
	"LSEG_ARRAY",
	"PATH_ARRAY",
	"BOX_ARRAY",
	"FLOAT4_ARRAY",
	"FLOAT8_ARRAY",
	"ABSTIME_ARRAY",
	"RELTIME_ARRAY",
	"TINTERVAL_ARRAY",
	"POLYGON_ARRAY",
	"ACLITEM",
	"ACLITEM_ARRAY",
	"MACADDR_ARRAY",
	"INET_ARRAY",
	"CIDR_ARRAY",
	"BPCHAR",
	"TIMESTAMP_ARRAY",
	"DATE_ARRAY",
	"TIME_ARRAY",
	"TIMESTAMPTZ",
	"TIMESTAMPTZ_ARRAY",
	"INTERVAL",
	"INTERVAL_ARRAY",
	"NUMERIC_ARRAY",
	"TIMETZ",
	"TIMETZ_ARRAY",
	"BIT_ARRAY",
	"VARBIT",
	"VARBIT_ARRAY",
	"REFCURSOR",
	"REFCURSOR_ARRAY",
	"REGPROCEDURE",
	"REGOPER",
	"REGOPERATOR",
	"REGCLASS",
	"REGTYPE",
	"REGPROCEDURE_ARRAY",
	"REGOPER_ARRAY",
	"REGOPERATOR_ARRAY",
	"REGCLASS_ARRAY",
	"REGTYPE_ARRAY",
	"RECORD",
	"CSTRING",
	"ANY",
	"ANYARRAY",
	"TRIGGER",
	"LANGUAGE_HANDLER",
	"INTERNAL",
	"OPAQUE",
	"ANYELEMENT",
	"PG_TYPE",
	"PG_ATTRIBUTE",
	"PG_PROC",
	"PG_CLASS",
	// none added by sqlite
	// added by sqlserver
	"UBIGINT",
	"UNIQUEIDENTIFIER",
	// added by informix
	"SMALLFLOAT",
	"BYTE",
	"BOOLEAN",
	// also added by mysql
	"TINYTEXT",
	"MEDIUMTEXT",
	"LONGTEXT",
	"JSON",
	"GEOMETRY",
	// also added by oracle
	"SDO_GEOMETRY",
	// added by mssql
	"NCHAR",
	"NVARCHAR",
	"NTEXT",
	"XML",
	"DATETIMEOFFSET",
	NULL
};
#endif

#ifdef NEED_IS_BIT_TYPE_CHAR
static bool isBitTypeChar(const char *type) {
	return (!charstring::compareIgnoringCase(type,"BIT") ||
		!charstring::compareIgnoringCase(type,"VARBIT"));
}
#endif

#ifdef NEED_IS_BIT_TYPE_INT
static bool isBitTypeInt(int16_t type) {
	return (type==BIT_DATATYPE ||
		type==VARBIT_DATATYPE);
}
#endif

#ifdef NEED_IS_BOOL_TYPE_CHAR
static bool isBoolTypeChar(const char *type) {
	return !charstring::compareIgnoringCase(type,"BOOL");
}
#endif

#ifdef NEED_IS_BOOL_TYPE_INT
static bool isBoolTypeInt(int16_t type) {
	return (type==BOOL_DATATYPE);
}
#endif

#ifdef NEED_IS_FLOAT_TYPE_CHAR
static bool isFloatTypeChar(const char *type) {
	return (!charstring::compareIgnoringCase(type,"NUMERIC") ||
		!charstring::compareIgnoringCase(type,"REAL") ||
		!charstring::compareIgnoringCase(type,"FLOAT") ||
		!charstring::compareIgnoringCase(type,"DOUBLE") ||
		!charstring::compareIgnoringCase(type,"D_FLOAT") ||
		!charstring::compareIgnoringCase(type,"DECIMAL") ||
		!charstring::compareIgnoringCase(type,"MONEY") ||
		!charstring::compareIgnoringCase(type,"SMALLMONEY") ||
		!charstring::compareIgnoringCase(type,"DOUBLE PRECISION") ||
		!charstring::compareIgnoringCase(type,"FLOAT4") ||
		!charstring::compareIgnoringCase(type,"FLOAT8") ||
		!charstring::compareIgnoringCase(type,"_NUMERIC"));
}
#endif

#ifdef NEED_IS_NONSCALE_FLOAT_TYPE_CHAR
static bool isNonScaleFloatTypeChar(const char *type) {
	return (!charstring::compareIgnoringCase(type,"REAL") ||
		!charstring::compareIgnoringCase(type,"FLOAT") ||
		!charstring::compareIgnoringCase(type,"DOUBLE") ||
		!charstring::compareIgnoringCase(type,"D_FLOAT") ||
		!charstring::compareIgnoringCase(type,"DOUBLE PRECISION") ||
		!charstring::compareIgnoringCase(type,"FLOAT4") ||
		!charstring::compareIgnoringCase(type,"FLOAT8"));
}
#endif

#ifdef NEED_IS_FLOAT_TYPE_INT
static bool isFloatTypeInt(int16_t type) {
	return (type==NUMERIC_DATATYPE ||
		type==REAL_DATATYPE ||
		type==FLOAT_DATATYPE ||
		type==DOUBLE_DATATYPE ||
		type==D_FLOAT_DATATYPE ||
		type==DECIMAL_DATATYPE ||
		type==MONEY_DATATYPE ||
		type==SMALLMONEY_DATATYPE ||
		type==DOUBLE_PRECISION_DATATYPE ||
		type==FLOAT4_DATATYPE ||
		type==FLOAT8_DATATYPE ||
		type==_NUMERIC_DATATYPE);
}
#endif

#ifdef NEED_BIT_STRING_TO_LONG
static int32_t bitStringToLong(const char *str) {
	uint32_t	result=0;
	size_t		length=charstring::length(str);
	for (size_t i=0; i<length; i++) {
		result=(result<<1)|(str[i]=='1');
	}
	return result;
}
#endif

#ifdef NEED_IS_NUMBER_TYPE_CHAR
static bool isNumberTypeChar(const char *type) {
	return (!charstring::compareIgnoringCase(type,"NUMBER") ||
		!charstring::compareIgnoringCase(type,"INT") ||
		!charstring::compareIgnoringCase(type,"SMALLINT") ||
		!charstring::compareIgnoringCase(type,"TINYINT") ||
		!charstring::compareIgnoringCase(type,"REAL") ||
		!charstring::compareIgnoringCase(type,"FLOAT") ||
		!charstring::compareIgnoringCase(type,"USHORT") ||
		!charstring::compareIgnoringCase(type,"DOUBLE") ||
		!charstring::compareIgnoringCase(type,"UINT") ||
		!charstring::compareIgnoringCase(type,"LASTREAL") ||
		!charstring::compareIgnoringCase(type,"TINY") ||
		!charstring::compareIgnoringCase(type,"SHORT") ||
		!charstring::compareIgnoringCase(type,"LONGLONG") ||
		!charstring::compareIgnoringCase(type,"MEDIUMINT") ||
		!charstring::compareIgnoringCase(type,"YEAR") ||
		!charstring::compareIgnoringCase(type,"BIGINT") ||
		!charstring::compareIgnoringCase(type,"INTEGER") ||
		!charstring::compareIgnoringCase(type,"D_FLOAT") ||
		!charstring::compareIgnoringCase(type,"DECIMAL") ||
		!charstring::compareIgnoringCase(type,"INT64") ||
		!charstring::compareIgnoringCase(type,"MONEY") ||
		!charstring::compareIgnoringCase(type,"SMALLMONEY") ||
		!charstring::compareIgnoringCase(type,"DOUBLE PRECISION") ||
		!charstring::compareIgnoringCase(type,"INT8") ||
		!charstring::compareIgnoringCase(type,"INT2") ||
		!charstring::compareIgnoringCase(type,"INT4") ||
		!charstring::compareIgnoringCase(type,"OID") ||
		!charstring::compareIgnoringCase(type,"TID") ||
		!charstring::compareIgnoringCase(type,"XID") ||
		!charstring::compareIgnoringCase(type,"CID") ||
		!charstring::compareIgnoringCase(type,"FLOAT4") ||
		!charstring::compareIgnoringCase(type,"FLOAT8") ||
		!charstring::compareIgnoringCase(type,"TINTERVAL") ||
		!charstring::compareIgnoringCase(type,"_MONEY") ||
		!charstring::compareIgnoringCase(type,"_INT2") ||
		!charstring::compareIgnoringCase(type,"_INT4") ||
		!charstring::compareIgnoringCase(type,"_oid") ||
		!charstring::compareIgnoringCase(type,"_TID") ||
		!charstring::compareIgnoringCase(type,"_XID") ||
		!charstring::compareIgnoringCase(type,"_CID") ||
		!charstring::compareIgnoringCase(type,"_INT8") ||
		!charstring::compareIgnoringCase(type,"_FLOAT4") ||
		!charstring::compareIgnoringCase(type,"_FLOAT8") ||
		!charstring::compareIgnoringCase(type,"_TINTERVAL") ||
		!charstring::compareIgnoringCase(type,"INTERVAL") ||
		!charstring::compareIgnoringCase(type,"_INTERVAL") ||
		!charstring::compareIgnoringCase(type,"NUMERIC"));
}
#endif

#ifdef NEED_IS_NUMBER_TYPE_INT
static bool isNumberTypeInt(int16_t type) {
	return (type==NUMBER_DATATYPE ||
		type==INT_DATATYPE ||
		type==SMALLINT_DATATYPE ||
		type==TINYINT_DATATYPE ||
		type==BIT_DATATYPE ||
		type==REAL_DATATYPE ||
		type==FLOAT_DATATYPE ||
		type==USHORT_DATATYPE ||
		type==DOUBLE_DATATYPE ||
		type==UINT_DATATYPE ||
		type==LASTREAL_DATATYPE ||
		type==TINY_DATATYPE ||
		type==SHORT_DATATYPE ||
		type==LONGLONG_DATATYPE ||
		type==MEDIUMINT_DATATYPE ||
		type==YEAR_DATATYPE ||
		type==BIGINT_DATATYPE ||
		type==INTEGER_DATATYPE ||
		type==D_FLOAT_DATATYPE || 
		type==DECIMAL_DATATYPE ||
		type==INT64_DATATYPE ||
		type==MONEY_DATATYPE ||
		type==SMALLMONEY_DATATYPE ||
		type==DOUBLE_PRECISION_DATATYPE ||
		type==INT8_DATATYPE ||
		type==INT2_DATATYPE ||
		type==INT4_DATATYPE ||
		type==TID_DATATYPE ||
		type==XID_DATATYPE ||
		type==CID_DATATYPE ||
		type==FLOAT4_DATATYPE ||
		type==FLOAT8_DATATYPE ||
		type==TINTERVAL_DATATYPE ||
		type==_MONEY_DATATYPE ||
		type==_INT2_DATATYPE ||
		type==_INT4_DATATYPE ||
		type==_TID_DATATYPE ||
		type==_XID_DATATYPE ||
		type==_CID_DATATYPE ||
		type==_INT8_DATATYPE ||
		type==_FLOAT4_DATATYPE ||
		type==_FLOAT8_DATATYPE ||
		type==_TINTERVAL_DATATYPE ||
		type==INTERVAL_DATATYPE ||
		type==_INTERVAL_DATATYPE ||
		type==NUMERIC_DATATYPE);
}
#endif

#ifdef NEED_IS_BLOB_TYPE_CHAR
static bool isBlobTypeChar(const char *type) { 
	return (!charstring::compareIgnoringCase(type,"IMAGE") ||
		!charstring::compareIgnoringCase(type,"BINARY") ||
		!charstring::compareIgnoringCase(type,"VARBINARY") ||
		!charstring::compareIgnoringCase(type,"LONGCHAR") ||
		!charstring::compareIgnoringCase(type,"LONGBINARY") ||
		!charstring::compareIgnoringCase(type,"LONG") ||
		!charstring::compareIgnoringCase(type,"TINYBLOB") ||
		!charstring::compareIgnoringCase(type,"MEDIUMBLOB") ||
		!charstring::compareIgnoringCase(type,"LONGBLOB") ||
		!charstring::compareIgnoringCase(type,"BLOB") ||
		!charstring::compareIgnoringCase(type,"LONGVARBINARY") ||
		!charstring::compareIgnoringCase(type,"LONGVARCHAR") ||
		!charstring::compareIgnoringCase(type,"RAW") ||
		!charstring::compareIgnoringCase(type,"LONG_RAW") ||
		!charstring::compareIgnoringCase(type,"CLOB") ||
		!charstring::compareIgnoringCase(type,"BFILE") ||
		!charstring::compareIgnoringCase(type,"DBCLOB") ||
		!charstring::compareIgnoringCase(type,"TINYTEXT") ||
		!charstring::compareIgnoringCase(type,"MEDIUMTEXT") ||
		!charstring::compareIgnoringCase(type,"LONGTEXT") ||
		!charstring::compareIgnoringCase(type,"JSON") ||
		!charstring::compareIgnoringCase(type,"GEOMETRY") ||
		!charstring::compareIgnoringCase(type,"SDO_GEOMETRY") ||
		!charstring::compareIgnoringCase(type,"NTEXT") ||
		!charstring::compareIgnoringCase(type,"XML") ||
		!charstring::compareIgnoringCase(type,"GRAPHIC") ||
		!charstring::compareIgnoringCase(type,"VARGRAPHIC") ||
		!charstring::compareIgnoringCase(type,"LONGVARGRAPHIC") ||
		!charstring::compareIgnoringCase(type,"DBCLOB"));
}
#endif

#ifdef NEED_IS_BLOB_TYPE_INT
static bool isBlobTypeInt(int16_t type) { 
	return (type==IMAGE_DATATYPE ||
		type==BINARY_DATATYPE ||
		type==VARBINARY_DATATYPE ||
		type==LONGCHAR_DATATYPE ||
		type==LONGBINARY_DATATYPE ||
		type==LONG_DATATYPE ||
		type==TINY_BLOB_DATATYPE ||
		type==MEDIUM_BLOB_DATATYPE ||
		type==LONG_BLOB_DATATYPE ||
		type==BLOB_DATATYPE ||
		type==RAW_DATATYPE ||
		type==LONG_RAW_DATATYPE ||
		type==CLOB_DATATYPE ||
		type==BFILE_DATATYPE ||
		type==DBCLOB_DATATYPE ||
		type==TINYTEXT_DATATYPE ||
		type==MEDIUMTEXT_DATATYPE ||
		type==LONGTEXT_DATATYPE ||
		type==JSON_DATATYPE ||
		type==GEOMETRY_DATATYPE ||
		type==SDO_GEOMETRY_DATATYPE);
}
#endif

#ifdef NEED_IS_UNSIGNED_TYPE_CHAR
static bool isUnsignedTypeChar(const char *type) { 
	return (!charstring::compareIgnoringCase(type,"USHORT") ||
		!charstring::compareIgnoringCase(type,"UINT")||
		!charstring::compareIgnoringCase(type,"YEAR") ||
		!charstring::compareIgnoringCase(type,"TIMESTAMP"));
}
#endif

#ifdef NEED_IS_UNSIGNED_TYPE_INT
static bool isUnsignedTypeInt(int16_t type) { 
	return (type==USHORT_DATATYPE ||
		type==UINT_DATATYPE ||
		type==YEAR_DATATYPE ||
		type==TIMESTAMP_DATATYPE);
}
#endif

#ifdef NEED_IS_BINARY_TYPE_CHAR
static bool isBinaryTypeChar(const char *type) { 
	return (!charstring::compareIgnoringCase(type,"IMAGE") ||
		!charstring::compareIgnoringCase(type,"BINARY") ||
		!charstring::compareIgnoringCase(type,"VARBINARY") ||
		!charstring::compareIgnoringCase(type,"LONGBINARY") ||
		!charstring::compareIgnoringCase(type,"TINYBLOB") ||
		!charstring::compareIgnoringCase(type,"MEDIUMBLOB") ||
		!charstring::compareIgnoringCase(type,"LONGBLOB") ||
		!charstring::compareIgnoringCase(type,"BLOB") ||
		!charstring::compareIgnoringCase(type,"BFILE") ||
		!charstring::compareIgnoringCase(type,"LONGVARBINARY") ||
		!charstring::compareIgnoringCase(type,"GRAPHIC") ||
		!charstring::compareIgnoringCase(type,"VARGRAPHIC") ||
		!charstring::compareIgnoringCase(type,"LONGVARGRAPHIC") ||
		!charstring::compareIgnoringCase(type,"OID") ||
		!charstring::compareIgnoringCase(type,"_OID") ||
		!charstring::compareIgnoringCase(type,"OIDVECTOR") ||
		!charstring::compareIgnoringCase(type,"_BYTEA") ||
		!charstring::compareIgnoringCase(type,"TIMESTAMP") ||
		!charstring::compareIgnoringCase(type,"DATE") ||
		!charstring::compareIgnoringCase(type,"TIME") ||
		!charstring::compareIgnoringCase(type,"DATETIME") ||
		!charstring::compareIgnoringCase(type,"NEWDATE"));
}
#endif

#ifdef NEED_IS_BINARY_TYPE_INT
static bool isBinaryTypeInt(int16_t type) { 
	return (type==IMAGE_DATATYPE ||
		type==BINARY_DATATYPE ||
		type==VARBINARY_DATATYPE ||
		type==LONGBINARY_DATATYPE ||
		type==TINY_BLOB_DATATYPE ||
		type==MEDIUM_BLOB_DATATYPE ||
		type==LONG_BLOB_DATATYPE ||
		type==BLOB_DATATYPE ||
		type==BFILE_DATATYPE ||
		type==LONGVARBINARY_DATATYPE ||
		type==GRAPHIC_DATATYPE ||
		type==VARGRAPHIC_DATATYPE ||
		type==LONGVARGRAPHIC_DATATYPE ||
		type==OID_DATATYPE ||
		type==_OID_DATATYPE ||
		type==OIDVECTOR_DATATYPE ||
		type==_BYTEA_DATATYPE ||
		type==TIMESTAMP_DATATYPE ||
		type==DATE_DATATYPE ||
		type==TIME_DATATYPE ||
		type==DATETIME_DATATYPE ||
		type==NEWDATE_DATATYPE);
}
#endif

#ifdef NEED_IS_DATETIME_TYPE_CHAR
static bool isDateTimeTypeChar(const char *type) {
	return (!charstring::compareIgnoringCase(type,"DATETIME") ||
		!charstring::compareIgnoringCase(type,"SMALLDATETIME") ||
		!charstring::compareIgnoringCase(type,"DATE") ||
		!charstring::compareIgnoringCase(type,"TIME") ||
		!charstring::compareIgnoringCase(type,"TIMESTAMP") ||
		!charstring::compareIgnoringCase(type,"NEWDATE") ||
		!charstring::compareIgnoringCase(type,"DATETIMEOFFSET"));
}
#endif

#ifdef NEED_IS_DATETIME_TYPE_INT
static bool isDateTimeTypeInt(int16_t type) {
	return (type==DATETIME_DATATYPE ||
		type==SMALLDATETIME_DATATYPE ||
		type==DATE_DATATYPE ||
		type==TIME_DATATYPE ||
		type==TIMESTAMP_DATATYPE ||
		type==NEWDATE_DATATYPE);
}
#endif

}

#endif
