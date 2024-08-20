// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/wcharstring.h>
#include <rudiments/process.h>
#include <rudiments/sys.h>
#include <rudiments/datetime.h>
#include <rudiments/error.h>


// TDS protocol definitions

// message types
#define SQL_BATCH			0x01
#define PRE_TDS7_LOGIN			0x02
#define RPC				0x03
#define TABULAR_RESULT			0x04
#define ATTENTION_SIGNAL		0x05
#define BULK_LOAD_DATA			0x07
#define FEDERATED_AUTHENTICATION_TOKEN	0x08
#define TRANSACTION_MANAGER_REQUEST	0x0E
#define TDS7_LOGIN			0x10
#define SSPI				0x11
#define PRE_LOGIN			0x12

// response tokens
#define TOKEN_LOGIN_ACK			0xAD
#define TOKEN_COLMETADATA		0x81
#define TOKEN_ROW			0xD1
#define TOKEN_ENV_CHANGE		0xE3
#define TOKEN_INFO			0xAB
#define TOKEN_ERROR			0xAA
#define TOKEN_DONE			0xFD
#define TOKEN_DONEPROC			0xFE
#define TOKEN_DONEINPROC		0xFF
#define TOKEN_RETURNSTATUS		0x79
#define TOKEN_RETURNVALUE		0xAC

// status bitmap
#define	STATUS_NORMAL			0x00
#define	STATUS_EOM			0x01
#define	STATUS_IGNORE			0x02
#define	STATUS_RESETCONNECTION		0x08
#define	STATUS_RESETCONNECTIONSKIPTRAN	0x10

// pre-login option token
#define	PL_VERSION			0x00
#define	PL_ENCRYPTION			0x01
#define	PL_INSTOPT			0x02
#define	PL_THREADID			0x03
#define	PL_MARS				0x04
#define	PL_TRACEID			0x05
#define	PL_FEDAUTHREQUIRED		0x06
#define	PL_NONCEOPT			0x07
#define	PL_TERMINATOR			0xFF

// encryption options
#define ENCRYPT_OFF	0x00
#define ENCRYPT_ON	0x01
#define ENCRYPT_NOT_SUP	0x02
#define ENCRYPT_REQ	0x03

// byte order
#define	ORDER_X86	0x00
#define	ORDER_68000	0x01

// character set
#define	CHARSET_ASCII	0x00
#define	CHARSET_EBDDIC	0x01

// floating point type
#define	FLOAT_IEEE_754	0x00
#define	FLOAT_VAX	0x01
#define	FLOAT_ND5000	0x02

// dump/load
#define DUMPLOAD_ON	0x00
#define DUMPLOAD_OFF	0x01

// warn when using db
#define	USE_DB_WARN_OFF	0x00
#define	USE_DB_WARN_ON	0x01

// use db flag
#define USE_DB_WARN	0x00
#define USE_DB_FATAL	0x01

// warn when setting language
#define	SET_LANG_WARN_OFF	0x00
#define	SET_LANG_WARN_ON	0x01

// set language flag
#define	SET_LANG_WARN	0x00
#define	SET_LANG_FATAL	0x01

// odbc
#define ODBC_OFF	0x00
#define ODBC_ON		0x01

// user type
#define	USER_NORMAL	0x00
#define	USER_SERVER	0x01
#define	USER_REMUSER	0x02
#define	USER_SQLREPL	0x03

// integrated security
#define	INTEGRATED_SECURITY_OFF	0x00
#define	INTEGRATED_SECURITY_ON	0x01

// sql type
#define SQL_DFLT	0x00
#define SQL_TSQL	0x01

// oledb
#define OLEDB_OFF	0x00
#define OLEDB_ON	0x01

// envchange types
#define ENV_CHANGE_DATABASE					1
#define ENV_CHANGE_LANGUAGE					2
#define ENV_CHANGE_CHARSET					3
#define ENV_CHANGE_PACKET_SIZE					4
#define ENV_CHANGE_UNICODE_DATA_SORTING_LOCAL_ID		5
#define ENV_CHANGE_UNICODE_DATA_SORTING_COMPARISON_FLAGS	6
#define ENV_CHANGE_SQL_COLLATION				7
#define ENV_CHANGE_BEGIN_TRANSACTION				8
#define ENV_CHANGE_COMMIT_TRANSACTION				9
#define ENV_CHANGE_ROLLBACK_TRANSACTION				10
#define ENV_CHANGE_ENLIST_DTC_TRANSACTION			11
#define ENV_CHANGE_DEFECT_TRANSACTION				12
#define ENV_CHANGE_REAL_TIME_LOG_SHIPPING			13
#define ENV_CHANGE_PROMOTE_TRANSACTION				15
#define ENV_CHANGE_TRANSACTION_MANAGER_ADDRESS			16
#define ENV_CHANGE_TRANSACTION_ENDED				17
#define ENV_CHANGE_RESETCONNECTION_COMPLETION_ACKNOWLEDGEMENT	18
#define ENV_GET_USER_INSTANCE					19
#define ENV_GET_ROUTING_INFORMATION				20

// done statuses
#define DONE_FINAL	0x0000
#define DONE_MORE	0x0001
#define DONE_ERROR	0x0002
#define DONE_INXACT	0x0004
#define DONE_COUNT	0x0010
#define DONE_ATTN	0x0020
#define DONE_RPCINBATCH	0x0080
#define DONE_SRVERROR	0x0100

// stream headers
#define ALL_HEADERS_QUERY_NOTIFICATIONS		0x0001
#define ALL_HEADERS_TRANSACTION_DESCRIPTOR	0x0002
#define ALL_HEADERS_TRACE_ACTIVITY		0x0003

// data types
#define TDS_TYPE_NULL			0x1F	// NULL
#define TDS_TYPE_INT1			0x30	// TinyInt
#define TDS_TYPE_BIT			0x32	// Bit
#define TDS_TYPE_INT2			0x34	// SmallInt
#define TDS_TYPE_INT4			0x38	// Int
#define TDS_TYPE_DATETIM4		0x3A	// SmallDateTime
#define TDS_TYPE_FLT4			0x3B	// Real
#define TDS_TYPE_MONEY			0x3C	// Money
#define TDS_TYPE_DATETIME		0x3D	// DateTime
#define TDS_TYPE_FLT8			0x3E	// Float
#define TDS_TYPE_MONEY4			0x7A	// SmallMoney
#define TDS_TYPE_INT8			0x7F	// BigInt
#define TDS_TYPE_GUID			0x24	// UniqueIdentifier
#define TDS_TYPE_INTN			0x26	// Int (variable length)
#define TDS_TYPE_DECIMAL		0x37	// Decimal (legacy support)
#define TDS_TYPE_NUMERIC		0x3F	// Numeric (legacy support)
#define TDS_TYPE_BITN			0x68	// Bit (variable length)
#define TDS_TYPE_DECIMALN		0x6A	// Decimal
#define TDS_TYPE_NUMERICN		0x6C	// Numeric
#define TDS_TYPE_FLTN			0x6D	// Float (variable length)
#define TDS_TYPE_MONEYN			0x6E	// Money (variable length)
#define TDS_TYPE_DATETIMN		0x6F	// DateTime (variable length)
#define TDS_TYPE_DATEN			0x28	// (introduced in TDS 7.3)
#define TDS_TYPE_TIMEN			0x29	// (introduced in TDS 7.3)
#define TDS_TYPE_DATETIME2N		0x2A	// (introduced in TDS 7.3)
#define TDS_TYPE_DATETIMEOFFSETN	0x2B	// (introduced in TDS 7.3)
#define TDS_TYPE_CHAR			0x2F	// Char (legacy support)
#define TDS_TYPE_VARCHAR		0x27	// VarChar (legacy support)
#define TDS_TYPE_BINARY			0x2D	// Binary (legacy support)
#define TDS_TYPE_VARBINARY		0x25	// VarBinary (legacy support)
#define TDS_TYPE_BIGVARBIN		0xA5	// VarBinary
#define TDS_TYPE_BIGVARCHR		0xA7	// VarChar
#define TDS_TYPE_BIGBINARY		0xAD	// Binary
#define TDS_TYPE_BIGCHAR		0xAF	// Char
#define TDS_TYPE_NVARCHAR		0xE7	// NVarChar
#define TDS_TYPE_NCHAR			0xEF	// NChar
#define TDS_TYPE_XML			0xF1	// XML
						// (introduced in TDS 7.2)
#define TDS_TYPE_UDT			0xF0	// CLR UDT
						// (introduced in TDS 7.2)
#define TDS_TYPE_TEXT			0x23	// Text
#define TDS_TYPE_IMAGE			0x22	// Image
#define TDS_TYPE_NTEXT			0x63	// NText
#define TDS_TYPE_SSVARIANT		0x62	// Sql_Variant
						// (introduced in TDS 7.2)
#define TDS_TYPE_TVP			0xF3	// Table Valued Parameter
						// (introduced in TDS 7.3)

static byte_t	tdstypemap[]={
	// "UNKNOWN"
	(byte_t)TDS_TYPE_NULL,
	// addded by freetds
	// "CHAR"
	(byte_t)TDS_TYPE_BIGCHAR,
	// "INT"
	(byte_t)TDS_TYPE_INTN,
	// "SMALLINT"
	(byte_t)TDS_TYPE_INTN,
	// "TINYINT"
	(byte_t)TDS_TYPE_INTN,
	// "MONEY"
	(byte_t)TDS_TYPE_MONEYN,
	// "DATETIME"
	(byte_t)TDS_TYPE_DATETIMN,
	// "NUMERIC"
	(byte_t)TDS_TYPE_NUMERICN,
	// "DECIMAL"
	(byte_t)TDS_TYPE_DECIMALN,
	// "SMALLDATETIME"
	(byte_t)TDS_TYPE_DATETIMN,
	// "SMALLMONEY"
	(byte_t)TDS_TYPE_MONEYN,
	// "IMAGE"
	(byte_t)TDS_TYPE_IMAGE,
	// "BINARY"
	(byte_t)TDS_TYPE_BIGBINARY,
	// "BIT"
	(byte_t)TDS_TYPE_BITN,
	// "REAL"
	(byte_t)TDS_TYPE_FLTN,
	// "FLOAT"
	(byte_t)TDS_TYPE_FLTN,
	// "TEXT"
	(byte_t)TDS_TYPE_TEXT,
	// "VARCHAR"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "VARBINARY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "LONGCHAR"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "LONGBINARY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "LONG"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "ILLEGAL"
	(byte_t)TDS_TYPE_NULL,
	// "SENSITIVITY"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "BOUNDARY"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "VOID"
	(byte_t)TDS_TYPE_NULL,
	// "USHORT"
	(byte_t)TDS_TYPE_INTN,
	// added by lago
	// "UNDEFINED"
	(byte_t)TDS_TYPE_NULL,
	// "DOUBLE"
	(byte_t)TDS_TYPE_FLTN,
	// "DATE"
	(byte_t)TDS_TYPE_DATEN,
	// "TIME"
	(byte_t)TDS_TYPE_TIMEN,
	// "TIMESTAMP"
	(byte_t)TDS_TYPE_DATETIME2N,
	// added by msql
	// "UINT"
	(byte_t)TDS_TYPE_INTN,
	// "LASTREAL"
	(byte_t)TDS_TYPE_DECIMALN,
	// added by mysql
	// "STRING"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "VARSTRING"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "LONGLONG"
	(byte_t)TDS_TYPE_INTN,
	// "MEDIUMINT"
	(byte_t)TDS_TYPE_INTN,
	// "YEAR"
	(byte_t)TDS_TYPE_INTN,
	// "NEWDATE"
	(byte_t)TDS_TYPE_DATEN,
	// "NULL"
	(byte_t)TDS_TYPE_NULL,
	// "ENUM"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "SET"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "TINYBLOB"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "MEDIUMBLOB"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "LONGBLOB"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "BLOB"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// added by oracle
	// "VARCHAR2"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "NUMBER"
	(byte_t)TDS_TYPE_DECIMALN,
	// "ROWID"
	(byte_t)TDS_TYPE_INTN,
	// "RAW"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "LONG_RAW"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "MLSLABEL"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "CLOB"
	(byte_t)TDS_TYPE_TEXT,
	// "BFILE"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// added by odbc
	// "BIGINT"
	(byte_t)TDS_TYPE_INTN,
	// "INTEGER"
	(byte_t)TDS_TYPE_INTN,
	// "LONGVARBINARY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "LONGVARCHAR"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// added by db2
	// "GRAPHIC"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "VARGRAPHIC"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "LONGVARGRAPHIC"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "DBCLOB"
	(byte_t)TDS_TYPE_TEXT,
	// "DATALINK"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "USER_DEFINED_TYPE"
	(byte_t)TDS_TYPE_UDT,
	// "SHORT_DATATYPE"
	(byte_t)TDS_TYPE_INTN,
	// "TINY_DATATYPE"
	(byte_t)TDS_TYPE_INTN,
	// added by firebird
	// "D_FLOAT"
	(byte_t)TDS_TYPE_INTN,
	// "ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "QUAD"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "INT64"
	(byte_t)TDS_TYPE_INTN,
	// "DOUBLE PRECISION"
	(byte_t)TDS_TYPE_INTN,
	// added by postgresql
	// "BOOL"
	(byte_t)TDS_TYPE_BITN,
	// "BYTEA"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "NAME"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "INT8"
	(byte_t)TDS_TYPE_INTN,
	// "INT2"
	(byte_t)TDS_TYPE_INTN,
	// "INT2VECTOR"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "INT4"
	(byte_t)TDS_TYPE_INTN,
	// "REGPROC"
	(byte_t)TDS_TYPE_INTN,
	// "OID"
	(byte_t)TDS_TYPE_INTN,
	// "TID"
	(byte_t)TDS_TYPE_INTN,
	// "XID"
	(byte_t)TDS_TYPE_INTN,
	// "CID"
	(byte_t)TDS_TYPE_INTN,
	// "OIDVECTOR"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "SMGR"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "POINT"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "LSEG"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "PATH"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "BOX"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "POLYGON"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "LINE"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "LINE_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "FLOAT4"
	(byte_t)TDS_TYPE_FLTN,
	// "FLOAT8"
	(byte_t)TDS_TYPE_FLTN,
	// "ABSTIME"
	(byte_t)TDS_TYPE_INTN,
	// "RELTIME"
	(byte_t)TDS_TYPE_INTN,
	// "TINTERVAL"
	(byte_t)TDS_TYPE_INTN,
	// "CIRCLE"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "CIRCLE_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "MONEY_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "MACADDR"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "INET"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "CIDR"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "BOOL_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "BYTEA_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "CHAR_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "NAME_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "INT2_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "INT2VECTOR_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "INT4_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "REGPROC_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "TEXT_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "OID_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "TID_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "XID_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "CID_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "OIDVECTOR_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "BPCHAR_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "VARCHAR_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "INT8_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "POINT_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "LSEG_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "PATH_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "BOX_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "FLOAT4_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "FLOAT8_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "ABSTIME_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "RELTIME_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "TINTERVAL_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "POLYGON_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "ACLITEM"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "ACLITEM_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "MACADDR_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "INET_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "CIDR_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "BPCHAR"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "TIMESTAMP_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "DATE_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "TIME_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "TIMESTAMPTZ"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "TIMESTAMPTZ_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "INTERVAL"
	(byte_t)TDS_TYPE_INTN,
	// "INTERVAL_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "NUMERIC_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "TIMETZ"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "TIMETZ_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "BIT_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "VARBIT"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "VARBIT_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "REFCURSOR"
	(byte_t)TDS_TYPE_INTN,
	// "REFCURSOR_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "REGPROCEDURE"
	(byte_t)TDS_TYPE_INTN,
	// "REGOPER"
	(byte_t)TDS_TYPE_INTN,
	// "REGOPERATOR"
	(byte_t)TDS_TYPE_INTN,
	// "REGCLASS"
	(byte_t)TDS_TYPE_INTN,
	// "REGTYPE"
	(byte_t)TDS_TYPE_INTN,
	// "REGPROCEDURE_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "REGOPER_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "REGOPERATOR_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "REGCLASS_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "REGTYPE_ARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "RECORD"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "CSTRING"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "ANY"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "ANYARRAY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "TRIGGER"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "LANGUAGE_HANDLER"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "INTERNAL"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "OPAQUE"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "ANYELEMENT"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "PG_TYPE"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "PG_ATTRIBUTE"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "PG_PROC"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// "PG_CLASS"
	(byte_t)TDS_TYPE_BIGVARCHR,
	// none added by sqlite
	// added by sqlserver
	// "UBIGINT"
	(byte_t)TDS_TYPE_INTN,
	// "UNIQUEIDENTIFIER"
	(byte_t)TDS_TYPE_GUID,
	// added by informix
	// "SMALLFLOAT"
	(byte_t)TDS_TYPE_FLTN,
	// "BYTE"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "BOOLEAN"
	(byte_t)TDS_TYPE_BITN,
	// "TINYTEXT"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "MEDIUMTEXT"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "LONGTEXT"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "JSON"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "GEOMETRY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "SDO_GEOMETRY"
	(byte_t)TDS_TYPE_BIGVARBIN,
	// "NCHAR"
	(byte_t)TDS_TYPE_NCHAR,
	// "NVARCHAR"
	(byte_t)TDS_TYPE_NVARCHAR,
	// "NTEXT"
	(byte_t)TDS_TYPE_NTEXT,
	// "XML"
	(byte_t)TDS_TYPE_XML,
	// "DATETIMEOFFSET"
	(byte_t)TDS_TYPE_DATETIMEOFFSETN
};

// rpc proc ids
#define	SP_CURSOR		1
#define	SP_CURSOR_OPEN		2
#define SP_CURSOR_PREPARE	3 
#define SP_CURSOR_EXECUTE	4 
#define SP_CURSOR_PREP_EXEC	5 
#define SP_CURSOR_UNPREPARE	6 
#define SP_CURSOR_FETCH		7 
#define SP_CURSOR_OPTION	8 
#define SP_CURSOR_CLOSE		9 
#define SP_EXECUTE_SQL		10 
#define SP_PREPARE		11 
#define SP_EXECUTE		12 
#define SP_PREP_EXEC		13 
#define SP_PREP_EXEC_RPC	14 
#define SP_UNPREPARE		15 

static const char *procids[]={
	"",
	"SP_CURSOR",
	"SP_CURSOR_OPEN",
	"SP_CURSOR_PREPARE",
	"SP_CURSOR_EXECUTE",
	"SP_CURSOR_PREP_EXEC",
	"SP_CURSOR_UNPREPARE",
	"SP_CURSOR_FETCH",
	"SP_CURSOR_OPTION",
	"SP_CURSOR_CLOSE",
	"SP_EXECUTE_SQL",
	"SP_PREPARE",
	"SP_EXECUTE",
	"SP_PREP_EXEC",
	"SP_PREP_EXEC_RPC",
	"SP_UNPREPARE"
};


// TDS protocol class
class SQLRSERVER_DLLSPEC sqlrprotocol_tds : public sqlrprotocol {
	public:
		sqlrprotocol_tds(sqlrservercontroller *cont,
							domnode *parameters);
		virtual	~sqlrprotocol_tds();

		clientsessionexitstatus_t	clientSession(
							filedescriptor *cs);
	private:
		void	init();
		void	free();
		void	reInit();

		bool	recvPacket(byte_t *packettype);
		bool	sendPacket();

		wchar_t	*readPassword(const byte_t *rp,
					size_t charcount);

		void		getServerTdsVersion();
		uint32_t	tdsVersionHexToDec(uint32_t tdsversion);
		uint32_t	tdsVersionDecToHex(uint32_t tdsversion,
								bool client);
		void		negotiateTdsVersion();

		bool	preLogin();

		bool	preTds7Login();

		bool	tds7Login();
		bool	auth(const wchar_t *username,
						size_t usernamelen,
						const wchar_t *password,
						size_t passwordlen);
		void	loginAck();
		void	authError(const wchar_t *username,
						size_t usernamelen);
		bool	changeDatabase(const wchar_t *database,
						size_t databaselen);
		void	changeDatabaseInfo(const wchar_t *database,
						size_t databaselen);
		void	changeDatabaseError(const wchar_t *database,
						size_t databaselen,
						bool warning);
		bool	changeCollation(uint32_t lcid);
		void	envChangeSqlCollation(uint32_t lcid,
						byte_t sortid);
		bool	changeLanguage(const wchar_t *language,
						size_t languagelen);
		void	changeLanguageInfo(const wchar_t *language,
						size_t languagelen);
		void	changeLanguageError(const wchar_t *language,
						size_t languagelen,
						bool warning);
		void	negotiatePacketSize(uint32_t packetsize);
		void	envChangePacketSize();

		bool	federatedAuthenticationToken();
		bool	attention();
		bool	transactionManagerRequest();
		bool	sspi();
		void	unimplementedFeatureError();

		bool	sqlBatch(sqlrservercursor *cursor);
		void	allHeaders(const byte_t *rp,
					size_t rpsize,
					const byte_t **rpout,
					size_t *rpsizeout);
		void	colMetaData(sqlrservercursor *cursor, bool nometadata);
		void	cekTable();
		byte_t	mapType(uint16_t type);
		void	colData(sqlrservercursor *cursor, uint16_t col);
		void	userType(byte_t tdstype);
		void	colFlags(sqlrservercursor *cursor,
					uint16_t col,
					byte_t tdstype);
		void	typeInfo(sqlrservercursor *cursor,
					uint16_t col,
					byte_t tdstype);
		void	tableName(byte_t tdstype);
		void	cryptoMetaData();
		void	colName(sqlrservercursor *cursor, uint16_t col);
		bool	isCaseSensitiveType(byte_t tdstype);
		bool	isFixedLenType(byte_t tdstype);
		bool	isVarLenType(byte_t tdstype);
		bool	isPartLenType(byte_t tdstype);
		uint64_t	rows(sqlrservercursor *cursor);
		void	lobData(byte_t tdstype);
		void	field(byte_t tdstype,
					uint32_t colsize,
					const char *field,
					uint64_t fieldsize,
					bool null);
		void	dateTime(const char *datetime,
					int32_t *dayssince1900,
					uint32_t *threehundredths);
		void	date(const char *datetime, uint16_t *dayssince1);
		void	daten(const char *field);
		void	timen(const char *field);
		void	decimal(const char *field,
					byte_t *ispositive,
					byte_t *size,
					byte_t *val);
		void	guid(const char *field, byte_t *g);
		byte_t	charsToHex(const char *chars);
		void	sqlBatchError(sqlrservercursor *cursor);

		bool	bulkLoad(sqlrservercursor *cursor);

		bool	remoteProcedureCall(sqlrservercursor *cursor);
		bool	params(sqlrservercursor *cursor,
					const byte_t *rp,
					size_t rpsize,
					const byte_t **rpout);
		bool	param(sqlrservercursor *cursor,
					uint16_t *inbindcount,
					uint16_t *outbindcount,
					sqlrserverbindvar *inbinds,
					sqlrserverbindvar *outbinds,
					const byte_t *rp,
					const byte_t **rpout,
					bool exceededinbind,
					bool exceededoutbind);
		void	tdsProtocolError();

		void	envChange(byte_t type,
					const wchar_t *newvalue,
					size_t newvaluesize,
					const wchar_t *oldvalue,
					size_t oldvaluesize);
		void	info(uint32_t number,
					byte_t state,
					byte_t infoclass,
					const char *msgtext,
					const char *srvname,
					const char *procname,
					uint32_t linenumber);
		void	error(uint32_t number,
					byte_t state,
					byte_t errclass,
					const char *msgtext,
					const char *srvname,
					const char *procname,
					uint32_t linenumber);
		void	infoOrError(byte_t token,
					uint32_t number,
					byte_t state,
					byte_t infoerrclass,
					const char *msgtext,
					const char *srvname,
					const char *procname,
					uint32_t linenumber);
		void	done();
		void	done(uint16_t status,
					uint16_t curcmd,
					uint64_t donerowcount);
		void	done(byte_t token,
					uint16_t status,
					uint16_t curcmd,
					uint64_t donerowcount);
		void	doneInProc(uint16_t status,
					uint16_t curcmd,
					uint64_t donerowcount);
		void	returnStatus(sqlrservercursor *cursor);
		void	returnValues(sqlrservercursor *cursor);
		void	returnValue(sqlrservercursor *cursor,
					uint16_t param);
		void	doneProc(uint16_t status,
					uint16_t curcmd,
					uint64_t donerowcount);

		void	debugSystemError();

		filedescriptor	*clientsock;
		const char	*srvname;

		uint32_t	maxquerysize;
		uint16_t	maxbindcount;

		char		**bindvarnames;

		byte_t		packetid;

		bytebuffer	reqpacket;
		bytebuffer	resppacket;

		const char	*dbversion;
		uint32_t	servertdsversion;
		uint32_t	clienttdsversion;
		uint32_t	negotiatedtdsversion;

		uint32_t	oldpacketsize;
		uint32_t	negotiatedpacketsize;

		bool		dbistds;
};

sqlrprotocol_tds::sqlrprotocol_tds(sqlrservercontroller *cont,
					domnode *parameters) :
					sqlrprotocol(cont,parameters) {

	clientsock=NULL;

	debugStart("parameters");
	debugEnd();

	srvname=cont->getDbHostName();

	dbversion=cont->getDbVersion();
	getServerTdsVersion();

	const char	*dbtype=cont->getDbType();
	dbistds=(!charstring::compare(dbtype,"freetds") ||
			!charstring::compare(dbtype,"sap"));

	maxquerysize=cont->getConfig()->getMaxQuerySize();
	maxbindcount=cont->getConfig()->getMaxBindCount();

	bindvarnames=new char *[maxbindcount];
	for (uint16_t i=0; i<maxbindcount; i++) {
		charstring::printf(&bindvarnames[i],"@%d",i+1);
	}

	init();
}

sqlrprotocol_tds::~sqlrprotocol_tds() {
	free();

	for (uint16_t i=0; i<maxbindcount; i++) {
		delete[] bindvarnames[i];
	}
}

void sqlrprotocol_tds::init() {

	packetid=0;

	clienttdsversion=700;
	negotiatedtdsversion=700;

	oldpacketsize=4096;
	negotiatedpacketsize=4096;
}

void sqlrprotocol_tds::free() {
}

void sqlrprotocol_tds::reInit() {
	free();
	init();
}

bool sqlrprotocol_tds::recvPacket(byte_t *packettype) {

	// clear the receive buffer
	reqpacket.clear();

	byte_t		packetstatus=0;
	uint16_t	packetsize=0;
	uint16_t	spid=0;
	byte_t		packetwindow=0;

	do {

		// get the packet type
		if (clientsock->read(packettype)!=sizeof(*packettype)) {
			debugWrite("read packet type failed");
			debugSystemError();
			return false;
		}

		// get the packet status
		if (clientsock->read(&packetstatus)!=sizeof(packetstatus)) {
			debugWrite("read packet status failed");
			debugSystemError();
			return false;
		}

		// get the packet size
		if (clientsock->read(&packetsize)!=sizeof(packetsize)) {
			debugWrite("read packet size failed");
			debugSystemError();
			return false;
		}

		// get the spid
		if (clientsock->read(&spid)!=sizeof(spid)) {
			debugWrite("read spid failed");
			debugSystemError();
			return false;
		}

		// get the packet id
		if (clientsock->read(&packetid)!=sizeof(packetid)) {
			debugWrite("read packet id failed");
			debugSystemError();
			return false;
		}

		// get the packet window
		if (clientsock->read(&packetwindow)!=sizeof(packetwindow)) {
			debugWrite("read packet window failed");
			debugSystemError();
			return false;
		}

		// sanity checks
		if (*packettype!=SQL_BATCH &&
			*packettype!=PRE_TDS7_LOGIN &&
			*packettype!=RPC &&
			*packettype!=TABULAR_RESULT &&
			*packettype!=ATTENTION_SIGNAL &&
			*packettype!=BULK_LOAD_DATA &&
			*packettype!=FEDERATED_AUTHENTICATION_TOKEN &&
			*packettype!=TRANSACTION_MANAGER_REQUEST &&
			*packettype!=TDS7_LOGIN &&
			*packettype!=SSPI &&
			*packettype!=PRE_LOGIN) {
			debugWrite("invalid packet type: 0x%02x",*packettype);
			debugSystemError();
			return false;
		}
		if (packetsize<8) {
			debugWrite("invalid packet size: %d",packetsize);
			debugSystemError();
			return false;
		}

		// bump the packet size down
		packetsize-=8;

		// get the packet data
		byte_t	*packet=new byte_t[packetsize];
		if (clientsock->read(packet,packetsize)!=packetsize) {
			debugWrite("read packet failed");
			debugSystemError();
			return false;
		}

		// append the data to the receive buffer
		reqpacket.append(packet,packetsize);

		// bump the packet size back up
		packetsize+=8;

		debugStart("recv");
		debugWrite("packet type: 0x%02x",*packettype);
		debugWrite("packet status: 0x%02x",packetstatus);
		debugWrite("packet size: %d",packetsize);
		debugWrite("spid: %d",spid);
		debugWrite("packet id: %d",packetid);
		debugWrite("packet window: %d",packetwindow);
		debugHexDump(packet,packetsize-8);
		debugEnd();

	} while (!(packetstatus&STATUS_EOM));

	// bump sequence
	packetid=(packetid+1)%256;

	return true;
}

bool sqlrprotocol_tds::sendPacket() {

	const byte_t	*packet=resppacket.getBuffer();
	uint64_t	remaining=resppacket.getSize();

	do {

		// set header parts
		byte_t		packettype=TABULAR_RESULT;
		byte_t		packetstatus=0;
		uint16_t	packetsize=remaining;
		if (packetsize>negotiatedpacketsize) {
			packetsize=negotiatedpacketsize;
		}
		remaining-=packetsize;
		if (!remaining) {
			packetstatus|=STATUS_EOM;
		}
		packetsize+=8;
		uint16_t	spid=0;
		byte_t		packetwindow=0;

		debugStart("send");
		debugWrite("packet type: 0x%02x",packettype);
		debugWrite("packet status: 0x%02x",packetstatus);
		debugWrite("packet size: %d",packetsize);
		debugWrite("spid: %d",spid);
		debugWrite("packet id: %d",packetid);
		debugWrite("packet window: %d",packetwindow);
		debugHexDump(packet,packetsize-8);
		debugEnd();

		// send the packet type
		if (clientsock->write(packettype)!=sizeof(packettype)) {
			debugWrite("write packet type failed");
			debugSystemError();
			return false;
		}

		// send the packet status
		if (clientsock->write(packetstatus)!=sizeof(packetstatus)) {
			debugWrite("write packet status failed");
			debugSystemError();
			return false;
		}

		// send the packet size
		if (clientsock->write(packetsize)!=sizeof(packetsize)) {
			debugWrite("write packet size failed");
			debugSystemError();
			return false;
		}

		// send the spid
		if (clientsock->write(spid)!=sizeof(spid)) {
			debugWrite("write spid failed");
			debugSystemError();
			return false;
		}

		// send the packet id
		if (clientsock->write(packetid)!=sizeof(packetid)) {
			debugWrite("write packet id failed");
			debugSystemError();
			return false;
		}

		// send the packet window
		if (clientsock->write(packetwindow)!=sizeof(packetwindow)) {
			debugWrite("write packet window failed");
			debugSystemError();
			return false;
		}

		// bump the packet size down
		packetsize-=8;

		// send the packet data
		if (clientsock->write(packet,packetsize)!=packetsize) {
			debugWrite("write packet data failed");
			debugSystemError();
			return false;
		}

		if (!clientsock->flushWriteBuffer(-1,-1)) {
			debugWrite("flush write buffer failed");
			debugSystemError();
			return false;
		}

		// bump sequence
		packetid=(packetid+1)%256;

	} while (remaining);

	return true;
}

wchar_t *sqlrprotocol_tds::readPassword(const byte_t *rp,
						size_t charcount) {
	uint16_t	size=charcount*sizeof(uint16_t);
	byte_t		*temp=(byte_t *)bytestring::duplicate(rp,size);
	byte_t		*ch=temp;
	for (uint16_t i=0; i<size; i++) {
		*ch=*ch^0xA5;
		*ch=((*ch&0x0F)<<4)|((*ch&0xF0)>>4);
		ch++;
	}
	wchar_t	*password=wcharstring::duplicateUcs2((const ucs2_t *)temp,
								charcount);
	delete[] temp;
	return password;
}

void sqlrprotocol_tds::getServerTdsVersion() {

	servertdsversion=420;

	// versions reported by FreeTDS...
	if (charstring::contains(dbversion,"SQL Server 2016") ||
			charstring::contains(dbversion,"SQL Server 2014") ||
			charstring::contains(dbversion,"SQL Server 2012")) {
		servertdsversion=740;
	} else if (charstring::contains(dbversion,"SQL Server 2008 R2")) {
		servertdsversion=731;
	} else if (charstring::contains(dbversion,"SQL Server 2008")) {
		servertdsversion=730;
	} else if (charstring::contains(dbversion,"SQL Server 2005")) {
		servertdsversion=720;
	} else if (charstring::contains(dbversion,"SQL Server 2000 SP1")) {
		servertdsversion=711;
	} else if (charstring::contains(dbversion,"SQL Server 2000")) {
		servertdsversion=710;
	} else if (charstring::contains(dbversion,"SQL Server 7.0")) {
		servertdsversion=700;
	} else if (charstring::contains(dbversion,"Adaptive Server") ||
			charstring::contains(dbversion,"SQL Anywhere")) {
		servertdsversion=500;
	} else if (charstring::contains(dbversion,"SQL Server 6.")) {
		servertdsversion=420;
	}

	// versions reported by ODBC...
	// FIXME: other versions...
	if (charstring::contains(dbversion,"12.00.2000")) {
		servertdsversion=740;
	}

	// FIXME: what if the backend isn't MSSQL?

	debugStart("server tds version");
	debugWrite("dbversion:\n %s",dbversion);
	debugWrite("servertdsversion: %d",servertdsversion);
	debugEnd();
}

uint32_t sqlrprotocol_tds::tdsVersionHexToDec(uint32_t tdsversion) {
	switch (tdsversion) {
		case 0x00000042:
		case 0x42000000:
			// Sybase < 10
			// SQL Server 6.x
			return 420;
		case 0x00000050:
		case 0x05000000:
			// Sybase 10+
			// Sybase SQL Anywhere (all versions)
			return 500;
		case 0x00000070:
		case 0x07000000:
			// SQL Server 7.0
			return 700;
		case 0x00000071:
		case 0x07010000:
			// SQL Server 2000
			return 710;
		case 0x01000071:
		case 0x71000001:
			// SQL Server 2000 SP1
			return 711;
		case 0x02000972:
		case 0x72090002:
			// SQL Server 2005
			return 720;
		case 0x03000A73:
		case 0x730A0003:
			// SQL Server 2008
			return 730;
		case 0x03000B73:
		case 0x730B0003:
			// SQL Server 2008 R2
			return 731;
		case 0x04000074:
		case 0x74000004:
			// SQL Server 2012, 2014, 2106
			return 740;
		default:
			return 700;
	}
}

uint32_t sqlrprotocol_tds::tdsVersionDecToHex(uint32_t tdsversion,
							bool toclient) {
	if (toclient) {
		switch (tdsversion) {
			case 420:
				// Sybase < 10
				// SQL Server 6.x
				return 0x42000000;
			case 500:
				// Sybase 10+
				// Sybase SQL Anywhere (all versions)
				return 0x05000000;
			case 700:
				// SQL Server 7.0
				return 0x07000000;
			case 710:
				// SQL Server 2000
				return 0x07010000;
			case 711:
				// SQL Server 2000 SP1
				return 0x71000001;
			case 720:
				// SQL Server 2005
				return 0x72090002;
			case 730:
				// SQL Server 2008
				return 0x730A0003;
			case 731:
				// SQL Server 2008 R2
				return 0x730B0003;
			case 740:
				// SQL Server 2012, 2014, 2106
				return 0x74000004;
			default:
				return 0x07000000;
		}
	} else {
		switch (tdsversion) {
			case 420:
				// Sybase < 10
				// SQL Server 6.x
				return 0x00000042;
			case 500:
				// Sybase 10+
				// Sybase SQL Anywhere (all versions)
				return 0x00000050;
			case 700:
				// SQL Server 7.0
				return 0x00000070;
			case 710:
				// SQL Server 2000
				return 0x00000071;
			case 711:
				// SQL Server 2000 SP1
				return 0x01000071;
			case 720:
				// SQL Server 2005
				return 0x02000972;
			case 730:
				// SQL Server 2008
				return 0x03000A73;
			case 731:
				// SQL Server 2008 R2
				return 0x03000B73;
			case 740:
				// SQL Server 2012, 2014, 2106
				return 0x04000074;
			default:
				return 0x00000070;
		}
	}
}

void sqlrprotocol_tds::negotiateTdsVersion() {

	negotiatedtdsversion=
		(clienttdsversion<servertdsversion)?
				clienttdsversion:servertdsversion;

	debugStart("negotiate tds version");
	debugWrite("client: %d",clienttdsversion);
	debugWrite("server: %d",servertdsversion);
	debugWrite("negotiated: %d",negotiatedtdsversion);
	debugEnd();
}

clientsessionexitstatus_t sqlrprotocol_tds::clientSession(
							filedescriptor *cs) {

	clientsock=cs;

	// set up the socket
	clientsock->setTranslateByteOrder(true);
	clientsock->setNaglesAlgorithmEnabled(false);
	//clientsock->setSocketReadBufferSize(65536);
	//clientsock->setSocketWriteBufferSize(65536);
	clientsock->setReadBufferSize(65536);
	clientsock->setWriteBufferSize(65536);

	reInit();

	bool	endsession=true;

	clientsessionexitstatus_t	status=CLIENTSESSIONEXITSTATUS_ERROR;

	// loop, getting and executing requests
	bool	loop=true;
	do {

		// get the request...
		byte_t	packettype;
		if (!recvPacket(&packettype)) {
			break;
		}

		// some requests don't need a cursor or will request a
		// specific cursor internally...
		switch (packettype) {
			case PRE_LOGIN:
				loop=preLogin();
				break;
			case PRE_TDS7_LOGIN:
				loop=preTds7Login();
				break;
			case TDS7_LOGIN:
				loop=tds7Login();
				break;
			case FEDERATED_AUTHENTICATION_TOKEN:
				loop=federatedAuthenticationToken();
				break;
			case ATTENTION_SIGNAL:
				loop=attention();
				break;
			case TRANSACTION_MANAGER_REQUEST:
				loop=transactionManagerRequest();
				break;
			case SSPI:
				loop=sspi();
				break;
		}
		if (!loop) {
			break;
		}

		// for the rest of the requests, we need a new cursor...
		sqlrservercursor	*cursor=cont->getCursor();
		if (!cursor) {
			// FIXME: send some kind of error
			//if (!noCursorAvailableError()) {
				break;
			//}
			continue;
		}
		switch (packettype) {
			case SQL_BATCH:
				loop=sqlBatch(cursor);
				break;
			case BULK_LOAD_DATA:
				loop=bulkLoad(cursor);
				break;
			case RPC:
				loop=remoteProcedureCall(cursor);
				break;
		}
		// release the cursor
		// FIXME: kludgy
		cont->release(cursor);

	} while (loop);

	// close the client connection
	cont->closeClientConnection(0);

	// end the session if necessary
	if (endsession) {
		cont->endSession();
	}

	return status;
}

bool sqlrprotocol_tds::preLogin() {

	uint32_t	version=0;
	uint16_t	subbuild=0;
	byte_t		encryption=0;
	char		*instvalidity=NULL;
	uint32_t	threadid=0;
	byte_t		mars=0;
	byte_t		connid[16];
	byte_t		activityid[20];
	byte_t		fedauthrequired=0;
	byte_t		nonce[32];

	connid[0]='\0';
	activityid[0]='\0';
	nonce[0]='\0';

	const byte_t		*rp=reqpacket.getBuffer();
	const byte_t		*startrp=rp;

	debugStart("pre-login");
	debugWrite("receiving...");

	// some useful variables
	byte_t		plopttok;
	uint16_t	ploptoff;
	uint16_t	ploptsize;

	for (;;) {

		// get the option token
		read(rp,&plopttok,&rp);
		debugWrite("token: 0x%02x",plopttok);
		if (plopttok==PL_TERMINATOR) {
			break;
		}

		// get the option offset
		readBE(rp,&ploptoff,&rp);
		debugWrite("offset: %hd",ploptoff);

		// get the option size
		readBE(rp,&ploptsize,&rp);
		debugWrite("size: %hd",ploptsize);

		// FIXME: verify that the packet is as long
		// as the sum of the option sizes claim

		// get the option data
		const byte_t		*dummy;
		switch (plopttok) {

			case PL_VERSION:
				// FIXME: bail if this isn't the first option
				readLE(startrp+ploptoff,&version,&dummy);
				readLE(startrp+ploptoff+sizeof(version),
							&subbuild,&dummy);
				debugWrite("pl_version");
				debugWrite("version: %d",version);
				debugWrite("subbuiild: %hd",subbuild);
				break;

			case PL_ENCRYPTION:
				read(startrp+ploptoff,&encryption,&dummy);
				debugWrite("pl_encryption");
				debugWrite("encryption:	0x%02x",encryption);
				break;

			case PL_INSTOPT:
				instvalidity=new char[ploptsize+1];
				read(startrp+ploptoff,
					instvalidity,ploptsize,&dummy);
				instvalidity[ploptsize]='\0';
				debugWrite("pl_instopt");
				debugWrite("instvalidity: %s",instvalidity);
				break;

			case PL_THREADID:
				readLE(startrp+ploptoff,&threadid,&dummy);
				debugWrite("pl_threadid");
				debugWrite("threadid: %d",threadid);
				break;

			case PL_MARS:
				read(startrp+ploptoff,&mars,&dummy);
				debugWrite("mars");
				debugWrite("mars: %d",mars);
				break;

			case PL_TRACEID:
				read(startrp+ploptoff,
						connid,sizeof(connid),
						&dummy);
				read(startrp+ploptoff+sizeof(connid),
						activityid,sizeof(activityid),
						&dummy);
				debugWrite("traceid");
				debugWrite("connid: %.*s",
						sizeof(connid),connid);
				debugWrite("activityid: %.*s",
						sizeof(activityid),activityid);
				break;

			case PL_FEDAUTHREQUIRED:
				read(startrp+ploptoff,
						&fedauthrequired,
						&dummy);
				debugWrite("fedauthrequired");
				debugWrite("fedauthrequired:%d",
						fedauthrequired);
				break;

			case PL_NONCEOPT:
				read(startrp+ploptoff,
						nonce,sizeof(nonce),
						&dummy);
				debugWrite("nonceopt");
				debugWrite("nonce: %.*s",sizeof(nonce),nonce);
				break;
		}
	}

	debugWrite("sending...");


	// begin building the response packet
	resppacket.clear();
	bytebuffer	packetdata;

	// Ha!  You have to know ahead of time how many tokens you plan on
	// sending to set this correctly.
	// Update this accordingly if you add tokens!!!
	ploptoff=5*(sizeof(byte_t)+
				sizeof(uint16_t)+
				sizeof(uint16_t))+
			sizeof(byte_t);

	// respond in the same format as the request...

	// version
	write(&resppacket,(byte_t)PL_VERSION);
	writeBE(&resppacket,ploptoff);
	ploptsize=sizeof(version)+sizeof(subbuild);
	ploptoff+=ploptsize;
	writeBE(&resppacket,ploptsize);
	// FIXME: we should probably send an accurate version
	// instead of regurgitating what the client sent us
	packetdata.append(version);
	packetdata.append(subbuild);
	debugWrite("pl_version");
	debugWrite("version: %d",version);
	debugWrite("subbuiild: %hd",subbuild);

	// encryption
	write(&resppacket,(byte_t)PL_ENCRYPTION);
	writeBE(&resppacket,ploptoff);
	ploptsize=sizeof(encryption);
	ploptoff+=ploptsize;
	writeBE(&resppacket,ploptsize);
	// FIXME: implement encryption
	encryption=ENCRYPT_NOT_SUP;
	packetdata.append(encryption);
	debugWrite("pl_encryption");
	debugWrite("encryption: 0x%02x",encryption);

	// instopt
	write(&resppacket,(byte_t)PL_INSTOPT);
	writeBE(&resppacket,ploptoff);
	ploptsize=charstring::getLength(instvalidity)+1;
	ploptoff+=ploptsize;
	writeBE(&resppacket,ploptsize);
	// FIXME: we should probably send an accurate instopt
	// instead of regurgitating what the client sent us
	packetdata.append(instvalidity,ploptsize);
	debugWrite("pl_instopt");
	debugWrite("instvalidity: %s",instvalidity);
	
	// threadid
	write(&resppacket,(byte_t)PL_THREADID);
	writeBE(&resppacket,ploptoff);
	ploptsize=sizeof(threadid);
	ploptoff+=ploptsize;
	writeBE(&resppacket,ploptsize);
	threadid=process::getProcessId();
	packetdata.append(threadid);
	debugWrite("pl_threadid");
	debugWrite("threadid: %d",threadid);

	// mars
	write(&resppacket,(byte_t)PL_MARS);
	writeBE(&resppacket,ploptoff);
	ploptsize=sizeof(mars);
	ploptoff+=ploptsize;
	writeBE(&resppacket,ploptsize);
	// FIXME: SQL Relay actually does support multiple active result sets
	mars=0;
	packetdata.append(mars);
	debugWrite("mars");
	debugWrite("mars: %d",mars);

	// no need to send traceid, fedauthrequired, or nonce in response

	// terminator
	write(&resppacket,(byte_t)PL_TERMINATOR);

	// append the packet data to the resppacket
	write(&resppacket,packetdata.getBuffer(),packetdata.getSize());

	// send the response packet
	bool	retval=sendPacket();

	debugEnd();

	// clean up
	delete[] instvalidity;

	return retval;
}

bool sqlrprotocol_tds::preTds7Login() {

	debugStart("pre-tds7 login");
	debugEnd();

	// FIXME: actually implement this

	resppacket.clear();
	unimplementedFeatureError();
	done();
	sendPacket();
	return false;
}

bool sqlrprotocol_tds::tds7Login() {

	const byte_t	*rp=reqpacket.getBuffer();
	const byte_t	*startrp=rp;

	// initialize values...
	uint32_t	size=0;
	uint32_t	tdsversion=0;
	clienttdsversion=700;
	uint32_t	packetsize=0;
	uint32_t	clientprogver=0;
	uint32_t	clientpid=0;
	uint32_t	connectionid=0;
	byte_t		optionflags1=0;
	byte_t		optionflags2=0;
	byte_t		typeflags=0;
	byte_t		optionflags3=0;
	uint32_t	clienttimzone=0;
	uint32_t	clientlcid=0;

	uint16_t	ibhostname=0;
	uint16_t	cchhostname=0;
	uint16_t	ibusername=0;
	uint16_t	cchusername=0;
	uint16_t	ibpassword=0;
	uint16_t	cchpassword=0;
	uint16_t	ibappname=0;
	uint16_t	cchappname=0;
	uint16_t	ibservername=0;
	uint16_t	cchservername=0;
	uint16_t	ibextension=0;
	uint16_t	cbextension=0;
	uint16_t	ibcltintname=0;
	uint16_t	cchcltintname=0;
	uint16_t	iblanguage=0;
	uint16_t	cchlanguage=0;
	uint16_t	ibdatabase=0;
	uint16_t	cchdatabase=0;
	char		clientid[6];
	uint16_t	ibsspi=0;
	uint16_t	cbsspi=0;
	uint16_t	ibatchdbfile=0;
	uint16_t	cchatchdbfile=0;
	uint16_t	ibchangepassword=0;
	uint16_t	cchchangepassword=0;
	uint32_t	cbsspilong=0;

	char		fbyteorder=0;
	char		fcharset=0;
	uint16_t	ffloattype=0;
	char		fdumpload=0;
	char		fusedbwarn=0;
	char		fusedbfatal=0;
	char		fsetlangwarn=0;

	char		fsetlangfatal=0;
	char		fodbc=0;
	bool		ftranboundary=false;
	bool		fcachecontent=false;
	uint32_t	fusertype=0;
	char		fintsecurity=0;

	uint32_t	fsqltype=SQL_DFLT;
	char		foledb=0;
	bool		freadonlyintent=false;

	bool		fchangepassword=false;
	bool		fuserinstance=false;
	bool		fsendyukonbinaryxml=false;
	bool		funknowncollationhandling=false;
	bool		fextension=false;

	wchar_t		*hostname=NULL;
	wchar_t		*username=NULL;
	wchar_t		*password=NULL;
	wchar_t		*appname=NULL;
	wchar_t		*servername=NULL;
	byte_t		*extension=NULL;
	wchar_t		*cltintname=NULL;
	wchar_t		*language=NULL;
	wchar_t		*database=NULL;
	wchar_t		*atchdbfile=NULL;
	wchar_t		*changepassword=NULL;
	byte_t		*sspi=NULL;

	// copy values out of the recv packet...
	readLE(rp,&size,&rp);
	readBE(rp,&tdsversion,&rp);
	clienttdsversion=tdsVersionHexToDec(tdsversion);
	readLE(rp,&packetsize,&rp);
	readBE(rp,&clientprogver,&rp);
	readLE(rp,&clientpid,&rp);
	readLE(rp,&connectionid,&rp);
	read(rp,&optionflags1,&rp);
	read(rp,&optionflags2,&rp);
	read(rp,&typeflags,&rp);
	read(rp,&optionflags3,&rp);
	readLE(rp,&clienttimzone,&rp);
	readLE(rp,&clientlcid,&rp);
	readLE(rp,&ibhostname,&rp);
	readLE(rp,&cchhostname,&rp);
	readLE(rp,&ibusername,&rp);
	readLE(rp,&cchusername,&rp);
	readLE(rp,&ibpassword,&rp);
	readLE(rp,&cchpassword,&rp);
	readLE(rp,&ibappname,&rp);
	readLE(rp,&cchappname,&rp);
	readLE(rp,&ibservername,&rp);
	readLE(rp,&cchservername,&rp);
	if (clienttdsversion>=740) {
		readLE(rp,&ibextension,&rp);
		readLE(rp,&cbextension,&rp);
		if (!fextension) {
			ibextension=0;
			cbextension=0;
		}
	} else {
		rp=rp+sizeof(ibextension);
		rp=rp+sizeof(cbextension);
	}
	readLE(rp,&ibcltintname,&rp);
	readLE(rp,&cchcltintname,&rp);
	readLE(rp,&iblanguage,&rp);
	readLE(rp,&cchlanguage,&rp);
	readLE(rp,&ibdatabase,&rp);
	readLE(rp,&cchdatabase,&rp);
	read(rp,clientid,sizeof(clientid),&rp);
	readLE(rp,&ibsspi,&rp);
	readLE(rp,&cbsspi,&rp);
	readLE(rp,&ibatchdbfile,&rp);
	readLE(rp,&cchatchdbfile,&rp);
	if (clienttdsversion>=720) {
		readLE(rp,&ibchangepassword,&rp);
		readLE(rp,&cchchangepassword,&rp);
		if (!fchangepassword) {
			ibchangepassword=0;
			cchchangepassword=0;
		}
		readLE(rp,&cbsspilong,&rp);
	}
	if (cchhostname<=128) {
		hostname=wcharstring::duplicateUcs2(
					(ucs2_t *)(startrp+ibhostname),
					(size_t)cchhostname);
	}
	if (cchusername<=128) {
		username=wcharstring::duplicateUcs2(
					(ucs2_t *)(startrp+ibusername),
					(size_t)cchusername);
	}
	if (cchpassword<=128) {
		password=readPassword(startrp+ibpassword,cchpassword);
	}
	if (cchappname<=128) {
		appname=wcharstring::duplicateUcs2(
					(ucs2_t *)(startrp+ibappname),
					(size_t)cchappname);
	}
	if (cchservername<=128) {
		servername=wcharstring::duplicateUcs2(
					(ucs2_t *)(startrp+ibservername),
					(size_t)cchservername);
	}
	if (clienttdsversion>=740 && cbextension<=255) {
		extension=(byte_t *)bytestring::duplicate(
						startrp+ibextension,
						cbextension);
		// FIXME: decode this...
	}
	if (cchcltintname<=128) {
		cltintname=wcharstring::duplicateUcs2(
					(ucs2_t *)(startrp+ibcltintname),
					(size_t)cchcltintname);
	}
	if (cchlanguage<=128) {
		language=wcharstring::duplicateUcs2(
					(ucs2_t *)(startrp+iblanguage),
					(size_t)cchlanguage);
	}
	if (cchdatabase<=128) {
		database=wcharstring::duplicateUcs2(
					(ucs2_t *)(startrp+ibdatabase),
					(size_t)cchdatabase);
	}
	if (cchatchdbfile<=260) {
		atchdbfile=wcharstring::duplicateUcs2(
					(ucs2_t *)(startrp+ibatchdbfile),
					(size_t)cchatchdbfile);
	}
	if (clienttdsversion>=720 && cchchangepassword<=128) {
		changepassword=readPassword(startrp+ibchangepassword,
							cchchangepassword);
	}
	uint32_t	sspisize=0;
	if (sspisize<65535) {
		sspisize=cbsspi;
	} else {
		if (cbsspilong==0) {
			sspisize=65535;
		} else {
			sspisize=cbsspilong;
		}
	}
	sspi=(byte_t *)bytestring::duplicate(startrp+ibsspi,sspisize);

	// set option/type flags...
	fbyteorder=(optionflags1&(0x01));
	fcharset=(optionflags1&(0x01<<1))>>1;
	ffloattype=(optionflags1&(0x03<<2))>>2;
	fdumpload=(optionflags1&(0x01<<3))>>3;
	fusedbwarn=(optionflags1&(0x01<<4))>>4;
	fusedbfatal=(optionflags1&(0x01<<5))>>5;
	fsetlangwarn=(optionflags1&(0x01<<6))>>6;

	fsetlangfatal=(optionflags2&(0x01));
	fodbc=(optionflags2&(0x01<<1))>>1;
	ftranboundary=(optionflags2&(0x01<<2))>>2;
	fcachecontent=(optionflags2&(0x01<<3))>>3;
	fusertype=(optionflags2&(0x07<<4))>>4;
	fintsecurity=(optionflags2&(0x01<<5))>>5;

	fsqltype=(typeflags&(0x0F));
	foledb=(typeflags&(0x01<<4))>>4;
	freadonlyintent=(typeflags&(0x01<<3))>>3;

	fchangepassword=(optionflags3&(0x01));
	fuserinstance=(optionflags3&(0x01<<2))>>2;
	fsendyukonbinaryxml=(optionflags3&(0x01<<3))>>3;
	funknowncollationhandling=(optionflags3&(0x01<<4))>>4;
	fextension=(optionflags3&(0x01<<5))>>5;

	if (fodbc==ODBC_ON || foledb==OLEDB_ON) {
		// FIXME: set:
		// * ANSI_DEFAULTS ON
		// * CURSOR_CLOSE_ON_COMMIT OFF
		// * IMPLICIT_TRANSACTIONS OFF
		// * TEXTSIZE 0x7FFFFFFF for tds <= 7.2
		// * TEXTSIZE infinite for tds >= 7.3
		// * ROWCOUNT infinite
	}

	if (getDebug()) {
		debugStart("tds7 login");
		debugWrite("size: %d",size);
		debugWrite("tdsversion: 0x%08x (%d)",
					tdsversion,clienttdsversion);
		debugWrite("packetsize: %d",packetsize);
		debugWrite("clientprogver: 0x%08x (%d)",
					clientprogver,clientprogver);
		debugWrite("clientpid: %d",clientpid);
		debugWrite("connectionid: %d",connectionid);
		stringbuffer	b;
		b.printBits(optionflags1);
		debugWrite("optionflags1: %s",b.getString());
		debugWrite("fbyteorder: %d",fbyteorder);
		debugWrite("fcharset: %d",fcharset);
		debugWrite("ffloattype: %d",ffloattype);
		debugWrite("fdumpload: %d",fdumpload);
		debugWrite("fusedbwarn: %d",fusedbwarn);
		debugWrite("fusedbfatal: %d",fusedbfatal);
		debugWrite("fsetlangwarn: %d",fsetlangwarn);
		b.clear();
		b.printBits(optionflags2);
		debugWrite("optionflags2: %s",b.getString());
		debugWrite("fsetlangfatal: %d",fsetlangfatal);
		debugWrite("fodbc: %d",fodbc);
		debugWrite("ftranboundary: %d",ftranboundary);
		debugWrite("fcachecontent: %d",fcachecontent);
		debugWrite("fusertype: %d",fusertype);
		debugWrite("fintsecurity: %d",fintsecurity);
		b.clear();
		b.printBits(typeflags);
		debugWrite("typeflags: %s",b.getString());
		debugWrite("fsqltype: %d",fsqltype);
		debugWrite("foledb: %d",foledb);
		debugWrite("freadonlyintent: %d",freadonlyintent);
		b.clear();
		b.printBits(optionflags3);
		debugWrite("optionflags3: %s",b.getString());
		debugWrite("fchangepassword: %d",fchangepassword);
		debugWrite("fuserinstance: %d",fuserinstance);
		debugWrite("fsendyukonbinaryxml: %d",fsendyukonbinaryxml);
		debugWrite("funknowncollationhandling: %d",
					funknowncollationhandling);
		debugWrite("fextension: %d",fextension);
		debugWrite("clienttimzone: %d",clienttimzone);
		b.clear();
		b.printBits(clientlcid);
		debugWrite("clientlcid: %s",b.getString());
		debugWrite("hostname: (%hd,%hd) %S",
					ibhostname,cchhostname,hostname);
		debugWrite("username: (%hd,%hd) %S",
					ibusername,cchusername,username);
		debugWrite("password: (%hd,%hd) %S",
					ibpassword,cchpassword,password);
		debugWrite("appname: (%hd,%hd) %S",
					ibappname,cchappname,appname);
		debugWrite("servername: (%hd,%hd) %S",
					ibservername,cchservername,servername);
		b.clear();
		b.safePrint(extension,cbextension);
		debugWrite("extension: (%hd,%hd) %s",
					ibextension,cbextension,b.getString());
		debugWrite("cltintname: (%hd,%hd) %S",
					ibcltintname,cchcltintname,cltintname);
		debugWrite("language: (%hd,%hd) %S",
					iblanguage,cchlanguage,language);
		debugWrite("database: (%hd,%hd) %S",
					ibdatabase,cchdatabase,database);
		debugWrite("clientid: %02x:%02x:%02x:%02x:%02x:%02x",
					clientid[0],clientid[1],clientid[2],
					clientid[3],clientid[4],clientid[5]);
		debugWrite("atchdbfile: (%hd,%hd) %S",
					ibatchdbfile,cchatchdbfile,atchdbfile);
		debugWrite("changepassword: (%hd,%hd) %S",
					ibchangepassword,cchchangepassword,
					changepassword);
		debugWrite("sspi: (%hd,%hd,%d)",ibsspi,cbsspi,cbsspilong);
		debugHexDump((byte_t *)sspi,sspisize);
		debugEnd();
	}

	// FIXME: validate some of these values

	// negotiate tds version
	negotiateTdsVersion();


	// begin building the response packet
	resppacket.clear();

	bool	retval=true;

	// auth the user...
	if (retval) {
		if (auth(username,cchusername,password,cchpassword)) {
			loginAck();
		} else {
			authError(username,cchusername);
			retval=false;
		}
	}

	// change database...
	if (retval && cchdatabase) {

		char		*olddatabase=cont->getCurrentDatabase();
		uint32_t	olddatabaselen=
					charstring::getLength(olddatabase);
		wchar_t		*olddatabase32=wcharstring::duplicate(
						olddatabase,olddatabaselen);

		if (changeDatabase(database,cchdatabase)) {

			envChange(ENV_CHANGE_DATABASE,
					database,
					cchdatabase,
					olddatabase32,
					olddatabaselen);

			changeDatabaseInfo(database,cchdatabase);

		} else {
			if (fusedbfatal==USE_DB_FATAL) {
				changeDatabaseError(database,cchdatabase,false);
				retval=false;
			} else if (fusedbwarn==USE_DB_WARN_ON) {
				changeDatabaseError(database,cchdatabase,true);
			}
		}

		delete[] olddatabase32;
		delete[] olddatabase;
	}

	// change collation...
	if (retval) {
		if (changeCollation(clientlcid)) {
			envChangeSqlCollation(clientlcid,0);
		}
	}

	// change language...
	if (retval && cchlanguage) {
		if (changeLanguage(language,cchlanguage)) {

			envChange(ENV_CHANGE_LANGUAGE,
					language,
					cchlanguage,
					// FIXME: send the actual old language
					// instead if just sending the new
					// language as the old language
					language,
					cchlanguage);

			changeLanguageInfo(language,cchlanguage);

		} else {
			if (fsetlangfatal==SET_LANG_FATAL) {
				changeLanguageError(language,cchlanguage,false);
				retval=false;
			} else if (fsetlangwarn==SET_LANG_WARN_ON) {
				changeLanguageError(language,cchlanguage,true);
			}
		}
	}

	// change packet size
	if (retval) {
		negotiatePacketSize(packetsize);
		envChangePacketSize();

		// reset "old" packet size
		oldpacketsize=negotiatedpacketsize;
	}

	// done
	done();

	// send the response packet
	retval=sendPacket();

	// clean up
	delete[] hostname;
	delete[] username;
	delete[] password;
	delete[] appname;
	delete[] servername;
	delete[] extension;
	delete[] cltintname;
	delete[] language;
	delete[] database;
	delete[] atchdbfile;
	delete[] changepassword;
	delete[] sspi;

	return retval;
}

bool sqlrprotocol_tds::auth(const wchar_t *username,
				size_t usernamelen,
				const wchar_t *password,
				size_t passwordlen) {

	char	*username8=charstring::duplicate(username,usernamelen);
	char	*password8=charstring::duplicate(password,passwordlen);

	sqlruserpasswordcredentials	cred;
	cred.setUser(username8);
	cred.setPassword(password8);

	bool	authsuccess=cont->auth(&cred);

	debugStart("authenticate");
	debugWrite("username: %s",username8);
	debugWrite("password: %s",password8);
	debugWrite((authsuccess)?"success":"failed");
	debugEnd();

	delete[] username8;
	delete[] password8;

	return authsuccess;
}

void sqlrprotocol_tds::loginAck() {

	byte_t		token=TOKEN_LOGIN_ACK;
					
	byte_t		iface=SQL_TSQL;
	uint32_t	tdsversion=
			tdsVersionDecToHex(negotiatedtdsversion,true);
	const char	*progname=dbversion;
	byte_t		prognamesize=(byte_t)charstring::getLength(progname);
	ucs2_t		*progname16=ucs2charstring::duplicate(progname,
							(size_t)prognamesize);
	byte_t		majorver=0;
	byte_t		minorver=0;
	byte_t		buildnumhi=0;
	byte_t		buildnumlow=0;

	uint16_t	tokensize=sizeof(byte_t)+
					sizeof(uint32_t)+
					sizeof(byte_t)+
					prognamesize*sizeof(ucs2_t)+
					sizeof(byte_t)+
					sizeof(byte_t)+
					sizeof(byte_t)+
					sizeof(byte_t);
	
	debugStart("login ack");
	debugWrite("token: 0x%02x",token);
	debugWrite("tokensize: 0x%02x (%hd)",tokensize,tokensize);
	debugWrite("interface: %d",iface);
	debugWrite("tdsversion: 0x%08x (%d)",tdsversion,negotiatedtdsversion);
	debugWrite("prognamesize: %d",prognamesize);
	debugWrite("progname: %s",progname);
	debugWrite("majorver: %d",majorver);
	debugWrite("minorver: %d",minorver);
	debugWrite("buildnumhi: %d",buildnumhi);
	debugWrite("buildnumlow: %d",buildnumlow);
	debugEnd();

	write(&resppacket,token);
	write(&resppacket,hostToLE(tokensize));
	write(&resppacket,iface);
	writeBE(&resppacket,tdsversion);
	write(&resppacket,prognamesize);
	write(&resppacket,progname16,prognamesize);
	write(&resppacket,majorver);
	write(&resppacket,minorver);
	write(&resppacket,buildnumhi);
	write(&resppacket,buildnumlow);
}

void sqlrprotocol_tds::authError(const wchar_t *username,
					size_t usernamelen) {

	char	*username8=charstring::duplicate(username,usernamelen);

	stringbuffer	err;
	err.append("Login failed for user '");
	err.append(username8);
	err.append("'.");

	error(18456,1,14,err.getString(),srvname,NULL,1);

	delete[] username8;
}

bool sqlrprotocol_tds::changeDatabase(const wchar_t *database,
						size_t databaselen) {

	char	*database8=charstring::duplicate(database,databaselen);

	bool	changedbsuccess=cont->selectDatabase(database8);

	debugStart("change db");
	debugWrite("db: %s",database8);
	debugWrite((changedbsuccess)?"success":"failed");
	debugEnd();

	delete[] database8;

	return changedbsuccess;
}

void sqlrprotocol_tds::changeDatabaseInfo(const wchar_t *database,
						size_t databaselen) {

	char	*database8=charstring::duplicate(database,databaselen);

	stringbuffer	inf;
	inf.append("Changed database context to '");
	inf.append(database8)->append("'.");

	info(5701,2,0,inf.getString(),NULL,NULL,1);

	delete[] database8;
}

void sqlrprotocol_tds::changeDatabaseError(const wchar_t *database,
						size_t databaselen,
						bool warning) {

	char	*database8=charstring::duplicate(database,databaselen);

	// FIXME: verify this message for warning
	stringbuffer	err;
	err.append("Cannot open database '")->append(database8);
	err.append("' requested by the login.");
	if (!warning) {
		err.append(" The login failed.");
	}

	// FIXME: verify these for warning
	error(4060,1,(warning)?9:11,err.getString(),srvname,NULL,1);

	delete[] database8;
}

bool sqlrprotocol_tds::changeCollation(uint32_t lcid) {

	bool		lcidignorecase=(lcid&(0x01));
	bool		lcidignoreaccent=(lcid&(0x01<<1))>>1;
	bool		lcidignorewidth=(lcid&(0x01<<2))>>2;
	bool		lcidignorekana=(lcid&(0x01<<3))>>3;
	bool		lcidbinary=(lcid&(0x01<<4))>>4;
	bool		lcidbinary2=(lcid&(0x01<<5))>>5;
	byte_t		lcidversion=(lcid&(0x0F<<8))>>8;

	// FIXME: actually implement this

	bool	changecollationsuccess=true;

	if (getDebug()) {
		debugStart("change collation");
		stringbuffer	b;
		b.printBits(lcid);
		debugWrite("lcid: %s",b.getString());
		debugWrite("lcidignorecase: %d",lcidignorecase);
		debugWrite("lcidignoreaccent: %d",lcidignoreaccent);
		debugWrite("lcidignorewidth: %d",lcidignorewidth);
		debugWrite("lcidignorekana: %d",lcidignorekana);
		debugWrite("lcidbinary: %d",lcidbinary);
		debugWrite("lcidbinary2: %d",lcidbinary2);
		debugWrite("lcidversion: %d",lcidversion);
		debugWrite((changecollationsuccess)?"success":"failed");
		debugEnd();
	}

	return changecollationsuccess;
}

void sqlrprotocol_tds::envChangeSqlCollation(uint32_t lcid,
						byte_t sortid) {

	byte_t		token=TOKEN_ENV_CHANGE;

	byte_t		type=ENV_CHANGE_SQL_COLLATION;
	
	uint16_t	tokensize=
				sizeof(byte_t)+
				sizeof(byte_t)+
				sizeof(uint32_t)+
				sizeof(byte_t)+
				sizeof(byte_t);

	if (getDebug()) {
		debugStart("env change");
		debugWrite("token: 0x%02x",token);
		debugWrite("tokensize: 0x%02x (%hd)",tokensize,tokensize);
		debugWrite("type: %d",type);
		debugWrite("newvaluesize: %d",sizeof(uint32_t)+sizeof(byte_t));
		stringbuffer	b;
		b.printBits(lcid);
		debugWrite("newvalue: %s %d",b.getString(),sortid);
		debugHexDump((byte_t *)&lcid,sizeof(lcid));
		debugHexDump((byte_t *)&sortid,sizeof(sortid));
		debugWrite("oldvaluesize: 0");
		debugEnd();
	}

	write(&resppacket,token);
	write(&resppacket,hostToLE(tokensize));
	write(&resppacket,type);
	write(&resppacket,(byte_t)(sizeof(lcid)+sizeof(sortid)));
	writeBE(&resppacket,lcid);
	write(&resppacket,sortid);
	write(&resppacket,(byte_t)0);
}

bool sqlrprotocol_tds::changeLanguage(const wchar_t *language,
						size_t languagelen) {

	char	*language8=charstring::duplicate(language,languagelen);

	// FIXME: actually implement this...

	bool	changelangsuccess=true;

	debugStart("change lang");
	debugWrite("lang: %s",language8);
	debugWrite((changelangsuccess)?"success":"failed");
	debugEnd();

	delete[] language8;

	return changelangsuccess;
}

void sqlrprotocol_tds::changeLanguageInfo(const wchar_t *language,
						size_t languagelen) {

	char	*language8=charstring::duplicate(language,languagelen);

	stringbuffer	inf;
	inf.append("Changed language setting to ");
	inf.append(language8)->append(".");

	info(5703,1,0,inf.getString(),NULL,NULL,1);

	delete[] language8;
}

void sqlrprotocol_tds::changeLanguageError(const wchar_t *language,
						size_t languagelen,
						bool warning) {

	char	*language8=charstring::duplicate(language,languagelen);

	// FIXME: verify this message for error and warning
	stringbuffer	err;
	err.append("Cannot change language to '");
	err.append(language8);
	err.append("' requested by the login.");
	if (!warning) {
		err.append(" The login failed.");
	}

	// FIXME: verify these for warning
	error(0,1,(warning)?9:10,err.getString(),srvname,NULL,1);

	delete[] language8;
}

void sqlrprotocol_tds::negotiatePacketSize(uint32_t packetsize) {

	bool	changepacketsizesuccess=true;
	if (packetsize<=65536) {
		negotiatedpacketsize=packetsize;
		// FIXME: reset read/write buffer sizes?
	} else {
		changepacketsizesuccess=false;
	}

	debugStart("change packet size");
	debugWrite("requested packetsize: %d",packetsize);
	debugWrite((changepacketsizesuccess)?"success":"failed");
	debugEnd();
}

void sqlrprotocol_tds::envChangePacketSize() {

	char		*npsize=charstring::parseNumber(negotiatedpacketsize);
	uint32_t	npsizesize=charstring::getLength(npsize);
	wchar_t		*npsize32=wcharstring::duplicate(npsize,npsizesize);

	char		*opsize=charstring::parseNumber(oldpacketsize);
	uint32_t	opsizesize=charstring::getLength(opsize);
	wchar_t		*opsize32=wcharstring::duplicate(opsize,opsizesize);

	envChange(ENV_CHANGE_PACKET_SIZE,
				npsize32,npsizesize,
				opsize32,opsizesize);

	delete[] npsize32;
	delete[] npsize;
	delete[] opsize32;
	delete[] opsize;
}

bool sqlrprotocol_tds::federatedAuthenticationToken() {

	debugStart("fed auth token");
	debugEnd();

	// FIXME: actually implement this

	resppacket.clear();
	unimplementedFeatureError();
	done();
	return sendPacket();
}

bool sqlrprotocol_tds::attention() {

	//const byte_t	*rp=reqpacket.getBuffer();

	debugStart("attention");
	debugEnd();

	// FIXME: actually implement this

	resppacket.clear();
	unimplementedFeatureError();
	done();
	return sendPacket();
}

bool sqlrprotocol_tds::transactionManagerRequest() {

	//const byte_t	*rp=reqpacket.getBuffer();

	debugStart("tx mgr request");
	debugEnd();

	// FIXME: actually implement this

	resppacket.clear();
	unimplementedFeatureError();
	done();
	return sendPacket();
}

bool sqlrprotocol_tds::sspi() {

	//const byte_t	*rp=reqpacket.getBuffer();

	debugStart("sspi");
	debugEnd();

	// FIXME: actually implement this

	resppacket.clear();
	unimplementedFeatureError();
	done();
	return sendPacket();
}

void sqlrprotocol_tds::unimplementedFeatureError() {

	// FIXME: is there a real error message/number/state/class for this?
	error(0,1,10,"Unimplemented feature",srvname,NULL,1);
}

bool sqlrprotocol_tds::sqlBatch(sqlrservercursor *cursor) {

	// FIXME: this works for DML/DDL, but not for select,
	// ct_results() returns CS_FAIL

	const byte_t	*rp=reqpacket.getBuffer();
	size_t		rpsize=reqpacket.getSize();

	debugStart("sql batch");

	// get the headers
	if (negotiatedtdsversion>=720) {
		allHeaders(rp,rpsize,&rp,&rpsize);
	}

	// get the sql
	const ucs2_t	*sql=(const ucs2_t *)rp;
	uint16_t	sqlsize=rpsize/sizeof(ucs2_t);
	// FIXME: use maxquerysize here

	// FIXME: Ideally we could just send the unconverted query, as long
	// as we also send the proper size in bytes.  SQL Relay really
	// appears to want ascii queries though, or at least it wants the
	// query itself (other than embedded values) to be acsii.
	char	*sql8=charstring::duplicateUcs2(sql,(size_t)sqlsize);

	debugWrite("sql: %s",sql8);
	debugWrite("sqlsize: %d",sqlsize);
	debugEnd();

	// run the query
	bool	success=
		cont->prepareQuery(cursor,sql8,sqlsize,true,true,true) &&
		cont->executeQuery(cursor,true,true,true,true);

	// clean up
	delete[] sql8;


	// begin building the response packet
	resppacket.clear();

	if (success) {
		colMetaData(cursor,false);
		done(DONE_FINAL|DONE_COUNT,0,rows(cursor));
	} else {
		sqlBatchError(cursor);
		done();
	}

	// send the response packet
	return sendPacket();
}

void sqlrprotocol_tds::allHeaders(const byte_t *rp,
					size_t rpsize,
					const byte_t **rpout,
					size_t *rpsizeout) {

	// get the size of all headers
	uint32_t	allheaderssize;
	readLE(rp,&allheaderssize,&rp);

	debugWrite("all-headers size: %d",allheaderssize);

	// decrement remaining sizes
	allheaderssize-=sizeof(allheaderssize);
	rpsize-=sizeof(allheaderssize);

	while (allheaderssize) {

		// get header size and type
		uint32_t	headersize;
		readLE(rp,&headersize,&rp);
		uint16_t	headertype;
		readLE(rp,&headertype,&rp);

		debugWrite("header size: %d",headersize);
		debugWrite("header type: 0x%04x",headertype);

		switch (headertype) {
			case ALL_HEADERS_QUERY_NOTIFICATIONS:
				{
				uint16_t	notifyidlen;
				ucs2_t		*notifyid;
				uint16_t	ssbdeploymentlen;
				ucs2_t		*ssbdeployment;
				uint32_t	notifytimeout;

				readLE(rp,&notifyidlen,&rp);
				notifyid=new ucs2_t[notifyidlen];
				read(rp,notifyid,notifyidlen,&rp);

				readLE(rp,&ssbdeploymentlen,&rp);
				ssbdeployment=new ucs2_t[ssbdeploymentlen];
				read(rp,ssbdeployment,ssbdeploymentlen,&rp);

				readLE(rp,&notifytimeout,&rp);

				// FIXME: do something useful with this info

				delete[] notifyid;
				delete[] ssbdeployment;
				}
				break;

			case ALL_HEADERS_TRANSACTION_DESCRIPTOR:
				{
				uint32_t	outstandingrequestcount;
				uint64_t	transactiondescriptor;
				readLE(rp,&outstandingrequestcount,&rp);
				readLE(rp,&transactiondescriptor,&rp);
				// FIXME: do something useful with this info
				}
				break;

			case ALL_HEADERS_TRACE_ACTIVITY:
				{
				byte_t	activityid[20];
				read(rp,activityid,sizeof(activityid),&rp);
				// FIXME: do something useful with this info
				}
				break;

		}

		// decrement remaining sizes
		allheaderssize-=headersize;
		rpsize-=headersize;
	}

	// copy out pointer and size
	*rpout=rp;
	if (rpsizeout) {
		*rpsizeout=rpsize;
	}
}

void sqlrprotocol_tds::colMetaData(sqlrservercursor *cursor, bool nometadata) {

	// get col count and bail if there are no columns
	uint16_t	count=cont->colCount(cursor);
	if (!count) {
		return;
	}

	byte_t	token=TOKEN_COLMETADATA;

	write(&resppacket,token);
	write(&resppacket,hostToLE(count));

	debugStart("col meta data");
	debugWrite("token: 0x%02x",token);
	debugWrite("count: %d",count);

	cekTable();

	if (nometadata) {
		write(&resppacket,(uint16_t)0xFFFF);
		debugWrite("no metadata");
	} else {
		for (uint16_t col=0; col<count; col++) {
			colData(cursor,col);
		}
	}

	debugEnd();
}

void sqlrprotocol_tds::cekTable() {

	if (negotiatedtdsversion<730) {
		return;
	}

	// FIXME: The client doesn't seem to care that this isn't
	// being sent.  How do we decide when to send it?

	// FIXME: actually implement this...

#if 0
	uint16_t	ekvaluecount=0;
	for (uint16_t i=0; i<ekvaluecount; i++) {

		uint32_t	databaseid;
		uint32_t	cekid;
		uint32_t	cekversion;
		uint64_t	cekmdversion;

		byte_t		ekcount=0;
		for (uint16_t j=0; i<ekcount; j++) {

			uint16_t	encryptedkeylen;
			uint16_t	*encryptedkey;

			byte_t		keystorenamelen;
			uint16_t	*keystorename;

			uint16_t	keypathlen;
			uint16_t	*keypath;

			byte_t		asymmetricalgolen;
			uint16_t	*asymmetricalgo;
		}
	}
#endif
}

byte_t sqlrprotocol_tds::mapType(uint16_t type) {

	// Some protocol versions don't support some types.  If the server
	// returned a type not supported by the protocol, then map it to a
	// type that is.

	// FIXME: just use multiple type maps instead of the switch/ifs...

	byte_t	tdstype=tdstypemap[type];
	if (negotiatedtdsversion<730) {
		switch (tdstype) {
			case TDS_TYPE_DATEN:
				// FIXME: do something...
				break;
			case TDS_TYPE_TIMEN:
				// FIXME: do something...
				break;
			case TDS_TYPE_DATETIME2N:
				tdstype=TDS_TYPE_DATETIMN;
				break;
			case TDS_TYPE_DATETIMEOFFSETN:
				// FIXME: do something...
				break;
		}
	}
	if (negotiatedtdsversion<720) {
		switch (tdstype) {
			case TDS_TYPE_XML:
				tdstype=TDS_TYPE_TEXT;
				break;
			case TDS_TYPE_UDT:
				// FIXME: do something...
				break;
			case TDS_TYPE_SSVARIANT:
				// FIXME: do something...
				break;
			case TDS_TYPE_TVP:
				// FIXME: do something...
				break;
		}
	}
	return tdstype;
}

void sqlrprotocol_tds::colData(sqlrservercursor *cursor, uint16_t col) {

	debugStart("col %d",col);

	byte_t	tdstype=mapType(cont->getColumnType(cursor,col));

	userType(tdstype);
	colFlags(cursor,col,tdstype);
	typeInfo(cursor,col,tdstype);
	tableName(tdstype);
	cryptoMetaData();
	colName(cursor,col);

	debugEnd();
}

void sqlrprotocol_tds::userType(byte_t tdstype) {

	uint32_t	usertype=0;

	// * = 0x0000 by default
	// * = 0x0050 for timestamp types
	// * > 0x00FF for alias types (FIXME: how to identify these?)
	if (tdstype==TDS_TYPE_DATETIME2N) {
		usertype=0x0050;
	}

	if (negotiatedtdsversion<720) {
		write(&resppacket,hostToLE((uint16_t)usertype));
	} else {
		write(&resppacket,hostToLE(usertype));
	}

	debugWrite("usertype: %d",usertype);
}

void sqlrprotocol_tds::colFlags(sqlrservercursor *cursor,
						uint16_t col,
						byte_t tdstype) {

	uint16_t	flags=0;

	// is nullable
	flags|=((cont->getColumnIsNullable(cursor,col))?0x0001:0);

	// case-sensitive
	flags|=((isCaseSensitiveType(tdstype))?(0x0001<<1):0);

	// updateable (0 = readonly, 1 = read/write, 2 = unknown) (FIXME)
	flags|=((true)?(0x0002<<2):0);

	// identity
	flags|=((cont->getColumnIsAutoIncrement(cursor,col))?(0x0001<<4):0);

	if (negotiatedtdsversion>=720) {

		// computed (FIXME)
		flags|=((false)?(0x0001<<5):0);

		// reserved ODBC
		flags|=((false)?(0x0011<<6):0);

		if (negotiatedtdsversion>=74) {

			// sparse column set (FIXME)
			flags|=((false)?(0x0001<<8):0);

			// encrypted (FIXME)
			flags|=((false)?(0x0001<<9):0);

			// this bit is reserved

			// fixed size clr type (FIXME)
			flags|=((false)?(0x0001<<11):0);

			// these 4 bits are reserved

			// hidden (FIXME)
			flags|=((false)?(0x0001<<16):0);

			// key in select...for browse (FIXME)
			flags|=((false)?(0x0001<<17):0);

			// nullable unknown (FIXME)
			flags|=((false)?(0x0001<<18):0);
		}
	}
	writeBE(&resppacket,flags);

	if (getDebug()) {
		stringbuffer	b;
		b.printBits(flags);
		debugWrite("flags: %s",b.getString());
	}
}

void sqlrprotocol_tds::typeInfo(sqlrservercursor *cursor,
						uint16_t col,
						byte_t tdstype) {

	write(&resppacket,tdstype);

	debugWrite("tdstype: 0x%02x",tdstype);

	if (isFixedLenType(tdstype)) {

		debugWrite("fixedlentype...");

	} else if (isVarLenType(tdstype)) {

		debugWrite("varlentype...");

		uint32_t size=cont->getColumnSize(cursor,col);
		uint32_t precision=cont->getColumnPrecision(cursor,col);
		uint32_t scale=cont->getColumnScale(cursor,col);

		// size
		switch (tdstype) {
			case TDS_TYPE_SSVARIANT:
			case TDS_TYPE_TEXT:
			case TDS_TYPE_NTEXT:
			case TDS_TYPE_IMAGE:
			case TDS_TYPE_XML:
				// limit the size to 2^31-1 because the
				// client will interpret it as signed
				if (size>2147483647) {
					size=2147483647;
				}
				write(&resppacket,hostToLE(size));
				debugWrite("size: %d (32-bit)",size);
				break;
			case TDS_TYPE_BIGCHAR:
			case TDS_TYPE_BIGVARCHR:
			case TDS_TYPE_NCHAR:
			case TDS_TYPE_NVARCHAR:
			case TDS_TYPE_BIGBINARY:
			case TDS_TYPE_BIGVARBIN:
				// limit the size to 2^15-1 because the
				// client will interpret it as signed
				if (size>32767) {
					size=32767;
				}
				write(&resppacket,hostToLE((uint16_t)size));
				debugWrite("size: %d (16-bit)",size);
				break;
			case TDS_TYPE_DATEN:
				// don't actually send a size for this type
				break;
			case TDS_TYPE_TIMEN:
			case TDS_TYPE_DATETIME2N:
			case TDS_TYPE_DATETIMEOFFSETN:
				// don't actually send a size for these types,
				// we'll send a scale below instead
				break;
			case TDS_TYPE_DATETIMN:
				// valid sizes for these are 4 and 8,
				// but sometimes size is reported as 0
				if (size!=4 && size!=8) {
					size=4;
				}
				write(&resppacket,(byte_t)size);
				debugWrite("size: %d (8-bit)",size);
				break;
			default:
				// limit the size to 2^7-1 because the
				// client will interpret it as signed
				if (size>127) {
					size=127;
				}
				write(&resppacket,(byte_t)size);
				debugWrite("size: %d (8-bit)",size);
				break;
		}

		// collation
		if (negotiatedtdsversion>=710) {
			switch (tdstype) {
				case TDS_TYPE_BIGCHAR:
				case TDS_TYPE_BIGVARCHR:
				case TDS_TYPE_TEXT:
				case TDS_TYPE_NTEXT:
				case TDS_TYPE_NCHAR:
				case TDS_TYPE_NVARCHAR:
					{
					// FIXME: collation...
					// send negotiated lcid?
					// for now, send "raw" collation
					byte_t	coll[5]={0,0,0,0,0};
					write(&resppacket,coll,sizeof(coll));
					if (getDebug()) {
						stringbuffer	b;
						b.printBits(coll,sizeof(coll));
						debugWrite(
							"collation: %s",
							b.getString());
					}
					}
					break;
			}
		}

		// precision
		switch (tdstype) {
			case TDS_TYPE_NUMERIC:
			case TDS_TYPE_NUMERICN:
			case TDS_TYPE_DECIMAL:
			case TDS_TYPE_DECIMALN:
				write(&resppacket,(byte_t)precision);
				debugWrite("precision: %d",precision);
				break;
		}

		// scale
		switch (tdstype) {
			case TDS_TYPE_NUMERIC:
			case TDS_TYPE_NUMERICN:
			case TDS_TYPE_DECIMAL:
			case TDS_TYPE_DECIMALN:
			case TDS_TYPE_TIMEN:
			case TDS_TYPE_DATETIME2N:
			case TDS_TYPE_DATETIMEOFFSETN:
				write(&resppacket,(byte_t)scale);
				debugWrite("scale: %d",scale);
				break;
		}

	} else if (isPartLenType(tdstype)) {

		debugWrite("partlentype...");

		// FIXME: [ushortmaxlen] [collation] [xml_info] [utd_info]
	}
}

void sqlrprotocol_tds::tableName(byte_t tdstype) {

	if (tdstype!=TDS_TYPE_TEXT &&
			tdstype!=TDS_TYPE_NTEXT &&
			tdstype!=TDS_TYPE_IMAGE) {
		return;
	}

	// It's not really clear what this is...
	// We only send it for text, ntext, and image columns.  It's called
	// "table" name but it appears to be a list of "part names".  I assume
	// they are partition names, but why would the client need to know
	// about those, and how do we get them?

	// FIXME: how do we get this?
	byte_t	numparts=1;

	// The spec is confusing about this, but it appears that 7.1- only
	// supports 1 partname, while 7.2+ supports more than 1, and you have
	// to tell it how many you're going to send.
	if (negotiatedtdsversion<720) {
		numparts=1;
	} else {
		write(&resppacket,numparts);
	}

	for (uint16_t i=0; i<numparts; i++) {
		const char	*partname8="";
		uint16_t	partnamelen=charstring::getLength(partname8);
		ucs2_t		*partname=ucs2charstring::duplicate(
							partname8,
							(size_t)partnamelen);
		write(&resppacket,partnamelen);
		write(&resppacket,partname,partnamelen);
		delete[] partname;

		debugWrite("part name: %s",partname8);
	}
}

void sqlrprotocol_tds::cryptoMetaData() {

	if (negotiatedtdsversion<740) {
		return;
	}

	// FIXME: The client doesn't seem to care that this isn't
	// being sent.  How do we decide when to send it?

	// FIXME: actually implement this...

#if 0
	// 0-based index of encryption key info location in cektable
	uint16_t	ordinal;

	// usertype (from above?)
	// basetypeinfo (call typeInfo)

	// 0     - custom (algonamelen/algoname required)
	// 1     - AEAD_AES_256_CHC_HMAC_SHA512
	// other - ???
	byte_t		encryptionalgo;

	byte_t		algonamelen;
	uint16_t	*algoname;

	// 1 - deterministic
	// 2 - randomized
	byte_t		encryptionalgotype;

	// ??? starts at 1
	byte_t		normversion;
#endif
}

void sqlrprotocol_tds::colName(sqlrservercursor *cursor,
						uint16_t col) {

	size_t 		namelen=cont->getColumnNameSize(cursor,col);
	const char	*name=cont->getColumnName(cursor,col);
	ucs2_t		*name16=ucs2charstring::duplicate(name,namelen);
	write(&resppacket,(byte_t)namelen);
	write(&resppacket,name16,namelen);

	debugWrite("namelen: %d",namelen);
	debugWrite("name: %s",name);

	delete[] name16;
}

bool sqlrprotocol_tds::isCaseSensitiveType(byte_t tdstype) {

	switch (tdstype) {
		case TDS_TYPE_CHAR:
		case TDS_TYPE_VARCHAR:
		case TDS_TYPE_BIGVARCHR:
		case TDS_TYPE_BIGCHAR:
		case TDS_TYPE_NVARCHAR:
		case TDS_TYPE_NCHAR:
		case TDS_TYPE_TEXT:
		case TDS_TYPE_NTEXT:
			// FIXME: should only be 1 when
			// binary collation is used
			return true;
		case TDS_TYPE_XML:
			return true;
		default:
			return false;
	}
}

bool sqlrprotocol_tds::isFixedLenType(byte_t tdstype) {

	switch (tdstype) {
		case TDS_TYPE_NULL:
		case TDS_TYPE_INT1:
		case TDS_TYPE_BIT:
		case TDS_TYPE_INT2:
		case TDS_TYPE_INT4:
		case TDS_TYPE_DATETIM4:
		case TDS_TYPE_FLT4:
		case TDS_TYPE_MONEY:
		case TDS_TYPE_DATETIME:
		case TDS_TYPE_FLT8:
		case TDS_TYPE_MONEY4:
		case TDS_TYPE_INT8:
			return true;
		default:
			return false;
	}
}

bool sqlrprotocol_tds::isVarLenType(byte_t tdstype) {

	switch (tdstype) {
		case TDS_TYPE_GUID:
		case TDS_TYPE_INTN:
		case TDS_TYPE_DECIMAL:
		case TDS_TYPE_NUMERIC:
		case TDS_TYPE_BITN:
		case TDS_TYPE_DECIMALN:
		case TDS_TYPE_NUMERICN:
		case TDS_TYPE_FLTN:
		case TDS_TYPE_MONEYN:
		case TDS_TYPE_DATETIMN:
		case TDS_TYPE_DATEN:
		case TDS_TYPE_TIMEN:
		case TDS_TYPE_DATETIME2N:
		case TDS_TYPE_DATETIMEOFFSETN:
		case TDS_TYPE_CHAR:
		case TDS_TYPE_VARCHAR:
		case TDS_TYPE_BINARY:
		case TDS_TYPE_VARBINARY:
		case TDS_TYPE_BIGVARBIN:
		case TDS_TYPE_BIGVARCHR:
		case TDS_TYPE_BIGBINARY:
		case TDS_TYPE_BIGCHAR:
		case TDS_TYPE_NVARCHAR:
		case TDS_TYPE_NCHAR:
		case TDS_TYPE_XML:
		case TDS_TYPE_UDT:
		case TDS_TYPE_TEXT:
		case TDS_TYPE_IMAGE:
		case TDS_TYPE_NTEXT:
		case TDS_TYPE_SSVARIANT:
			return true;
		default:
			return false;
	}
}

bool sqlrprotocol_tds::isPartLenType(byte_t tdstype) {

	switch (tdstype) {
		case TDS_TYPE_XML:
		case TDS_TYPE_BIGVARCHR:
		case TDS_TYPE_BIGVARBIN:
		case TDS_TYPE_NVARCHAR:
		case TDS_TYPE_UDT:
			return true;
		default:
			return false;
	}
}

uint64_t sqlrprotocol_tds::rows(sqlrservercursor *cursor) {

	// get col count and bail if there are no columns
	uint32_t	colcount=cont->colCount(cursor);
	if (!colcount) {
		// return the affected row count though
		return cont->getAffectedRows(cursor);
	}

	// for each row...
	uint64_t	rowcount=0;
	for (;;) {

		// fetch a row
		bool	error;
		if (!cont->fetchRow(cursor,&error)) {
			if (error) {
				// FIXME: handle error
			}
			break;
		}

		// append the token to the packet
		byte_t	token=TOKEN_ROW;
		write(&resppacket,token);

		debugStart("row");
		debugWrite("token: 0x%02x",token);

		// append the fields to the packet
		for (uint32_t col=0; col<colcount; col++) {

			// get/map the column type
			// FIXME: cache this earlier and just look it up here
			byte_t	tdstype=
				mapType(cont->getColumnType(cursor,col));

			debugStart("col %d",col);
			debugWrite("tdstype: 0x%02x",tdstype);

			lobData(tdstype);

			// get the field
			const char	*fld=NULL;
			uint64_t	fldsize=0;
			bool		lob=false;
			bool		null=false;
			if (!cont->getField(cursor,col,
					&fld,&fldsize,&lob,&null)) {
				// FIXME: handle error
			}

			// send the field
			field(tdstype,
				// FIXME: cache this earlier and
				// just look it up here
				cont->getColumnSize(cursor,col),
				fld,fldsize,null);

			debugEnd();
		}

		// FIXME: kludgy
		cont->nextRow(cursor);

		debugEnd();

		// bump row count
		rowcount++;
	}

	return rowcount;
}

void sqlrprotocol_tds::lobData(byte_t tdstype) {

	if (tdstype!=TDS_TYPE_TEXT &&
			tdstype!=TDS_TYPE_NTEXT &&
			tdstype!=TDS_TYPE_IMAGE) {
		return;
	}

	// I have no idea what these are or how to get them.  SQL Server itself
	// appears to send dummy versions of them though, so we'll do the same.

	// dummy textpointer
	const char	*textptr="dummy textptr   ";
	byte_t		textptrsize=charstring::getLength(textptr);
	write(&resppacket,textptrsize);
	write(&resppacket,textptr,textptrsize);

	// dummy timestamp
	const char	*ts="dummyTS";
	write(&resppacket,ts,8);

	debugWrite("textptrsize: %d",textptrsize);
	debugWrite("textptr: %s",textptr);
	debugWrite("ts: %s",ts);
}

void sqlrprotocol_tds::field(byte_t tdstype,
				uint32_t colsize,
				const char *field,
				uint64_t fieldsize,
				bool null) {

	// handle nulls
	if (null) {

		debugWrite("data: null");

		switch (tdstype) {
			case TDS_TYPE_NULL:
			case TDS_TYPE_GUID:
			case TDS_TYPE_INTN:
			case TDS_TYPE_BITN:
			case TDS_TYPE_DECIMALN:
			case TDS_TYPE_NUMERICN:
			case TDS_TYPE_FLTN:
			case TDS_TYPE_MONEYN:
			case TDS_TYPE_DATETIMN:
			case TDS_TYPE_DATEN:
			case TDS_TYPE_TIMEN:
			case TDS_TYPE_DATETIME2N:
			case TDS_TYPE_DATETIMEOFFSETN:
				write(&resppacket,(byte_t)0x00);
				break;
			case TDS_TYPE_CHAR:
			case TDS_TYPE_VARCHAR:
			case TDS_TYPE_BINARY:
			case TDS_TYPE_VARBINARY:
				write(&resppacket,(byte_t)0xFF);
				break;
			case TDS_TYPE_BIGBINARY:
			case TDS_TYPE_BIGVARBIN:
			case TDS_TYPE_BIGCHAR:
			case TDS_TYPE_BIGVARCHR:
			case TDS_TYPE_NCHAR:
			case TDS_TYPE_NVARCHAR:
				write(&resppacket,(uint16_t)0xFFFF);
				break;
			case TDS_TYPE_UDT:
				// FIXME: ???
				break;
			case TDS_TYPE_XML:
			case TDS_TYPE_TEXT:
			case TDS_TYPE_NTEXT:
			case TDS_TYPE_IMAGE:
			case TDS_TYPE_SSVARIANT:
				write(&resppacket,(uint32_t)0xFFFFFFFF);
				break;
		}

		return;
	}

	// handle variable size types by appending the size, then
	// changing the type so the switch below will append the data
	switch (tdstype) {
		case TDS_TYPE_INTN:
			{
			byte_t	size=colsize;
			write(&resppacket,size);
			switch (size) {
				case 1:
					tdstype=TDS_TYPE_INT1;
					break;
				case 2:
					tdstype=TDS_TYPE_INT2;
					break;
				case 4:
					tdstype=TDS_TYPE_INT4;
					break;
				case 8:
					tdstype=TDS_TYPE_INT8;
					break;
			}
			}
			break;
		case TDS_TYPE_BITN:
			write(&resppacket,(byte_t)1);
			tdstype=TDS_TYPE_BIT;
			break;
		case TDS_TYPE_FLTN:
			{
			byte_t	size=colsize;
			write(&resppacket,size);
			switch (size) {
				case 4:
					tdstype=TDS_TYPE_FLT4;
					break;
				case 8:
					tdstype=TDS_TYPE_FLT8;
					break;
			}
			}
			break;
		case TDS_TYPE_MONEYN:
			{
			byte_t	size=colsize;
			write(&resppacket,size);
			switch (size) {
				case 4:
					tdstype=TDS_TYPE_MONEY4;
					break;
				case 8:
					tdstype=TDS_TYPE_MONEY;
					break;
			}
			}
			break;
		case TDS_TYPE_DATETIMN:
			{
			byte_t	size=colsize;
			write(&resppacket,size);
			switch (size) {
				case 4:
					tdstype=TDS_TYPE_DATETIM4;
					break;
				case 8:
					tdstype=TDS_TYPE_DATETIME;
					break;
			}
			}
			break;
	}

	// append the data
	switch (tdstype) {
		case TDS_TYPE_INT1:
		case TDS_TYPE_BIT:
			{
			char	data=charstring::convertToInteger(field);
			write(&resppacket,data);
			debugWrite("data: ");
			debugWrite("%d (1 byte)",data);
			}
			break;
		case TDS_TYPE_INT2:
			{
			int16_t	data=charstring::convertToInteger(field);
			write(&resppacket,hostToLE((uint16_t)data));
			debugWrite("data: ");
			debugWrite("%hd (2 bytes)",data);
			}
			break;
		case TDS_TYPE_INT4:
			{
			int32_t	data=charstring::convertToInteger(field);
			write(&resppacket,hostToLE((uint32_t)data));
			debugWrite("data: ");
			debugWrite("%ld (4 bytes)",data);
			}
			break;
		case TDS_TYPE_DATETIM4:
			{
			int32_t		dayssince1900;
			uint32_t	threehundredths;
			dateTime(field,&dayssince1900,&threehundredths);
			uint16_t	days=(dayssince1900>0)?
							dayssince1900:0;
			uint16_t	minutes=threehundredths/300/60;
			write(&resppacket,hostToLE(days));
			write(&resppacket,hostToLE(minutes));
			debugWrite("data: ");
			debugWrite("%d,%d",(uint32_t)days,(uint32_t)minutes);
			}
			break;
		case TDS_TYPE_FLT4:
			{
			float	data=charstring::convertToFloat(field);
			write(&resppacket,data);
			debugWrite("data: ");
			debugWrite("%f",data);
			}
			break;
		case TDS_TYPE_MONEY:
			{
			char	*copy=charstring::duplicate(field);
			charstring::strip(copy,'.');
			int64_t	data=charstring::convertToInteger(copy)*100;
			delete[] copy;
			write(&resppacket,
				hostToLE(
				(uint32_t)((data&0xFFFFFFFF00000000LL)>>32)));
			write(&resppacket,
				hostToLE(
				(uint32_t)(data&0x00000000FFFFFFFFLL)));
			debugWrite("data: ");
			debugWrite("%ld %ld (%s)",
				(uint32_t)((data&0xFFFFFFFF00000000LL)>>32),
				(uint32_t)(data&0x00000000FFFFFFFFLL),
				field);
			}
			break;
		case TDS_TYPE_DATETIME:
			{
			int32_t		dayssince1900;
			uint32_t	threehundredths;
			dateTime(field,&dayssince1900,&threehundredths);
			write(&resppacket,hostToLE((uint32_t)dayssince1900));
			write(&resppacket,hostToLE(threehundredths));
			debugWrite("data: ");
			debugWrite("%d,%d",dayssince1900,threehundredths);
			}
			break;
		case TDS_TYPE_FLT8:
			{
			double	data=charstring::convertToFloat(field);
			write(&resppacket,data);
			debugWrite("data: ");
			debugWrite("%f",data);
			}
			break;
		case TDS_TYPE_MONEY4:
			{
			char	*copy=charstring::duplicate(field);
			charstring::strip(copy,'.');
			int32_t	data=charstring::convertToInteger(copy)*100;
			delete[] copy;
			write(&resppacket,hostToLE((uint32_t)data));
			debugWrite("data: ");
			debugWrite("%lld (%s)",data,field);
			}
			break;
		case TDS_TYPE_INT8:
			{
			int64_t	data=charstring::convertToInteger(field);
			write(&resppacket,hostToLE((uint64_t)data));
			debugWrite("data: ");
			debugWrite("%lld (8 bytes)",data);
			}
			break;
		case TDS_TYPE_GUID:
			{
			byte_t	g[16];
			guid(field,g);
			if (getDebug()) {
				debugWrite("data: ");
				for (uint16_t i=0; i<16; i++) {
					debugWrite("%02x",g[i]);
				}
			}
			write(&resppacket,(byte_t)sizeof(g));
			write(&resppacket,g,sizeof(g));
			}
			break;
		case TDS_TYPE_DECIMAL:
		case TDS_TYPE_NUMERIC:
		case TDS_TYPE_DECIMALN:
		case TDS_TYPE_NUMERICN:
			{
			byte_t	ispositive;
			byte_t	size;
			byte_t	val[16];
			decimal(field,&ispositive,&size,val);
			if (tdstype==TDS_TYPE_DECIMALN ||
				tdstype==TDS_TYPE_NUMERICN) {
				write(&resppacket,size);
				write(&resppacket,ispositive);
				write(&resppacket,val,size-1);
			} else {
				write(&resppacket,ispositive);
				write(&resppacket,val,size);
			}
			debugWrite("data: ");
			debugWrite("%d ",ispositive);
			switch (size) {
				case 4: {
					uint32_t	*v=(uint32_t *)val;
					debugWrite("%d ",*v);
					break;
					}
				case 8: {
					uint64_t	*v=(uint64_t *)val;
					debugWrite("%lld ",*v);
					break;
					}
				case 12:
					debugWrite("... ");
					break;
				case 16:
					debugWrite("... ");
					break;
			}
			debugWrite("(%s %d)",field,size);
			}
			break;
		case TDS_TYPE_DATEN:
			daten(field);
			break;
		case TDS_TYPE_TIMEN:
			timen(field);
			break;
		case TDS_TYPE_DATETIME2N:
			timen(field);
			daten(field);
			break;
		case TDS_TYPE_DATETIMEOFFSETN:
			timen(field);
			daten(field);
			// FIXME:
			// int16_t - timezone offset - minutes from utc
			// 				(between -840 and 840)
			write(&resppacket,(uint16_t)0);
			break;
		case TDS_TYPE_CHAR:
		case TDS_TYPE_VARCHAR:
			{
			write(&resppacket,(byte_t)fieldsize);
			write(&resppacket,field,fieldsize);
			debugWrite("size: %d",fieldsize);
			debugWrite("data: ");
			debugWrite("%.*s",fieldsize,field);
			}
			break;
		case TDS_TYPE_BINARY:
		case TDS_TYPE_VARBINARY:
			{
			// FIXME: TDS backends encode these as text, where
			// each pair of characters are the hex value of a
			// byte.  However, other databases may encode them
			// differently.
			write(&resppacket,(byte_t)(fieldsize/2));
			const char	*f=field;
			for (byte_t i=0; i<fieldsize/2; i++) {
				write(&resppacket,charsToHex(f));
				f+=2;
			}
			debugWrite("size: %d",fieldsize);
			debugWrite("data:");
			debugHexDump((byte_t *)field,fieldsize);
			}
			break;
		case TDS_TYPE_BIGBINARY:
		case TDS_TYPE_BIGVARBIN:
			{
			// FIXME: TDS backends encode these as text, where
			// each pair of characters are the hex value of a
			// byte.  However, other databases may encode them
			// differently.
			write(&resppacket,hostToLE((uint16_t)(fieldsize/2)));
			const char	*f=field;
			for (uint16_t i=0; i<(fieldsize/2); i++) {
				write(&resppacket,charsToHex(f));
				f+=2;
			}
			debugWrite("size: %d",fieldsize);
			debugWrite("data:");
			debugHexDump((byte_t *)field,fieldsize);
			}
			break;
		case TDS_TYPE_BIGCHAR:
		case TDS_TYPE_BIGVARCHR:
		case TDS_TYPE_NCHAR:
		case TDS_TYPE_NVARCHAR:
			{
			write(&resppacket,hostToLE((uint16_t)fieldsize));
			write(&resppacket,field,fieldsize);
			debugWrite("size: %d",fieldsize);
			debugWrite("data: ");
			debugWrite("%.*s",fieldsize,field);
			}
			break;
		case TDS_TYPE_UDT:
			// FIXME: ???
			break;
		case TDS_TYPE_XML:
		case TDS_TYPE_TEXT:
		case TDS_TYPE_NTEXT:
			{
			write(&resppacket,hostToLE((uint32_t)fieldsize));
			write(&resppacket,field,fieldsize);
			debugWrite("size: %d",fieldsize);
			debugWrite("data: ");
			debugWrite("%.*s",fieldsize,field);
			}
			break;
		case TDS_TYPE_IMAGE:
		case TDS_TYPE_SSVARIANT:
			{
			// FIXME: TDS backends encode these as text, where
			// each pair of characters are the hex value of a
			// byte.  However, other databases may encode them
			// differently.
			write(&resppacket,hostToLE((uint32_t)(fieldsize/2)));
			const char	*f=field;
			for (uint32_t i=0; i<fieldsize/2; i++) {
				write(&resppacket,charsToHex(f));
				f+=2;
			}
			debugWrite("size: %d",fieldsize);
			debugWrite("data:");
			debugHexDump((byte_t *)field,fieldsize);
			}
			break;
	}
}

static uint16_t mdays[]={31,28,31,30,31,30,31,31,30,31,30,31};

void sqlrprotocol_tds::dateTime(const char *datetime,
					int32_t *dayssince1900,
					uint32_t *threehundredths) {

	// parse the date/time
	int16_t	year;
	int16_t	month;
	int16_t	day;
	int16_t	hour;
	int16_t	minute;
	int16_t	second;
	int32_t	usec;
	bool	isnegative;
	// FIXME: set ddmm and yyyyddmm somehow
	datetime::parse(datetime,false,false,"/-.:",
				&year,&month,&day,
				&hour,&minute,&second,
				&usec,&isnegative);

	// calculate days since 1900
	*dayssince1900=((year-1900)*365);
	for (uint16_t i=0; i<month-1; i++) {
		(*dayssince1900)+=mdays[i];
	}
	(*dayssince1900)+=(day-1);

	// add leap years between 1900 and the specified year
	// * years divisible by 4 are leap years
	//  * unless they are divisible by 100
	//   * unless they are divisible by 400
	for (uint16_t i=1900; i<year; i++) {
		if (i%4) {
			// common year
		} else if (i%100) {
			// leap year
			(*dayssince1900)++;
		} else if (i%400) {
			// common year
		} else {
			// leap year
			(*dayssince1900)++;
		}
		/*if (!(i%4)) {
			if (!(i%100)) {
				if (!(i%400)) {
					(*dayssince1900)++;
				}
			} else {
				(*dayssince1900)++;
			}
		}*/
	}

	// if the specified year is a leap year...
	if (month>2) {
		if (year%4) {
			// common year
		} else if (year%100) {
			// leap year
			(*dayssince1900)++;
		} else if (year%400) {
			// common year
		} else {
			// leap year
			(*dayssince1900)++;
		}
		/*if (!(year%4)) {
			if (year%100) {
				if (!(year%400)) {
					(*dayssince1900)++;
				}
			} else {
				(*dayssince1900)++;
			}
		}*/
	} else if (month==2 && day==29) {
		(*dayssince1900)++;
	}

	// FIXME: there's got to be a less iterative way to do leap years

	// calculate three-hundredths of a second since 12AM
	*threehundredths=((hour*60*60+minute*60+second)*300)+(usec*3/10000);

	debugStart("datetime");
	debugWrite("string: %s",datetime);
	debugWrite("year: %d",year);
	debugWrite("month: %d",month);
	debugWrite("day: %d",day);
	debugWrite("hour: %d",hour);
	debugWrite("minute: %d",minute);
	debugWrite("second: %d",second);
	debugWrite("usec: %d",usec);
	debugWrite("isnegative: %d",isnegative);
	debugWrite("days since 1900: %d",*dayssince1900);
	debugWrite("300ths since 12AM: %d",*threehundredths);
	debugEnd();
}

void sqlrprotocol_tds::date(const char *datetime, uint16_t *dayssince1) {

	// parse the date/time
	int16_t	year;
	int16_t	month;
	int16_t	day;
	int16_t	hour;
	int16_t	minute;
	int16_t	second;
	int32_t	usec;
	bool	isnegative;
	// FIXME: set ddmm and yyyyddmm somehow
	datetime::parse(datetime,false,false,"/-.:",
				&year,&month,&day,
				&hour,&minute,&second,
				&usec,&isnegative);

	// calculate days since 1
	*dayssince1=((year-1)*365);
	for (uint16_t i=0; i<month-1; i++) {
		*dayssince1=mdays[i];
	}
	// FIXME: this gregorian adjustment isn't correct
	// Sept 3-13, 1752 don't exist
	if (year>1752) {
		*dayssince1-=11;
	}
	// FIXME: this leap year calc isn't correct
	// * years divisible by 4 are leap years
	//  * unless they are divisible by 100
	//   * unless they are divisible by 400
	*dayssince1+=day+((year-1)/4);

	debugStart("date");
	debugWrite("string: %s",datetime);
	debugWrite("year: %d",year);
	debugWrite("month: %d",month);
	debugWrite("day: %d",day);
	debugWrite("hour: %d",hour);
	debugWrite("minute: %d",minute);
	debugWrite("second: %d",second);
	debugWrite("usec: %d",usec);
	debugWrite("isnegative: %d",isnegative);
	debugWrite("days since 1: %d",*dayssince1);
	debugEnd();
}

void sqlrprotocol_tds::daten(const char *field) {
	uint16_t	dayssince1;
	date(field,&dayssince1);
	write(&resppacket,(byte_t)2);
	write(&resppacket,hostToLE(dayssince1));
	debugWrite("data: ");
	debugWrite("%hd",dayssince1);
}

void sqlrprotocol_tds::timen(const char *field) {
	// FIXME: actually implement this
	// 1 unsigned integer - number of 10^-n second
	// 			increments since 12 am
	// 			within a day.
	// 3 bytes if 0 <= n <= 2
	// 4 bytes if 3 <= n <= 4
	// 5 bytes if 5 <= n <= 7
	byte_t	size=3;
	write(&resppacket,size);
	byte_t	bytes[3]={0,0,0};
	write(&resppacket,bytes,sizeof(bytes));
	debugWrite("data: ...");
}

void sqlrprotocol_tds::decimal(const char *field,
				byte_t *ispositive,
				byte_t *size,
				byte_t *val) {

	uint32_t	precision=charstring::getLength(field);

	*ispositive=1;
	if (field[0]=='-') {
		*ispositive=0;
		precision--;
	}

	char	*copy=charstring::duplicate(field);
	if (charstring::contains(copy,'.')) {
		charstring::strip(copy,'.');
		precision--;
	}

	if (precision>=1 && precision<=9) {
		*size=4;
		int32_t	v=charstring::convertToInteger((*ispositive)?copy:copy+1);
		v=hostToLE((uint32_t)v);
		bytestring::copy(val,&v,sizeof(v));
	} else if (precision>=10 && precision<=19) {
		*size=8;
		int64_t	v=charstring::convertToInteger((*ispositive)?copy:copy+1);
		v=hostToLE((uint64_t)v);
		bytestring::copy(val,&v,sizeof(v));
	} else if (precision>=20 && precision<=28) {
		*size=12;
		// FIXME: actually implement this...
	} else if (precision>=29 && precision<=38) {
		*size=16;
		// FIXME: actually implement this...
	}

	delete[] copy;
}

void sqlrprotocol_tds::guid(const char *field, byte_t *g) {

	// convert string into 16 hex values...
	for (uint16_t i=0; i<16; i++) {
		if (*field=='-') {
			field++;
		}
		g[i]=charsToHex(field);
		field+=2;
	}

	// swap first 4 bytes (apparently)
	byte_t	tmp=g[0];
	g[0]=g[3];
	g[3]=tmp;
	tmp=g[1];
	g[1]=g[2];
	g[2]=tmp;

	// swap next 2 bytes (apparently)
	tmp=g[4];
	g[4]=g[5];
	g[5]=tmp;

	// swap next 2 bytes (apparently)
	tmp=g[6];
	g[6]=g[7];
	g[7]=tmp;

	// leave the rest alone (apparently)
}

byte_t sqlrprotocol_tds::charsToHex(const char *chars) {

	// FIXME: this method is really brute-force...

	byte_t	sixteens=0;
	byte_t	ones=0;

	char	ch=*chars;
	if (ch) {
		if (ch>='A' && ch<='F') {
			sixteens=ch-'A'+10;
		} else if (ch>='a' && ch<='f') {
			sixteens=ch-'a'+10;
		} else if (ch>='0' && ch<='9') {
			sixteens=ch-'0';
		}
	}

	chars++;

	ch=*chars;
	if (ch) {
		char	ch=*chars;
		if (ch>='A' && ch<='F') {
			ones=ch-'A'+10;
		} else if (ch>='a' && ch<='f') {
			ones=ch-'a'+10;
		} else if (ch>='0' && ch<='9') {
			ones=ch-'0';
		}
	}

	return sixteens*16+ones;
}

void sqlrprotocol_tds::sqlBatchError(sqlrservercursor *cursor) {

	// get the error
	const char	*errorstring;
	uint32_t	errorsize;
	int64_t		errorcode;
	bool		liveconnection;
	cont->getError(cursor,&errorstring,
				&errorsize,
				&errorcode,
				&liveconnection);
	char	*errorbuffer=charstring::duplicate(errorstring,errorsize);
	char	*errptr=errorbuffer;

	// if the server is mssql/sap then parse the various parts
	// out of the errorbuffer, which looks like:
	// Server message: ... severity(...) number(...) state(...) line(...)
	// Server Name:... Procedure Name:...
	// (note 2 spaces between line(...) and Server Name and no spaces after
	// the colons after Server Name and Procedure Name)
	byte_t		state=1;
	byte_t		errclass=0;
	uint32_t	linenumber=1;
	char		*srvn=NULL;
	char		*procn=NULL;
	char		*severityptr=NULL;
	char		*procptr=NULL;
	if (dbistds &&
		!charstring::compare(errorbuffer,"Server message: ",16)) {

		errptr=errorbuffer+16;

		severityptr=charstring::findFirst(errptr," severity(");
		if (severityptr) {
			errclass=charstring::convertToInteger(severityptr+10);
		}

		char	*stateptr=charstring::findFirst(errptr," state(");
		if (stateptr) {
			state=charstring::convertToInteger(stateptr+7);
		}

		char	*lineptr=charstring::findFirst(errptr," line(");
		if (lineptr) {
			linenumber=charstring::convertToInteger(lineptr+6);
		}

		char	*srvptr=charstring::findFirst(errptr," Server Name:");
		procptr=charstring::findFirst(errptr,"  Procedure Name:");
		if (srvptr && procptr) {
			*procptr='\0';
			srvn=charstring::duplicate(srvptr+13);
			procn=charstring::duplicate(procptr+17);
		}

		*severityptr='\0';
	}

	// append the error to the send packet
	error(errorcode,
		state,errclass,
		errptr,(srvn)?srvn:srvname,procn,
		linenumber);

	// reset nulls to spaces
	if (severityptr) {
		*severityptr=' ';
	}
	if (procptr) {
		*procptr=' ';
	}

	// clean up
	delete[] srvn;
	delete[] procn;
	delete[] errorbuffer;
}

bool sqlrprotocol_tds::bulkLoad(sqlrservercursor *cursor) {

	//const byte_t	*rp=reqpacket.getBuffer();

	debugStart("bulk load");
	debugEnd();

	return false;
}

bool sqlrprotocol_tds::remoteProcedureCall(sqlrservercursor *cursor) {

	const byte_t	*rp=reqpacket.getBuffer();
	size_t		rpsize=reqpacket.getSize();
	stringbuffer	query;

	debugStart("rpc");


	// get the headers
	if (negotiatedtdsversion>=720) {
		allHeaders(rp,rpsize,&rp,&rpsize);
	}


	// get proc name/id
	uint16_t	procnamelen=0;
	ucs2_t		*procname16=NULL;
	char		*procname=NULL;
	uint16_t	procid=0;

	readLE(rp,&procnamelen,&rp);
	rpsize-=sizeof(procnamelen);

	if (procnamelen==0xFFFF) {

		// get the proc id
		readLE(rp,&procid,&rp);
		rpsize-=sizeof(procid);

		debugWrite("procid: %hd (%s)",
				procid,procids[(procid<=15)?procid:0]);

	} else {

		// FIXME: validate procnamelen against maxquerysize

		// get the procname
		procname16=new ucs2_t[procnamelen];
		read(rp,procname16,procnamelen,&rp);
		rpsize-=procnamelen*sizeof(ucs2_t);
		procname=charstring::duplicateUcs2(procname16,
							(size_t)procnamelen);

		// build the query
		query.append("exec ")->append(procname);

		debugWrite("procname: %s",procname);
		debugWrite("query: %s",query.getString());

		// clean up
		delete[] procname16;
		delete[] procname;
	}


	// get option flags
	uint16_t	optionflags=0;
	readLE(rp,&optionflags,&rp);
	rpsize-=sizeof(optionflags);

	// parse the flags
	bool	withrecomp=(optionflags&0x0001);
	bool	nometadata=(optionflags&(0x0001<<2))>>2;
	bool	reusemetadata=(optionflags&(0x0001<<3))>>3;

	if (getDebug()) {
		stringbuffer	b;
		b.printBits(optionflags);
		debugWrite("optionflags: %s",b.getString());
		debugWrite("withrecomp: %d",withrecomp);
		debugWrite("nometadata: %d",nometadata);
		debugWrite("reusemetadata: %d",reusemetadata);
	}


	bool	retval=false;
	if (procname) {
		// prepare the query
		retval=cont->prepareQuery(cursor,
					query.getString(),query.getSize(),
					true,true,true);
	} else {
		// do whatever the procid asked for
		switch (procid) {
			case SP_CURSOR:
				// FIXME: actually implement this
				break;
			case SP_CURSOR_OPEN:
				// FIXME: actually implement this
				break;
			case SP_CURSOR_PREPARE:
				// FIXME: actually implement this
				break;
			case SP_CURSOR_EXECUTE:
				// FIXME: actually implement this
				break;
			case SP_CURSOR_PREP_EXEC:
				// FIXME: actually implement this
				break;
			case SP_CURSOR_UNPREPARE:
				// FIXME: actually implement this
				break;
			case SP_CURSOR_FETCH:
				// FIXME: actually implement this
				break;
			case SP_CURSOR_OPTION:
				// FIXME: actually implement this
				break;
			case SP_CURSOR_CLOSE:
				// FIXME: actually implement this
				break;
			case SP_EXECUTE_SQL:
				// FIXME: actually implement this
				break;
			case SP_PREPARE:
				// FIXME: actually implement this
				break;
			case SP_EXECUTE:
				// FIXME: actually implement this
				break;
			case SP_PREP_EXEC:
				// FIXME: actually implement this
				break;
			case SP_PREP_EXEC_RPC:
				// FIXME: actually implement this
				break;
			case SP_UNPREPARE:
				// FIXME: actually implement this
				break;
		}
	}


	// apparently the packet can end here if there
	// are no parameters or trailing flags
	if (retval && rpsize) {

		if (!params(cursor,rp,rpsize,&rp)) {
			tdsProtocolError();
			done();
			retval=false;
		}

		// FIXME:
		// *((BatchFlag|NoExecFlag)RPCReqBatch)
		// [BatchFlag|NoExecFlag]
	}

	debugEnd();


	if (retval) {

		// begin building the response packet
		resppacket.clear();

		if (procname) {

			// execute the query
			bool	success=cont->executeQuery(cursor);

			// build the response packet
			if (success) {
				doneInProc(DONE_MORE|DONE_COUNT,0,
						cont->getAffectedRows(cursor));
				returnStatus(cursor);
				returnValues(cursor);
				doneProc(DONE_FINAL,0,0);
			} else {
				sqlBatchError(cursor);
				done();
			}

		} else {

			// do whatever the procid asked for
			switch (procid) {
				case SP_CURSOR:
					// FIXME: actually implement this
					break;
				case SP_CURSOR_OPEN:
					// FIXME: actually implement this
					break;
				case SP_CURSOR_PREPARE:
					// FIXME: actually implement this
					break;
				case SP_CURSOR_EXECUTE:
					// FIXME: actually implement this
					break;
				case SP_CURSOR_PREP_EXEC:
					// FIXME: actually implement this
					break;
				case SP_CURSOR_UNPREPARE:
					// FIXME: actually implement this
					break;
				case SP_CURSOR_FETCH:
					// FIXME: actually implement this
					break;
				case SP_CURSOR_OPTION:
					// FIXME: actually implement this
					break;
				case SP_CURSOR_CLOSE:
					// FIXME: actually implement this
					break;
				case SP_EXECUTE_SQL:
					// FIXME: actually implement this
					break;
				case SP_PREPARE:
					// FIXME: actually implement this
					break;
				case SP_EXECUTE:
					// FIXME: actually implement this
					break;
				case SP_PREP_EXEC:
					// FIXME: actually implement this
					break;
				case SP_PREP_EXEC_RPC:
					// FIXME: actually implement this
					break;
				case SP_UNPREPARE:
					// FIXME: actually implement this
					break;
			}
		}

		// send the response packet
		retval=sendPacket();
	}

	return retval;
}

bool sqlrprotocol_tds::params(sqlrservercursor *cursor,
					const byte_t *rp,
					size_t rpsize,
					const byte_t **rpout) {

	sqlrserverbindvar	*inbinds=cont->getInputBinds(cursor);
	sqlrserverbindvar	*outbinds=cont->getOutputBinds(cursor);
	uint16_t		inbindcount=0;
	uint16_t		outbindcount=0;

	bool		exceededinbind=false;
	bool		exceededoutbind=false;
	const byte_t	*newrp;
	while (rpsize) {

		if (!param(cursor,&inbindcount,&outbindcount,
				inbinds,outbinds,rp,&newrp,
				exceededinbind,exceededoutbind)) {
			// protocol error
			return false;
		}

		if (inbindcount==maxbindcount) {
			exceededinbind=true;
		}
		if (outbindcount==maxbindcount) {
			exceededoutbind=true;
		}

		rpsize-=newrp-rp;
		rp=newrp;

		// FIXME: It's not 100% clear how we know when there
		// are no more parameters.  For now we're bailing when
		// we've read all of the packet, but apparently some
		// flags can follow it, so there must be some other way.
	}

	cont->setInputBindCount(cursor,inbindcount);
	cont->setOutputBindCount(cursor,outbindcount);

	*rpout=rp;

	return true;
}

bool sqlrprotocol_tds::param(sqlrservercursor *cursor,
					uint16_t *inbindcount,
					uint16_t *outbindcount,
					sqlrserverbindvar *inbinds,
					sqlrserverbindvar *outbinds,
					const byte_t *rp,
					const byte_t **rpout,
					bool exceededinbind,
					bool exceededoutbind) {

	// param name
	byte_t	pnamelen;
	read(rp,&pnamelen,&rp);
	ucs2_t	*pname16=NULL;
	char	*pname=NULL;
	if (pnamelen) {
		pname16=new ucs2_t[pnamelen];
		read(rp,pname16,pnamelen,&rp);
		pname=charstring::duplicateUcs2(pname16,(size_t)pnamelen);
	}


	// status flags
	byte_t	statusflags=0;
	read(rp,&statusflags,&rp);
	bool	byrefvalue=(statusflags&0x01);
	bool	defaultvalue=(statusflags&(0x01<<1))>>1;
	// this bit is reserved
	bool	encrypted=(statusflags&(0x01<<3))>>3;
	// these 4 bits are reserved


	// FIXME: do something if defaultvalue is set...
	// FIXME: support encryption


	// input or output bind...
	uint16_t		param=0;
	sqlrserverbindvar	*bv=NULL;
	if (!exceededinbind) {
		param=*inbindcount;
		bv=&(inbinds[param]);
	}
	if (byrefvalue && !exceededoutbind) {
		param=*outbindcount;
		bv=&(outbinds[param]);
	}


	// debug
	if (getDebug()) {
		debugStart("param %d",(bv)?param:-1);
		debugWrite("pnamelen: %d",pnamelen);
		debugWrite("pname: %s",pname);
		stringbuffer	b;
		b.printBits(statusflags);
		debugWrite("statusflags: %s",b.getString());
		debugWrite("byrefvalue: %d",byrefvalue);
		debugWrite("defaultvalue: %d",defaultvalue);
		debugWrite("encrypted: %d",encrypted);
	}


	// type info...
	byte_t	tdstype;
	read(rp,&tdstype,&rp);
	debugWrite("tdstype: 0x%02x",tdstype);

	uint32_t	maxsize=0;
	byte_t		precision=0;
	byte_t		scale=0;

	if (isFixedLenType(tdstype)) {

		debugWrite("fixedlentype...");

	} else if (isVarLenType(tdstype)) {

		debugWrite("varlentype...");

		// maxsize
		switch (tdstype) {
			case TDS_TYPE_SSVARIANT:
			case TDS_TYPE_TEXT:
			case TDS_TYPE_NTEXT:
			case TDS_TYPE_IMAGE:
				readLE(rp,&maxsize,&rp);
				debugWrite("maxsize: %d",maxsize);
				break;
			case TDS_TYPE_BIGCHAR:
			case TDS_TYPE_BIGVARCHR:
			case TDS_TYPE_NCHAR:
			case TDS_TYPE_NVARCHAR:
			case TDS_TYPE_BIGBINARY:
			case TDS_TYPE_BIGVARBIN:
				{
				uint16_t	size;
				readLE(rp,&size,&rp);
				maxsize=size;
				debugWrite("maxsize: %d",maxsize);
				}
				break;
			default:
				{
				byte_t	size;
				read(rp,&size,&rp);
				maxsize=size;
				debugWrite("maxsize: %d",maxsize);
				}
				break;
		}

		// precision
		switch (tdstype) {
			case TDS_TYPE_NUMERIC:
			case TDS_TYPE_NUMERICN:
			case TDS_TYPE_DECIMAL:
			case TDS_TYPE_DECIMALN:
				read(rp,&precision,&rp);
				debugWrite("precision: %d",precision);
				break;
		}

		// scale
		switch (tdstype) {
			case TDS_TYPE_NUMERIC:
			case TDS_TYPE_NUMERICN:
			case TDS_TYPE_DECIMAL:
			case TDS_TYPE_DECIMALN:
			case TDS_TYPE_TIMEN:
			case TDS_TYPE_DATETIME2N:
			case TDS_TYPE_DATETIMEOFFSETN:
				read(rp,&scale,&rp);
				debugWrite("scale: %d",scale);
				break;
		}

	} else if (isPartLenType(tdstype)) {

		debugWrite("partlentype...");

		// FIXME: [ushortmaxlen] [collation] [xml_info] [utd_info]
	} 

	// FIXME: use pname/pnamelen if pnamelen>0
	if (bv) {
		bv->variable=bindvarnames[param];
		bv->variablesize=charstring::getLength(bv->variable);
	}

	// param data...

	// FIXME: how do I detect nulls?

	// FIXME: handle output binds too

	// handle variable size types by getting the size, then
	// changing the type so the switch below will get the data
	switch (tdstype) {
		case TDS_TYPE_INTN:
			{
			byte_t	size;
			read(rp,&size,&rp);
			switch (size) {
				case 1:
					tdstype=TDS_TYPE_INT1;
					break;
				case 2:
					tdstype=TDS_TYPE_INT2;
					break;
				case 4:
					tdstype=TDS_TYPE_INT4;
					break;
				case 8:
					tdstype=TDS_TYPE_INT8;
					break;
			}
			}
			break;
		case TDS_TYPE_BITN:
			{
			byte_t	size;
			read(rp,&size,&rp);
			tdstype=TDS_TYPE_BIT;
			}
			break;
		case TDS_TYPE_FLTN:
			{
			byte_t	size;
			read(rp,&size,&rp);
			switch (size) {
				case 4:
					tdstype=TDS_TYPE_FLT4;
					break;
				case 8:
					tdstype=TDS_TYPE_FLT8;
					break;
			}
			}
			break;
		case TDS_TYPE_MONEYN:
			{
			byte_t	size;
			read(rp,&size,&rp);
			switch (size) {
				case 4:
					tdstype=TDS_TYPE_MONEY4;
					break;
				case 8:
					tdstype=TDS_TYPE_MONEY;
					break;
			}
			}
			break;
		case TDS_TYPE_DATETIMN:
			{
			byte_t	size;
			read(rp,&size,&rp);
			switch (size) {
				case 4:
					tdstype=TDS_TYPE_DATETIM4;
					break;
				case 8:
					tdstype=TDS_TYPE_DATETIME;
					break;
			}
			}
			break;
	}

	// get the collation
	switch (tdstype) {
		case TDS_TYPE_BIGCHAR:
		case TDS_TYPE_BIGVARCHR:
		case TDS_TYPE_TEXT:
		case TDS_TYPE_NTEXT:
		case TDS_TYPE_NCHAR:
		case TDS_TYPE_NVARCHAR:
			{
			// FIXME: do something with this
			byte_t	coll[5];
			read(rp,coll,sizeof(coll),&rp);
			if (getDebug()) {
				stringbuffer	b;
				b.printBits(coll,sizeof(coll));
				debugWrite("collation: %s",b.getString());
			}
			}
			break;
	}

	// get the data
	switch (tdstype) {
		case TDS_TYPE_INT1:
		case TDS_TYPE_BIT:
			{
			char	val;
			read(rp,(byte_t *)&val,&rp);

			if (bv) {
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				bv->valuesize=1;
				bv->isnull=cont->getNonNullBindValue();
				bv->value.integerval=val;
			}

			if (bv) {
				debugWrite("valuesize: %d",bv->valuesize);
				debugWrite("value: %lld",bv->value.integerval);
			}
			}
			break;
		case TDS_TYPE_INT2:
			{

			int16_t	val;
			readLE(rp,(uint16_t *)&val,&rp);

			if (bv) {
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				bv->valuesize=2;
				bv->isnull=cont->getNonNullBindValue();
				bv->value.integerval=val;
			}
	
			if (bv) {
				debugWrite("valuesize: %d",bv->valuesize);
				debugWrite("value: %lld",bv->value.integerval);
			}
			}
			break;
		case TDS_TYPE_INT4:
			{

			int32_t	val;
			readLE(rp,(uint32_t *)&val,&rp);

			if (bv) {
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				bv->valuesize=4;
				bv->isnull=cont->getNonNullBindValue();
				bv->value.integerval=val;
			}

			if (bv) {
				debugWrite("valuesize: %d",bv->valuesize);
				debugWrite("value: %lld",bv->value.integerval);
			}
			}
			break;
		case TDS_TYPE_DATETIM4:
			{
			uint16_t	days;
			uint16_t	minutes;
			readLE(rp,&days,&rp);
			readLE(rp,&minutes,&rp);
			// FIXME: actually implement this
			}
			break;
		case TDS_TYPE_FLT4:
			{

			float	val;
			read(rp,(float *)&val,&rp);

			if (bv) {
				bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
				bv->isnull=cont->getNonNullBindValue();
				bv->value.doubleval.value=val;

				// FIXME: kludgy
				char	*num=charstring::parseNumber(
						bv->value.doubleval.value);
				size_t	size=charstring::getLength(num);
				bv->value.doubleval.precision=size-
					(charstring::contains(num,'-')?1:0)-
					(charstring::contains(num,'.')?1:0);
				bv->value.doubleval.scale=
					(num+size)-
					charstring::findFirstOrEnd(num,'.')-1;
				delete[] num;
			}

			if (bv) {
				debugWrite("valuesize: %d",bv->valuesize);
				debugWrite("value: %f",bv->value.doubleval);
				debugWrite("precision: %d",
						bv->value.doubleval.
						precision);
				debugWrite("scale: %d",
						bv->value.doubleval.
						scale);
			}
			}
			break;
		case TDS_TYPE_MONEY:
			{
			uint32_t	high;
			uint32_t	low;
			readLE(rp,&high,&rp);
			readLE(rp,&low,&rp);
			// FIXME: actually implement this
			}
			break;
		case TDS_TYPE_DATETIME:
			{
			int32_t		dayssince1900;
			uint32_t	threehundredths;
			readLE(rp,(uint32_t *)&dayssince1900,&rp);
			readLE(rp,&threehundredths,&rp);
			// FIXME: actually implement this
			}
			break;
		case TDS_TYPE_FLT8:
			{

			double	val;
			read(rp,(double *)&val,&rp);

			if (bv) {
				bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
				bv->isnull=cont->getNonNullBindValue();
				bv->value.doubleval.value=val;

				// FIXME: kludgy
				char	*num=charstring::parseNumber(
						bv->value.doubleval.value);
				size_t	size=charstring::getLength(num);
				bv->value.doubleval.precision=size-
					(charstring::contains(num,'-')?1:0)-
					(charstring::contains(num,'.')?1:0);
				bv->value.doubleval.scale=
					(num+size)-
					charstring::findFirstOrEnd(num,'.')-1;
				delete[] num;
			}

			if (bv) {
				debugWrite("valuesize: %d",bv->valuesize);
				debugWrite("value: %f",bv->value.doubleval);
				debugWrite("precision: %d",
						bv->value.doubleval.
						precision);
				debugWrite("scale: %d",
						bv->value.doubleval.
						scale);
			}
			}
			break;
		case TDS_TYPE_MONEY4:
			{
			int32_t	val;
			readLE(rp,(uint32_t *)&val,&rp);
			// FIXME: actually implement this
			}
			break;
		case TDS_TYPE_INT8:
			{

			int64_t	val;
			readLE(rp,(uint64_t *)&val,&rp);

			if (bv) {
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				bv->valuesize=8;
				bv->isnull=cont->getNonNullBindValue();
				bv->value.integerval=val;
			}

			if (bv) {
				debugWrite("valuesize: %d",bv->valuesize);
				debugWrite("value: %lld",bv->value.integerval);
			}
			}
			break;
		case TDS_TYPE_GUID:
			{
			byte_t	size;
			read(rp,&size,&rp);
			// FIXME: actually implement this
			rp+=size;
			}
			break;
		case TDS_TYPE_DECIMAL:
		case TDS_TYPE_NUMERIC:
		case TDS_TYPE_DECIMALN:
		case TDS_TYPE_NUMERICN:
			{
			byte_t	ispositive;
			byte_t	val[16];
			if (tdstype==TDS_TYPE_DECIMALN ||
				tdstype==TDS_TYPE_NUMERICN) {
				byte_t	size;
				read(rp,&size,&rp);
				read(rp,&ispositive,&rp);
				read(rp,val,size-1,&rp);
			} else {
				read(rp,&ispositive,&rp);
				read(rp,val,sizeof(val),&rp);
			}
			// FIXME: actually implement this
			}
			break;
		case TDS_TYPE_DATEN:
			{
			byte_t	size;
			uint16_t	dayssince1;
			read(rp,&size,&rp);
			readLE(rp,&dayssince1,&rp);
			// FIXME: actually implement this
			}
			break;
		case TDS_TYPE_TIMEN:
			// FIXME:
			// 1 unsigned integer - number of 10^-n second
			// 			increments since 12 am
			// 			within a day.
			// 3 bytes if 0 <= n <= 2
			// 4 bytes if 3 <= n <= 4
			// 5 bytes if 5 <= n <= 7
			// FIXME: actually implement this
			break;
		case TDS_TYPE_DATETIME2N:
			// FIXME:
			// concat of timen and daten
			// FIXME: actually implement this
			break;
		case TDS_TYPE_DATETIMEOFFSETN:
			// FIXME:
			// concat of datetime2n and
			// int16_t - timezone offset - minutes from utc
			// 				(between -840 and 840)
			// FIXME: actually implement this
			break;
		case TDS_TYPE_CHAR:
		case TDS_TYPE_VARCHAR:
			{
			byte_t	size;
			read(rp,&size,&rp);
			// FIXME: actually implement this
			rp+=size;
			}
			break;
		case TDS_TYPE_BINARY:
		case TDS_TYPE_VARBINARY:
			{
			byte_t	size;
			read(rp,&size,&rp);
			// FIXME: actually implement this
			rp+=size;
			}
			break;
		case TDS_TYPE_BIGBINARY:
		case TDS_TYPE_BIGVARBIN:
			{
			uint16_t	size;
			readLE(rp,&size,&rp);
			// FIXME: actually implement this
			rp+=size;
			}
			break;
		case TDS_TYPE_BIGCHAR:
		case TDS_TYPE_BIGVARCHR:
		case TDS_TYPE_NCHAR:
		case TDS_TYPE_NVARCHAR:
			{
			uint16_t	size;
			readLE(rp,&size,&rp);

			if (bv) {
				bv->type=SQLRSERVERBINDVARTYPE_STRING;
				bv->valuesize=size;
				bv->value.stringval=(char *)rp;
				bv->isnull=cont->getNonNullBindValue();
			}

			if (bv) {
				debugWrite("valuesize: %d",bv->valuesize);
				debugWrite("value: %.*s",
						bv->valuesize,
						bv->value.stringval);
			}

			rp+=size;
			}
			break;
		case TDS_TYPE_UDT:
			// FIXME: actually implement this
			break;
		case TDS_TYPE_XML:
		case TDS_TYPE_TEXT:
		case TDS_TYPE_NTEXT:
			{
			uint32_t	size;
			readLE(rp,&size,&rp);
			// FIXME: actually implement this
			rp+=size;
			}
			break;
		case TDS_TYPE_IMAGE:
		case TDS_TYPE_SSVARIANT:
			{
			uint32_t	size;
			readLE(rp,&size,&rp);
			// FIXME: actually implement this
			rp+=size;
			}
			break;
		case TDS_TYPE_TVP:
			if (negotiatedtdsversion>=730) {
				// FIXME:
				// TVP_TYPENAME
				// TVP_COLMETADATA
				// [TVP_ORDER_UNIQUE]
				// [TVP_COLUMN_ORDERING]
				// TVP_END_TOKEN
				// *TVP_ROW
				// TVP_END_TOKEN
				// FIXME: actually implement this
			} else {
				// protocol error
				return false;
			}
			break;
	}

	if (negotiatedtdsversion>=740) {
		// FIXME:
		// paramcipherdata =
		// 	type_info
		// 	EncryptionAlgo - byte   (tds 7.4+)
		//   	[AlgoName] - b_varchar  (tds 7.4+)
		//   	EncryptionType - byte   (tds 7.4+)
		//   	CekHash - ???           (tds 7.4+)
		//   	NormVersion - byte      (tds 7.4+)
	}

	debugEnd();

	// clean up
	delete[] pname16;
	delete[] pname;

	// copy out pointer
	*rpout=rp;

	// copy out param index
	if (byrefvalue && bv) {
		(*outbindcount)++;
	} else {
		(*inbindcount)++;
	}

	return true;
}

void sqlrprotocol_tds::tdsProtocolError() {
	// FIXME: is there a real error message/number/state/class for this?
	error(0,0,10,"TDS Protocol Error",srvname,NULL,1);
}

void sqlrprotocol_tds::envChange(byte_t type,
					const wchar_t *newvalue,
					size_t newvaluelen,
					const wchar_t *oldvalue,
					size_t oldvaluelen) {

	byte_t		token=TOKEN_ENV_CHANGE;

	ucs2_t		*newvalue16=ucs2charstring::duplicate(
						newvalue,newvaluelen);
	ucs2_t		*oldvalue16=ucs2charstring::duplicate(
						oldvalue,oldvaluelen);

	uint16_t	newvaluelensize=
			(type==ENV_CHANGE_PROMOTE_TRANSACTION)?
						sizeof(uint32_t):
						sizeof(byte_t);
	uint16_t	oldvaluelensize=sizeof(byte_t);

	uint16_t	tokensize=
				sizeof(byte_t)+
				newvaluelensize+
				newvaluelen*sizeof(uint16_t)+
				oldvaluelensize+
				oldvaluelen*sizeof(uint16_t);

	debugStart("env change");
	debugWrite("token: 0x%02x",token);
	debugWrite("tokensize: 0x%02x (%hd)",tokensize,tokensize);
	debugWrite("type: %d",type);
	debugWrite("newvaluelensize:%d",newvaluelensize);
	debugWrite("newvaluelen: %d",newvaluelen);
	debugWrite("newvalue: %S",newvalue);
	debugWrite("oldvaluelensize:%d",oldvaluelensize);
	debugWrite("oldvaluelen: %d",oldvaluelen);
	debugWrite("oldvalue: %S",oldvalue);
	debugEnd();

	write(&resppacket,token);
	write(&resppacket,hostToLE(tokensize));
	write(&resppacket,type);
	if (newvaluelensize==sizeof(byte_t)) {
		write(&resppacket,(byte_t)newvaluelen);
	} else {
		write(&resppacket,(uint32_t)newvaluelen);
	}
	write(&resppacket,newvalue16,newvaluelen);
	write(&resppacket,(byte_t)oldvaluelen);
	write(&resppacket,oldvalue16,oldvaluelen);

	delete[] newvalue16;
	delete[] oldvalue16;
}

void sqlrprotocol_tds::info(uint32_t number,
					byte_t state,
					byte_t infoclass,
					const char *msgtext,
					const char *srvname,
					const char *procname,
					uint32_t linenumber) {
	infoOrError(TOKEN_INFO,number,state,infoclass,
				msgtext,srvname,procname,linenumber);
}

void sqlrprotocol_tds::error(uint32_t number,
					byte_t state,
					byte_t errclass,
					const char *msgtext,
					const char *srvname,
					const char *procname,
					uint32_t linenumber) {
	infoOrError(TOKEN_ERROR,number,state,errclass,
				msgtext,srvname,procname,linenumber);
}

void sqlrprotocol_tds::infoOrError(byte_t token,
					uint32_t number,
					byte_t state,
					byte_t infoerrclass,
					const char *msgtext,
					const char *srvname,
					const char *procname,
					uint32_t linenumber) {

	uint16_t	msgtextlen=charstring::getLength(msgtext);
	ucs2_t		*msgtext16=ucs2charstring::duplicate(
					msgtext,(size_t)msgtextlen);
	byte_t		srvnamelen=charstring::getLength(srvname);
	ucs2_t		*srvname16=ucs2charstring::duplicate(
					srvname,(size_t)srvnamelen);
	byte_t		procnamelen=charstring::getLength(procname);
	ucs2_t		*procname16=ucs2charstring::duplicate(
					procname,(size_t)procnamelen);

	uint16_t	tokensize=sizeof(uint32_t)+
					sizeof(byte_t)+
					sizeof(byte_t)+
					sizeof(uint16_t)+
					msgtextlen*sizeof(ucs2_t)+
					sizeof(byte_t)+
					srvnamelen*sizeof(ucs2_t)+
					sizeof(byte_t)+
					procnamelen*sizeof(ucs2_t)+
					((negotiatedtdsversion<720)?
						sizeof(uint16_t):
						sizeof(uint32_t));

	debugStart((token==TOKEN_INFO)?"info":"error");
	debugWrite("token: 0x%02x",token);
	debugWrite("tokensize: 0x%02x (%hd)",tokensize,tokensize);
	debugWrite("number: %d",number);
	debugWrite("state: %d",state);
	debugWrite("class: %d",infoerrclass);
	debugWrite("msgtext: %s",msgtext);
	debugWrite("srvname: %s",srvname);
	debugWrite("procname: %s",procname);
	debugWrite("linenumber: %d",linenumber);
	debugEnd();

	write(&resppacket,token);
	write(&resppacket,hostToLE(tokensize));
	write(&resppacket,hostToLE(number));
	write(&resppacket,state);
	write(&resppacket,infoerrclass);
	write(&resppacket,hostToLE((uint16_t)msgtextlen));
	write(&resppacket,msgtext16,msgtextlen);
	write(&resppacket,srvnamelen);
	write(&resppacket,srvname16,srvnamelen);
	write(&resppacket,procnamelen);
	write(&resppacket,procname16,procnamelen);
	if (negotiatedtdsversion<720) {
		write(&resppacket,hostToLE((uint16_t)linenumber));
	} else {
		write(&resppacket,hostToLE(linenumber));
	}
}

void sqlrprotocol_tds::done() {
	done(DONE_FINAL,0,0);
}

void sqlrprotocol_tds::done(uint16_t status,
				uint16_t curcmd,
				uint64_t donerowcount) {
	done(TOKEN_DONE,status,curcmd,donerowcount);
}

void sqlrprotocol_tds::done(byte_t token,
				uint16_t status,
				uint16_t curcmd,
				uint64_t donerowcount) {

	switch (token) {
		case TOKEN_DONEINPROC:
			debugStart("done-in-proc");
			break;
		case TOKEN_DONEPROC:
			debugStart("done-proc");
			break;
		default:
			debugStart("done");
			break;
	}
	debugWrite("token: 0x%02x",token);
	debugWrite("status: 0x%02x",status);
	debugWrite("curcmd: 0x%02x",curcmd);
	debugWrite("donerowcount: %lld",donerowcount);
	debugEnd();

	write(&resppacket,token);
	write(&resppacket,hostToLE(status));
	write(&resppacket,hostToLE(curcmd));
	if (negotiatedtdsversion<720) {
		write(&resppacket,hostToLE((uint32_t)donerowcount));
	} else {
		write(&resppacket,hostToLE(donerowcount));
	}
}

void sqlrprotocol_tds::doneInProc(uint16_t status,
					uint16_t curcmd,
					uint64_t donerowcount) {
	done(TOKEN_DONEINPROC,status,curcmd,donerowcount);
}

void sqlrprotocol_tds::returnStatus(sqlrservercursor *cursor) {

	byte_t		token=TOKEN_RETURNSTATUS;
	// FIXME: SQL Relay doesn't know the return status
	uint32_t	value=0;

	write(&resppacket,token);
	write(&resppacket,hostToLE(value));

	debugStart("return-status");
	debugWrite("token: 0x%02x",token);
	debugWrite("status: %d",value);
	debugEnd();
}

void sqlrprotocol_tds::returnValues(sqlrservercursor *cursor) {

	// for each output bind...
	uint16_t	outbindcount=cont->getOutputBindCount(cursor);
	for (uint16_t i=0; i<outbindcount; i++) {
		returnValue(cursor,i);
	}
}

void sqlrprotocol_tds::returnValue(sqlrservercursor *cursor, uint16_t param) {

	debugStart("return-value");

	byte_t	token=TOKEN_RETURNVALUE;

	write(&resppacket,token);

	// FIXME: actually implement this

	// param ordinal
	// param name
	// user type
	// flags
	// type info
	// crypto meta data
	// value

	debugEnd();
}

void sqlrprotocol_tds::doneProc(uint16_t status,
					uint16_t curcmd,
					uint64_t donerowcount) {
	done(TOKEN_DONEPROC,status,curcmd,donerowcount);
}

void sqlrprotocol_tds::debugSystemError() {
	char	*err=error::getErrorString();
	debugWrite(err);
	delete[] err;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrprotocol	*new_sqlrprotocol_tds(
						sqlrservercontroller *cont,
						domnode *parameters) {
		return new sqlrprotocol_tds(cont,parameters);
	}
}
