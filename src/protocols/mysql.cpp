// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/bytebuffer.h>
#include <rudiments/character.h>
#include <rudiments/randomnumber.h>
#include <rudiments/process.h>
#include <rudiments/error.h>

#include <defines.h>

// capability flags
#define CLIENT_LONG_PASSWORD				0x00000001
#define CLIENT_FOUND_ROWS				0x00000002
#define CLIENT_LONG_FLAG				0x00000004
#define CLIENT_CONNECT_WITH_DB				0x00000008
#define CLIENT_NO_SCHEMA				0x00000010
#define CLIENT_COMPRESS					0x00000020
#define CLIENT_ODBC					0x00000040
#define CLIENT_LOCAL_FILES				0x00000080
#define CLIENT_IGNORE_SPACE				0x00000100
#define CLIENT_PROTOCOL_41				0x00000200
#define CLIENT_INTERACTIVE				0x00000400
#define CLIENT_SSL					0x00000800
#define CLIENT_IGNORE_SIGPIPE				0x00001000
#define CLIENT_TRANSACTIONS				0x00002000
#define CLIENT_RESERVED					0x00004000
#define CLIENT_SECURE_CONNECTION			0x00008000
#define CLIENT_MULTI_STATEMENTS				0x00010000
#define CLIENT_MULTI_RESULTS				0x00020000
#define CLIENT_PS_MULTI_RESULTS				0x00040000
#define CLIENT_PLUGIN_AUTH				0x00080000
#define CLIENT_CONNECT_ATTRS				0x00100000
#define CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA		0x00200000
#define CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS		0x00400000
#define CLIENT_SESSION_TRACK				0x00800000
#define CLIENT_DEPRECATE_EOF				0x01000000

// character sets
#define LATIN1_SWEDISH_CI	0x08
#define UTF8_GENERAL_CI		0x21
#define UTF8MB4_GENERAL_CI	0xff

// status flags
#define SERVER_STATUS_IN_TRANS			0x0001
#define SERVER_STATUS_AUTOCOMMIT		0x0002
#define SERVER_MORE_RESULTS_EXISTS		0x0008
#define SERVER_STATUS_NO_GOOD_INDEX_USED	0x0010
#define SERVER_STATUS_NO_INDEX_USED		0x0020
#define SERVER_STATUS_CURSOR_EXISTS		0x0040
#define SERVER_STATUS_LAST_ROW_SENT		0x0080
#define SERVER_STATUS_DB_DROPPED		0x0100
#define SERVER_STATUS_NO_BACKSLASH_ESCAPES	0x0200
#define SERVER_STATUS_METADATA_CHANGED		0x0400
#define SERVER_QUERY_WAS_SLOW			0x0800
#define SERVER_PS_OUT_PARAMS			0x1000
#define SERVER_STATUS_IN_TRANS_READONLY		0x2000
#define SERVER_SESSION_STATE_CHANGED		0x4000

// commands
#define COM_SLEEP		0x00
#define COM_QUIT		0x01
#define COM_INIT_DB		0x02
#define COM_QUERY		0x03
#define COM_FIELD_LIST		0x04
#define COM_CREATE_DB		0x05
#define COM_DROP_DB		0x06
#define COM_REFRESH		0x07
#define COM_SHUTDOWN		0x08
#define COM_STATISTICS		0x09
#define COM_PROCESS_INFO	0x0a
#define COM_CONNECT		0x0b
#define COM_PROCESS_KILL	0x0c
#define COM_DEBUG		0x0d
#define COM_PING		0x0e
#define COM_TIME		0x0f
#define COM_DELAYED_INSERT	0x10
#define COM_CHANGE_USER		0x11
#define COM_BINLOG_DUMP		0x12
#define COM_TABLE_DUMP		0x13
#define COM_CONNECT_OUT		0x14
#define COM_REGISTER_SLAVE	0x15
#define COM_STMT_PREPARE	0x16
#define COM_STMT_EXECUTE	0x17
#define COM_STMT_SEND_LONG_DATA	0x18
#define COM_STMT_CLOSE		0x19
#define COM_STMT_RESET		0x1a
#define COM_SET_OPTION		0x1b
#define COM_STMT_FETCH		0x1c
#define COM_DAEMON		0x1d
#define COM_BINLOG_DUMP_GTID	0x1e
#define COM_RESET_CONNECTION	0x1f

// column types
#define MYSQL_TYPE_DECIMAL	0x00
#define MYSQL_TYPE_TINY		0x01
#define MYSQL_TYPE_SHORT	0x02
#define MYSQL_TYPE_LONG		0x03
#define MYSQL_TYPE_FLOAT	0x04
#define MYSQL_TYPE_DOUBLE	0x05
#define MYSQL_TYPE_NULL		0x06
#define MYSQL_TYPE_TIMESTAMP	0x07
#define MYSQL_TYPE_LONGLONG	0x08
#define MYSQL_TYPE_INT24	0x09
#define MYSQL_TYPE_DATE		0x0a
#define MYSQL_TYPE_TIME		0x0b
#define MYSQL_TYPE_DATETIME	0x0c
#define MYSQL_TYPE_YEAR		0x0d
#define MYSQL_TYPE_NEWDATE	0x0e
#define MYSQL_TYPE_VARCHAR	0x0f
#define MYSQL_TYPE_BIT		0x10
#define MYSQL_TYPE_TIMESTAMP2	0x11
#define MYSQL_TYPE_DATETIME2	0x12
#define MYSQL_TYPE_TIME2	0x13
#define MYSQL_TYPE_NEWDECIMAL	0xf6
#define MYSQL_TYPE_ENUM		0xf7
#define MYSQL_TYPE_SET		0xf8
#define MYSQL_TYPE_TINY_BLOB	0xf9
#define MYSQL_TYPE_MEDIUM_BLOB	0xfa
#define MYSQL_TYPE_LONG_BLOB	0xfb
#define MYSQL_TYPE_BLOB		0xfc
#define MYSQL_TYPE_VAR_STRING	0xfd
#define MYSQL_TYPE_STRING	0xfe
#define MYSQL_TYPE_GEOMETRY	0xff

static unsigned char	mysqltypemap[]={
	// "UNKNOWN"
	(unsigned char)MYSQL_TYPE_NULL,
	// addded by freetds
	// "CHAR"
	(unsigned char)MYSQL_TYPE_STRING,
	// "INT"
	(unsigned char)MYSQL_TYPE_LONG,
	// "SMALLINT"
	(unsigned char)MYSQL_TYPE_SHORT,
	// "TINYINT"
	(unsigned char)MYSQL_TYPE_TINY,
	// "MONEY"
	(unsigned char)MYSQL_TYPE_NEWDECIMAL,
	// "DATETIME"
	(unsigned char)MYSQL_TYPE_DATETIME,
	// "NUMERIC"
	(unsigned char)MYSQL_TYPE_NEWDECIMAL,
	// "DECIMAL"
	(unsigned char)MYSQL_TYPE_NEWDECIMAL,
	// "SMALLDATETIME"
	(unsigned char)MYSQL_TYPE_DATETIME,
	// "SMALLMONEY"
	(unsigned char)MYSQL_TYPE_NEWDECIMAL,
	// "IMAGE"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "BINARY"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "BIT"
	(unsigned char)MYSQL_TYPE_TINY,
	// "REAL"
	(unsigned char)MYSQL_TYPE_DECIMAL,
	// "FLOAT"
	(unsigned char)MYSQL_TYPE_FLOAT,
	// "TEXT"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "VARCHAR"
	(unsigned char)MYSQL_TYPE_VAR_STRING,
	// "VARBINARY"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "LONGCHAR"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "LONGBINARY"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "LONG"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "ILLEGAL"
	(unsigned char)MYSQL_TYPE_NULL,
	// "SENSITIVITY"
	(unsigned char)MYSQL_TYPE_STRING,
	// "BOUNDARY"
	(unsigned char)MYSQL_TYPE_STRING,
	// "VOID"
	(unsigned char)MYSQL_TYPE_NULL,
	// "USHORT"
	(unsigned char)MYSQL_TYPE_SHORT,
	// added by lago
	// "UNDEFINED"
	(unsigned char)MYSQL_TYPE_NULL,
	// "DOUBLE"
	(unsigned char)MYSQL_TYPE_DOUBLE,
	// "DATE"
	(unsigned char)MYSQL_TYPE_DATE,
	// "TIME"
	(unsigned char)MYSQL_TYPE_TIME,
	// "TIMESTAMP"
	(unsigned char)MYSQL_TYPE_TIMESTAMP,
	// added by msql
	// "UINT"
	(unsigned char)MYSQL_TYPE_LONG,
	// "LASTREAL"
	(unsigned char)MYSQL_TYPE_DECIMAL,
	// added by mysql
	// "STRING"
	(unsigned char)MYSQL_TYPE_STRING,
	// "VARSTRING"
	(unsigned char)MYSQL_TYPE_VAR_STRING,
	// "LONGLONG"
	(unsigned char)MYSQL_TYPE_LONGLONG,
	// "MEDIUMINT"
	(unsigned char)MYSQL_TYPE_INT24,
	// "YEAR"
	(unsigned char)MYSQL_TYPE_YEAR,
	// "NEWDATE"
	(unsigned char)MYSQL_TYPE_NEWDATE,
	// "NULL"
	(unsigned char)MYSQL_TYPE_NULL,
	// "ENUM"
	(unsigned char)MYSQL_TYPE_ENUM,
	// "SET"
	(unsigned char)MYSQL_TYPE_SET,
	// "TINYBLOB"
	(unsigned char)MYSQL_TYPE_TINY_BLOB,
	// "MEDIUMBLOB"
	(unsigned char)MYSQL_TYPE_MEDIUM_BLOB,
	// "LONGBLOB"
	(unsigned char)MYSQL_TYPE_LONG_BLOB,
	// "BLOB"
	(unsigned char)MYSQL_TYPE_BLOB,
	// added by oracle
	// "VARCHAR2"
	(unsigned char)MYSQL_TYPE_VAR_STRING,
	// "NUMBER"
	(unsigned char)MYSQL_TYPE_NEWDECIMAL,
	// "ROWID"
	(unsigned char)MYSQL_TYPE_LONGLONG,
	// "RAW"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "LONG_RAW"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "MLSLABEL"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "CLOB"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "BFILE"
	(unsigned char)MYSQL_TYPE_BLOB,
	// added by odbc
	// "BIGINT"
	(unsigned char)MYSQL_TYPE_LONGLONG,
	// "INTEGER"
	(unsigned char)MYSQL_TYPE_LONG,
	// "LONGVARBINARY"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "LONGVARCHAR"
	(unsigned char)MYSQL_TYPE_BLOB,
	// added by db2
	// "GRAPHIC"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "VARGRAPHIC"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "LONGVARGRAPHIC"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "DBCLOB"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "DATALINK"
	(unsigned char)MYSQL_TYPE_STRING,
	// "USER_DEFINED_TYPE"
	(unsigned char)MYSQL_TYPE_STRING,
	// "SHORT_DATATYPE"
	(unsigned char)MYSQL_TYPE_SHORT,
	// "TINY_DATATYPE"
	(unsigned char)MYSQL_TYPE_TINY,
	// added by firebird
	// "D_FLOAT"
	(unsigned char)MYSQL_TYPE_DOUBLE,
	// "ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "QUAD"
	(unsigned char)MYSQL_TYPE_SET,
	// "INT64"
	(unsigned char)MYSQL_TYPE_LONGLONG,
	// "DOUBLE PRECISION"
	(unsigned char)MYSQL_TYPE_DOUBLE,
	// added by postgresql
	// "BOOL"
	(unsigned char)MYSQL_TYPE_TINY,
	// "BYTEA"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "NAME"
	(unsigned char)MYSQL_TYPE_STRING,
	// "INT8"
	(unsigned char)MYSQL_TYPE_LONG,
	// "INT2"
	(unsigned char)MYSQL_TYPE_SHORT,
	// "INT2VECTOR"
	(unsigned char)MYSQL_TYPE_SET,
	// "INT4"
	(unsigned char)MYSQL_TYPE_LONG,
	// "REGPROC"
	(unsigned char)MYSQL_TYPE_LONG,
	// "OID"
	(unsigned char)MYSQL_TYPE_LONG,
	// "TID"
	(unsigned char)MYSQL_TYPE_LONG,
	// "XID"
	(unsigned char)MYSQL_TYPE_LONG,
	// "CID"
	(unsigned char)MYSQL_TYPE_LONG,
	// "OIDVECTOR"
	(unsigned char)MYSQL_TYPE_SET,
	// "SMGR"
	(unsigned char)MYSQL_TYPE_STRING,
	// "POINT"
	(unsigned char)MYSQL_TYPE_STRING,
	// "LSEG"
	(unsigned char)MYSQL_TYPE_STRING,
	// "PATH"
	(unsigned char)MYSQL_TYPE_STRING,
	// "BOX"
	(unsigned char)MYSQL_TYPE_STRING,
	// "POLYGON"
	(unsigned char)MYSQL_TYPE_STRING,
	// "LINE"
	(unsigned char)MYSQL_TYPE_STRING,
	// "LINE_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "FLOAT4"
	(unsigned char)MYSQL_TYPE_FLOAT,
	// "FLOAT8"
	(unsigned char)MYSQL_TYPE_DOUBLE,
	// "ABSTIME"
	(unsigned char)MYSQL_TYPE_LONG,
	// "RELTIME"
	(unsigned char)MYSQL_TYPE_LONG,
	// "TINTERVAL"
	(unsigned char)MYSQL_TYPE_LONG,
	// "CIRCLE"
	(unsigned char)MYSQL_TYPE_STRING,
	// "CIRCLE_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "MONEY_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "MACADDR"
	(unsigned char)MYSQL_TYPE_STRING,
	// "INET"
	(unsigned char)MYSQL_TYPE_STRING,
	// "CIDR"
	(unsigned char)MYSQL_TYPE_STRING,
	// "BOOL_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "BYTEA_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "CHAR_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "NAME_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "INT2_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "INT2VECTOR_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "INT4_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "REGPROC_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "TEXT_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "OID_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "TID_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "XID_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "CID_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "OIDVECTOR_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "BPCHAR_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "VARCHAR_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "INT8_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "POINT_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "LSEG_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "PATH_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "BOX_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "FLOAT4_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "FLOAT8_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "ABSTIME_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "RELTIME_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "TINTERVAL_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "POLYGON_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "ACLITEM"
	(unsigned char)MYSQL_TYPE_SET,
	// "ACLITEM_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "MACADDR_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "INET_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "CIDR_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "BPCHAR"
	(unsigned char)MYSQL_TYPE_STRING,
	// "TIMESTAMP_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "DATE_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "TIME_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "TIMESTAMPTZ"
	(unsigned char)MYSQL_TYPE_STRING,
	// "TIMESTAMPTZ_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "INTERVAL"
	(unsigned char)MYSQL_TYPE_LONG,
	// "INTERVAL_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "NUMERIC_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "TIMETZ"
	(unsigned char)MYSQL_TYPE_STRING,
	// "TIMETZ_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "BIT_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "VARBIT"
	(unsigned char)MYSQL_TYPE_STRING,
	// "VARBIT_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "REFCURSOR"
	(unsigned char)MYSQL_TYPE_LONG,
	// "REFCURSOR_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "REGPROCEDURE"
	(unsigned char)MYSQL_TYPE_LONG,
	// "REGOPER"
	(unsigned char)MYSQL_TYPE_LONG,
	// "REGOPERATOR"
	(unsigned char)MYSQL_TYPE_LONG,
	// "REGCLASS"
	(unsigned char)MYSQL_TYPE_LONG,
	// "REGTYPE"
	(unsigned char)MYSQL_TYPE_LONG,
	// "REGPROCEDURE_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "REGOPER_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "REGOPERATOR_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "REGCLASS_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "REGTYPE_ARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "RECORD"
	(unsigned char)MYSQL_TYPE_SET,
	// "CSTRING"
	(unsigned char)MYSQL_TYPE_STRING,
	// "ANY"
	(unsigned char)MYSQL_TYPE_STRING,
	// "ANYARRAY"
	(unsigned char)MYSQL_TYPE_SET,
	// "TRIGGER"
	(unsigned char)MYSQL_TYPE_STRING,
	// "LANGUAGE_HANDLER"
	(unsigned char)MYSQL_TYPE_STRING,
	// "INTERNAL"
	(unsigned char)MYSQL_TYPE_STRING,
	// "OPAQUE"
	(unsigned char)MYSQL_TYPE_STRING,
	// "ANYELEMENT"
	(unsigned char)MYSQL_TYPE_STRING,
	// "PG_TYPE"
	(unsigned char)MYSQL_TYPE_STRING,
	// "PG_ATTRIBUTE"
	(unsigned char)MYSQL_TYPE_STRING,
	// "PG_PROC"
	(unsigned char)MYSQL_TYPE_STRING,
	// "PG_CLASS"
	(unsigned char)MYSQL_TYPE_STRING,
	// none added by sqlite
	// added by sqlserver
	// "UBIGINT"
	(unsigned char)MYSQL_TYPE_LONGLONG,
	// "UNIQUEIDENTIFIER"
	(unsigned char)MYSQL_TYPE_STRING,
	// added by informix
	// "SMALLFLOAT"
	(unsigned char)MYSQL_TYPE_FLOAT,
	// "BYTE"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "BOOLEAN"
	(unsigned char)MYSQL_TYPE_TINY,
	// "TINYTEXT"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "MEDIUMTEXT"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "LONGTEXT"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "JSON"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "GEOMETRY"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "SDO_GEOMETRY"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "NCHAR"
	(unsigned char)MYSQL_TYPE_STRING,
	// "NVARCHAR"
	(unsigned char)MYSQL_TYPE_VAR_STRING,
	// "NTEXT"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "XML"
	(unsigned char)MYSQL_TYPE_BLOB,
	// "DATETIMEOFFSET"
	(unsigned char)MYSQL_TYPE_DATETIME

};

// column flags
#define NOT_NULL_FLAG		1
#define PRI_KEY_FLAG		2
#define UNIQUE_KEY_FLAG		4
#define MULTIPLE_KEY_FLAG	8
#define UNSIGNED_FLAG		32
#define ZEROFILL_FLAG		64
#define BINARY_FLAG		128
#define AUTO_INCREMENT_FLAG	512
#define ENUM_FLAG		256
#define SET_FLAG		2048
#define BLOB_FLAG		16
#define TIMESTAMP_FLAG		1024
#define NUM_FLAG		32768
#define NO_DEFAULT_VALUE_FLAG	4096
#define ON_UPDATE_NOW_FLAG	8192

enum mysqllisttype_t {
	MYSQLLISTTYPE_DATABASE_LIST=0,
	MYSQLLISTTYPE_TABLE_LIST,
	MYSQLLISTTYPE_COLUMN_LIST
};

// refresh commands
#define REFRESH_GRANT	0x01
#define REFRESH_LOG	0x02
#define REFRESH_TABLES	0x04
#define REFRESH_HOSTS	0x08
#define REFRESH_STATUS	0x10
#define REFRESH_THREADS	0x20
#define REFRESH_SLAVE	0x40
#define REFRESH_MASTER	0x80

// shutdown commands
#define SHUTDOWN_DEFAULT		0x00
#define SHUTDOWN_WAIT_CONNECTIONS	0x01
#define SHUTDOWN_WAIT_TRANSACTIONS	0x02
#define SHUTDOWN_WAIT_UPDATES		0x08
#define SHUTDOWN_WAIT_ALL_BUFFERS	0x10
#define SHUTDOWN_WAIT_CRITICAL_BUFFERS	0x11
#define KILL_QUERY			0xfe
#define KILL_CONNECTION			0xff

// stmt execute flags
#define CURSOR_TYPE_NO_CURSOR	0x00
#define CURSOR_TYPE_READ_ONLY	0x01
#define CURSOR_TYPE_FOR_UPDATE	0x02
#define CURSOR_TYPE_SCROLLABLE	0x04

// multi statement options
#define MYSQL_OPTION_MULTI_STATEMENTS_ON	0
#define MYSQL_OPTION_MULTI_STATEMENTS_OFF	1


class SQLRSERVER_DLLSPEC sqlrprotocol_mysql : public sqlrprotocol {
	public:
			sqlrprotocol_mysql(sqlrservercontroller *cont,
							sqlrprotocols *ps,
							domnode *parameters);
		virtual	~sqlrprotocol_mysql();

		clientsessionexitstatus_t	clientSession(
							filedescriptor *cs);

	private:
		void	init();
		void	free();
		void	reInit();

		void	resetSendPacketBuffer();
		bool	sendPacket();
		bool	sendPacket(bool flush);
		bool	recvPacket();

		void	generateChallenge();

		bool	initialHandshake();
		bool	sendHandshake();
		void	buildHandshake10();
		void	buildHandshake9();
		bool	recvHandshakeResponse();
		void	parseHandshakeResponse41(
					const unsigned char *rp,
					uint64_t rplen);
		void	parseHandshakeResponse320(
					const unsigned char *rp,
					uint64_t rplen);
		bool	negotiateAuthMethod();
		bool	sendAuthSwitchRequest();
		bool	sendOldAuthSwitchRequest();
		bool	recvAuthResponse();
		bool	authenticate();
		bool	negotiateMoreData();
		bool	sendAuthMoreDataPacket();
		bool	sendOkPacket();
		bool	sendOkPacket(bool noteof,
					uint64_t affectedrows,
					uint64_t lastinsertid,
					uint16_t statusflags,
					uint16_t warnings,
					const char *info,
					char sessionstatechangetype,
					const char *sessionstatechangedata);
		bool	sendErrPacket(uint16_t errorcode,
					const char *errormessage,
					const char *sqlstate);
		bool	sendErrPacket(uint16_t errorcode,
					const char *errormessage,
					uint64_t errorlength,
					const char *sqlstate);
		bool	sendEofPacket(uint16_t warnings,
					uint16_t statusflags);
		bool	selectDatabase();

		void	debugCapabilityFlags(uint32_t capabilityflags);
		void	debugCharacterSet(unsigned char characterset);
		void	debugStatusFlags(uint16_t statusflags);
		void	debugColumnType(const char *name,
					unsigned char columntype);
		void	debugColumnType(unsigned char columntype);
		void	debugColumnFlags(uint16_t statusflags);
		void	debugSystemError();
		void	debugRefreshCommand(unsigned char command);
		void	debugShutdownCommand(unsigned char command);
		void	debugStmtExecuteFlags(unsigned char flags);
		void	debugMultiStatementOption(uint16_t multistmtoption);

		bool	getRequest(char *request);

		// com_sleep
		bool	comSleep();

		// com_init_db
		bool	comInitDb();

		// com_statistics
		bool	comStatistics();

		// com_connect
		bool	comConnect();

		// com_debug
		bool	comDebug();

		// com_ping
		bool	comPing();

		// com_time
		bool	comTime();

		// com_delayed_insert
		bool	comDelayedInsert();

		// com_change_user
		bool	comChangeUser();

		// com_binlog_dump
		bool	comBinLogDump();

		// com_table_dump
		bool	comTableDump();

		// com_connect_out
		bool	comConnectOut();

		// com_register_slave
		bool	comRegisterSlave();

		// com_daemon
		bool	comDaemon();

		// com_binlog_dump_gtid
		bool	comBinlogDumpGtid();

		// com_reset_connection
		bool	comResetConnection();

		// com_create_db
		bool	comCreateDb(sqlrservercursor *cursor);

		// com_drop_db
		bool	comDropDb(sqlrservercursor *cursor);

		// com_query
		bool	comQuery(sqlrservercursor *cursor);
		bool	sendQuery(sqlrservercursor *cursor,
						const char *query);
		bool	sendQuery(sqlrservercursor *cursor,
						const char *query,
						uint64_t querylen);
		bool	sendQueryResult(sqlrservercursor *cursor,
						bool binary);
		bool	sendResultSet(sqlrservercursor *cursor,
						uint32_t colcount,
						bool binary);
		void	cacheColumnDefinitions(sqlrservercursor *cursor,
							uint32_t colcount);
		bool	sendColumnDefinitions(sqlrservercursor *cursor,
							uint32_t colcount,
							bool binary);
		bool	sendColumnDefinition(sqlrservercursor *cursor,
							uint32_t column);
		bool	sendColumnDefinition(sqlrservercursor *cursor,
						uint32_t column,
						const char *catalog,
						const char *schema,
						const char *table,
						const char *orgtable,
						const char *colname,
						const char *orgcolname,
						uint32_t length,
						const char *columntypestring,
						uint32_t scale,
						unsigned char columntype,
						uint16_t flags,
						const char *defaults,
						bool fieldlistcommand);
		unsigned char	getColumnType(const char *columntypestring,
						uint16_t columntypelen,
						uint32_t scale);
		uint16_t	getColumnFlags(sqlrservercursor *cursor,
						uint32_t column,
						uint16_t sqlrcolumntype,
						unsigned char columntype,
						const char *columntypestring);
		uint16_t	getColumnFlags(sqlrservercursor *cursor,
						uint16_t sqlrcolumntype,
						unsigned char columntype,
						const char *columntypestring,
						bool isnullable,
						bool isprimarykey,
						bool isunique,
						bool ispartofkey,
						bool isunsigned,
						bool iszerofilled,
						bool isbinary,
						bool isautoincrement);
		bool	sendResultSetRows(sqlrservercursor *cursor,
							uint32_t colcount,
							uint32_t rowcount,
							bool binary);
		bool	buildBinaryRow(sqlrservercursor *cursor,
						uint32_t colcount);
		void	buildBinaryField(const char *field,
						uint64_t fieldlength,
						unsigned char columntype);
		bool	buildTextRow(sqlrservercursor *cursor,
						uint32_t colcount);
		void	buildLobField(sqlrservercursor *cursor, uint32_t col);

		// com_field_list
		bool	comFieldList(sqlrservercursor *cursor);
		bool	getListByApiCall(sqlrservercursor *cursor,
						mysqllisttype_t listtype,
						const char *table,
						const char *wild);
		bool	getListByQuery(sqlrservercursor *cursor,
						mysqllisttype_t listtype,
						const char *table,
						const char *wild);
		bool	buildListQuery(sqlrservercursor *cursor,
						const char *query,
						const char *wild,
						const char *table);
		void	escapeParameter(stringbuffer *buffer,
						const char *parameter);
		bool	sendFieldListResponse(sqlrservercursor *cursor);

		// com_refresh
		bool	comRefresh(sqlrservercursor *cursor);

		// com_shutdown
		bool	comShutdown(sqlrservercursor *cursor);

		// com_process_info
		bool	comProcessInfo(sqlrservercursor *cursor);

		// com_process_kill
		bool	comProcessKill(sqlrservercursor *cursor);

		// com_stmt_prepare
		bool	comStmtPrepare(sqlrservercursor *cursor);
		bool	sendStmtPrepareOk(sqlrservercursor *cursor);

		// com_stmt_execute
		bool	comStmtExecute();
		void	bindParameters(sqlrservercursor *cursor,
					uint16_t pcount,
					uint16_t *ptypes,
					const unsigned char *nullbitmap,
					const unsigned char *in,
					const unsigned char **out);
		void	clearParams(sqlrservercursor *cursor);

		// com_stmt_send_long_data
		bool	comStmtSendLongData();

		// com_stmt_close
		bool	comStmtClose();

		// com_stmt_reset
		bool	comStmtReset();

		// com_set_option
		bool	comSetOption(sqlrservercursor *cursor);

		// com_stmt_fetch
		bool	comStmtFetch();

		bool	sendError();
		bool	sendQueryError(sqlrservercursor *cursor);
		bool	sendNotImplementedError();
		bool	sendCursorNotOpenError();
		bool	sendMalformedPacketError();

		filedescriptor	*clientsock;

		uint64_t	handshake;
		uint64_t	clientprotocol;
		bool		datetodatetime;
		bool		zeroscaledecimaltobigint;
		bool		oldmariadbjdbcservercapabilitieshack;

		bytebuffer	resppacket;
		unsigned char	seq;

		memorypool	reqpacketpool;
		unsigned char	*reqpacket;
		uint64_t	reqpacketsize;

		randomnumber	r;
		uint32_t	seed;

		uint32_t	servercapabilityflags;
		char		servercharacterset;
		uint32_t	clientcapabilityflags;
		char		clientcharacterset;
		char		*username;
		char		*challenge;
		char		*response;
		uint64_t	responselength;
		const char	*serverauthpluginname;
		const char	*clientauthpluginname;
		char		*database;
		stringbuffer	moredata;

		uint16_t	maxcursorcount;
		uint32_t	maxquerysize;
		uint16_t	maxbindcount;

		char		**bindvarnames;
		int16_t		*bindvarnamesizes;

		char		lobbuffer[32768];

		uint16_t	*pcounts;
		uint16_t	**ptypes;
		bool		*columntypescached;
		unsigned char	**columntypes;
		unsigned char	**nullbitmap;
};

sqlrprotocol_mysql::sqlrprotocol_mysql(sqlrservercontroller *cont,
					sqlrprotocols *ps,
					domnode *parameters) :
					sqlrprotocol(cont,ps,parameters) {

	clientsock=NULL;

	handshake=charstring::toInteger(
				parameters->getAttributeValue(
						"handshake"));
	if (!handshake) {
		handshake=10;
	}

	clientprotocol=charstring::toInteger(
				parameters->getAttributeValue(
						"clientprotocol"));
	if (!clientprotocol) {
		clientprotocol=41;
	}

	datetodatetime=charstring::isYes(
			parameters->getAttributeValue("datetodatetime"));
	zeroscaledecimaltobigint=
			charstring::isYes(
				parameters->getAttributeValue(
					"zeroscaledecimaltobigint"));

	oldmariadbjdbcservercapabilitieshack=
		charstring::isYes(
			parameters->getAttributeValue(
				"oldmariadbjdbcservercapabilitieshack"));

	if (getDebug()) {
		debugStart("parameters");
		stdoutput.printf("	handshake: %d\n",handshake);
		stdoutput.printf("	clientprotocol: %d\n",clientprotocol);
		stdoutput.printf("	datetodatetime: %d\n",datetodatetime);
		stdoutput.printf("	zeroscaledecimaltobigint"
				": %d\n",zeroscaledecimaltobigint);
		stdoutput.printf("	oldmariadbjdbcservercapabilitieshack"
				": %d\n",oldmariadbjdbcservercapabilitieshack);
		debugEnd();
	}

	r.setSeed(randomnumber::getSeed());

	maxcursorcount=cont->getConfig()->getMaxCursors();
	maxquerysize=cont->getConfig()->getMaxQuerySize();
	maxbindcount=cont->getConfig()->getMaxBindCount();

	bindvarnames=new char *[maxbindcount];
	bindvarnamesizes=new int16_t[maxbindcount];
	for (uint16_t i=0; i<maxbindcount; i++) {
		charstring::printf(&bindvarnames[i],"?%d",i+1);
		bindvarnamesizes[i]=charstring::length(bindvarnames[i]);
	}

	pcounts=new uint16_t[maxcursorcount];
	ptypes=new uint16_t *[maxcursorcount];
	columntypescached=new bool[maxcursorcount];
	columntypes=new unsigned char *[maxcursorcount];
	nullbitmap=new unsigned char *[maxcursorcount];
	for (uint16_t i=0; i<maxcursorcount; i++) {
		pcounts[i]=0;
		ptypes[i]=new uint16_t[maxbindcount];
		columntypescached[i]=false;
		if (cont->getMaxColumnCount()) {
			columntypes[i]=new unsigned char[
					cont->getMaxColumnCount()];
			nullbitmap[i]=new unsigned char[
					(cont->getMaxColumnCount()+7+2)/8];
		} else {
			columntypes[i]=NULL;
			nullbitmap[i]=NULL;
		}
	}

	init();
}

sqlrprotocol_mysql::~sqlrprotocol_mysql() {
	free();

	for (uint16_t i=0; i<maxbindcount; i++) {
		delete[] bindvarnames[i];
	}
	delete[] bindvarnames;

	for (uint16_t i=0; i<maxcursorcount; i++) {
		delete[] ptypes[i];
		delete[] columntypes[i];
		delete[] nullbitmap[i];
	}
	delete[] pcounts;
	delete[] ptypes;
	delete[] columntypes;
	delete[] nullbitmap;
}

void sqlrprotocol_mysql::init() {
	seq=0;
	reqpacket=NULL;
	reqpacketsize=0;
	servercapabilityflags=0;
	servercharacterset=0;
	clientcapabilityflags=0;
	clientcharacterset=0;
	username=NULL;
	challenge=NULL;
	response=NULL;
	serverauthpluginname="mysql_native_password";
	clientauthpluginname=NULL;
	database=NULL;
}

void sqlrprotocol_mysql::free() {
	delete[] username;
	delete[] challenge;
	delete[] response;
	delete[] database;
	reqpacketpool.clear();
}

void sqlrprotocol_mysql::reInit() {
	free();
	init();
}

clientsessionexitstatus_t sqlrprotocol_mysql::clientSession(
						filedescriptor *cs) {

	clientsock=cs;

	// set up the socket
	clientsock->dontUseNaglesAlgorithm();
	clientsock->setSocketReadBufferSize(65536);
	clientsock->setSocketWriteBufferSize(65536);
	clientsock->setReadBufferSize(65536);
	clientsock->setWriteBufferSize(65536);

	reInit();

	bool	endsession=true;

	clientsessionexitstatus_t	status=CLIENTSESSIONEXITSTATUS_ERROR;
	if (initialHandshake()) {

		// loop, getting and executing requests
		bool	loop=true;
		do {

			// get the request...
			char	request;
			if (!getRequest(&request)) {
				break;
			}

			// handle invalid requests
			if (request>0x1f) {
				if (!sendNotImplementedError()) {
					break;
				}
				continue;
			}

			// some requests don't need a cursor or will request a
			// specific cursor internally...
			bool	loopback=false;
			switch (request) {
				case COM_SLEEP:
					loop=comSleep();
					loopback=true;
					break;
				case COM_QUIT:
					// just end the session and
					// close the connection
					debugStart("com_quit");
					debugEnd();
					loop=false;
					endsession=true;
					break;
				case COM_INIT_DB:
					loop=comInitDb();
					loopback=true;
					break;
				case COM_STATISTICS:
					loop=comStatistics();
					loopback=true;
					break;
				case COM_CONNECT:
					loop=comConnect();
					loopback=true;
					break;
				case COM_DEBUG:
					loop=comDebug();
					loopback=true;
					break;
				case COM_PING:
					loop=comPing();
					loopback=true;
					break;
				case COM_TIME:
					loop=comTime();
					loopback=true;
					break;
				case COM_DELAYED_INSERT:
					loop=comDelayedInsert();
					loopback=true;
					break;
				case COM_CHANGE_USER:
					loop=comChangeUser();
					loopback=true;
					break;
				case COM_BINLOG_DUMP:
					loop=comBinLogDump();
					loopback=true;
					break;
				case COM_TABLE_DUMP:
					loop=comTableDump();
					loopback=true;
					break;
				case COM_CONNECT_OUT:
					loop=comConnectOut();
					loopback=true;
					break;
				case COM_REGISTER_SLAVE:
					loop=comRegisterSlave();
					loopback=true;
					break;
				case COM_DAEMON:
					loop=comDaemon();
					loopback=true;
					break;
				case COM_BINLOG_DUMP_GTID:
					loop=comBinlogDumpGtid();
					loopback=true;
					break;
				case COM_RESET_CONNECTION:
					loop=comResetConnection();
					loopback=true;
					break;
				case COM_STMT_EXECUTE:
					loop=comStmtExecute();
					loopback=true;
					break;
				case COM_STMT_SEND_LONG_DATA:
					loop=comStmtSendLongData();
					loopback=true;
					break;
				case COM_STMT_CLOSE:
					loop=comStmtClose();
					loopback=true;
					break;
				case COM_STMT_RESET:
					loop=comStmtReset();
					loopback=true;
					break;
				case COM_STMT_FETCH:
					loop=comStmtFetch();
					loopback=true;
					break;
			}
			if (!loop) {
				break;
			}
			if (loopback) {
				continue;
			}

			// for the rest of the requests, we need a new cursor...
			sqlrservercursor	*cursor=cont->getCursor();
			if (!cursor) {
				// ideally we'd report that no cursor is
				// available, but this is the closest thing
				// there is to that
				if (!sendCursorNotOpenError()) {
					break;
				}
				continue;
			}
			switch (request) {
				case COM_CREATE_DB:
					loop=comCreateDb(cursor);
					break;
				case COM_DROP_DB:
					loop=comDropDb(cursor);
					break;
				case COM_QUERY:
					loop=comQuery(cursor);
					break;
				case COM_FIELD_LIST:
					loop=comFieldList(cursor);
					break;
				case COM_REFRESH:
					loop=comRefresh(cursor);
					break;
				case COM_SHUTDOWN:
					loop=comShutdown(cursor);
					break;
				case COM_PROCESS_INFO:
					loop=comProcessInfo(cursor);
					break;
				case COM_PROCESS_KILL:
					loop=comProcessKill(cursor);
					break;
				case COM_STMT_PREPARE:
					loop=comStmtPrepare(cursor);
					break;
				case COM_SET_OPTION:
					loop=comSetOption(cursor);
					break;
			}

			// release the cursor
			if (request!=COM_STMT_PREPARE) {
				cont->setState(cursor,
					SQLRCURSORSTATE_AVAILABLE);
			}

		} while (loop);
	}

	// close the client connection
	cont->closeClientConnection(0);

	// end the session if necessary
	if (endsession) {
		cont->endSession();
	}

	return status;
}

void sqlrprotocol_mysql::resetSendPacketBuffer() {
	resppacket.clear();
	writeLE(&resppacket,(uint32_t)0);
}

bool sqlrprotocol_mysql::sendPacket() {
	return sendPacket(false);
}

bool sqlrprotocol_mysql::sendPacket(bool flush) {

	// FIXME: what if resppacket.getSize() > maxpacketlength?

	// overwrite the first 4 bytes of the resppacket
	resppacket.setPosition(0);

	// size
	// 3 bytes
	uint32_t	size=hostToBE((uint32_t)resppacket.getSize()-4);
	unsigned char	*sizebytes=(unsigned char *)&size;
	resppacket.write(sizebytes[3]);
	resppacket.write(sizebytes[2]);
	resppacket.write(sizebytes[1]);

	// sequence
	// 1 byte
	resppacket.write(seq);

	if (getDebug()) {
		bytebuffer	temp;
		temp.append(sizebytes[3]);
		temp.append(sizebytes[2]);
		temp.append(sizebytes[1]);
		temp.append(seq);
		temp.append(resppacket.getBuffer(),resppacket.getSize());
		debugStart("send");
		stdoutput.printf("	size: %d\n",beToHost(size));
		stdoutput.printf("	seq:  %d\n",seq);
		debugHexDump(temp.getBuffer(),temp.getSize());
		debugEnd();
	}

	// packet data
	if (clientsock->write(resppacket.getBuffer(),
				resppacket.getSize())!=
				(ssize_t)resppacket.getSize()) {
		if (getDebug()) {
			stdoutput.write("write packet data failed\n");
			debugSystemError();
		}
		return false;
	}

	if (flush) {
		clientsock->flushWriteBuffer(-1,-1);
		if (getDebug()) {
			stdoutput.write("send packet flush...\n");
		}
	} else {
		if (getDebug()) {
			stdoutput.write("no flush...\n");
		}
	}

	// bump seq
	seq++;

	return true;
}

bool sqlrprotocol_mysql::recvPacket() {

	// size
	// 3 bytes
	uint32_t	size;
	unsigned char	*sizebytes=(unsigned char *)&size;
	sizebytes[0]=0;
	if (clientsock->read(&sizebytes[3])!=sizeof(unsigned char) ||
		clientsock->read(&sizebytes[2])!=sizeof(unsigned char) ||
		clientsock->read(&sizebytes[1])!=sizeof(unsigned char)) {
		if (getDebug()) {
			stdoutput.write("read packet size failed\n");
			debugSystemError();
		}
		return false;
	}
	reqpacketsize=beToHost(size);

	// sequence
	// 1 byte
	if (clientsock->read(&seq)!=sizeof(unsigned char)) {
		if (getDebug()) {
			stdoutput.write("read packet sequence failed\n");
			debugSystemError();
		}
		return false;
	}

	reqpacketpool.clear();
	reqpacket=reqpacketpool.allocate(reqpacketsize);

	// packet
	if (clientsock->read(reqpacket,reqpacketsize)!=(ssize_t)reqpacketsize) {
		if (getDebug()) {
			stdoutput.write("read packet failed\n");
			debugSystemError();
		}
		return false;
	}

	if (getDebug()) {
		debugStart("recv");
		stdoutput.printf("	size: %d\n",reqpacketsize);
		stdoutput.printf("	seq:  %d\n",seq);
		bytebuffer	temp;
		temp.append(sizebytes[3]);
		temp.append(sizebytes[2]);
		temp.append(sizebytes[1]);
		temp.append(seq);
		temp.append(reqpacket,reqpacketsize);
		debugHexDump(temp.getBuffer(),temp.getSize());
		debugEnd();
	}

	// bump seq
	seq++;

	return true;
}

void sqlrprotocol_mysql::generateChallenge() {

	// determine how many random bytes to generate,
	// based on the plugin type
	uint16_t	bytes=0;
	if (!charstring::compare(serverauthpluginname,
						"mysql_old_password")) {
		bytes=8;
	} else if (!charstring::compare(serverauthpluginname,
						"mysql_native_password") ||
			!charstring::compare(serverauthpluginname,
						"sha256_password") ||
			!charstring::compare(serverauthpluginname,
						"cached_sha2_password")) {
		bytes=20;
	} else if (!charstring::compare(serverauthpluginname,
						"mysql_clear_password")) {
		bytes=0;
	}

	// random bytes
	stringbuffer	str;
	uint32_t	number;
	for (uint16_t i=0; i<bytes; i++) {
		r.generateNumber(&number);
		str.append((char)randomnumber::scaleNumber(number,' ','~'));
	}
	delete[] challenge;
	challenge=str.detachString();
}

bool sqlrprotocol_mysql::initialHandshake() {
	return sendHandshake() &&
		recvHandshakeResponse() &&
		negotiateAuthMethod() &&
		negotiateMoreData() &&
		authenticate();
}

bool sqlrprotocol_mysql::sendHandshake() {

	resetSendPacketBuffer();

	if (handshake!=10) {
		buildHandshake9();
	} else {
		buildHandshake10();
	}

	return sendPacket(true);
}

void sqlrprotocol_mysql::buildHandshake10() {

	// set values to send
	char		protocolversion=0x0a;
	const char	*serverversion=cont->dbVersion();
	uint32_t	connectionid=process::getProcessId();
	serverauthpluginname="mysql_native_password";
	generateChallenge();
	servercapabilityflags=
			CLIENT_LONG_PASSWORD|
			//CLIENT_FOUND_ROWS| (client-only)
			CLIENT_LONG_FLAG|
			CLIENT_CONNECT_WITH_DB|
			//CLIENT_NO_SCHEMA|
			//CLIENT_COMPRESS|
			//CLIENT_ODBC| (client-only?)
			//CLIENT_LOCAL_FILES|
			//CLIENT_IGNORE_SPACE|
			((clientprotocol==41)?CLIENT_PROTOCOL_41:0)|
			//CLIENT_INTERACTIVE|
			//CLIENT_SSL|
			//CLIENT_IGNORE_SIGPIPE| (client-only)
			CLIENT_TRANSACTIONS|
			//CLIENT_RESERVED|
			CLIENT_SECURE_CONNECTION|
			//((clientprotocol==41)?CLIENT_MULTI_STATEMENTS:0)|
			//((clientprotocol==41)?CLIENT_MULTI_RESULTS:0)|
			//((clientprotocol==41)?CLIENT_PS_MULTI_RESULTS:0)|
			((clientprotocol==41)?CLIENT_PLUGIN_AUTH:0)|
			CLIENT_CONNECT_ATTRS|
			CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA|
			//CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS|
			//CLIENT_SESSION_TRACK|
			CLIENT_DEPRECATE_EOF|
			0
			;
	servercharacterset=LATIN1_SWEDISH_CI;
	uint16_t	statusflags=SERVER_STATUS_AUTOCOMMIT;
	char		reserved[10]={0,0,0,0,0,0,0,0,0,0};

	// debug
	if (getDebug()) {
		debugStart("handshake 10");
		stdoutput.printf("	protocol version: 0x%02x\n",
					(uint32_t)(0x000000ff&protocolversion));
		stdoutput.printf("	server version: \"%s\"\n",
							serverversion);
		stdoutput.printf("	connectionid: %ld\n",connectionid);
		stdoutput.printf("	challenge: \"%s\"\n",challenge);
		debugCapabilityFlags(servercapabilityflags);
		debugCharacterSet(servercharacterset);
		debugStatusFlags(statusflags);
		if (servercapabilityflags&CLIENT_PLUGIN_AUTH) {
			stdoutput.printf("	auth plugin name: \"%s\"\n",
							serverauthpluginname);
		}
		debugEnd();
	}

	// split up status flags
	uint16_t	lowservercapabilityflags=
				(servercapabilityflags&0x0000FFFF);
	uint16_t	highservercapabilityflags=
				((servercapabilityflags&0xFFFF0000)>>16);

	// build packet
	write(&resppacket,protocolversion);
	write(&resppacket,serverversion,charstring::length(serverversion)+1);
	writeLE(&resppacket,connectionid);
	write(&resppacket,challenge,8);
	write(&resppacket,(char)0x00);
	writeLE(&resppacket,lowservercapabilityflags);
	write(&resppacket,servercharacterset);
	writeLE(&resppacket,statusflags);
	// Old enough versions of the mariadb jdbc driver (eg. 1.4.6) don't
	// properly process the server capabilities and crash soon after.
	// Sending all 1's for the high-order bits of the flags appears to
	// solve the problem.
	if (oldmariadbjdbcservercapabilitieshack) {
		writeLE(&resppacket,(uint16_t)0xFFFF);
	} else {
		writeLE(&resppacket,highservercapabilityflags);
	}
	if (servercapabilityflags&CLIENT_PLUGIN_AUTH) {
		write(&resppacket,
			(unsigned char)(charstring::length(challenge)+1));
	} else {
		write(&resppacket,(char)0x00);
	}
	write(&resppacket,reserved,sizeof(reserved));
	if (servercapabilityflags&CLIENT_SECURE_CONNECTION) {
		write(&resppacket,challenge+8,
				charstring::length(challenge+8)+1);
	}
	if (servercapabilityflags&CLIENT_PLUGIN_AUTH) {
		write(&resppacket,serverauthpluginname,
				charstring::length(serverauthpluginname)+1);
	}
}

void sqlrprotocol_mysql::buildHandshake9() {

	// set values to send
	char		protocolversion=0x09;
	uint32_t	connectionid=process::getProcessId();
	const char	*serverversion=cont->dbVersion();
	serverauthpluginname="mysql_old_password";
	generateChallenge();

	// debug
	if (getDebug()) {
		debugStart("handshake 9");
		stdoutput.printf("	protocol version: 0x%02x\n",
					(uint32_t)(0x000000ff&protocolversion));
		stdoutput.printf("	server version: \"%s\"\n",
							serverversion);
		stdoutput.printf("	connectionid: %ld\n",connectionid);
		stdoutput.printf("	scramble: \"%s\"\n",challenge);
		debugCapabilityFlags(servercapabilityflags);
		debugEnd();
	}

	// convert some values to little endian
	servercapabilityflags=hostToLE(servercapabilityflags);

	// build packet
	write(&resppacket,protocolversion);
	write(&resppacket,serverversion,charstring::length(serverversion)+1);
	writeLE(&resppacket,connectionid);
	write(&resppacket,challenge,charstring::length(challenge)+1);
}

bool sqlrprotocol_mysql::recvHandshakeResponse() {

	if (!recvPacket()) {
		return false;
	}

	const unsigned char	*rp=reqpacket;

	uint32_t	capabilityflags;
	readLE(rp,&capabilityflags,&rp);
	rp-=sizeof(uint32_t);

	if (capabilityflags&CLIENT_PROTOCOL_41) {
		parseHandshakeResponse41(rp,reqpacketsize);
	} else {
		parseHandshakeResponse320(rp,reqpacketsize);
	}
	return true;
}

void sqlrprotocol_mysql::parseHandshakeResponse41(
					const unsigned char *rp,
					uint64_t rplen) {

	const unsigned char	*end=rp+rplen;

	debugStart("handshake response 41");

	// capability flags
	readLE(rp,&clientcapabilityflags,&rp);
	if (getDebug()) {
		debugCapabilityFlags(clientcapabilityflags);
	}

	// max-packet size
	uint32_t	maxpacketsize;
	readLE(rp,&maxpacketsize,&rp);
	if (getDebug()) {
		stdoutput.printf("	max-packet size: %d\n",maxpacketsize);
	}

	// character set
	clientcharacterset=*rp;
	rp++;
	if (getDebug()) {
		debugCharacterSet(clientcharacterset);
	}

	// reserved
	rp+=23;

	// username
	delete[] username;
	username=charstring::duplicate((const char *)rp);
	rp+=charstring::length(username)+1;
	if (getDebug()) {
		stdoutput.printf("	username: \"%s\"\n",username);
	}

	// challenge response
	responselength=0;
	if (servercapabilityflags&
			CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA &&
		clientcapabilityflags&
			CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA) {
		responselength=readLenEncInt(rp,&rp);
		delete[] response;
		response=(char *)bytestring::duplicate(rp,responselength);
		rp+=responselength;
	} else if (servercapabilityflags&CLIENT_SECURE_CONNECTION &&
			clientcapabilityflags&CLIENT_SECURE_CONNECTION) {
		responselength=*((char *)rp);
		rp++;
		delete[] response;
		response=(char *)bytestring::duplicate(rp,responselength);
		rp+=responselength;
	} else {
		for (const unsigned char *r=rp; *r && r!=end; r++) {
			responselength++;
		}
		delete[] response;
		response=(char *)bytestring::duplicate(rp,responselength);
		rp+=responselength+1;
	}

	// sometimes there's a null terminator here
	// eg. the Connector/J 5.1.45 sends one, even when it sends the length
	if (!*rp) {
		rp++;
	}

	if (getDebug()) {
		stdoutput.printf("	challenge response length: %lld\n",
								responselength);
		stdoutput.printf("	challenge response: \"");
		stdoutput.safePrint(response,responselength);
		stdoutput.printf("\"\n");
		if (rp==end) {
			if (clientcapabilityflags&CLIENT_CONNECT_WITH_DB) {
				stdoutput.write("	short packet, "
							"db missing\n");
			}
			if (clientcapabilityflags&CLIENT_PLUGIN_AUTH) {
				stdoutput.write("	short packet, "
							"client plugin name "
							"missing\n");
			}
			if (clientcapabilityflags&CLIENT_CONNECT_ATTRS) {
				stdoutput.write("	short packet, "
							"connect attrs "
							"missing\n");
			}
		}
	}

	// database
	delete[] database;
	database=NULL;
	if (rp<end && clientcapabilityflags&CLIENT_CONNECT_WITH_DB) {
		database=charstring::duplicate((const char *)rp);
		rp+=charstring::length(database)+1;
		if (getDebug()) {
			stdoutput.printf("	database: \"%s\"\n",database);
		}
	}

	// auth plugin name
	if (rp<end && clientcapabilityflags&CLIENT_PLUGIN_AUTH) {
		clientauthpluginname=(const char *)rp;
		rp+=charstring::length(clientauthpluginname)+1;
		if (getDebug()) {
			stdoutput.printf("	auth plugin name: \"%s\"\n",
							clientauthpluginname);
		}
	}

	if (rp<end && clientcapabilityflags&CLIENT_CONNECT_ATTRS) {

		if (getDebug()) {
			stdoutput.write("	connect attrs:\n");
		}

		// length of all key-values
		uint64_t		alllen=readLenEncInt(rp,&rp);
		const unsigned char	*start=rp;
		while ((uint64_t)(rp-start)<alllen) {

			// key
			uint64_t	keylen=readLenEncInt(rp,&rp);
			char		*key=charstring::duplicate(
						(const char *)rp,keylen);
			rp+=keylen;

			// value
			uint64_t	vallen=readLenEncInt(rp,&rp);
			char		*val=charstring::duplicate(
						(const char *)rp,vallen);
			rp+=vallen;

			if (getDebug()) {
				stdoutput.printf("		%s=%s\n",
								key,val);
			}

			// FIXME: do something with these
			delete[] key;
			delete[] val;
		}
	}

	// if the client wasn't capable of sending us the plugin name but
	// did send us a response, then assume that it liked the plugin
	if (!(clientcapabilityflags&CLIENT_CONNECT_ATTRS) &&
			!charstring::isNullOrEmpty(response)) {
		clientauthpluginname=serverauthpluginname;
	}

	debugEnd();
}

void sqlrprotocol_mysql::parseHandshakeResponse320(
					const unsigned char *rp,
					uint64_t rplen) {

	debugStart("handshake response 320");

	// capability flags
	uint16_t	shortcapabilityflags;
	readLE(rp,&shortcapabilityflags,&rp);
	if (getDebug()) {
		debugCapabilityFlags(shortcapabilityflags);
	}
	clientcapabilityflags=shortcapabilityflags;

	// max-packet size
	uint32_t	maxpacketsize;
	bytestring::copy(&maxpacketsize,&rp,sizeof(uint32_t));
	rp+=3;
	maxpacketsize=(maxpacketsize&0xFFFFFF00);
	maxpacketsize=leToHost(maxpacketsize);
	// FIXME: sanity check on maxpacketsize
	if (getDebug()) {
		stdoutput.printf("	max-packet size: %d\n",maxpacketsize);
	}

	// username
	delete[] username;
	username=charstring::duplicate((const char *)rp);
	rp+=charstring::length(username)+1;
	if (getDebug()) {
		stdoutput.printf("	username: \"%s\"\n",username);
	}

	// challenge response
	delete[] response;
	response=charstring::duplicate((const char *)rp);
	responselength=charstring::length(response);
	rp+=charstring::length(response)+1;
	if (getDebug()) {
		stdoutput.write("	challenge response: ");
		stdoutput.safePrint(response);
		stdoutput.write("\n");
	}

	// database
	delete[] database;
	database=NULL;
	if (clientcapabilityflags&CLIENT_CONNECT_WITH_DB) {
		database=charstring::duplicate((const char *)rp);
		rp+=charstring::length(database)+1;
		if (getDebug()) {
			stdoutput.printf("	database: \"%s\"\n",database);
		}
	}

	// with protocol 320, assume "mysql_old_password"
	clientauthpluginname="mysql_old_password";

	debugEnd();
}

static const char *supportedauthplugins[]={
	"mysql_native_password",
	//"mysql_old_password",
	//"authentication_windows_client",
	"mysql_clear_password",
	//"sha256_password",
	//"caching_sha2_password",
	NULL
};

bool sqlrprotocol_mysql::negotiateAuthMethod() {

	// protocol 320 only supports mysql_old_password
	// if the client says it doesn't support protocol 41, then we must be
	// using protocol 320, and so we must also be using mysql_old_password
	if (!(clientcapabilityflags&CLIENT_PROTOCOL_41)) {
		if (getDebug()) {
			debugStart("negotiate auth method");
			stdoutput.write("	mysql_old_password "
					"(because using protocol 320)\n");
			debugEnd();
		}
		return true;
	}

	// if the client doesn't support plugin auths, then we need to use
	// the "old" auth switch request to request that it switch to
	// mysql_old_password
	if (!(clientcapabilityflags&CLIENT_PLUGIN_AUTH)) {
		serverauthpluginname="mysql_old_password";
		if (getDebug()) {
			debugStart("negotiate auth method");
			stdoutput.printf("	trying %s\n",
						serverauthpluginname);
			debugEnd();
		}
		generateChallenge();
		return sendOldAuthSwitchRequest() && recvAuthResponse();
	}

	// if the client requested the auth that we offered, then we're good
	if (!charstring::compare(clientauthpluginname,serverauthpluginname)) {
		if (getDebug()) {
			debugStart("negotiate auth method");
			stdoutput.printf("	agreed on %s\n",
						clientauthpluginname);
			debugEnd();
		}
		return true;
	}

	// if the client requested an auth that we support, then offer it
	if (charstring::inSet(clientauthpluginname,supportedauthplugins)) {

		// generate challenge...
		for (const char * const *plugin=supportedauthplugins;
							*plugin; plugin++) {
			if (!charstring::compare(*plugin,
						clientauthpluginname)) {
				serverauthpluginname=*plugin;
			}
		}
		if (getDebug()) {
			debugStart("negotiate auth method");
			stdoutput.printf("	trying %s\n",
						serverauthpluginname);
			debugEnd();
		}
		generateChallenge();

		// negotiate...
		if (!sendAuthSwitchRequest() || !recvAuthResponse()) {
			return false;
		}
		clientauthpluginname=
			(!charstring::isNullOrEmpty(response))?
					serverauthpluginname:NULL;
		if (clientauthpluginname) {
			if (getDebug()) {
				debugStart("negotiate auth method");
				stdoutput.printf("	agreed on %s\n",
							serverauthpluginname);
				debugEnd();
			}
			return true;
		}
	}

	// try all plugins, one at a time...
	clientauthpluginname=NULL;
	for (const char * const *plugin=supportedauthplugins;
				*plugin && !clientauthpluginname; plugin++) {

		// generate challenge...
		serverauthpluginname=*plugin;
		if (getDebug()) {
			debugStart("negotiate auth method");
			stdoutput.printf("	trying %s\n",
						serverauthpluginname);
			debugEnd();
		}
		generateChallenge();

		// negotiate
		if (!sendAuthSwitchRequest() || !recvAuthResponse()) {
			return false;
		}
		clientauthpluginname=
			(!charstring::isNullOrEmpty(response))?
					serverauthpluginname:NULL;
		if (clientauthpluginname) {
			if (getDebug()) {
				debugStart("negotiate auth method");
				stdoutput.printf("	agreed on %s\n",
							serverauthpluginname);
				debugEnd();
			}
			return true;
		}
	}

	if (getDebug()) {
		debugStart("negotiate auth method");
		stdoutput.write("	could not agree on auth method\n");
		debugEnd();
	}
	// FIXME: send error?
	return false;
}

bool sqlrprotocol_mysql::sendAuthSwitchRequest() {

	resetSendPacketBuffer();

	if (getDebug()) {
		debugStart("auth switch request");
		stdoutput.printf("	auth plugin name: \"%s\"\n",
							serverauthpluginname);
		stdoutput.printf("	challenge: \"%s\"\n",challenge);
		debugEnd();
	}

	write(&resppacket,(char)0xFE);
	write(&resppacket,serverauthpluginname,
			charstring::length(serverauthpluginname)+1);
	write(&resppacket,challenge,charstring::length(challenge)+1);

	return sendPacket(true);
}

bool sqlrprotocol_mysql::sendOldAuthSwitchRequest() {

	resetSendPacketBuffer();

	if (getDebug()) {
		debugStart("old auth switch request");
		debugEnd();
	}

	write(&resppacket,(char)0xFE);

	return sendPacket(true);
}

bool sqlrprotocol_mysql::recvAuthResponse() {

	if (!recvPacket()) {
		return false;
	}

	const unsigned char	*rp=reqpacket;
	delete[] response;
	response=charstring::duplicate((const char *)rp,reqpacketsize);
	responselength=reqpacketsize;

	// assume client accepted the auth type...
	clientauthpluginname=serverauthpluginname;

	if (getDebug()) {
		debugStart("auth response");
		stdoutput.printf("	challenge response length: %lld\n",
								responselength);
		stdoutput.printf("	challenge response: \"");
		stdoutput.safePrint(response,responselength);
		stdoutput.printf("\"\n");
		debugEnd();
	}

	return true;
}

bool sqlrprotocol_mysql::negotiateMoreData() {

	for (;;) {

		// if the client sent 0x01 as the challenge response,
		// then we need to send the server's rsa public key file...
		if (responselength==1 && response[0]==0x01) {

			moredata.clear();
			// FIXME: send the actual rsa public key file...
			moredata.append("...servers rsa public key file...");

			if (!sendAuthMoreDataPacket() && recvAuthResponse()) {
				return false;
			}
		} else

		// there could be other cases too...
		{
			break;
		}
	}

	return true;
}

bool sqlrprotocol_mysql::sendAuthMoreDataPacket() {

	resetSendPacketBuffer();

	if (getDebug()) {
		debugStart("auth more data");
		stdoutput.printf("	more data: %s\n",moredata.getString());
		debugEnd();
	}

	write(&resppacket,(char)0x01);
	write(&resppacket,moredata.getBuffer(),moredata.getSize());

	return sendPacket(true);
}

bool sqlrprotocol_mysql::authenticate() {

	sqlrmysqlcredentials	cred;
	cred.setUser(username);
	cred.setPassword(response);
	cred.setPasswordLength(responselength);
	cred.setMethod(clientauthpluginname);
	cred.setExtra(challenge);
	bool	retval=cont->auth(&cred);
	if (getDebug()) {
		debugStart("authenticate");
		stdoutput.printf("	auth %s\n",(retval)?"success":"failed");
		debugEnd();
	}

	// FIXME: there are apparently cases where an
	// AuthMoreData packet should be sent here

	if (!retval) {
		char	*peeraddr=clientsock->getPeerAddress();
		stringbuffer	err;
		err.append("Access denied for user ");
		err.append(username);
		err.append('@');
		// FIXME: convert to host name?
		err.append(peeraddr);
		err.append(" using password: YES)");
		delete[] peeraddr;
		sendErrPacket(1045,err.getString(),"28000");
		return false;
	}

	// if we were passed a database at login, then select it here
	if (database) {
		retval=cont->selectDatabase(database);
		if (getDebug()) {
			debugStart("select database");
			stdoutput.printf("	%s: %s\n",
					database,(retval)?"success":"failed");
			debugEnd();
		}
		if (!retval) {
			return sendError();
		}
	}

	return sendOkPacket();
}

bool sqlrprotocol_mysql::sendOkPacket() {
	return sendOkPacket(true,0,0,0,0,"",0,"");
}

bool sqlrprotocol_mysql::sendOkPacket(bool noteof,
					uint64_t affectedrows,
					uint64_t lastinsertid,
					uint16_t statusflags,
					uint16_t warnings,
					const char *info,
					char sessionstatechangetype,
					const char *sessionstatechangedata) {

	// update statusflags
	if (cont->inTransaction()) {
		statusflags|=SERVER_STATUS_IN_TRANS;
	} else {
		statusflags|=SERVER_STATUS_AUTOCOMMIT;
	}

	char	header=(noteof)?0x00:0xFE;

	if (getDebug()) {
		debugStart((noteof)?"ok":"ok (eof)");
		stdoutput.printf("	header: 0x%02x\n",
				(uint32_t)(0x000000ff&header));
		stdoutput.printf("	affected rows: %lld\n",affectedrows);
		stdoutput.printf("	last insert id: %lld\n",lastinsertid);
		if (servercapabilityflags&CLIENT_PROTOCOL_41 &&
				clientcapabilityflags&CLIENT_PROTOCOL_41) {
			debugStatusFlags(statusflags);
			stdoutput.printf("	warnings: %hd\n",warnings);
		} else if (servercapabilityflags&CLIENT_TRANSACTIONS &&
				clientcapabilityflags&CLIENT_TRANSACTIONS) {
			debugStatusFlags(statusflags);
		}
		stdoutput.printf("	info: \"%s\"\n",info);
		if (statusflags&SERVER_SESSION_STATE_CHANGED) {
			stdoutput.printf("	session state change "
						"type: 0x%02x\n",
				(uint32_t)(0x000000ff&sessionstatechangetype));
			stdoutput.printf("	session state change "
						"data: \"%s\"\n",
							sessionstatechangedata);
		}
		debugEnd();
	}

	resetSendPacketBuffer();

	write(&resppacket,header);
	writeLenEncInt(&resppacket,affectedrows);
	writeLenEncInt(&resppacket,lastinsertid);
	if (servercapabilityflags&CLIENT_PROTOCOL_41 &&
			clientcapabilityflags&CLIENT_PROTOCOL_41) {
		writeLE(&resppacket,statusflags);
		writeLE(&resppacket,warnings);
	} else if (servercapabilityflags&CLIENT_TRANSACTIONS &&
			clientcapabilityflags&CLIENT_TRANSACTIONS) {
		writeLE(&resppacket,statusflags);
	}
	if (servercapabilityflags&CLIENT_SESSION_TRACK &&
			clientcapabilityflags&CLIENT_SESSION_TRACK) {
		writeLenEncStr(&resppacket,info);
		if (statusflags&SERVER_SESSION_STATE_CHANGED) {
			write(&resppacket,sessionstatechangetype);
			writeLenEncStr(&resppacket,sessionstatechangedata);
		}
	} else {
		write(&resppacket,info,charstring::length(info));
	}

	return sendPacket(true);
}

bool sqlrprotocol_mysql::sendErrPacket(uint16_t errorcode,
					const char *errormessage,
					const char *sqlstate) {
	return sendErrPacket(errorcode,errormessage,
				charstring::length(errormessage),sqlstate);
}

bool sqlrprotocol_mysql::sendErrPacket(uint16_t errorcode,
					const char *errormessage,
					uint64_t errorlength,
					const char *sqlstate) {
	resetSendPacketBuffer();

	if (getDebug()) {
		debugStart("err");
		stdoutput.printf("	error code: %hd\n",errorcode);
		stdoutput.printf("	error message: \"%.*s\"\n",
							(uint32_t)errorlength,
							errormessage);
		stdoutput.printf("	error length: %lld\n",errorlength);
		stdoutput.printf("	sql state: \"%s\"\n",sqlstate);
		debugEnd();
	}

	write(&resppacket,(char)0xFF);
	writeLE(&resppacket,errorcode);
	if (clientcapabilityflags&CLIENT_PROTOCOL_41) {
		write(&resppacket,(char)0x23);
		write(&resppacket,sqlstate);
	}
	write(&resppacket,errormessage,errorlength);
	write(&resppacket,'\0');

	return sendPacket(true);
}

bool sqlrprotocol_mysql::sendEofPacket(uint16_t warnings,
					uint16_t statusflags) {

	// there are actually 2 versions of this packet...

	// the first is just a modified ok packet and should
	// be sent if the client requested CLIENT_DEPRECATE_EOF...
	if (servercapabilityflags&CLIENT_DEPRECATE_EOF &&
		clientcapabilityflags&CLIENT_DEPRECATE_EOF) {
		return sendOkPacket(false,0,0,statusflags,warnings,"",0,"");
	}

	// the second is an "old school" eof packet...
	resetSendPacketBuffer();

	// update statusflags
	if (cont->inTransaction()) {
		statusflags|=SERVER_STATUS_IN_TRANS;
	} else {
		statusflags|=SERVER_STATUS_AUTOCOMMIT;
	}

	if (getDebug()) {
		debugStart("eof");
		stdoutput.write("	header: 0xfe\n");
		stdoutput.printf("	warnings: %hd\n",warnings);
		debugStatusFlags(statusflags);
		debugEnd();
	}

	write(&resppacket,(char)0xfe);
	if (servercapabilityflags&CLIENT_PROTOCOL_41 &&
		clientcapabilityflags&CLIENT_PROTOCOL_41) {
		writeLE(&resppacket,warnings);
		writeLE(&resppacket,statusflags);
	}

	return sendPacket(true);
}

void sqlrprotocol_mysql::debugCapabilityFlags(uint32_t capabilityflags) {
	stdoutput.write("	capability flags:\n");
	stdoutput.write("		");
	stdoutput.printf("0x%08x\n",capabilityflags);
	stdoutput.write("		");
	stdoutput.printBits(capabilityflags);
	stdoutput.write("\n");
	if (capabilityflags&CLIENT_LONG_PASSWORD) {
		stdoutput.write("		"
				"CLIENT_LONG_PASSWORD\n");
	}
	if (capabilityflags&CLIENT_LONG_FLAG) {
		stdoutput.write("		"
				"CLIENT_LONG_FLAG\n");
	}
	if (capabilityflags&CLIENT_CONNECT_WITH_DB) {
		stdoutput.write("		"
				"CLIENT_CONNECT_WITH_DB\n");
	}
	if (capabilityflags&CLIENT_NO_SCHEMA) {
		stdoutput.write("		"
				"CLIENT_NO_SCHEMA\n");
	}
	if (capabilityflags&CLIENT_COMPRESS) {
		stdoutput.write("		"
				"CLIENT_COMPRESS\n");
	}
	if (capabilityflags&CLIENT_ODBC) {
		stdoutput.write("		"
				"CLIENT_ODBC\n");
	}
	if (capabilityflags&CLIENT_LOCAL_FILES) {
		stdoutput.write("		"
				"CLIENT_LOCAL_FILES\n");
	}
	if (capabilityflags&CLIENT_IGNORE_SPACE) {
		stdoutput.write("		"
				"CLIENT_IGNORE_SPACE\n");
	}
	if (capabilityflags&CLIENT_PROTOCOL_41) {
		stdoutput.write("		"
				"CLIENT_PROTOCOL_41\n");
	}
	if (capabilityflags&CLIENT_INTERACTIVE) {
		stdoutput.write("		"
				"CLIENT_INTERACTIVE\n");
	}
	if (capabilityflags&CLIENT_SSL) {
		stdoutput.write("		"
				"CLIENT_SSL\n");
	}
	if (capabilityflags&CLIENT_IGNORE_SIGPIPE) {
		stdoutput.write("		"
				"CLIENT_IGNORE_SIGPIPE\n");
	}
	if (capabilityflags&CLIENT_TRANSACTIONS) {
		stdoutput.write("		"
				"CLIENT_TRANSACTIONS\n");
	}
	if (capabilityflags&CLIENT_RESERVED) {
		stdoutput.write("		"
				"CLIENT_RESERVED\n");
	}
	if (capabilityflags&CLIENT_SECURE_CONNECTION) {
		stdoutput.write("		"
				"CLIENT_SECURE_CONNECTION\n");
	}
	if (capabilityflags&CLIENT_MULTI_STATEMENTS) {
		stdoutput.write("		"
				"CLIENT_MULTI_STATEMENTS\n");
	}
	if (capabilityflags&CLIENT_MULTI_RESULTS) {
		stdoutput.write("		"
				"CLIENT_MULTI_RESULTS\n");
	}
	if (capabilityflags&CLIENT_PS_MULTI_RESULTS) {
		stdoutput.write("		"
				"CLIENT_PS_MULTI_RESULTS\n");
	}
	if (capabilityflags&CLIENT_PLUGIN_AUTH) {
		stdoutput.write("		"
				"CLIENT_PLUGIN_AUTH\n");
	}
	if (capabilityflags&CLIENT_CONNECT_ATTRS) {
		stdoutput.write("		"
				"CLIENT_CONNECT_ATTRS\n");
	}
	if (capabilityflags&CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA) {
		stdoutput.write("		"
				"CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA\n");
	}
	if (capabilityflags&CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS) {
		stdoutput.write("		"
				"CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS\n");
	}
	if (capabilityflags&CLIENT_SESSION_TRACK) {
		stdoutput.write("		"
				"CLIENT_SESSION_TRACK\n");
	}
	if (capabilityflags&CLIENT_DEPRECATE_EOF) {
		stdoutput.write("		"
				"CLIENT_DEPRECATE_EOF\n");
	}
}

void sqlrprotocol_mysql::debugCharacterSet(unsigned char characterset) {
	stdoutput.printf("	character set: 0x%02x\n",
				(uint32_t)(0x000000ff&characterset));
}

void sqlrprotocol_mysql::debugStatusFlags(uint16_t statusflags) {
	stdoutput.write("	status flags:\n");
	stdoutput.write("		");
	stdoutput.printf("0x%04x\n",statusflags);
	stdoutput.write("		");
	stdoutput.printBits(statusflags);
	stdoutput.write("\n");
	if (statusflags&SERVER_STATUS_IN_TRANS) {
		stdoutput.write("		"
				"SERVER_STATUS_IN_TRANS\n");
	}
	if (statusflags&SERVER_STATUS_AUTOCOMMIT) {
		stdoutput.write("		"
				"SERVER_STATUS_AUTOCOMMIT\n");
	}
	if (statusflags&SERVER_MORE_RESULTS_EXISTS) {
		stdoutput.write("		"
				"SERVER_MORE_RESULTS_EXISTS\n");
	}
	if (statusflags&SERVER_STATUS_NO_GOOD_INDEX_USED) {
		stdoutput.write("		"
				"SERVER_STATUS_NO_GOOD_INDEX_USED\n");
	}
	if (statusflags&SERVER_STATUS_NO_INDEX_USED) {
		stdoutput.write("		"
				"SERVER_STATUS_NO_INDEX_USED\n");
	}
	if (statusflags&SERVER_STATUS_CURSOR_EXISTS) {
		stdoutput.write("		"
				"SERVER_STATUS_CURSOR_EXISTS\n");
	}
	if (statusflags&SERVER_STATUS_LAST_ROW_SENT) {
		stdoutput.write("		"
				"SERVER_STATUS_LAST_ROW_SENT\n");
	}
	if (statusflags&SERVER_STATUS_DB_DROPPED) {
		stdoutput.write("		"
				"SERVER_STATUS_DB_DROPPED\n");
	}
	if (statusflags&SERVER_STATUS_NO_BACKSLASH_ESCAPES) {
		stdoutput.write("		"
				"SERVER_STATUS_NO_BACKSLASH_ESCAPES\n");
	}
	if (statusflags&SERVER_STATUS_METADATA_CHANGED) {
		stdoutput.write("		"
				"SERVER_STATUS_METADATA_CHANGED\n");
	}
	if (statusflags&SERVER_QUERY_WAS_SLOW) {
		stdoutput.write("		"
				"SERVER_QUERY_WAS_SLOW\n");
	}
	if (statusflags&SERVER_PS_OUT_PARAMS) {
		stdoutput.write("		"
				"SERVER_PS_OUT_PARAMS\n");
	}
	if (statusflags&SERVER_STATUS_IN_TRANS_READONLY) {
		stdoutput.write("		"
				"SERVER_STATUS_IN_TRANS_READONLY\n");
	}
	if (statusflags&SERVER_SESSION_STATE_CHANGED) {
		stdoutput.write("		"
				"SERVER_SESSION_STATE_CHANGED\n");
	}
}

void sqlrprotocol_mysql::debugColumnType(const char *name,
						unsigned char columntype) {
	stdoutput.printf("	type: %s (0x%02x)\n",name,
				(uint32_t)(0x000000ff&columntype));
	debugColumnType(columntype);
}

void sqlrprotocol_mysql::debugColumnType(unsigned char columntype) {
	stdoutput.write("		");
	switch (columntype) {
		case MYSQL_TYPE_DECIMAL:
			stdoutput.write("MYSQL_TYPE_DECIMAL\n");
			break;
		case MYSQL_TYPE_TINY:
			stdoutput.write("MYSQL_TYPE_TINY\n");
			break;
		case MYSQL_TYPE_SHORT:
			stdoutput.write("MYSQL_TYPE_SHORT\n");
			break;
		case MYSQL_TYPE_LONG:
			stdoutput.write("MYSQL_TYPE_LONG\n");
			break;
		case MYSQL_TYPE_FLOAT:
			stdoutput.write("MYSQL_TYPE_FLOAT\n");
			break;
		case MYSQL_TYPE_DOUBLE:
			stdoutput.write("MYSQL_TYPE_DOUBLE\n");
			break;
		case MYSQL_TYPE_NULL:
			stdoutput.write("MYSQL_TYPE_NULL\n");
			break;
		case MYSQL_TYPE_TIMESTAMP:
			stdoutput.write("MYSQL_TYPE_TIMESTAMP\n");
			break;
		case MYSQL_TYPE_LONGLONG:
			stdoutput.write("MYSQL_TYPE_LONGLONG\n");
			break;
		case MYSQL_TYPE_INT24:
			stdoutput.write("MYSQL_TYPE_INT24\n");
			break;
		case MYSQL_TYPE_DATE:
			stdoutput.write("MYSQL_TYPE_DATE\n");
			break;
		case MYSQL_TYPE_TIME:
			stdoutput.write("MYSQL_TYPE_TIME\n");
			break;
		case MYSQL_TYPE_DATETIME:
			stdoutput.write("MYSQL_TYPE_DATETIME\n");
			break;
		case MYSQL_TYPE_YEAR:
			stdoutput.write("MYSQL_TYPE_YEAR\n");
			break;
		case MYSQL_TYPE_NEWDATE:
			stdoutput.write("MYSQL_TYPE_NEWDATE\n");
			break;
		case MYSQL_TYPE_VARCHAR:
			stdoutput.write("MYSQL_TYPE_VARCHAR\n");
			break;
		case MYSQL_TYPE_BIT:
			stdoutput.write("MYSQL_TYPE_BIT\n");
			break;
		case MYSQL_TYPE_TIMESTAMP2:
			stdoutput.write("MYSQL_TYPE_TIMESTAMP2\n");
			break;
		case MYSQL_TYPE_DATETIME2:
			stdoutput.write("MYSQL_TYPE_DATETIME2\n");
			break;
		case MYSQL_TYPE_TIME2:
			stdoutput.write("MYSQL_TYPE_TIME2\n");
			break;
		case MYSQL_TYPE_NEWDECIMAL:
			stdoutput.write("MYSQL_TYPE_NEWDECIMAL\n");
			break;
		case MYSQL_TYPE_ENUM:
			stdoutput.write("MYSQL_TYPE_ENUM\n");
			break;
		case MYSQL_TYPE_SET:
			stdoutput.write("MYSQL_TYPE_SET\n");
			break;
		case MYSQL_TYPE_TINY_BLOB:
			stdoutput.write("MYSQL_TYPE_TINY_BLOB\n");
			break;
		case MYSQL_TYPE_MEDIUM_BLOB:
			stdoutput.write("MYSQL_TYPE_MEDIUM_BLOB\n");
			break;
		case MYSQL_TYPE_LONG_BLOB:
			stdoutput.write("MYSQL_TYPE_LONG_BLOB\n");
			break;
		case MYSQL_TYPE_BLOB:
			stdoutput.write("MYSQL_TYPE_BLOB\n");
			break;
		case MYSQL_TYPE_VAR_STRING:
			stdoutput.write("MYSQL_TYPE_VAR_STRING\n");
			break;
		case MYSQL_TYPE_STRING:
			stdoutput.write("MYSQL_TYPE_STRING\n");
			break;
		case MYSQL_TYPE_GEOMETRY:
			stdoutput.write("MYSQL_TYPE_GEOMETRY\n");
			break;
		default:
			stdoutput.write("unknown MYSQL_TYPE\n");
			break;
	}
}

void sqlrprotocol_mysql::debugColumnFlags(uint16_t columnflags) {
	stdoutput.write("	column flags:\n");
	stdoutput.write("		");
	stdoutput.printf("0x%04x\n",columnflags);
	stdoutput.write("		");
	stdoutput.printBits(columnflags);
	stdoutput.write("\n");
	if (columnflags&NOT_NULL_FLAG) {
		stdoutput.write("		"
				"NOT_NULL_FLAG\n");
	}
	if (columnflags&PRI_KEY_FLAG) {
		stdoutput.write("		"
				"PRI_KEY_FLAG\n");
	}
	if (columnflags&UNIQUE_KEY_FLAG) {
		stdoutput.write("		"
				"UNIQUE_KEY_FLAG\n");
	}
	if (columnflags&MULTIPLE_KEY_FLAG) {
		stdoutput.write("		"
				"MULTIPLE_KEY_FLAG\n");
	}
	if (columnflags&UNSIGNED_FLAG) {
		stdoutput.write("		"
				"UNSIGNED_FLAG\n");
	}
	if (columnflags&ZEROFILL_FLAG) {
		stdoutput.write("		"
				"ZEROFILL_FLAG\n");
	}
	if (columnflags&BINARY_FLAG) {
		stdoutput.write("		"
				"BINARY_FLAG\n");
	}
	if (columnflags&AUTO_INCREMENT_FLAG) {
		stdoutput.write("		"
				"AUTO_INCREMENT_FLAG\n");
	}
	if (columnflags&ENUM_FLAG) {
		stdoutput.write("		"
				"ENUM_FLAG\n");
	}
	if (columnflags&SET_FLAG) {
		stdoutput.write("		"
				"SET_FLAG\n");
	}
	if (columnflags&BLOB_FLAG) {
		stdoutput.write("		"
				"BLOB_FLAG\n");
	}
	if (columnflags&TIMESTAMP_FLAG) {
		stdoutput.write("		"
				"TIMESTAMP_FLAG\n");
	}
	if (columnflags&NUM_FLAG) {
		stdoutput.write("		"
				"NUM_FLAG\n");
	}
}

void sqlrprotocol_mysql::debugSystemError() {
	char	*err=error::getErrorString();
	stdoutput.printf("%s\n",err);
	delete[] err;
}

void sqlrprotocol_mysql::debugRefreshCommand(unsigned char command) {
	stdoutput.write("	refresh command:\n");
	stdoutput.printf("		%08x\n",command);
	switch (command) {
		case REFRESH_GRANT:
			stdoutput.write("		"
					"REFRESH_GRANT\n");
			break;
		case REFRESH_LOG:
			stdoutput.write("		"
					"REFRESH_LOG\n");
			break;
		case REFRESH_TABLES:
			stdoutput.write("		"
					"REFRESH_TABLES\n");
			break;
		case REFRESH_HOSTS:
			stdoutput.write("		"
					"REFRESH_HOSTS\n");
			break;
		case REFRESH_STATUS:
			stdoutput.write("		"
					"REFRESH_STATUS\n");
			break;
		case REFRESH_THREADS:
			stdoutput.write("		"
					"REFRESH_THREADS\n");
			break;
		case REFRESH_SLAVE:
			stdoutput.write("		"
					"REFRESH_SLAVE\n");
			break;
		case REFRESH_MASTER:
			stdoutput.write("		"
					"REFRESH_MASTER\n");
			break;
	}
}

void sqlrprotocol_mysql::debugShutdownCommand(unsigned char command) {
	stdoutput.write("	shutdown command:\n");
	stdoutput.printf("		%08x\n",command);
	if (command==SHUTDOWN_DEFAULT) {
		stdoutput.write("		"
					"SHUTDOWN_DEFAULT\n");
	}
	if (command&SHUTDOWN_WAIT_CONNECTIONS) {
		stdoutput.write("		"
					"SHUTDOWN_WAIT_CONNECTIONS\n");
	}
	if (command&SHUTDOWN_WAIT_TRANSACTIONS) {
		stdoutput.write("		"
					"SHUTDOWN_WAIT_TRANSACTIONS\n");
	}
	if (command&SHUTDOWN_WAIT_UPDATES) {
		stdoutput.write("		"
					"SHUTDOWN_WAIT_UPDATES\n");
	}
	if (command&SHUTDOWN_WAIT_ALL_BUFFERS) {
		stdoutput.write("		"
					"SHUTDOWN_WAIT_ALL_BUFFERS\n");
	}
	if (command&SHUTDOWN_WAIT_CRITICAL_BUFFERS) {
		stdoutput.write("		"
					"SHUTDOWN_WAIT_CRITICAL_BUFFERS\n");
	}
	if (command&KILL_QUERY) {
		stdoutput.write("		"
					"KILL_QUERY\n");
	}
	if (command&KILL_CONNECTION) {
		stdoutput.write("		"
					"KILL_CONNECTION\n");
	}
}

void sqlrprotocol_mysql::debugStmtExecuteFlags(unsigned char flags) {
	stdoutput.write("	flags:\n");
	if (flags&CURSOR_TYPE_NO_CURSOR) {
		stdoutput.write("		"
					"CURSOR_TYPE_NO_CURSOR\n");
	}
	if (flags&CURSOR_TYPE_READ_ONLY) {
		stdoutput.write("		"
					"CURSOR_TYPE_READ_ONLY\n");
	}
	if (flags&CURSOR_TYPE_FOR_UPDATE) {
		stdoutput.write("		"
					"CURSOR_TYPE_FOR_UPDATE\n");
	}
	if (flags&CURSOR_TYPE_SCROLLABLE) {
		stdoutput.write("		"
					"CURSOR_TYPE_SCROLLABLE\n");
	}
}

void sqlrprotocol_mysql::debugMultiStatementOption(uint16_t multistmtoption) {
	stdoutput.write("	multi statement option:\n");
	if (multistmtoption==MYSQL_OPTION_MULTI_STATEMENTS_ON) {
		stdoutput.write("		"
					"MYSQL_OPTION_MULTI_STATEMENTS_ON\n");
	}
	if (multistmtoption==MYSQL_OPTION_MULTI_STATEMENTS_OFF) {
		stdoutput.write("		"
					"MYSQL_OPTION_MULTI_STATEMENTS_OFF\n");
	}
}

bool sqlrprotocol_mysql::getRequest(char *request) {
	if (!recvPacket()) {
		return false;
	}
	*request=*(reqpacket);
	return true;
}

bool sqlrprotocol_mysql::comSleep() {
	// internal server command
	if (getDebug()) {
		debugStart("com_sleep");
		debugEnd();
	}
	return sendNotImplementedError();
}

bool sqlrprotocol_mysql::comInitDb() {

	// analogous to "use db"

	const char	*rp=(const char *)reqpacket;
	uint64_t	rplen=reqpacketsize;
	char		*schemaname=charstring::duplicate(rp+1,rplen-1);

	if (getDebug()) {
		debugStart("com_init_db");
		stdoutput.printf("	schemaname: \"%s\"\n",schemaname);
		debugEnd();
	}

	bool	retval=true;
	if (!cont->selectDatabase(schemaname)) {
		retval=sendError();
	} else {
		retval=sendOkPacket();
	}
	delete[] schemaname;
	return retval;
}

bool sqlrprotocol_mysql::comStatistics() {
	// returns a text representation of various statistics
	const char	*statistics="Uptime: 0  Threads: 0  Questions: 0  "
					"Slow queries: 0  Opens: 0  "
					"Flush tables: 0  Open tables: 0  "
					"Queries per second avg: 0";
	// FIXME: implement this somehow
	if (getDebug()) {
		debugStart("com_statistics");
		stdoutput.printf("	%s\n",statistics);
		debugEnd();
	}
	resetSendPacketBuffer();
	write(&resppacket,statistics,charstring::length(statistics));
	return sendPacket(true);
}

bool sqlrprotocol_mysql::comConnect() {
	// internal server command
	if (getDebug()) {
		debugStart("com_connect");
		debugEnd();
	}
	return sendNotImplementedError();
}

bool sqlrprotocol_mysql::comDebug() {
	// asks the server to dump internal debug info to stdoutput
	if (getDebug()) {
		debugStart("com_debug");
		debugEnd();
	}
	return sendNotImplementedError();
}

bool sqlrprotocol_mysql::comPing() {
	// pings the server
	if (getDebug()) {
		debugStart("com_ping");
		debugEnd();
	}
	if (!cont->ping()) {
		return sendError();
	}
	return sendOkPacket();
}

bool sqlrprotocol_mysql::comTime() {
	// internal server command
	if (getDebug()) {
		debugStart("com_time");
		debugEnd();
	}
	return sendNotImplementedError();
}

bool sqlrprotocol_mysql::comDelayedInsert() {
	// internal server command
	if (getDebug()) {
		debugStart("com_delayed_insert");
		debugEnd();
	}
	return sendNotImplementedError();
}

bool sqlrprotocol_mysql::comChangeUser() {
	// switches from the current user to the specified user
	if (getDebug()) {
		debugStart("com_change_user");
		stdoutput.printf("	...\n");
		debugEnd();
	}
	// FIXME: implement this...
	return sendNotImplementedError();
}

bool sqlrprotocol_mysql::comBinLogDump() {
	// part of replication protocol
	if (getDebug()) {
		debugStart("com_bin_log_dump");
		stdoutput.printf("	...\n");
		debugEnd();
	}
	return sendNotImplementedError();
}

bool sqlrprotocol_mysql::comTableDump() {
	// part of replication protocol
	if (getDebug()) {
		debugStart("com_table_dump");
		stdoutput.printf("	...\n");
		debugEnd();
	}
	return sendNotImplementedError();
}

bool sqlrprotocol_mysql::comConnectOut() {
	// part of replication protocol
	if (getDebug()) {
		debugStart("com_connect_out");
		stdoutput.printf("	...\n");
		debugEnd();
	}
	return sendNotImplementedError();
}

bool sqlrprotocol_mysql::comRegisterSlave() {
	// part of replication protocol
	if (getDebug()) {
		debugStart("com_register_slave");
		stdoutput.printf("	...\n");
		debugEnd();
	}
	return sendNotImplementedError();
}

bool sqlrprotocol_mysql::comDaemon() {
	// internal server command
	if (getDebug()) {
		debugStart("com_daemon");
		stdoutput.printf("	...\n");
		debugEnd();
	}
	return sendNotImplementedError();
}

bool sqlrprotocol_mysql::comBinlogDumpGtid() {
	// part of replication protocol
	if (getDebug()) {
		debugStart("com_binlog_dump_gtid");
		stdoutput.printf("	...\n");
		debugEnd();
	}
	return sendNotImplementedError();
}

bool sqlrprotocol_mysql::comResetConnection() {

	// resets the state of the current connection
	// without logging out and logging back in

	if (getDebug()) {
		debugStart("com_reset_connection");
		debugEnd();
	}

	// FIXME: SQL Relay doesn't have a good analog for this.
	if (true) {
		return sendNotImplementedError();
	}
	return sendOkPacket();
}

bool sqlrprotocol_mysql::comCreateDb(sqlrservercursor *cursor) {

	// creates a new database

	const char	*rp=(const char *)reqpacket;
	uint64_t	rplen=reqpacketsize;
	char		*schemaname=charstring::duplicate(rp+1,rplen-1);

	if (getDebug()) {
		debugStart("com_create_db");
		stdoutput.printf("	schemaname: \"%s\"\n",schemaname);
		debugEnd();
	}

	stringbuffer	query;
	query.append("create database ")->append(schemaname);

	bool	retval=sendQuery(cursor,
				query.getString(),
				query.getStringLength());

	delete[] schemaname;
	return retval;
}

bool sqlrprotocol_mysql::comDropDb(sqlrservercursor *cursor) {

	// drops an existing database

	const char	*rp=(const char *)reqpacket;
	uint64_t	rplen=reqpacketsize;
	char		*schemaname=charstring::duplicate(rp+1,rplen-1);

	if (getDebug()) {
		debugStart("com_drop_db");
		stdoutput.printf("	schemaname: \"%s\"\n",schemaname);
		debugEnd();
	}

	stringbuffer	query;
	query.append("drop database ")->append(schemaname);

	bool	retval=sendQuery(cursor,
				query.getString(),
				query.getStringLength());

	delete[] schemaname;
	return retval;
}

bool sqlrprotocol_mysql::comQuery(sqlrservercursor *cursor) {

	// prepare and execute the specified query

	// get the query and query size
	const char	*query=(const char *)reqpacket+1;
	uint64_t	querylen=reqpacketsize-1;

	// bounds checking
	if (querylen>maxquerysize) {
		stringbuffer	err;
		err.append("Query loo large (");
		err.append(querylen);
		err.append(">");
		err.append(maxquerysize);
		err.append(")");
		return sendErrPacket(1105,err.getString(),"24000");
	}

	if (getDebug()) {
		debugStart("com_query");
		stdoutput.printf("	query: \"");
		stdoutput.safePrint(query,(uint32_t)querylen);
		stdoutput.printf("\"\n");
		stdoutput.printf("	query length: %d\n",querylen);
		debugEnd();
	}

	return sendQuery(cursor,query,querylen);
}

bool sqlrprotocol_mysql::sendQuery(sqlrservercursor *cursor,
						const char *query) {
	return sendQuery(cursor,query,charstring::length(query));
}

bool sqlrprotocol_mysql::sendQuery(sqlrservercursor *cursor,
						const char *query,
						uint64_t querylen) {
	// FIXME: handle custom cursors
	columntypescached[cont->getId(cursor)]=false;
	clearParams(cursor);
	return (cont->prepareQuery(cursor,query,querylen,true,true,true) &&
			cont->executeQuery(cursor,true,true,true,true))?
			sendQueryResult(cursor,false):
			sendQueryError(cursor);
}

bool sqlrprotocol_mysql::sendQueryResult(sqlrservercursor *cursor,
							bool binary) {

	uint32_t	colcount=cont->colCount(cursor);
	if (colcount) {
		return sendResultSet(cursor,colcount,binary);
	}

	uint64_t	id=0;
	cont->getLastInsertId(&id);
	// NOTE; getLastInsertId can fail, but that usually just means that the
	// db doesn't support it.  So, rather than return an error, we'll just
	// leave id=0.

	// FIXME: for the following queries, info should be set:
	// 	insert into ... select ...
	// 		Records: xxx Duplicates: xxx Warnings: xxx
	// 	insert into ... values (...),(...),(...)...
	// 		Records: xxx Duplicates: xxx Warnings: xxx
	// 	load data infile ...
	// 		Records: xxx Deleted: xxx Skipped: xxx Warnings: xxx
	// 	alter table
	// 		Records: xxx Duplicates: xxx Warnings: xxx
	// 	update
	// 		Rows matched: xxx Changed: xxx Warnings: xxx
	return sendOkPacket(true,cont->affectedRows(cursor),id,0,0,"",0,"");
}

bool sqlrprotocol_mysql::sendResultSet(sqlrservercursor *cursor,
						uint32_t colcount,
						bool binary) {
	cacheColumnDefinitions(cursor,colcount);
	return (sendColumnDefinitions(cursor,colcount,binary) &&
				sendResultSetRows(cursor,colcount,0,binary));
}

void sqlrprotocol_mysql::cacheColumnDefinitions(sqlrservercursor *cursor,
							uint32_t colcount) {

	uint16_t	curid=cont->getId(cursor);

	if (columntypescached[curid]) {
		return;
	}

	if (!cont->getMaxColumnCount()) {
		delete[] columntypes[curid];
		if (colcount) {
			columntypes[curid]=new unsigned char[colcount];
		} else {
			columntypes[curid]=NULL;
		}
	}

	unsigned char	*ct=columntypes[curid];

	for (uint32_t i=0; i<colcount; i++) {
		ct[i]=getColumnType(cont->getColumnTypeName(cursor,i),
					cont->getColumnTypeNameLength(cursor,i),
					cont->getColumnScale(cursor,i));
	}

	columntypescached[curid]=true;
}

bool sqlrprotocol_mysql::sendColumnDefinitions(sqlrservercursor *cursor,
							uint32_t colcount,
							bool binary) {

	// column count
	if (getDebug()) {
		debugStart("column count");
		stdoutput.printf("	count: %d\n",colcount);
		debugEnd();
	}
	resetSendPacketBuffer();
	writeLenEncInt(&resppacket,colcount);
	if (!sendPacket()) {
		return false;
	}

	// column definitions
	for (uint32_t i=0; i<colcount; i++) {
		if (!sendColumnDefinition(cursor,i)) {
			return false;
		}
	}

	// EOF (if not deprecated)
	if (!(servercapabilityflags&CLIENT_DEPRECATE_EOF &&
		clientcapabilityflags&CLIENT_DEPRECATE_EOF)) {
		if (!sendEofPacket(0,0)) {
			return false;
		}
	} else {
		clientsock->flushWriteBuffer(-1,-1);
		if (getDebug()) {
			stdoutput.write("col defs flush...\n");
		}
	}
	return true;
}

bool sqlrprotocol_mysql::sendColumnDefinition(sqlrservercursor *cursor,
							uint32_t column) {
	uint16_t	curid=cont->getId(cursor);

	const char	*columnname=cont->getColumnName(cursor,column);
	uint16_t	sqlrcolumntype=cont->getColumnType(cursor,column);
	const char	*columntypestring=
				cont->getColumnTypeName(cursor,column);
	unsigned char	columntype=columntypes[curid][column];
	uint16_t	columnflags=getColumnFlags(cursor,column,
							sqlrcolumntype,
							columntype,
							columntypestring);
	const char	*columntable=cont->getColumnTable(cursor,column);

	return sendColumnDefinition(cursor,
					column,
					"def",
					// FIXME: get the schema somehow
					"",
					columntable,
					// FIXME: get the orignal
					// table somehow
					"",
					columnname,
					// FIXME: get the original
					// column name somehow
					"",
					cont->getColumnPrecision(cursor,column),
					columntypestring,
					cont->getColumnScale(cursor,column),
					columntype,
					columnflags,
					NULL,
					false);
}

bool sqlrprotocol_mysql::sendColumnDefinition(sqlrservercursor *cursor,
						uint32_t column,
						const char *catalog,
						const char *schema,
						const char *table,
						const char *orgtable,
						const char *colname,
						const char *orgcolname,
						uint32_t length,
						const char *columntypestring,
						uint32_t scale,
						unsigned char columntype,
						uint16_t flags,
						const char *defaults,
						bool fieldlistcommand) {

	// get decimals/scale
	// * 0x00 for integers and static strings
	// * 0x1f for dynamic strings, double, float
	// * 0x00 to 0x51 for decimals
	// (FIXME: what to do for blobs, null set, enum, etc.)
	char		decimals=0;
	if (columntype==MYSQL_TYPE_FLOAT ||
		columntype==MYSQL_TYPE_DOUBLE ||
		columntype==MYSQL_TYPE_VARCHAR ||
		columntype==MYSQL_TYPE_VAR_STRING) {
		decimals=(char)0x1f;
	} else if (columntype==MYSQL_TYPE_DECIMAL ||
		columntype==MYSQL_TYPE_NEWDECIMAL) {
		decimals=(char)scale;
		if (decimals>0x51) {
			decimals=0x51;
		}
	}

	if (getDebug()) {
		stdoutput.printf("column %d {\n",column);
		stdoutput.printf("	catalog: %s\n",catalog);
		stdoutput.printf("	schema: %s\n",schema);
		stdoutput.printf("	table: %s\n",table);
		stdoutput.printf("	org table: %s\n",orgtable);
		stdoutput.printf("	name: %s\n",colname);
		stdoutput.printf("	org name: %s\n",orgcolname);
		debugCharacterSet(servercharacterset);
		stdoutput.printf("	length: %ld\n",length);
		debugColumnType(columntypestring,columntype);
		debugColumnFlags(flags);
		stdoutput.printf("	defaults: %s\n",defaults);
		stdoutput.printf("	decimals: %d (0x%02x)\n",decimals,
					(uint32_t)(0x000000ff&decimals));
		debugEnd();
	}

	resetSendPacketBuffer();

	if (clientcapabilityflags&CLIENT_PROTOCOL_41) {

		// Column Definition 41

		writeLenEncStr(&resppacket,catalog);
		writeLenEncStr(&resppacket,schema);
		writeLenEncStr(&resppacket,table);
		writeLenEncStr(&resppacket,orgtable);
		writeLenEncStr(&resppacket,colname);
		writeLenEncStr(&resppacket,orgcolname);
		write(&resppacket,(char)0x0c);
		// 2 bytes for the server character set here
		// (odd because it's 1 byte everywhere else)
		writeLE(&resppacket,(uint16_t)servercharacterset);
		writeLE(&resppacket,length);
		write(&resppacket,columntype);
		writeLE(&resppacket,flags);
		write(&resppacket,(char)decimals);
		write(&resppacket,(char)0x00);
		write(&resppacket,(char)0x00);

	} else {

		// Column Definition 320

		writeLenEncStr(&resppacket,table);
		writeLenEncStr(&resppacket,colname);
		write(&resppacket,(char)0x03);
		writeTriplet(&resppacket,length);
		writeLenEncInt(&resppacket,1);
		write(&resppacket,columntype);
  		if (clientcapabilityflags&CLIENT_LONG_FLAG) {
			writeLenEncInt(&resppacket,3);
			writeLE(&resppacket,flags);
  		} else {
			writeLenEncInt(&resppacket,2);
			write(&resppacket,(unsigned char)(flags&0x00FF));
		}
		write(&resppacket,(char)decimals);
	}

	if (fieldlistcommand) {
		if (!charstring::isNullOrEmpty(defaults)) {
			uint64_t	len=charstring::length(defaults);
			writeLenEncInt(&resppacket,len);
			write(&resppacket,defaults,len);
		} else {
			// send NULL as 0xfb
			write(&resppacket,(char)0xfb);
		}
  	}

	return sendPacket();
}

unsigned char sqlrprotocol_mysql::getColumnType(const char *columntypestring,
						uint16_t columntypelen,
						uint32_t scale) {

	// sometimes column types have parentheses, like CHAR(40)
	const char	*leftparen=charstring::findFirst(columntypestring,"(");
	if (leftparen) {
		columntypelen=leftparen-columntypestring;
	}

	const char * const 	*datatypestring=cont->dataTypeStrings();

	for (uint32_t index=0; datatypestring[index]; index++) {

		// compare "columntypelen" bytes but also make sure that the
		// byte afterward is a NULL, we don't want "DATE" to match
		// "DATETIME" for example
		if (!charstring::compareIgnoringCase(
					datatypestring[index],
					columntypestring,
					columntypelen) &&
				datatypestring[index][columntypelen]=='\0') {

			unsigned char	retval=mysqltypemap[index];

			// Some DB's, like oracle, don't distinguish between
			// decimal and integer types, they just have a numeric
			// field which may or may not have decimal points.
			// By default, even if they have 0 decimal points,
			// those fields types get translated to "decimal", but
			// we also provide the option of mapping them to
			// MYSQL_TYPE_LONGLONG (AKA BIGINT).
			if ((retval==MYSQL_TYPE_DECIMAL ||
				retval==MYSQL_TYPE_NEWDECIMAL) && !scale &&
				zeroscaledecimaltobigint) {
				retval=MYSQL_TYPE_LONGLONG;
			}

			// Some DB's, like oracle, don't have separate DATE
			// and DATETIME types.  Rather, a DATE can store the
			// date and time, but which components it reports
			// depends on something like the NLS_DATE_FORMAT.  By
			// default, we map DATE to MYSQL_TYPE_DATE, but we also
			// provide the option of mapping it to
			// MYSQL_TYPE_DATETIME.
			if (retval==MYSQL_TYPE_DATE && datetodatetime) {
				retval=MYSQL_TYPE_DATETIME;
			}
			return retval;
		}
	}
	return MYSQL_TYPE_NULL;
}
		
uint16_t sqlrprotocol_mysql::getColumnFlags(sqlrservercursor *cursor,
						uint32_t column,
						uint16_t sqlrcolumntype,
						unsigned char columntype,
						const char *columntypestring) {
	return getColumnFlags(cursor,
				sqlrcolumntype,
				columntype,
				columntypestring,
				cont->getColumnIsNullable(cursor,column),
				cont->getColumnIsPrimaryKey(cursor,column),
				cont->getColumnIsUnique(cursor,column),
				cont->getColumnIsPartOfKey(cursor,column),
				cont->getColumnIsUnsigned(cursor,column),
				cont->getColumnIsZeroFilled(cursor,column),
				cont->getColumnIsBinary(cursor,column),
				cont->getColumnIsAutoIncrement(cursor,column));
}

uint16_t sqlrprotocol_mysql::getColumnFlags(sqlrservercursor *cursor,
						uint16_t sqlrcolumntype,
						unsigned char columntype,
						const char *columntypestring,
						bool isnullable,
						bool isprimarykey,
						bool isunique,
						bool ispartofkey,
						bool isunsigned,
						bool iszerofilled,
						bool isbinary,
						bool isautoincrement) {
	uint16_t	flags=0;
	if (!isnullable) {
		flags|=NOT_NULL_FLAG;
	}
	if (isprimarykey) {
		flags|=PRI_KEY_FLAG;
	}
	if (isunique) {
		flags|=UNIQUE_KEY_FLAG;
	}
	if (ispartofkey) {
		flags|=MULTIPLE_KEY_FLAG;
	}
	if (columntype==MYSQL_TYPE_TINY_BLOB ||
		columntype==MYSQL_TYPE_MEDIUM_BLOB ||
		columntype==MYSQL_TYPE_LONG_BLOB ||
		columntype==MYSQL_TYPE_BLOB) {
		flags|=BLOB_FLAG;
	}
	if (isunsigned || (sqlrcolumntype!=(uint16_t)-1)?
				cont->isUnsignedType(sqlrcolumntype):
				cont->isUnsignedType(columntypestring)) {
		flags|=UNSIGNED_FLAG;
	}
	if (iszerofilled) {
		flags|=ZEROFILL_FLAG;
	}
	if (isbinary || (sqlrcolumntype!=(uint16_t)-1)?
				cont->isBinaryType(sqlrcolumntype):
				cont->isBinaryType(columntypestring)) {
		flags|=BINARY_FLAG;
	}
	if (columntype==MYSQL_TYPE_ENUM) {
		flags|=ENUM_FLAG;
	}
	if (isautoincrement) {
		flags|=AUTO_INCREMENT_FLAG;
	}
	if (columntype==MYSQL_TYPE_TIMESTAMP ||
		columntype==MYSQL_TYPE_TIMESTAMP2) {
		flags|=TIMESTAMP_FLAG|ON_UPDATE_NOW_FLAG;
	}
	if (columntype==MYSQL_TYPE_SET) {
		flags|=SET_FLAG;
	}
	if ((sqlrcolumntype!=(uint16_t)-1)?
			cont->isNumberType(sqlrcolumntype):
			cont->isNumberType(columntypestring)) {
		flags|=NUM_FLAG;
	}
	return flags;
}

bool sqlrprotocol_mysql::sendResultSetRows(sqlrservercursor *cursor,
							uint32_t colcount,
							uint32_t rowcount,
							bool binary) {

	bool	retval=false;

	// for each row...
	uint32_t	rowsfetched=0;
	for (;;) {

		// fetch a row
		bool	error;
		if (!cont->fetchRow(cursor,&error)) {

			if (error) {
				retval=sendQueryError(cursor);
			} else {
				retval=sendEofPacket(0,
					SERVER_STATUS_LAST_ROW_SENT);
			}
			break;
		}

		debugStart("row");

		resetSendPacketBuffer();

		if (!((binary)?buildBinaryRow(cursor,colcount):
				buildTextRow(cursor,colcount))) {
			debugEnd();
			retval=sendQueryError(cursor);
			break;
		}

		// FIXME: kludgy
		cont->nextRow(cursor);

		debugEnd();

		if (!sendPacket()) {
			retval=false;
			break;
		}

		// bump rows fetched
		if (rowcount) {
			rowsfetched++;
			if (rowsfetched==rowcount) {
				retval=(binary)?sendEofPacket(0,0):true;
				break;
			}
		}
	}

	return retval;
}

bool sqlrprotocol_mysql::buildBinaryRow(sqlrservercursor *cursor,
						uint32_t colcount) {

	// get the cursor id
	uint16_t	curid=cont->getId(cursor);

	// packet header
	write(&resppacket,(char)0x00);

	// field pointers
	const char	*field;
	uint64_t	fieldlength;
	bool		blob;
	bool		null;

	// get the column type
	unsigned char	*ct=columntypes[curid];

	// prepare the null bitmap
	uint16_t	nullbitmapsize=(colcount+7+2)/8;
	if (!cont->getMaxColumnCount()) {
		delete[] nullbitmap[curid];
		nullbitmap[curid]=new unsigned char[nullbitmapsize];
	}
	unsigned char	*nb=nullbitmap[curid];
	bytestring::zero(nb,nullbitmapsize);

	// get the fields
	uint32_t	i;
	for (i=0; i<colcount; i++) {

		// get the field
		null=false;
		if (!cont->getField(cursor,i,&field,&fieldlength,&blob,&null)) {
			return false;
		}

		// append to the null bitmap
		nb[(i+2)/8]|=(((unsigned char)null)<<((i+2)%8));
	}

	if (getDebug()) {
		stdoutput.write("	null bitmap {\n");
		stdoutput.write("		");
		stdoutput.printBits(nb,nullbitmapsize);
		stdoutput.write('\n');
		stdoutput.write("	}\n");
	}

	// append the null bitmap
	write(&resppacket,nb,nullbitmapsize);

	// send the fields
	for (i=0; i<colcount; i++) {

		if (getDebug()) {
			stdoutput.printf("	col %d {\n",i);
			debugColumnType(ct[i]);
		}

		// get the field (again)
		fieldlength=0;
		blob=false;
		null=false;
		if (!cont->getField(cursor,i,&field,&fieldlength,&blob,&null)) {
			if (getDebug()) {
				stdoutput.write("	}\n");
			}
			return false;
		}

		// send the field
		if (blob) {
			if (getDebug()) {
				stdoutput.write("		LOB\n");
			}
			buildLobField(cursor,i);
		} else if (!null) {
			if (getDebug()) {
				stdoutput.printf("		\"%s\" (%d)\n",
							field,fieldlength);
			}
			buildBinaryField(field,fieldlength,ct[i]);
		}

		if (getDebug()) {
			stdoutput.write("	}\n");
		}
	}

	return true;
}

void sqlrprotocol_mysql::buildBinaryField(const char *field,
						uint64_t fieldlength,
						unsigned char columntype) {

	switch (columntype) {
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_ENUM:
		case MYSQL_TYPE_SET:
		case MYSQL_TYPE_GEOMETRY:
		case MYSQL_TYPE_BIT:
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
			// Why handle LOBs here?  The database connection module
			// might not correctly identify a LOB field as a LOB.
			// This happens with the mysql connection module when
			// it's using the traditional mysql API, which doesn't
			// handle LOBs differently from other data.  In cases
			// like that, the blob flag passed to getField() will be
			// false and this method will be called instead of
			// buildLobField().  So, this method has to handle LOBs
			// too.
			writeLenEncStr(&resppacket,field,fieldlength);
			break;

		case MYSQL_TYPE_LONGLONG:
			writeLE(&resppacket,
				(uint64_t)charstring::toInteger(field));
			break;

		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_INT24:
			writeLE(&resppacket,
				(uint32_t)charstring::toInteger(field));
			break;

		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_YEAR:
			writeLE(&resppacket,
				(uint16_t)charstring::toInteger(field));
			break;
		
		case MYSQL_TYPE_TINY:
			write(&resppacket,
				(char)charstring::toInteger(field));
			break;
		
		case MYSQL_TYPE_DOUBLE:
			{
			double		fval=charstring::toFloat(field);
			uint64_t	ival;
			bytestring::copy(&ival,&fval,sizeof(double));
			writeLE(&resppacket,ival);
			}
			break;
		
		case MYSQL_TYPE_FLOAT:
			{
			float		fval=charstring::toFloat(field);
			uint32_t	ival;
			bytestring::copy(&ival,&fval,sizeof(float));
			writeLE(&resppacket,ival);
			}
			break;

		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_DATETIME:
		case MYSQL_TYPE_TIMESTAMP:
			{
			int16_t	year;
			int16_t	month;
			int16_t	day;
			int16_t	hour;
			int16_t	minute;
			int16_t	second;
			int32_t	usec;
			bool	isnegative;
			// FIXME: set ddmm and yyyyddmm somehow
			cont->parseDateTime(field,false,false,"/-.:",
					&year,&month,&day,
					&hour,&minute,&second,
					&usec,&isnegative);
			// FIXME: this can be compressed if some parts are 0
			if (columntype==MYSQL_TYPE_DATE) {
				write(&resppacket,(unsigned char)4);
			} else {
				write(&resppacket,(unsigned char)11);
			}
			writeLE(&resppacket,(uint16_t)year);
			write(&resppacket,(unsigned char)month);
			write(&resppacket,(unsigned char)day);
			if (columntype!=MYSQL_TYPE_DATE) {
				write(&resppacket,(unsigned char)hour);
				write(&resppacket,(unsigned char)minute);
				write(&resppacket,(unsigned char)second);
				writeLE(&resppacket,(uint32_t)usec);
			}
			}
			break;

		case MYSQL_TYPE_TIME:
			{
			// examples:
			// 	"-12d 19:27:30.000 001"
			// 	"-12d 19:27:30"
			// 	"12d 19:27:30"
			// 	"0d 00:0:00"
			// or just:
			// 	"01:00:00"

			// get the -
			unsigned char	isneg=0;
			if (field[0]=='-') {
				isneg=1;
				field++;
			}

			// get the days
			uint32_t	days=0;
			const char	*d=charstring::findFirst(field,"d ");
			if (d) {
				days=charstring::toInteger(field);
				field=d+2;
			}

			// get the rest
			int16_t	year;
			int16_t	month;
			int16_t	day;
			int16_t	hour;
			int16_t	minute;
			int16_t	second;
			int32_t	usec;
			bool	isnegative;
			cont->parseDateTime(field,false,false,"/-.:",
					&year,&month,&day,
					&hour,&minute,&second,
					&usec,&isnegative);

			// FIXME: this can be compressed if some parts are 0
			write(&resppacket,(unsigned char)12);
			write(&resppacket,isneg);
			writeLE(&resppacket,days);
			write(&resppacket,(unsigned char)hour);
			write(&resppacket,(unsigned char)minute);
			write(&resppacket,(unsigned char)second);
			writeLE(&resppacket,(uint32_t)usec);
			}
			break;

		default:
			break;
	}
}

bool sqlrprotocol_mysql::buildTextRow(sqlrservercursor *cursor,
						uint32_t colcount) {

	// send the fields
	for (uint32_t i=0; i<colcount; i++) {

		if (getDebug()) {
			stdoutput.printf("	col %d {\n",i);
		}

		// get the field
		const char 	*field=NULL;
		uint64_t	fieldlength=0;
		bool		blob=false;
		bool		null=false;
		if (!cont->getField(cursor,i,&field,&fieldlength,&blob,&null)) {
			if (getDebug()) {
				stdoutput.write("	}\n");
			}
			return false;
		}

		// send the field
		if (null) {
			if (getDebug()) {
				stdoutput.write("		NULL\n");
			}
			write(&resppacket,(char)0xfb);
		} else if (blob) {
			if (getDebug()) {
				stdoutput.write("		LOB\n");
			}
			buildLobField(cursor,i);
		} else {
			if (getDebug()) {
				stdoutput.printf("		\"%s\" (%d)\n",
							field,fieldlength);
			}
			writeLenEncStr(&resppacket,field,fieldlength);
		}

		if (getDebug()) {
			stdoutput.write("	}\n");
		}
	}

	return true;
}

#define MAX_BYTES_PER_CHAR	4

/*void sqlrprotocol_mysql::buildLobField(sqlrservercursor *cursor,
							uint32_t col) {

	// Get lob length.  If this fails, send a NULL field.
	uint64_t	loblength=0;
	if (!cont->getLobFieldLength(cursor,col,&loblength)) {
		// send NULL as 0xfb
		write(&resppacket,(char)0xfb);
		cont->closeLobField(cursor,col);
		return;
	}

	if (getDebug()) {
		stdoutput.printf("		lob length: %lld\n",loblength);
	}

	// for lobs of 0 length
	if (!loblength) {
		writeLenEncInt(&resppacket,0);
		cont->closeLobField(cursor,col);
		return;
	}

	// initialize sizes and status
	uint64_t	charstoread=sizeof(lobbuffer)/MAX_BYTES_PER_CHAR;
	uint64_t	charsread=0;
	uint64_t	offset=0;
	bool		start=true;
	uint64_t	charssent=0;
	size_t		startbyte=resppacket.getPosition();

	for (;;) {

		// read a segment from the lob
		if (!cont->getLobFieldSegment(cursor,col,
					lobbuffer,sizeof(lobbuffer),
					offset,charstoread,&charsread) ||
					!charsread) {

			// if we fail to get a segment or got nothing...
			// if we haven't started sending yet, then send a NULL,
			// otherwise just end normally
			if (start) {
				// send NULL as 0xfb
				write(&resppacket,(char)0xfb);
			}
			cont->closeLobField(cursor,col);

			if (getDebug()) {
				stdoutput.printf("		"
						"chars sent: %lld\n",
						charssent);
				stdoutput.printf("		"
						"bytes sent: %lld\n",
						(uint64_t)
						(resppacket.getPosition()-
						startbyte));
			}
			return;

		} else {

			// if we haven't started sending yet, then do that now
			if (start) {
				writeLenEncInt(
						&resppacket,loblength);
				start=false;
			}

			// send the segment we just got
			write(&resppacket,lobbuffer,charsread);
			charssent+=charsread;

			offset=offset+charstoread;
		}
	}
}*/

void sqlrprotocol_mysql::buildLobField(sqlrservercursor *cursor,
							uint32_t col) {

	// Read the lob into a temp buffer, then append that to resppacket.
	// This isn't especially efficient.  However, the mysql protocol needs
	// us to send the number of bytes that compose the lob, but for clobs,
	// getLobFieldLength() may return the number of characters instead.  If
	// the lob contains multi-byte characters, then there will be more bytes
	// than characters and the client will complain about a malformed
	// packet, at best.  This is the only reliable way to get the actual
	// byte-length of the lob that I can think of.
	//
	// My original alternative solution idea was to write the data to the
	// resppacket, and then back-patch the length.  But, since it's a
	// length-encoded integer, we don't know how much space to leave
	// ourselves.
	//
	// The only other idea that I can think of is to keep a list of
	// resppackets, append data to them until we hit a lob, then start a
	// new packet, append it's length to the end of the previous packet,
	// and so on.  Then sendPacket could send all of the packets.  Maybe
	// we'll do that eventually.

	// temp buffer
	bytebuffer	temp;

	// initialize sizes and status
	uint64_t	charstoread=sizeof(lobbuffer)/MAX_BYTES_PER_CHAR;
	uint64_t	charsread=0;
	uint64_t	offset=0;
	bool		start=true;

	// FIXME: kludgy
	// this is necessary to open the lob, at least with mysql
	uint64_t	loblength=0;
	cont->getLobFieldLength(cursor,col,&loblength);

	for (;;) {

		// read a segment from the lob
		if (!cont->getLobFieldSegment(cursor,col,
					lobbuffer,sizeof(lobbuffer),
					offset,charstoread,&charsread) ||
					!charsread) {

			cont->closeLobField(cursor,col);

			// if we fail to get a segment or got nothing...
			if (start) {
				// if we haven't started sending yet,
				// then send a NULL as 0xfb
				write(&resppacket,(char)0xfb);
				return;
			} else {
				// otherwise just end normally
				writeLenEncInt(&resppacket,
							temp.getSize());
				write(&resppacket,temp.getBuffer(),
							temp.getSize());
				return;
			}

		} else {

			if (start) {
				start=false;
			}

			// append the segment we just got
			temp.append(lobbuffer,charsread);

			offset=offset+charstoread;
		}
	}
}

bool sqlrprotocol_mysql::comFieldList(sqlrservercursor *cursor) {

	// get the column list for the specified table

	const unsigned char	*rp=reqpacket;
	uint64_t		rplen=reqpacketsize;
	rp++;
	rplen--;

	// get the table
	char	*table=charstring::duplicate((const char *)rp);
	rp+=charstring::length(table);
	rplen-=charstring::length(table);

	// get the wildcard
	char	*wild=charstring::duplicate((const char *)rp,rplen);

	// some apps aren't well behaved, trim spaces off of both sides
	charstring::bothTrim(table);
	charstring::bothTrim(wild);

	// translate the table name, if necessary
	const char	*newtable=cont->translateTableName(table);
	if (newtable) {
		delete[] table;
		table=charstring::duplicate(newtable);
	}

	if (getDebug()) {
		debugStart("com_field_list");
		stdoutput.printf("	table: \"%s\"\n",table);
		stdoutput.printf("	wild: \"%s\"\n",wild);
		debugEnd();
	}

	// get the list
	bool	success=true;
	if (cont->getListsByApiCalls()) {
		success=getListByApiCall(cursor,
					MYSQLLISTTYPE_COLUMN_LIST,
					table,wild);
	} else {
		success=getListByQuery(cursor,
					MYSQLLISTTYPE_COLUMN_LIST,
					table,wild);
	}

	// clean up
	delete[] table;
	delete[] wild;

	if (!success) {
		return sendQueryError(cursor);
	}

	// FIXME: there are apparently cases with the real mysql server where
	// the column count is 0xfb and a local infile packet should be sent

	return sendFieldListResponse(cursor);
}

bool sqlrprotocol_mysql::getListByApiCall(sqlrservercursor *cursor,
						mysqllisttype_t listtype,
						const char *table,
						const char *wild) {
	switch (listtype) {
		case MYSQLLISTTYPE_DATABASE_LIST:
			cont->setDatabaseListColumnMap(
					SQLRSERVERLISTFORMAT_MYSQL);
			return cont->getDatabaseList(cursor,wild);
		case MYSQLLISTTYPE_TABLE_LIST:
			cont->setTableListColumnMap(
					SQLRSERVERLISTFORMAT_MYSQL);
			return cont->getTableList(cursor,wild,
							DB_OBJECT_TABLE|
							DB_OBJECT_VIEW|
							DB_OBJECT_ALIAS|
							DB_OBJECT_SYNONYM);
		case MYSQLLISTTYPE_COLUMN_LIST:
			cont->setColumnListColumnMap(
					SQLRSERVERLISTFORMAT_MYSQL);
			return cont->getColumnList(cursor,table,wild);
	}
	return false;
}

bool sqlrprotocol_mysql::getListByQuery(sqlrservercursor *cursor,
					mysqllisttype_t listtype,
					const char *table,
					const char *wild) {

	// build the appropriate query
	const char	*query=NULL;
	uint32_t	querylen=0;
	bool		havewild=charstring::length(wild);
	switch (listtype) {
		case MYSQLLISTTYPE_DATABASE_LIST:
			query=cont->getDatabaseListQuery(havewild);
			break;
		case MYSQLLISTTYPE_TABLE_LIST:
			query=cont->getTableListQuery(havewild,
							DB_OBJECT_TABLE|
							DB_OBJECT_VIEW|
							DB_OBJECT_ALIAS|
							DB_OBJECT_SYNONYM);
			break;
		case MYSQLLISTTYPE_COLUMN_LIST:
			query=cont->getColumnListQuery(table,havewild);
			break;
		default:
			break;
	}

	// FIXME: this can fail
	buildListQuery(cursor,query,wild,table);
	query=cont->getQueryBuffer(cursor);
	querylen=cont->getQueryLength(cursor);

	// prepare and execute the query
	return cont->prepareQuery(cursor,query,querylen,true,true,true) &&
				cont->executeQuery(cursor,true,true,true,true);
}

bool sqlrprotocol_mysql::buildListQuery(sqlrservercursor *cursor,
						const char *query,
						const char *wild,
						const char *table) {

	// If the table was given like catalog.schema.table, then just
	// get the table.
	const char	*realtable=charstring::findLast(table,".");
	if (realtable) {
		realtable++;
	} else {
		realtable=table;
	}

	// clean up buffers to avoid SQL injection
	stringbuffer	wildbuf;
	escapeParameter(&wildbuf,wild);
	stringbuffer	tablebuf;
	escapeParameter(&tablebuf,table);

	// bounds checking
	cont->setQueryLength(cursor,charstring::length(query)+
					wildbuf.getStringLength()+
					tablebuf.getStringLength());
	if (cont->getQueryLength(cursor)>maxquerysize) {
		stringbuffer	err;
		err.append("Query loo large (");
		err.append(cont->getQueryLength(cursor));
		err.append(">");
		err.append(maxquerysize);
		err.append(")");
		return sendErrPacket(1105,err.getString(),"24000");
	}

	// fill the query buffer and update the length
	char	*querybuffer=cont->getQueryBuffer(cursor);
	if (tablebuf.getStringLength()) {
		charstring::printf(querybuffer,maxquerysize+1,
						query,tablebuf.getString(),
						wildbuf.getString());
	} else {
		charstring::printf(querybuffer,maxquerysize+1,
						query,wildbuf.getString());
	}
	cont->setQueryLength(cursor,charstring::length(querybuffer));
	return true;
}

void sqlrprotocol_mysql::escapeParameter(stringbuffer *buffer,
						const char *parameter) {

	if (!parameter) {
		return;
	}

	// escape single quotes
	for (const char *ptr=parameter; *ptr; ptr++) {
		if (*ptr=='\'') {
			buffer->append('\'');
		}
		buffer->append(*ptr);
	}
}

bool sqlrprotocol_mysql::sendFieldListResponse(sqlrservercursor *cursor) {

	// translate the rows into fields...

	// for each row...
	uint32_t	column=0;
	bool		error;
	while (cont->fetchRow(cursor,&error)) {

		// convert to a column definition and send it
		const char	*name=NULL;
		const char	*typestring=NULL;
		const char	*lengthstring=NULL;
		const char	*precstring=NULL;
		const char	*scalestring=NULL;
		const char	*isnullable=NULL;
		const char	*columnkey=NULL;
		const char	*defaultvalue=NULL;
		const char	*extra=NULL;
		uint64_t	fieldlength=0;
		bool		blob=false;
		bool		null=false;
		cont->getField(cursor,0,
				&name,&fieldlength,
				&blob,&null);
		cont->getField(cursor,1,
				&typestring,&fieldlength,
				&blob,&null);
		cont->getField(cursor,2,
				&lengthstring,&fieldlength,
				&blob,&null);
		cont->getField(cursor,3,
				&precstring,&fieldlength,
				&blob,&null);
		cont->getField(cursor,4,
				&scalestring,&fieldlength,
				&blob,&null);
		cont->getField(cursor,5,
				&isnullable,&fieldlength,
				&blob,&null);
		cont->getField(cursor,6,
				&columnkey,&fieldlength,
				&blob,&null);
		cont->getField(cursor,7,
				&defaultvalue,&fieldlength,
				&blob,&null);
		cont->getField(cursor,8,
				&extra,&fieldlength,
				&blob,&null);

		uint32_t	prec=charstring::toInteger(precstring);
		uint32_t	scale=charstring::toInteger(scalestring);
		char		type=getColumnType(typestring,
						charstring::length(typestring),
						scale);
		uint32_t	length=0;
		if (!charstring::isNullOrEmpty(lengthstring)) {
			length=charstring::toInteger(lengthstring);
		} else {
			switch ((unsigned char)type) {
				case MYSQL_TYPE_DECIMAL:
					length=prec+2;
					break;
				case MYSQL_TYPE_TINY:
					length=4;
					break;
				case MYSQL_TYPE_SHORT:
					length=6;
					break;
				case MYSQL_TYPE_LONG:
					length=11;
					break;
				case MYSQL_TYPE_FLOAT:
					length=12;
					break;
				case MYSQL_TYPE_DOUBLE:
					length=22;
					break;
				case MYSQL_TYPE_TIMESTAMP:
					length=19;
					break;
				case MYSQL_TYPE_LONGLONG:
					length=20;
					break;
				case MYSQL_TYPE_INT24:
					length=9;
					break;
				case MYSQL_TYPE_DATE:
					length=10;
					break;
				case MYSQL_TYPE_TIME:
					length=10;
					break;
				case MYSQL_TYPE_DATETIME:
					length=19;
					break;
				case MYSQL_TYPE_YEAR:
					length=4;
					break;
				case MYSQL_TYPE_NEWDATE:
					length=10;
					break;
				case MYSQL_TYPE_BIT:
					length=1;
					break;
				case MYSQL_TYPE_TIMESTAMP2:
					length=19;
					break;
				case MYSQL_TYPE_DATETIME2:
					length=19;
					break;
				case MYSQL_TYPE_TIME2:
					length=10;
					break;
				case MYSQL_TYPE_NEWDECIMAL:
					length=prec+2;
					break;
				case MYSQL_TYPE_ENUM:
				case MYSQL_TYPE_SET:
				case MYSQL_TYPE_GEOMETRY:
					// FIXME: not really sure about these
					length=8;
					break;
				default:
					// fall back to 50
					length=50;
					break;
			}
		}

		// FIXME: this won't work with most db's
		bool	isunsigned=charstring::contains(typestring,"unsigned");

		// FIXME: there are probably other types that are zero-filled
		bool	iszerofilled=(type==MYSQL_TYPE_YEAR);

		uint16_t	flags=getColumnFlags(
					cursor,
					(uint16_t)-1,
					type,
					typestring,
					!charstring::compareIgnoringCase(
							isnullable,"yes",3),
					!charstring::compareIgnoringCase(
							columnkey,"pri",3),
					!charstring::compareIgnoringCase(
							columnkey,"uni",3),
					!charstring::isNullOrEmpty(columnkey),
					isunsigned,
					iszerofilled,
					false,
					charstring::contains(extra,
							"auto_increment"));

		// FIXME: get and send default value
		if (!sendColumnDefinition(cursor,column++,
					"def","","","",name,"",
					length,typestring,scale,
					type,flags,defaultvalue,true)) {
			return false;
		}

		// FIXME: kludgy
		cont->nextRow(cursor);
	}
	if (error) {
		// FIXME: handle error
	}

	// EOF
	return sendEofPacket(0,0);
}

bool sqlrprotocol_mysql::comRefresh(sqlrservercursor *cursor) {

	// refresh the server's view of the specified entity

	unsigned char	command=*(reqpacket+1);

	if (getDebug()) {
		debugStart("com_refresh");
		debugRefreshCommand(command);
		debugEnd();
	}

	const char	*query=NULL;
	if (command&REFRESH_GRANT) {
		query="flush privileges";
	} else if (command&REFRESH_LOG) {
		query="flush logs";
	} else if (command&REFRESH_TABLES) {
		query="flush tables";
	} else if (command&REFRESH_HOSTS) {
		query="flush hosts";
	} else if (command&REFRESH_STATUS) {
		query="flush status";
	} else if (command&REFRESH_THREADS) {
		// FIXME: do something?
		return sendOkPacket();
	} else if (command&REFRESH_SLAVE) {
		query="reset slave";
	} else if (command&REFRESH_MASTER) {
		query="reset master";
	} else {
		return sendMalformedPacketError();
	}
	return sendQuery(cursor,query);
}

bool sqlrprotocol_mysql::comShutdown(sqlrservercursor *cursor) {

	// ask the server to shutdown in the specified manner

	unsigned char	command=*(reqpacket+1);

	if (getDebug()) {
		debugStart("com_shutdown");
		debugShutdownCommand(command);
		debugEnd();
	}

	// Even though many shutdown commands are defined, Currently, mysql
	// only supports a "shutdown" without any arguments, which implements
	// the "shutdown_wait_all_buffers" command.
	return sendQuery(cursor,"shutdown");
}

bool sqlrprotocol_mysql::comProcessInfo(sqlrservercursor *cursor) {
	// returns a list of active server threads
	if (getDebug()) {
		debugStart("com_process_info");
		debugEnd();
	}
	return sendQuery(cursor,"show processlist");
}

bool sqlrprotocol_mysql::comProcessKill(sqlrservercursor *cursor) {

	// asks the server to kill the specified server thread

	const unsigned char	*rp=reqpacket+1;

	uint32_t	connid;
	readLE(rp,&connid,&rp);

	if (getDebug()) {
		debugStart("com_process_kill");
		stdoutput.printf("	connection id: %ld\n",connid);
		debugEnd();
	}

	stringbuffer	query;
	query.append("kill ")->append(connid);
	return sendQuery(cursor,query.getString(),query.getStringLength());
}

bool sqlrprotocol_mysql::comStmtPrepare(sqlrservercursor *cursor) {

	// prepares the specified query

	// reset column type cache flag
	columntypescached[cont->getId(cursor)]=false;

	// get the query and query size
	const char	*query=(const char *)reqpacket+1;
	uint64_t	querylen=reqpacketsize-1;

	// bounds checking
	if (querylen>maxquerysize) {
		stringbuffer	err;
		err.append("Query loo large (");
		err.append(querylen);
		err.append(">");
		err.append(maxquerysize);
		err.append(")");
		return sendErrPacket(1105,err.getString(),"24000");
	}

	// copy it into the cursor's query buffer
	char	*querybuffer=cont->getQueryBuffer(cursor);
	bytestring::copy(querybuffer,query,querylen);
	querybuffer[querylen]='\0';
	cont->setQueryLength(cursor,querylen);

	if (getDebug()) {
		debugStart("com_stmt_prepare");
		stdoutput.printf("	query: \"");
		stdoutput.safePrint(query,(uint32_t)querylen);
		stdoutput.printf("\"\n");
		stdoutput.printf("	query length: %d\n",querylen);
		debugEnd();
	}

	// prepare the query
	if (!cont->prepareQuery(cursor,cont->getQueryBuffer(cursor),
					cont->getQueryLength(cursor),
					true,true,true)) {
		return sendQueryError(cursor);
	}
	return sendStmtPrepareOk(cursor);
}

bool sqlrprotocol_mysql::sendStmtPrepareOk(sqlrservercursor *cursor) {

	uint16_t	ccount=cont->colCount(cursor);
	// FIXME: Some db's know the pcount (eg. mysql).  The connection modules
	// should use the db method to get this, or countBindVariables() if the
	// db doesn't know, and the sqlrservercursor class should expose it.
	uint16_t	pcount=cont->countBindVariables(
					cont->getQueryBuffer(cursor),
					cont->getQueryLength(cursor));
	uint16_t	warningcount=0;

	// bounds checking
	if (pcount>maxbindcount) {
		stringbuffer	err;
		err.append("Too mang binds (");
		err.append(pcount);
		err.append(">");
		err.append(maxbindcount);
		err.append(")");
		return sendErrPacket(1105,err.getString(),"24000");
	}

	// store the number of params
	pcounts[cont->getId(cursor)]=pcount;

	if (getDebug()) {
		debugStart("stmt_prepare_ok");
		stdoutput.printf("	statement id: %d\n",
						(uint32_t)cont->getId(cursor));
		stdoutput.printf("	number of columns: %hd\n",ccount);
		stdoutput.printf("	number of params: %hd\n",pcount);
		stdoutput.printf("	warning count: %hd\n",warningcount);
		debugEnd();
	}

	resetSendPacketBuffer();

	write(&resppacket,(char)0x00);
	writeLE(&resppacket,(uint32_t)cont->getId(cursor));
	writeLE(&resppacket,ccount);
	writeLE(&resppacket,pcount);
	write(&resppacket,(char)0x00);
	writeLE(&resppacket,warningcount);
	if (!sendPacket()) {
		return false;
	}

	bool	flush=true;

	// send param definitions as column definitions
	if (pcount) {
		// FIXME: databases don't expose this info
		for (uint16_t i=0; i<pcount; i++) {
			if (!sendColumnDefinition(cursor,i,
					"def","","","","?","",
					0,"VARCHAR",0,
					MYSQL_TYPE_VAR_STRING,BINARY_FLAG,
					NULL,false)) {
				return false;
			}
		}

		// According to the docs, an EOF packet should be sent after
		// the params.
		//
		// Also, if CLIENT_DEPRECATE_EOF is set then an OK packet
		// should be sent in place of an EOF.
		//
		// However, if CLIENT_DEPRECATE_EOF is set, and an OK or EOF
		// packet is sent, then the client throws a Malformed Packet
		// error, as if it were expecting something else.
		//
		// In that case, if we just don't send an OK or EOF packet,
		// then everything works.
		//
		// Strange.
		if (!(servercapabilityflags&CLIENT_DEPRECATE_EOF &&
			clientcapabilityflags&CLIENT_DEPRECATE_EOF)) {
			flush=false;
			if (!sendEofPacket(warningcount,0)) {
				return false;
			}
		}
	}

	// send column definitions
	if (ccount) {

		cacheColumnDefinitions(cursor,ccount);
		for (uint16_t i=0; i<ccount; i++) {
			if (!sendColumnDefinition(cursor,i)) {
				return false;
			}
		}

		// See EOF note above...
		if (!(servercapabilityflags&CLIENT_DEPRECATE_EOF &&
			clientcapabilityflags&CLIENT_DEPRECATE_EOF)) {
			flush=false;
			if (!sendEofPacket(warningcount,0)) {
				return false;
			}
		}
	}

	// flush, if necessary
	if (flush) {
		clientsock->flushWriteBuffer(-1,-1);
		if (getDebug()) {
			stdoutput.write("stmt prep ok flush...\n");
		}
	}

	return true;
}

bool sqlrprotocol_mysql::comStmtExecute() {

	// executes the previously prepared query

	const unsigned char	*rp=reqpacket;
	rp++;

	// get statement id
	uint32_t	stmtid;
	readLE(rp,&stmtid,&rp);

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(stmtid);
	if (!cursor) {
		return sendCursorNotOpenError();
	}

	// get flags
	unsigned char	flags=*rp;
	rp++;

	// get iteration count
	uint32_t	iterationcount;
	readLE(rp,&iterationcount,&rp);

	if (getDebug()) {
		debugStart("com_stmt_execute");
		stdoutput.printf("	statement id: %d\n",stmtid);
		debugStmtExecuteFlags(flags);
		stdoutput.printf("	iteration count: %d\n",iterationcount);
	}
	
	// get the parameters
	uint16_t	pcount=pcounts[cont->getId(cursor)];

	const unsigned char	*nullbitmap=NULL;
	unsigned char		newparamsbound=0;

	if (pcount) {

		// get null bitmap
		nullbitmap=rp;
		rp+=(pcount+7)/8;

		// get "new-params-bound" flag
		newparamsbound=*rp;
		rp++;

		if (getDebug()) {
			stdoutput.write("	null bitmap {\n");
			stdoutput.write("		");
			stdoutput.printBits(nullbitmap,(pcount+7)/8);
			stdoutput.write('\n');
			stdoutput.write("	}\n");
			stdoutput.printf("	new params bound: %d\n",
							newparamsbound);
		}

		uint16_t	*pt=ptypes[cont->getId(cursor)];

		if (newparamsbound==1) {
			// get parameter types
			for (uint16_t i=0; i<pcount; i++) {
				readLE(rp,&(pt[i]),&rp);
			}
		}

		// bind the parameters
		bindParameters(cursor,pcount,pt,nullbitmap,rp,&rp);
	} else {
		clearParams(cursor);
	}

	debugEnd();

	// execute the query
	if (!cont->executeQuery(cursor,true,true,true,true)) {
		return sendQueryError(cursor);
	}

	// return the query result
	return sendQueryResult(cursor,true);
}

void sqlrprotocol_mysql::bindParameters(sqlrservercursor *cursor,
					uint16_t pcount,
					uint16_t *ptypes,
					const unsigned char *nullbitmap,
					const unsigned char *in,
					const unsigned char **out) {

	if (pcount>maxbindcount) {
		pcount=maxbindcount;
	}

	const unsigned char	*rp=in;

	if (getDebug()) {
		stdoutput.write("	bind {\n");
	}

	memorypool		*bindpool=cont->getBindPool(cursor);
	sqlrserverbindvar	*inbinds=cont->getInputBinds(cursor);

	cont->setInputBindCount(cursor,pcount);
	bindpool->clear();

	for (uint16_t i=0; i<pcount; i++) {

		sqlrserverbindvar	*bv=&(inbinds[i]);

		// the bind variable name should be something like ?1, ?2, etc.
		bv->variable=bindvarnames[i];
		bv->variablesize=bindvarnamesizes[i];

		// handle nulls
		unsigned char	nullbitmapindex=nullbitmap[(i)/8];
		unsigned char	nullbitmapmask=(1<<(i%8));
		if (nullbitmapindex&nullbitmapmask) {
			bv->type=SQLRSERVERBINDVARTYPE_NULL;
			bv->isnull=cont->nullBindValue();
			if (getDebug()) {
				stdoutput.printf("		%d {\n",i);
				stdoutput.printf("			"
						"variable: %s\n",bv->variable);
				stdoutput.write("			");
				stdoutput.write("type: NULL\n");
				stdoutput.write("			"
						"isnull: true\n");
				stdoutput.write("		}\n");
			}
			continue;
		}

		// handle non-nulls
		switch (ptypes[i]) {
			case MYSQL_TYPE_TINY:
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				bv->value.integerval=*((char *)rp);
				bv->isnull=cont->nonNullBindValue();
				rp+=sizeof(char);
				break;
			case MYSQL_TYPE_SHORT:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				uint16_t	val;
				readLE(rp,&val,&rp);
				bv->value.integerval=(int16_t)val;
				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case MYSQL_TYPE_LONG:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				uint32_t	val;
				readLE(rp,&val,&rp);
				bv->value.integerval=(int32_t)val;
				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case MYSQL_TYPE_LONGLONG:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				uint64_t	val;
				readLE(rp,&val,&rp);
				bv->value.integerval=(int64_t)val;
				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case MYSQL_TYPE_FLOAT:
				{
				float	temp;
				bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
				read(rp,&temp,&rp);
				bv->value.doubleval.value=temp;
				bv->value.doubleval.precision=0;
				bv->value.doubleval.scale=0;
				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case MYSQL_TYPE_DOUBLE:
				bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
				read(rp,&bv->value.doubleval.value,&rp);
				bv->value.doubleval.precision=0;
				bv->value.doubleval.scale=0;
				bv->isnull=cont->nonNullBindValue();
				break;
			case MYSQL_TYPE_TIME:
				{
				bv->type=SQLRSERVERBINDVARTYPE_DATE;
				bv->value.dateval.year=-1;
				bv->value.dateval.month=-1;
				bv->value.dateval.day=0;
				bv->value.dateval.hour=0;
				bv->value.dateval.minute=0;
				bv->value.dateval.second=0;
				bv->value.dateval.microsecond=0;
				bv->value.dateval.tz=NULL;
				bv->value.dateval.isnegative=false;
				bv->isnull=cont->nonNullBindValue();
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					(char *)bindpool->allocate(
						bv->value.dateval.buffersize);

				char	len=*((char *)rp);
				rp+=sizeof(char);

				if (len) {
					char	isneg=*((char *)rp);
					bv->value.dateval.isnegative=isneg;
					rp+=sizeof(char);

					int32_t	days;
					bytestring::copy(&days,
							rp,sizeof(int32_t));
					bv->value.dateval.day=
						filedescriptor::
						littleEndianToHost(
							(uint32_t)days);
					rp+=sizeof(int32_t);
					
					bv->value.dateval.hour=
							*((char *)rp);
					rp+=sizeof(char);
					bv->value.dateval.minute=
							*((char *)rp);
					rp+=sizeof(char);
					bv->value.dateval.second=
							*((char *)rp);
					rp+=sizeof(char);
					if (len>8) {
						int32_t	ms;
						bytestring::copy(&ms,
							rp,sizeof(int32_t));
						bv->value.dateval.
							microsecond=
						filedescriptor::
						littleEndianToHost(
							(uint32_t)ms);
						rp+=sizeof(int32_t);
					}
				}
				}
				break;
			case MYSQL_TYPE_DATE:
				{
				bv->type=SQLRSERVERBINDVARTYPE_DATE;
				bv->value.dateval.year=0;
				bv->value.dateval.month=0;
				bv->value.dateval.day=0;
				bv->value.dateval.hour=-1;
				bv->value.dateval.minute=-1;
				bv->value.dateval.second=-1;
				bv->value.dateval.microsecond=-1;
				bv->value.dateval.tz=NULL;
				bv->value.dateval.isnegative=false;
				bv->isnull=cont->nonNullBindValue();
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					(char *)bindpool->allocate(
						bv->value.dateval.buffersize);

				char	len=*((char *)rp);
				rp+=sizeof(char);

				if (len) {
					int16_t	year;
					bytestring::copy(&year,
							rp,sizeof(int16_t));
					// FIXME: convert LE to host
					bv->value.dateval.year=
						filedescriptor::
						littleEndianToHost(
							(uint16_t)year);
					rp+=sizeof(int16_t);
					bv->value.dateval.month=*((char *)rp);
					rp+=sizeof(char);
					bv->value.dateval.day=*((char *)rp);
					rp+=sizeof(char);

					// ignore time parts
					if (len>4) {
						rp+=3*sizeof(char);
						if (len>7) {
							rp+=sizeof(int32_t);
						}
					}
				}
				}
				break;
			case MYSQL_TYPE_DATETIME:
			case MYSQL_TYPE_TIMESTAMP:
				{
				bv->type=SQLRSERVERBINDVARTYPE_DATE;
				bv->value.dateval.year=0;
				bv->value.dateval.month=0;
				bv->value.dateval.day=0;
				bv->value.dateval.hour=0;
				bv->value.dateval.minute=0;
				bv->value.dateval.second=0;
				bv->value.dateval.microsecond=0;
				bv->value.dateval.tz=NULL;
				bv->value.dateval.isnegative=false;
				bv->isnull=cont->nonNullBindValue();
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					(char *)bindpool->allocate(
						bv->value.dateval.buffersize);

				char	len=*((char *)rp);
				rp+=sizeof(char);

				if (len) {
					int16_t	year;
					bytestring::copy(&year,
							rp,sizeof(int16_t));
					// FIXME: convert LE to host
					bv->value.dateval.year=
						filedescriptor::
						littleEndianToHost(
							(uint16_t)year);
					rp+=sizeof(int16_t);
					bv->value.dateval.month=*((char *)rp);
					rp+=sizeof(char);
					bv->value.dateval.day=*((char *)rp);
					rp+=sizeof(char);
					if (len>4) {
						bv->value.dateval.hour=
								*((char *)rp);
						rp+=sizeof(char);
						bv->value.dateval.minute=
								*((char *)rp);
						rp+=sizeof(char);
						bv->value.dateval.second=
								*((char *)rp);
						rp+=sizeof(char);
						if (len>7) {
							int32_t	ms;
							bytestring::copy(&ms,
							rp,sizeof(int32_t));
							// FIXME: convert LE
							// to host
							bv->value.dateval.
								microsecond=
							filedescriptor::
							littleEndianToHost(
								(uint32_t)ms);
							rp+=sizeof(int32_t);
						}
					}
				}
				}
				break;
			case MYSQL_TYPE_TINY_BLOB:
			case MYSQL_TYPE_MEDIUM_BLOB:
			case MYSQL_TYPE_LONG_BLOB:
			case MYSQL_TYPE_BLOB:
				bv->type=SQLRSERVERBINDVARTYPE_BLOB;
				bv->valuesize=readLenEncInt(rp,&rp);
				bv->value.stringval=
					(char *)bindpool->allocate(
							bv->valuesize+1);
				bytestring::copy(bv->value.stringval,
							(const char *)rp,
							bv->valuesize);
				bv->value.stringval[bv->valuesize]='\0';
				bv->isnull=cont->nonNullBindValue();
				rp+=bv->valuesize;
				break;
			case MYSQL_TYPE_STRING:
			case MYSQL_TYPE_VAR_STRING:
			// (for all other types, assume string)
			default:
				bv->type=SQLRSERVERBINDVARTYPE_STRING;
				bv->valuesize=readLenEncInt(rp,&rp);
				bv->value.stringval=
					(char *)bindpool->allocate(
							bv->valuesize+1);
				bytestring::copy(bv->value.stringval,
							(const char *)rp,
							bv->valuesize);
				bv->value.stringval[bv->valuesize]='\0';
				bv->isnull=cont->nonNullBindValue();
				rp+=bv->valuesize;
				break;
		}

		if (getDebug()) {
			stdoutput.printf("		%d {\n",i);
			stdoutput.printf("			"
						"variable: %s\n",bv->variable);
			if (bv->type==SQLRSERVERBINDVARTYPE_STRING) {
				stdoutput.write("			"
						"type: STRING\n");
				stdoutput.printf("			"
						"value: %s\n",
						bv->value.stringval);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_INTEGER) {
				stdoutput.write("			"
						"type: INTEGER\n");
				stdoutput.printf("			"
						"value: %lld\n",
						bv->value.integerval);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_DOUBLE) {
				stdoutput.write("			"
						"type: DOUBLE\n");
				stdoutput.printf("			"
						"value: %f (%d,%d)\n",
						bv->value.doubleval.value,
						bv->value.doubleval.precision,
						bv->value.doubleval.scale);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_DATE) {
				stdoutput.write("			"
						"type: DATE\n");
				stdoutput.printf("			"
						"value: ... coming soon...\n");
			}
			stdoutput.printf("			"
					"value size: %d\n",bv->valuesize);
			stdoutput.write("			"
					"isnull: false\n");
			stdoutput.write("		}\n");
		}
	}

	if (getDebug()) {
		stdoutput.write("	}\n");
	}

	*out=rp;
}

void sqlrprotocol_mysql::clearParams(sqlrservercursor *cursor) {
	cont->getBindPool(cursor)->clear();
	cont->setInputBindCount(cursor,0);
}

bool sqlrprotocol_mysql::comStmtSendLongData() {

	// sends long data to the server

	const unsigned char	*rp=reqpacket;
	uint64_t		rplen=reqpacketsize;
	rp++;
	rplen--;

	// get statement id
	uint32_t	stmtid;
	readLE(rp,&stmtid,&rp);
	rplen-=sizeof(uint32_t);

	// get the parameter id
	uint16_t	paramid;
	readLE(rp,&paramid,&rp);
	rplen-=sizeof(uint16_t);

	// get the data
	const unsigned char	*data=rp;
	uint64_t		datalen=rplen;

	if (getDebug()) {
		debugStart("com_stmt_long_data");
		stdoutput.printf("	statement id: %d\n",stmtid);
		stdoutput.printf("	parameter id: %d\n",paramid);
		stdoutput.printf("	data length: %lld\n",datalen);
		debugHexDump(data,datalen);
		debugEnd();
	}

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(stmtid);
	if (!cursor) {
		// No response is sent to the client.  There's no need to
		// end the session if the wrong cursor is specified.
		return true;
	}

	// FIXME: implement this
	// We could cache this data here in a bytebuffer, and bindParameters
	// could look for it and use it if it exists.  That's probably not the
	// best solution though.  A better solution would be for the connection
	// classes to implement a method that calls the db-specific function
	// for this, and expose it via the controller.

	// No response is sent to the client.  There's no need to
	// end the session just because this doesn't work yet.
	return true;
}

bool sqlrprotocol_mysql::comStmtClose() {

	// deallocates the specified prepared statement

	const unsigned char	*rp=reqpacket;
	rp++;

	// get statement id
	uint32_t	stmtid;
	readLE(rp,&stmtid,&rp);

	if (getDebug()) {
		debugStart("com_stmt_close");
		stdoutput.printf("	statement id: %d\n",stmtid);
		debugEnd();
	}

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(stmtid);
	if (!cursor) {
		return sendCursorNotOpenError();
	}

	clearParams(cursor);
	pcounts[cont->getId(cursor)]=0;

	// release the cursor
	cont->setState(cursor,SQLRCURSORSTATE_AVAILABLE);

	return true;
}

bool sqlrprotocol_mysql::comStmtReset() {

	// resets the data of the specified prepared statement

	const unsigned char	*rp=reqpacket;
	rp++;

	// get statement id
	uint32_t	stmtid;
	readLE(rp,&stmtid,&rp);

	if (getDebug()) {
		debugStart("com_stmt_reset");
		stdoutput.printf("	statement id: %d\n",stmtid);
		debugEnd();
	}

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(stmtid);
	if (!cursor) {
		return sendCursorNotOpenError();
	}

	clearParams(cursor);
	pcounts[cont->getId(cursor)]=0;

	cont->closeResultSet(cursor);
	return sendOkPacket();
}

bool sqlrprotocol_mysql::comSetOption(sqlrservercursor *cursor) {

	// enables/disables multi-statement

	const unsigned char	*rp=reqpacket;
	rp++;

	// get multi-statement option
	uint16_t	multistmtoption;
	readLE(rp,&multistmtoption,&rp);

	if (getDebug()) {
		debugStart("com_set_option");
		debugMultiStatementOption(multistmtoption);
		debugEnd();
	}

	// FIXME: SQL Relay doesn't have a good analog for this.
	if (true) {
		return sendNotImplementedError();
	}
	return sendEofPacket(0,0);
}

bool sqlrprotocol_mysql::comStmtFetch() {

	// fetches the specified number of rows

	const unsigned char	*rp=reqpacket;
	rp++;

	// get statement id
	uint32_t	stmtid;
	readLE(rp,&stmtid,&rp);

	// get num rows
	uint32_t	numrows;
	readLE(rp,&numrows,&rp);

	if (getDebug()) {
		debugStart("com_stmt_fetch");
		stdoutput.printf("	statement id: %d\n",stmtid);
		stdoutput.printf("	number of rows: %d\n",numrows);
		debugEnd();
	}

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(stmtid);
	if (!cursor) {
		return sendCursorNotOpenError();
	}
	return sendResultSetRows(cursor,cont->colCount(cursor),numrows,true);
}

bool sqlrprotocol_mysql::sendError() {

	const char	*errorstring;
	uint32_t	errorlength;
	int64_t		errnum;
	bool		liveconnection;
	cont->errorMessage(&errorstring,
				&errorlength,
				&errnum,
				&liveconnection);

	return sendErrPacket(errnum,errorstring,errorlength,"42000");
}

bool sqlrprotocol_mysql::sendQueryError(sqlrservercursor *cursor) {

	const char	*errorstring;
	uint32_t	errorlength;
	int64_t		errnum;
	bool		liveconnection;
	cont->errorMessage(cursor,
				&errorstring,
				&errorlength,
				&errnum,
				&liveconnection);

	return sendErrPacket(errnum,errorstring,errorlength,"42000");
}

bool sqlrprotocol_mysql::sendNotImplementedError() {
	return sendErrPacket(2054,"This feature is not implemented yet","");
}

bool sqlrprotocol_mysql::sendCursorNotOpenError() {
	return sendErrPacket(1325,"Cursor is not open","24000");
}

bool sqlrprotocol_mysql::sendMalformedPacketError() {
	return sendErrPacket(2027,"Malformed packet","");
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrprotocol	*new_sqlrprotocol_mysql(
						sqlrservercontroller *cont,
						sqlrprotocols *ps,
						domnode *parameters) {
		return new sqlrprotocol_mysql(cont,ps,parameters);
	}
}
