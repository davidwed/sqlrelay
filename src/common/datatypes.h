// Copyright (c) 2000-2001  David Muse
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
	// added by interbase
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
	OPAQUE_DATATYPE
	// none added by sqlite
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
	"_LINE",
	"FLOAT4",
	"FLOAT8",
	"ABSTIME",
	"RELTIME",
	"TINTERVAL",
	"CIRCLE",
	"_CIRCLE",
	"_MONEY",
	"MACADDR",
	"INET",
	"CIDR",
	"_BOOL",
	"_BYTEA",
	"_CHAR",
	"_NAME",
	"_INT2",
	"_INT2VECTOR",
	"_INT4",
	"_REGPROC",
	"_TEXT",
	"_oid",
	"_TID",
	"_XID",
	"_CID",
	"_OIDVECTOR",
	"_BPCHAR",
	"_VARCHAR",
	"_INT8",
	"_POINT",
	"_LSEG",
	"_PATH",
	"_BOX",
	"_FLOAT4",
	"_FLOAT8",
	"_ABSTIME",
	"_RELTIME",
	"_TINTERVAL",
	"_POLYGON",
	"ACLITEM",
	"_ACLITEM",
	"_MACADDR",
	"_INET",
	"_CIDR",
	"BPCHAR",
	"_TIMESTAMP",
	"_DATE",
	"_TIME",
	"TIMESTAMPTZ",
	"_TIMESTAMPTZ",
	"INTERVAL",
	"_INTERVAL",
	"_NUMERIC",
	"TIMETZ",
	"_TIMETZ",
	"_BIT",
	"VARBIT",
	"_VARBIT",
	"REFCURSOR",
	"_REFCURSOR",
	"REGPROCEDURE",
	"REGOPER",
	"REGOPERATOR",
	"REGCLASS",
	"REGTYPE",
	"_REGPROCEDURE",
	"_REGOPER",
	"_REGOPERATOR",
	"_REGCLASS",
	"_REGTYPE",
	"RECORD",
	"CSTRING",
	"ANY",
	"ANYARRAY",
	"TRIGGER",
	"LANGUAGE_HANDLER",
	"INTERNAL",
	"OPAQUE",
	// none added by sqlite
	NULL
};
#endif

#ifdef NEED_IS_NUMBER_TYPE_CHAR
static bool isNumberTypeChar(const char *type) { 
	return (!charstring::compare(type,"NUMBER") ||
		!charstring::compare(type,"INT") ||
		!charstring::compare(type,"SMALLINT") ||
		!charstring::compare(type,"TINYINT") ||
		!charstring::compare(type,"NUMERIC") ||
		!charstring::compare(type,"BIT") ||
		!charstring::compare(type,"REAL") ||
		!charstring::compare(type,"FLOAT") ||
		!charstring::compare(type,"USHORT") ||
		!charstring::compare(type,"DOUBLE") ||
		!charstring::compare(type,"UINT") ||
		!charstring::compare(type,"LASTREAL") ||
		!charstring::compare(type,"TINY") ||
		!charstring::compare(type,"SHORT") ||
		!charstring::compare(type,"LONGLONG") ||
		!charstring::compare(type,"MEDIUMINT") ||
		!charstring::compare(type,"YEAR") ||
		!charstring::compare(type,"BIGINT") ||
		!charstring::compare(type,"INTEGER") ||
		!charstring::compare(type,"D_FLOAT") ||
		!charstring::compare(type,"DECIMAL") ||
		!charstring::compare(type,"INT64") ||
		!charstring::compare(type,"MONEY") ||
		!charstring::compare(type,"SMALLMONEY") ||
		!charstring::compare(type,"DOUBLE PRECISION") ||
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
		!charstring::compareIgnoringCase(type,"_NUMERIC"));
}
#endif

#ifdef NEED_IS_BLOB_TYPE_CHAR
static bool isBlobTypeChar(const char *type) { 
	return (!charstring::compare(type,"IMAGE") ||
		!charstring::compare(type,"BINARY") ||
		!charstring::compare(type,"VARBINARY") ||
		!charstring::compare(type,"LONGCHAR") ||
		!charstring::compare(type,"LONGBINARY") ||
		!charstring::compare(type,"LONG") ||
		!charstring::compare(type,"TINYBLOB") ||
		!charstring::compare(type,"MEDIUMBLOB") ||
		!charstring::compare(type,"LONGBLOB") ||
		!charstring::compare(type,"BLOB") ||
		!charstring::compare(type,"RAW") ||
		!charstring::compare(type,"LONG_RAW") ||
		!charstring::compare(type,"CLOB") ||
		!charstring::compare(type,"BFILE") ||
		!charstring::compare(type,"DBCLOB"));
}
#endif

#ifdef NEED_IS_UNSIGNED_TYPE_CHAR
static bool isUnsignedTypeChar(const char *type) { 
	return (!charstring::compare(type,"USHORT") ||
		!charstring::compare(type,"UINT"));
}
#endif

#ifdef NEED_IS_BINARY_TYPE_CHAR
static bool isBinaryTypeChar(const char *type) { 
	return (!charstring::compare(type,"IMAGE") ||
		!charstring::compare(type,"BINARY") ||
		!charstring::compare(type,"VARBINARY") ||
		!charstring::compare(type,"LONGBINARY") ||
		!charstring::compare(type,"TINYBLOB") ||
		!charstring::compare(type,"MEDIUMBLOB") ||
		!charstring::compare(type,"LONGBLOB") ||
		!charstring::compare(type,"BLOB") ||
		!charstring::compare(type,"BFILE") ||
		!charstring::compare(type,"LONGVARBINARY") ||
		!charstring::compare(type,"GRAPHIC") ||
		!charstring::compare(type,"VARGRAPHIC") ||
		!charstring::compare(type,"LONGVARGRAPHIC") ||
		!charstring::compare(type,"OID") ||
		!charstring::compare(type,"_OID") ||
		!charstring::compare(type,"OIDVECTOR") ||
		!charstring::compare(type,"_BYTEA"));
}
#endif

#ifdef NEED_IS_NUMBER_TYPE_INT
static bool isNumberTypeInt(int32_t type) {
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
		type==DOUBLE_PRECISION_DATATYPE || type==INT8_DATATYPE ||
		type==INT2_DATATYPE || type==INT4_DATATYPE ||
		type==TID_DATATYPE ||
		type==XID_DATATYPE || type==CID_DATATYPE ||
		type==FLOAT4_DATATYPE || type==FLOAT8_DATATYPE ||
		type==TINTERVAL_DATATYPE || type==_MONEY_DATATYPE ||
		type==_INT2_DATATYPE || type==_INT4_DATATYPE ||
		type==_TID_DATATYPE ||
		type==_XID_DATATYPE || type==_CID_DATATYPE ||
		type==_INT8_DATATYPE || type==_FLOAT4_DATATYPE ||
		type==_FLOAT8_DATATYPE || type==_TINTERVAL_DATATYPE ||
		type==INTERVAL_DATATYPE || type==_INTERVAL_DATATYPE ||
		type==_NUMERIC_DATATYPE);
}
#endif

}

#endif
