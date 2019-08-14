// Copyright (c) 1999-2019  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/character.h>
#include <rudiments/bytebuffer.h>
#include <rudiments/process.h>
#include <rudiments/randomnumber.h>
#include <rudiments/error.h>

// Define request/response packet types
#define MESSAGE_NULL			0x00
#define MESSAGE_AUTHENTICATION		'R'
#define MESSAGE_PASSWORD		'p'
#define MESSAGE_ERRORRESPONSE		'E'
#define MESSAGE_READYFORQUERY		'Z'
#define MESSAGE_COMMANDCOMPLETE		'C'
#define MESSAGE_QUERY			'Q'
#define MESSAGE_EMPTYQUERYRESPONSE	'I'
#define MESSAGE_ROWDESCRIPTION		'T'
#define MESSAGE_DATAROW			'D'
#define MESSAGE_PARSE			'P'
#define MESSAGE_PARSECOMPLETE		'1'
#define MESSAGE_BIND			'B'
#define MESSAGE_EXECUTE			'E'
#define MESSAGE_SYNC			'S'
#define MESSAGE_DESCRIBE		'D'
#define MESSAGE_CLOSE			'C'
#define MESSAGE_TERMINATE		'X'


#define RESPONSE_SUCCESS		0x00
#define REQUEST_CURSOR_EXECUTE		0x0C
#define RESPONSE_CURSOR_EXECUTE		0x0D
#define REQUEST_CURSOR_FETCH		0x0E
#define RESPONSE_CURSOR_FETCH		0x0F
#define RESPONSE_END_OF_RESULT_SET	0x10

// Define auth types
#define AUTH_NONE		0
#define AUTH_KRB5		2
#define AUTH_CLEARTEXT		3
#define AUTH_MD5		5
#define AUTH_SCM		6
#define AUTH_GSS		7
#define AUTH_GSS_CONT		8
#define AUTH_SSPI		9
#define AUTH_SASL		10
#define AUTH_SASL_CONT		11
#define AUTH_SASL_FINAL		12

// Define (error) field types
#define FIELD_TYPE_SEVERITY		'S'
#define FIELD_TYPE_SEVERITYV		'V'
#define FIELD_TYPE_CODE			'C'
#define FIELD_TYPE_MESSAGE		'M'
#define FIELD_TYPE_DETAIL		'D'
#define FIELD_TYPE_HINT			'H'
#define FIELD_TYPE_POSITION		'P'
#define FIELD_TYPE_INTERNAL_POSITION	'p'
#define FIELD_TYPE_INTERNAL_QUERY	'q'
#define FIELD_TYPE_WHERE		'W'
#define FIELD_TYPE_TABLE		't'
#define FIELD_TYPE_COLUMN		'c'
#define FIELD_TYPE_DATA_TYPE		'd'
#define FIELD_TYPE_CONSTRAINT		'n'
#define FIELD_TYPE_FILE			'F'
#define FIELD_TYPE_LINE			'L'
#define FIELD_TYPE_ROUTINE		'R'

// Define column/bind types
#define SAMPLEDB_TYPE_NULL		0x01
#define SAMPLEDB_TYPE_CHAR		0x02
#define SAMPLEDB_TYPE_VARCHAR		0x03
#define SAMPLEDB_TYPE_INT8		0x04
#define SAMPLEDB_TYPE_INT16		0x05
#define SAMPLEDB_TYPE_INT32		0x06
#define SAMPLEDB_TYPE_INT64		0x07
#define SAMPLEDB_TYPE_FLOAT		0x08
#define SAMPLEDB_TYPE_DOUBLE		0x09
#define SAMPLEDB_TYPE_DECIMAL		0x0A
#define SAMPLEDB_TYPE_DATE		0x0B
#define SAMPLEDB_TYPE_TIME		0x0C
#define SAMPLEDB_TYPE_DATETIME		0x0D
#define SAMPLEDB_TYPE_BLOB		0x0F
#define SAMPLEDB_TYPE_END		0xFF

// Define a sqlrelay-column-type to postgresql-column-type map.
// 
// In sqlrelay/src/common/datatypes.h SQL Relay defines a datatype enum, with
// members for the datatypes supported by all of the databases that SQL Relay
// supports.
//
// Element 0 is "UNKNOWN", element 1 is "CHAR", 2 is "INT", 3 is "SMALLINT",
// etc.
//
// This array maps those elements to the datatypes we just defined above.
// Eg.
// * If SQL Relay sends back a datatype of 0 (UNKNOWN) then we'll map that to
// our SAMPLEDB_TYPE_NULL type.
// * If SQL Relay sends back a datatype of 1 (CHAR) then we'll map that to
// our SAMPLEDB_TYPE_CHAR type.
// * If SQL Relay sends back a datatype of 2 (INT) then we'll map that to
// our SAMPLEDB_TYPE_INT32 type.
// * etc.
static unsigned char	postgresqltypemap[]={
	// "UNKNOWN"
	(unsigned char)SAMPLEDB_TYPE_NULL,
	// addded by freetds
	// "CHAR"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "INT"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "SMALLINT"
	(unsigned char)SAMPLEDB_TYPE_INT16,
	// "TINYINT"
	(unsigned char)SAMPLEDB_TYPE_INT8,
	// "MONEY"
	(unsigned char)SAMPLEDB_TYPE_DECIMAL,
	// "DATETIME"
	(unsigned char)SAMPLEDB_TYPE_DATETIME,
	// "NUMERIC"
	(unsigned char)SAMPLEDB_TYPE_DECIMAL,
	// "DECIMAL"
	(unsigned char)SAMPLEDB_TYPE_DECIMAL,
	// "SMALLDATETIME"
	(unsigned char)SAMPLEDB_TYPE_DATETIME,
	// "SMALLMONEY"
	(unsigned char)SAMPLEDB_TYPE_DECIMAL,
	// "IMAGE"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "BINARY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "BIT"
	(unsigned char)SAMPLEDB_TYPE_INT8,
	// "REAL"
	(unsigned char)SAMPLEDB_TYPE_DECIMAL,
	// "FLOAT"
	(unsigned char)SAMPLEDB_TYPE_FLOAT,
	// "TEXT"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "VARCHAR"
	(unsigned char)SAMPLEDB_TYPE_VARCHAR,
	// "VARBINARY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "LONGCHAR"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "LONGBINARY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "LONG"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "ILLEGAL"
	(unsigned char)SAMPLEDB_TYPE_NULL,
	// "SENSITIVITY"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "BOUNDARY"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "VOID"
	(unsigned char)SAMPLEDB_TYPE_NULL,
	// "USHORT"
	(unsigned char)SAMPLEDB_TYPE_INT16,
	// added by lago
	// "UNDEFINED"
	(unsigned char)SAMPLEDB_TYPE_NULL,
	// "DOUBLE"
	(unsigned char)SAMPLEDB_TYPE_DOUBLE,
	// "DATE"
	(unsigned char)SAMPLEDB_TYPE_DATE,
	// "TIME"
	(unsigned char)SAMPLEDB_TYPE_TIME,
	// "TIMESTAMP"
	(unsigned char)SAMPLEDB_TYPE_DATETIME,
	// added by msql
	// "UINT"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "LASTREAL"
	(unsigned char)SAMPLEDB_TYPE_DECIMAL,
	// added by mysql
	// "STRING"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "VARSTRING"
	(unsigned char)SAMPLEDB_TYPE_VARCHAR,
	// "LONGLONG"
	(unsigned char)SAMPLEDB_TYPE_INT64,
	// "MEDIUMINT"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "YEAR"
	(unsigned char)SAMPLEDB_TYPE_INT16,
	// "NEWDATE"
	(unsigned char)SAMPLEDB_TYPE_DATETIME,
	// "NULL"
	(unsigned char)SAMPLEDB_TYPE_NULL,
	// "ENUM"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "SET"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "TINYBLOB"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "MEDIUMBLOB"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "LONGBLOB"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "BLOB"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// added by oracle
	// "VARCHAR2"
	(unsigned char)SAMPLEDB_TYPE_VARCHAR,
	// "NUMBER"
	(unsigned char)SAMPLEDB_TYPE_DECIMAL,
	// "ROWID"
	(unsigned char)SAMPLEDB_TYPE_INT64,
	// "RAW"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "LONG_RAW"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "MLSLABEL"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "CLOB"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "BFILE"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// added by odbc
	// "BIGINT"
	(unsigned char)SAMPLEDB_TYPE_INT64,
	// "INTEGER"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "LONGVARBINARY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "LONGVARCHAR"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// added by db2
	// "GRAPHIC"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "VARGRAPHIC"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "LONGVARGRAPHIC"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "DBCLOB"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "DATALINK"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "USER_DEFINED_TYPE"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "SHORT_DATATYPE"
	(unsigned char)SAMPLEDB_TYPE_INT16,
	// "TINY_DATATYPE"
	(unsigned char)SAMPLEDB_TYPE_INT8,
	// added by firebird
	// "D_FLOAT"
	(unsigned char)SAMPLEDB_TYPE_DOUBLE,
	// "ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "QUAD"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "INT64"
	(unsigned char)SAMPLEDB_TYPE_INT64,
	// "DOUBLE PRECISION"
	(unsigned char)SAMPLEDB_TYPE_DOUBLE,
	// added by postgresql
	// "BOOL"
	(unsigned char)SAMPLEDB_TYPE_INT8,
	// "BYTEA"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "NAME"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "INT8"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "INT2"
	(unsigned char)SAMPLEDB_TYPE_INT16,
	// "INT2VECTOR"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "INT4"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "REGPROC"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "OID"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "TID"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "XID"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "CID"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "OIDVECTOR"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "SMGR"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "POINT"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "LSEG"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "PATH"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "BOX"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "POLYGON"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "LINE"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "LINE_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "FLOAT4"
	(unsigned char)SAMPLEDB_TYPE_FLOAT,
	// "FLOAT8"
	(unsigned char)SAMPLEDB_TYPE_DOUBLE,
	// "ABSTIME"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "RELTIME"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "TINTERVAL"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "CIRCLE"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "CIRCLE_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "MONEY_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "MACADDR"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "INET"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "CIDR"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "BOOL_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "BYTEA_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "CHAR_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "NAME_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "INT2_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "INT2VECTOR_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "INT4_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "REGPROC_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "TEXT_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "OID_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "TID_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "XID_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "CID_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "OIDVECTOR_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "BPCHAR_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "VARCHAR_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "INT8_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "POINT_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "LSEG_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "PATH_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "BOX_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "FLOAT4_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "FLOAT8_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "ABSTIME_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "RELTIME_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "TINTERVAL_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "POLYGON_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "ACLITEM"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "ACLITEM_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "MACADDR_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "INET_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "CIDR_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "BPCHAR"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "TIMESTAMP_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "DATE_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "TIME_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "TIMESTAMPTZ"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "TIMESTAMPTZ_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "INTERVAL"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "INTERVAL_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "NUMERIC_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "TIMETZ"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "TIMETZ_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "BIT_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "VARBIT"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "VARBIT_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "REFCURSOR"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "REFCURSOR_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "REGPROCEDURE"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "REGOPER"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "REGOPERATOR"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "REGCLASS"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "REGTYPE"
	(unsigned char)SAMPLEDB_TYPE_INT32,
	// "REGPROCEDURE_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "REGOPER_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "REGOPERATOR_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "REGCLASS_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "REGTYPE_ARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "RECORD"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "CSTRING"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "ANY"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "ANYARRAY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "TRIGGER"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "LANGUAGE_HANDLER"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "INTERNAL"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "OPAQUE"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "ANYELEMENT"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "PG_TYPE"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "PG_ATTRIBUTE"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "PG_PROC"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "PG_CLASS"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// none added by sqlite
	// added by sqlserver
	// "UBIGINT"
	(unsigned char)SAMPLEDB_TYPE_INT64,
	// "UNIQUEIDENTIFIER"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// added by informix
	// "SMALLFLOAT"
	(unsigned char)SAMPLEDB_TYPE_FLOAT,
	// "BYTE"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "BOOLEAN"
	(unsigned char)SAMPLEDB_TYPE_INT8,
	// "TINYTEXT"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "MEDIUMTEXT"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "LONGTEXT"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "JSON"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "GEOMETRY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "SDO_GEOMETRY"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "NCHAR"
	(unsigned char)SAMPLEDB_TYPE_CHAR,
	// "NVARCHAR"
	(unsigned char)SAMPLEDB_TYPE_VARCHAR,
	// "NTEXT"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "XML"
	(unsigned char)SAMPLEDB_TYPE_BLOB,
	// "DATETIMEOFFSET"
	(unsigned char)SAMPLEDB_TYPE_DATETIME
};


// Define the main protocol module class.
//
// The SQL Relay server architecture is as follows...
//
// Running sqlr-start runs 3 daemons:
// * sqlr-listener
// * sqlr-connection
// * sqlr-scaler
//
// There is only 1 sqlr-listener and 1 sqlr-scaler, but there are multiple
// sqlr-connection daemons.  Each of these opens and maintains a connection
// to the database.
//
// Clients connect to the sqlr-listener, where they are queued until a
// sqlr-connection is available.  When one is available, the client is handed
// off to the sqlr-connection and talks to it, exclusively, for the duration of
// the client session.  A sqlr-connection never talks to more than 1 client at
// a time.
//
// Each sqlr-connection creates an instance of the protocol module class and
// uses it to talk to the client.
class SQLRSERVER_DLLSPEC sqlrprotocol_postgresql : public sqlrprotocol {
	public:
			sqlrprotocol_postgresql(sqlrservercontroller *cont,
							sqlrprotocols *ps,
							domnode *parameters);
		virtual	~sqlrprotocol_postgresql();

		clientsessionexitstatus_t	clientSession(
							filedescriptor *cs);

	private:
		void	init();
		void	free();

		bool	recvPacket();
		bool	recvPacket(bool gettype);
		bool	sendPacket(unsigned char type);

		bool	initialHandshake();
		bool	recvStartupMessage();
		void	parseOptions(const char *opts);
		bool	sendStartupMessageResponse();
		bool	sendAuthenticationCleartextPassword();
		bool	sendAuthenticationMD5Password();
		bool	recvPasswordMessage();
		bool	authenticate();
		bool	sendAuthenticationOk();
		bool	sendReadyForQuery();

		bool	sendErrorResponse(const char *errorstring);
		bool	sendErrorResponse(const char *errorstring,
						uint16_t errorstringlength);
		bool	sendErrorResponse(const char *severity,
						const char *sqlstate,
						const char *errorstring);
		bool	sendErrorResponse(const char *severity,
						const char *sqlstate,
						const char *errorstring,
						uint16_t errorstringlength);
		bool	sendSuccessResponse();

		bool	query();
		bool	parse();
		bool	emptyQuery(const char *query);
		bool	sendQueryResult(sqlrservercursor *cursor);
		bool	sendResultSet(sqlrservercursor *cursor,
							uint16_t colcount);
		bool	sendRowDescription(sqlrservercursor *cursor,
							uint16_t colcount);
		bool	sendDataRow(sqlrservercursor *cursor,
							uint16_t colcount);
		bool	sendCommandComplete(sqlrservercursor *cursor);
		bool	sendEmptyQueryResponse();

		bool	cursorPrepare();
		bool	cursorExecute();
		bool	readInputBinds(sqlrservercursor *cursor,
					const unsigned char *rp,
					const unsigned char **rpout);
		bool	readOutputBinds(sqlrservercursor *cursor,
					const unsigned char *rp,
					const unsigned char **rpout);
		bool	readInputOutputBinds(sqlrservercursor *cursor,
					const unsigned char *rp,
					const unsigned char **rpout);
		void	writeOutputBinds(sqlrservercursor *cursor);
		void	writeInputOutputBinds(sqlrservercursor *cursor);
		bool	cursorFetch();
		bool	sendEndOfResultSetResponse();
		void	writeField(sqlrservercursor *cursor,
					uint16_t column,
					const char *field,
					uint64_t fieldlength,
					unsigned char columntype,
					bool blob,
					bool null);
		void	writeBlobField(sqlrservercursor *cursor,
						uint16_t column);
		bool	cursorClose();

		bool	sendCursorError(sqlrservercursor *cursor);
		bool	sendNotImplementedError();
		bool	sendOutOfCursorsError();
		bool	sendCursorNotOpenError();
		bool	sendTooManyBindsError();

		void	debugRecvTypeError();
		void	debugColumnType(unsigned char columntype);
		void	debugSystemError();

		void	readString(const unsigned char *rp,
					const unsigned char *rpend,
					stringbuffer *strb,
					const unsigned char **rpout);

		filedescriptor	*clientsock;

		bytebuffer	resppacket;

		uint32_t	reqpacketsize;
		unsigned char	*reqpacket;
		unsigned char	reqtype;

		uint32_t	protocolversion;

		char		*user;
		char		*password;
		char		*database;
		char		*replication;
		dictionary<char *, char *>	options;

		randomnumber	rand;

		uint32_t	maxquerysize;
		uint16_t	maxbindcount;

		char		lobbuffer[32768];

		dictionary<char *, uint16_t>	cursormap;
};


sqlrprotocol_postgresql::sqlrprotocol_postgresql(sqlrservercontroller *cont,
					sqlrprotocols *ps,
					domnode *parameters) :
					sqlrprotocol(cont,ps,parameters) {

	// Constructor!

	// The parameters are as follows:
	//
	// "cont" - The SQL Relay server "controller".  This is the main
	// 		SQL Relay server class.  Each sqlr-connection daemon
	// 		creates one instance of these.  In turn, it creates a
	// 		database connection, multiple database cursors,
	// 		various pluggable modules, etc. and coordinates
	// 		communication between them.  You'll call
	// 		cont->something() to do pretty much everything.
	//
	// "ps" - Not super important, don't worry about what it is or what it
	// 		does.
	//
	// "parameters" - Configurable parameters...
	//
	// This particular protocol module doesn't have any configurable
	// parameters, but it's not unusual for a protocol module to have them.
	//
	// The <listener> tag in the sqlrelay.conf file defines which protocol
	// module to load, and which port/socket for it to listen on.
	//
	// Eg.
	// <instance ...>
	// 	<listeners>
	// 		<listener protocol="postgresql"
	// 				port="2222"
	// 				socket="/tmp/postgresql.socket" .../>
	// 	</listeners>
	// </instance>
	//
	// In this case, the instance listens on inet port 2222 and unix socket
	// /tmp/postgresql.socket and talks to clients using the "postgresql" protocol
	// (this protocol).
	//
	// The "parameters" domnode defined above points to the <listener> tag.
	// The protocol, port, and socket attributes can be accessed by calling:
	//
	// const char	*protocol=parameters->getAttributeValue("protocol");
	// const char	*port=parameters->getAttributeValue("port");
	// const char	*socket=parameters->getAttributeValue("socket");
	//
	// If you want, you can define additional attributes, and access them
	// as well by calling:
	//
	// const char	*myparam1=parameters->getAttributeValue("myparam1");
	// const char	*myparam2=parameters->getAttributeValue("myparam2");
	// const char	*myparam3=parameters->getAttributeValue("myparam3");
	// etc.
	//
	// Arguably, you could even define and access nested XML tags and
	// attributes.
	//
	// Eg.
	// <instance ...>
	// 	<listeners>
	// 		<listener protocol="postgresql"
	// 				port="2222"
	// 				socket="/tmp/postgresql.socket" ...>
	// 			<mytag1 myattr1="..." .../>
	// 			<mytag2 myattr2="..." .../>
	// 		</listener>
	// 	</listeners>
	// </instance>
	//
	// Any they could be accessed like:
	//
	// domnode	*mytag1=parameters->getFirstTagChild("mytag1")
	// const char	*myattr1=mytag1->getAttributeValue("myattr1");
	//
	// domnode	*mytag2=parameters->getFirstTagChild("mytag2")
	// const char	*myattr2=mytag2->getAttributeValue("myattr2");
	//
	// And so on...


	// initialize everything...

	// The clientsock is the inet port or unix socket that the client is
	// talking to the server on.
	clientsock=NULL;

	// Request packet...
	reqpacketsize=0;
	reqpacket=NULL;
	reqtype=0;

	rand.setSeed(randomnumber::getSeed());

	// Get the max query size and max bind count from the controller.
	maxquerysize=cont->getConfig()->getMaxQuerySize();
	maxbindcount=cont->getConfig()->getMaxBindCount();

	init();
}

sqlrprotocol_postgresql::~sqlrprotocol_postgresql() {

	// Destructor!

	free();
	delete[] reqpacket;
}

void sqlrprotocol_postgresql::init() {

	// This will be called at startup, and at the beginning of each
	// client session.  Anything that should be re-initialized before
	// each client session should be initialized here.
	//
	// Anything that should ONLY be initialized at server startup should go
	// in the constructor instead of here.

	user=NULL;
	password=NULL;
	database=NULL;
	replication=NULL;
}

void sqlrprotocol_postgresql::free() {

	// This will be called at shutdown, and at the beginning of each
	// client session, before init().  Anything that should be
	// re-initialized before each client session should be freed here.
	//
	// Anything that should ONLY be freed at server shutdown should go in
	// the destructor, not here.

	delete[] user;
	delete[] password;
	delete[] database;
	delete[] replication;
	options.clearAndArrayDelete();
}


clientsessionexitstatus_t sqlrprotocol_postgresql::clientSession(
						filedescriptor *cs) {

	// This method gets called when a client is handed off to the
	// sqlr-connection and should not exit until the client's session
	// is over.
	//
	// It basically:
	// * initializes the connection
	// * handles client requests
	// * cleans up for client-disconnect


	// Get the file descriptor (inet/unix socket) that the client is
	// communicating over.
	clientsock=cs;

	// Set up the socket...
	clientsock->translateByteOrder();
	clientsock->dontUseNaglesAlgorithm();
	clientsock->setSocketReadBufferSize(65536);
	clientsock->setSocketWriteBufferSize(65536);
	clientsock->setReadBufferSize(65536);
	clientsock->setWriteBufferSize(65536);

	// Reinit session-local data...
	free();
	init();

	// state/status variables...
	bool				endsession=true;
	clientsessionexitstatus_t	status=CLIENTSESSIONEXITSTATUS_ERROR;

	// perform the initial handshake...
	if (initialHandshake()) {

		// loop, getting and executing requests
		bool	loop=true;
		do {

			// get the request...
			if (!sendReadyForQuery() || !recvPacket()) {
				status=
				CLIENTSESSIONEXITSTATUS_CLOSED_CONNECTION;
				break;
			}

			// execute the request
			switch (reqtype) {
				case MESSAGE_TERMINATE:
					// just end the session and
					// close the connection
					loop=false;
					endsession=true;
					status=
					CLIENTSESSIONEXITSTATUS_ENDED_SESSION;
					break;
				case MESSAGE_QUERY:
					loop=query();
					break;
				case MESSAGE_PARSE:
					loop=parse();
					break;
				case REQUEST_CURSOR_EXECUTE:
					loop=cursorExecute();
					break;
				case REQUEST_CURSOR_FETCH:
					loop=cursorFetch();
					break;
				case MESSAGE_CLOSE:
					loop=cursorClose();
					break;
				default:
					loop=sendNotImplementedError();
					break;
			}

		} while (loop);
	}

	// close the client connection
	cont->closeClientConnection(0);

	// end the session if necessary
	if (endsession) {
		cont->endSession();
	}

	// return the status
	return status;
}

bool sqlrprotocol_postgresql::recvPacket() {
	return recvPacket(true);
}

bool sqlrprotocol_postgresql::recvPacket(bool gettype) {

	// Read a request packet from the client...

	// request packet structure:
	//
	// data {
	// 	unsigned char	type
	//	uint32_t	size (of data, including itself)
	// 	unsigned char[]	data
	// }

	// packet header
	if (gettype) {
		if (clientsock->read(&reqtype)!=sizeof(unsigned char)) {
			if (getDebug()) {
				stdoutput.write("read packet type failed\n");
				debugSystemError();
			}
			return false;
		}
	} else {
		reqtype=MESSAGE_NULL;
	}
	if (clientsock->read(&reqpacketsize)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("read packet data size failed\n");
			debugSystemError();
		}
		return false;
	}

	// reqpacketsize includes itself
	reqpacketsize-=sizeof(uint32_t);

	// packet
	delete[] reqpacket;
	reqpacket=new unsigned char[reqpacketsize];
	if (clientsock->read(reqpacket,reqpacketsize)!=(ssize_t)reqpacketsize) {
		if (getDebug()) {
			stdoutput.write("read packet data failed\n");
			debugSystemError();
		}
		return false;
	}

	// debug
	debugStart("recv");
	if (getDebug()) {
		stdoutput.printf("	type: %c\n",reqtype);
		stdoutput.printf("	size: %d\n",reqpacketsize);
		debugHexDump(reqpacket,reqpacketsize);
	}
	debugEnd();

	return true;
}

bool sqlrprotocol_postgresql::sendPacket(unsigned char type) {

	// Read a response packet to the client...

	// response packet structure:
	// 
	// data {
	// 	unsigned char	type
	//	uint32_t	size (of data, including itself)
	// 	unsigned char[]	data
	// }

	// debug
	debugStart("send");
	if (getDebug()) {
		if (type!=MESSAGE_NULL) {
			stdoutput.printf("	type: %c\n",type);
		} else {
			stdoutput.printf("	type: (null)\n");
		}
		stdoutput.printf("	size: %d\n",resppacket.getSize());
		debugHexDump(resppacket.getBuffer(),resppacket.getSize());
	}
	debugEnd();

	// packet header
	if (clientsock->write(type)!=sizeof(unsigned char)) {
		if (getDebug()) {
			stdoutput.write("write packet type failed\n");
			debugSystemError();
		}
		return false;
	}
	if (clientsock->write((uint32_t)(resppacket.getSize()+
						sizeof(uint32_t)))!=
						sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("write packet size failed\n");
			debugSystemError();
		}
		return false;
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

	clientsock->flushWriteBuffer(-1,-1);

	return true;
}

bool sqlrprotocol_postgresql::initialHandshake() {

	// Perform the initial client-server handshake...
	return recvStartupMessage() &&
		sendStartupMessageResponse() &&
		recvPasswordMessage() &&
		authenticate();
		// FIXME: send BackendKeyData and ParameterStatus here...
}

bool sqlrprotocol_postgresql::recvStartupMessage() {

	// request packet data structure:
	//
	// data {
	// 	int32_t		protocol version
	//
	// 	// options...
	// 	char[]		string
	// 	char[]		value
	// 	char[]		string
	// 	char[]		value
	// 	...
	// }

	const unsigned char	*rp=NULL;
	const unsigned char	*rpend=NULL;

	bool	first=true;
	for (;;) {

		// receive request packet
		if (!recvPacket(false)) {
			return false;
		}

		// oddly, the startup message doesn't send a type...

		// parse request packet
		rp=reqpacket;
		rpend=reqpacket+reqpacketsize;

		// protocol version
		readBE(rp,&protocolversion,&rp);

		// if the client requested SSL, then deny it
		if (protocolversion==80877103) {

			// close the connection if this is the second time
			// we've gotten an ssl request in the same session
			if (!first) {
				return false;
			}

			debugStart("StartupMessage");
			if (getDebug()) {
				stdoutput.printf("	"
						"protocol version: %d\n",
						protocolversion);
			}
			debugEnd();

			debugStart("N");
			debugEnd();

			// return a single byte 'N'
			if (clientsock->write('N')!=sizeof(char)) {
				if (getDebug()) {
					stdoutput.write("write SSL N failed\n");
					debugSystemError();
				}
				return false;
			}
			clientsock->flushWriteBuffer(-1,-1);

		} else if (protocolversion!=196608) {

			// FIXME: return error of some kind...
			// FIXME: support various protocols
			return false;

		} else {
			break;
		}

		first=false;
	}

	// options
	stringbuffer		name;
	stringbuffer		value;
	while (rp<rpend) {
		readString(rp,rpend,&name,&rp);
		readString(rp,rpend,&value,&rp);
		if (!charstring::compare(
				name.getString(),"user")) {
			user=value.detachString();
		} else if (!charstring::compare(
				name.getString(),"database")) {
			database=value.detachString();
		} else if (!charstring::compare(
				name.getString(),"options")) {
			parseOptions(value.getString());
		} else if (!charstring::compare(
				name.getString(),"replication")) {
			replication=value.detachString();
		} else if (name.getSize()) {
			options.setValue(name.detachString(),
					value.detachString());
		}
		name.clear();
	}

	// NOTE: only user is required, others may be left null
	
	// debug
	debugStart("StartupMessage");
	if (getDebug()) {
		stdoutput.printf("	protocol version: %d\n",
							protocolversion);
		stdoutput.printf("	user: %s\n",user);
		stdoutput.printf("	database: %s\n",database);
		stdoutput.printf("	replication: %s\n",replication);
		linkedlist<char *>	*keys=options.getKeys();
		for (linkedlistnode<char *> *key=keys->getFirst();
						key; key=key->getNext()) {
			stdoutput.printf("	%s: %s\n",
					key->getValue(),
					options.getValue(key->getValue()));
		}
	}
	debugEnd();

	return true;
}

void sqlrprotocol_postgresql::parseOptions(const char *opts) {

	// skip leading whitespace
	while (character::isWhitespace(*opts)) {
		opts++;
	}

	// parse options of the form:
	// some\ name=some\ value some\ name=some\ value...
	stringbuffer	name;
	stringbuffer	value;
	stringbuffer	*strb=&name;
	for (const char *ch=opts; ch; ch++) {
		if (*ch==' ') {
			options.setValue(name.detachString(),
						value.detachString());
			strb=&name;
			name.clear();
			value.clear();
		} else if (*ch=='\\') {
			ch++;
			if (*ch) {
				strb->append(*ch);
				ch++;
			} else {
				break;
			}
		} else if (*ch=='=') {
			strb=(strb==&name)?&value:&name;
		} else {
			strb->append(*ch);
		}
	}
	if (name.getSize()) {
		options.setValue(name.detachString(),value.detachString());
	}
}

bool sqlrprotocol_postgresql::sendStartupMessageResponse() {

	// The user is required to have been sent in the StartupMessage.
	// Fail if it wasn't.
	if (!user) {
		// FIXME: return error of some kind...
		return false;
	}

	// FIXME: support either of these...
	//return sendAuthenticationMD5Password();
	return sendAuthenticationCleartextPassword();
}

bool sqlrprotocol_postgresql::sendAuthenticationCleartextPassword() {

	// respond, requesting a cleartext password

	// response packet data structure:
	//
	// data {
	//	uint32_t	passwordtype
	// }

	// set values to send
	uint32_t	authtype=AUTH_CLEARTEXT;

	// debug
	debugStart("AuthenticationCleartextPassword");
	if (getDebug()) {
		stdoutput.printf("	auth type: %d\n",authtype);
	}
	debugEnd();

	// build response packet
	resppacket.clear();
	writeBE(&resppacket,authtype);

	// send response packet
	return sendPacket(MESSAGE_AUTHENTICATION);
}

bool sqlrprotocol_postgresql::sendAuthenticationMD5Password() {

	// respond, requesting an MD5 password

	// response packet data structure:
	//
	// data {
	//	uint32_t	passwordtype
	//	int32_t		salt
	// }

	// set values to send
	uint32_t	authtype=AUTH_MD5;
	uint32_t	salt;
	rand.generateNumber(&salt);
	int32_t		signedsalt;
	bytestring::copy(&signedsalt,&salt,sizeof(int32_t));

	// debug
	debugStart("AuthenticationMD5Password");
	if (getDebug()) {
		stdoutput.printf("	auth type: %d\n",authtype);
		stdoutput.printf("	salt: %d\n",salt);
	}
	debugEnd();

	// build response packet
	resppacket.clear();
	writeBE(&resppacket,authtype);
	writeBE(&resppacket,salt);

	// send response packet
	return sendPacket(MESSAGE_AUTHENTICATION);
}

bool sqlrprotocol_postgresql::recvPasswordMessage() {

	// request packet data structure:
	//
	// data {
	// 	char[]		password
	// }

	// receive request packet
	if (!recvPacket()) {
		return false;
	}
	if (reqtype!=MESSAGE_PASSWORD) {
		debugRecvTypeError();
		return false;
	}

	// parse request packet
	const unsigned char	*rp=reqpacket;

	password=new char[reqpacketsize+1];
	read(rp,password,reqpacketsize,&rp);
	password[reqpacketsize]='\0';

	// debug
	debugStart("PasswordMessage");
	if (getDebug()) {
		stdoutput.printf("	password: %s\n",password);
	}
	debugEnd();

	return true;
}

bool sqlrprotocol_postgresql::authenticate() {

	// build auth credentials
	sqlruserpasswordcredentials	cred;
	cred.setUser(user);
	cred.setPassword(password);

	// authenticate
	bool	retval=cont->auth(&cred);

	// debug
	debugStart("authenticate");
	if (getDebug()) {
		stdoutput.printf("	auth %s\n",(retval)?"success":"failed");
	}
	debugEnd();

	// error
	if (!retval) {
		stringbuffer	err;
		err.append("password authentication failed for user \"");
		err.append(user);
		err.append("\"");
		sendErrorResponse("FATAL","28P01",
					err.getString(),err.getStringLength());
		return false;
	}

	// success
	return sendAuthenticationOk();
}

bool sqlrprotocol_postgresql::sendAuthenticationOk() {
	
	// respond, indicating that authentication succeeded

	// response packet data structure:
	//
	// data {
	//	int32_t		success (0)
	// }

	// set values to send
	uint32_t	success=0;

	// debug
	debugStart("AuthenticationOk");
	if (getDebug()) {
		stdoutput.printf("	success: %d\n",success);
	}
	debugEnd();

	// build response packet
	resppacket.clear();
	writeBE(&resppacket,success);

	// send response packet
	return sendPacket(MESSAGE_AUTHENTICATION);
}

bool sqlrprotocol_postgresql::sendReadyForQuery() {
	
	// respond, indicating that we're ready for a query

	// response packet data structure:
	//
	// data {
	//	char	txblockstatus
	// }

	// set values to send
	char	txblockstatus=(cont->inTransaction())?'T':'I';

	// debug
	debugStart("ReadyForQuery");
	if (getDebug()) {
		stdoutput.printf("	tx block status: %c\n",txblockstatus);
	}
	debugEnd();

	// build response packet
	resppacket.clear();
	write(&resppacket,txblockstatus);

	// send response packet
	return sendPacket(MESSAGE_READYFORQUERY);
}

bool sqlrprotocol_postgresql::sendErrorResponse(const char *errorstring) {
	return sendErrorResponse("ERROR","",
				errorstring,
				charstring::length(errorstring));
}

bool sqlrprotocol_postgresql::sendErrorResponse(const char *errorstring,
						uint16_t errorstringlength) {
	return sendErrorResponse("ERROR","",errorstring,errorstringlength);
}

bool sqlrprotocol_postgresql::sendErrorResponse(const char *severity,
						const char *sqlstate,
						const char *errorstring) {
	return sendErrorResponse(severity,sqlstate,
				errorstring,
				charstring::length(errorstring));
}

bool sqlrprotocol_postgresql::sendErrorResponse(const char *severity,
						const char *sqlstate,
						const char *errorstring,
						uint16_t errorstringlength) {

	// Respond to the client that an error occurred, with an error code
	// and error string.

	// response packet data structure:
	//
	// data {
	//
	// 	// fields...
	// 	unsigned char	field type
	// 	char[]		error string
	// 	unsigned char	field type
	// 	char[]		error string
	// 	...
	// }

	// if we didn't get a sqlstate then set it to "syntax error"
	// https://www.postgresql.org/docs/current/errcodes-appendix.html
	if (charstring::isNullOrEmpty(sqlstate)) {
		sqlstate="42601";
	}

	// debug
	debugStart("error");
	if (getDebug()) {
		stdoutput.printf("	field type: S\n");
		stdoutput.printf("	string: %s\n",severity);
		stdoutput.printf("	field type: C\n");
		stdoutput.printf("	string: %s\n",sqlstate);
		stdoutput.printf("	field type: M\n");
		stdoutput.printf("	string: %.*s\n",errorstringlength,
							errorstring);
		stdoutput.printf("	field type: (null)\n");
	}
	debugEnd();

	// build response packet
	resppacket.clear();

// FIXME: somehow the real server sends:
// psql: FATAL:  password authentication failed for user "testuser"
// when a login fails.  It's not clear what field the "psql" is sent in.

	write(&resppacket,(unsigned char)FIELD_TYPE_SEVERITY);
	write(&resppacket,severity);
	write(&resppacket,(unsigned char)'\0');

	write(&resppacket,(unsigned char)FIELD_TYPE_CODE);
	write(&resppacket,sqlstate);
	write(&resppacket,(unsigned char)'\0');

	write(&resppacket,(unsigned char)FIELD_TYPE_MESSAGE);
	write(&resppacket,errorstring,errorstringlength);
	write(&resppacket,(unsigned char)'\0');

	write(&resppacket,(unsigned char)'\0');

	// send response packet
	return sendPacket(MESSAGE_ERRORRESPONSE);
}

bool sqlrprotocol_postgresql::sendSuccessResponse() {

	// Respond to the client that the request succeeded.

	// response packet data structure:
	//
	// data {
	// 	(nothing)
	// }

	// debug
	debugStart("success");
	debugEnd();

	// build response packet
	resppacket.clear();

	// send response packet
	return sendPacket(RESPONSE_SUCCESS);
}

bool sqlrprotocol_postgresql::query() {

	// get an available cursor
	sqlrservercursor	*cursor=cont->getCursor();
	if (!cursor) {
		return sendErrorResponse("Out of cursors");
	}

	// request packet data structure:
	//
	// data {
	// 	char[]	query(ies)
	// }

	// parse request packet
	const unsigned char	*rp=reqpacket;

	const char	*query=(const char *)rp;
	uint32_t	querylength=reqpacketsize;

	// debug
	debugStart("query");
	if (getDebug()) {
		stdoutput.printf("	cursor id: %d\n",cursor->getId());
		stdoutput.printf("	query length: %d\n",querylength);
		stdoutput.printf("	query: %.*s\n",querylength,query);
	}
	debugEnd();

	// FIXME: There could be multiple queries.  If so, and if not in a
	// transaction, then we need to start one and commit/rollback it later.
	bool	result=false;
	if (emptyQuery(query)) {
		result=sendEmptyQueryResponse();
	} else {
		if (cont->prepareQuery(cursor,query,querylength,
						true,true,true) &&
			cont->executeQuery(cursor,true,true,true,true)) {
			result=sendQueryResult(cursor);
		} else {
			result=sendCursorError(cursor);
		}
	}

	// release the cursor
	cont->setState(cursor,SQLRCURSORSTATE_AVAILABLE);

	return result;
}

bool sqlrprotocol_postgresql::emptyQuery(const char *query) {
	return !(cont->skipWhitespaceAndComments(query)[0]);
}

bool sqlrprotocol_postgresql::sendQueryResult(sqlrservercursor *cursor) {
	uint16_t	colcount=cont->colCount(cursor);
	return (colcount)?sendResultSet(cursor,colcount):
				sendCommandComplete(cursor);
}

bool sqlrprotocol_postgresql::sendResultSet(sqlrservercursor *cursor,
							uint16_t colcount) {

	if (!sendRowDescription(cursor,colcount)) {
		return false;
	}

	for (;;) {

		bool	error;
		if (!cont->fetchRow(cursor,&error)) {
			if (error) {
				return sendCursorError(cursor);
			} else {
				break;
			}
		}

		if (!sendDataRow(cursor,colcount)) {
			return false;
		}

		// FIXME: kludgy
		cont->nextRow(cursor);
	}

	return sendCommandComplete(cursor);
}

bool sqlrprotocol_postgresql::sendRowDescription(sqlrservercursor *cursor,
							uint16_t colcount) {

	debugStart("RowDecription");

	// build response packet
	resppacket.clear();
	writeBE(&resppacket,colcount);
	for (uint16_t i=0; i<colcount; i++) {

		// field name
		resppacket.append(cont->getColumnName(cursor,i));
		resppacket.append('\0');

		// FIXME: table oid (or 0 if not known)
		resppacket.append((int32_t)0);

		// FIXME: column "attribute number" (index?) (or 0 if not known)
		resppacket.append((int16_t)0);

		// FIXME: data type oid
		resppacket.append((int32_t)0);

		// FIXME: data type size
		resppacket.append((int16_t)0);

		// FIXME: type modifier
		resppacket.append((int32_t)0);

		// FIXME: format code text=0, binary=1
		resppacket.append((int16_t)0);

		
		if (getDebug()) {
			stdoutput.printf("	column %d {\n",i);
			stdoutput.printf("		name: %s\n",
						cont->getColumnName(cursor,i));
			stdoutput.printf("		table oid: 0\n");
			stdoutput.printf("		attribute number: 0\n");
			stdoutput.printf("		data type oid: 0\n");
			stdoutput.printf("		data type size: 0\n");
			stdoutput.printf("		type modifier: 0\n");
			stdoutput.printf("		format code: 0\n");
			debugEnd(1);
		}
	}

	debugEnd();

	// send response packet
	return sendPacket(MESSAGE_ROWDESCRIPTION);
}

bool sqlrprotocol_postgresql::sendDataRow(sqlrservercursor *cursor,
							uint16_t colcount) {

	debugStart("DataRow");

	// build response packet
	resppacket.clear();
	writeBE(&resppacket,colcount);

	const char	*field;
	uint64_t	fieldlength;
	bool		blob;
	bool		null;
	for (uint16_t i=0; i<colcount; i++) {

		if (!cont->getField(cursor,i,&field,&fieldlength,&blob,&null)) {
			return false;
		}

		if (null) {
			int32_t		negone=-1;
			uint32_t	unegone=0;
			bytestring::copy(&unegone,&negone,sizeof(int32_t));
			writeBE(&resppacket,unegone);
		} else {
			writeBE(&resppacket,(uint32_t)fieldlength);
			write(&resppacket,field,fieldlength);
		}

		if (getDebug()) {
			stdoutput.printf("	column %d {\n",i);
			if (null) {
				stdoutput.printf("		(null)\n");
			} else {
				stdoutput.printf("		%d: %.*s\n",
						fieldlength,fieldlength,field);
			}
			debugEnd(1);
		}
	}

	debugEnd();

	// send response packet
	return sendPacket(MESSAGE_DATAROW);
}

bool sqlrprotocol_postgresql::sendCommandComplete(sqlrservercursor *cursor) {
	
	// response packet data structure:
	//
	// data {
	//	char	commandtag
	// }

	// set values to send

	// For most commands, we just return the command itself, uppercased.
	// If it has "table" after it (eg. DROP TABLE) then we include "table"
	// as well.
	stringbuffer	commandtag;
	const char	*query=cont->getQueryBuffer(cursor);
	const char	*q=cont->skipWhitespaceAndComments(query);
	const char	*end=charstring::findFirst(q,' ');
	if (!charstring::compareIgnoringCase(end+1,"table",5)) {
		end=end+6;
	}
	char	*newq=charstring::duplicate(q,end-q);
	charstring::upper(newq);
	commandtag.append(newq);

	// for some commands we append row counts and other stuff
	int64_t	affectedrows=(cont->knowsAffectedRows(cursor))?
						cont->affectedRows(cursor):0;
	if (!charstring::compare(newq,"SELECT")) {
		commandtag.append(' ');
		commandtag.append(cont->rowCount(cursor));
	} else if (!charstring::compare(newq,"INSERT")) {
		commandtag.append(' ');
		commandtag.append(0);
		commandtag.append(' ');
		commandtag.append(affectedrows);
	} else if (!charstring::compare(newq,"UPDATE") ||
			!charstring::compare(newq,"DELETE")) {
		commandtag.append(' ');
		commandtag.append(affectedrows);
	} else if (!charstring::compare(newq,"MOVE")) {
		commandtag.append(' ');
		// FIXME: number of rows skipped
		commandtag.append(0);
	} else if (!charstring::compare(newq,"FETCH")) {
		commandtag.append(' ');
		// FIXME: number of rows retrieved
		commandtag.append(0);
	} else if (!charstring::compare(newq,"COPY")) {
		commandtag.append(' ');
		// FIXME: number of rows copied
		commandtag.append(0);
	}
	delete[] newq;

	// debug
	debugStart("CommandComplete");
	if (getDebug()) {
		stdoutput.printf("	commandtag: %s\n",
					commandtag.getString());
	}
	debugEnd();

	// build response packet
	resppacket.clear();
	write(&resppacket,commandtag.getString(),commandtag.getSize());
	write(&resppacket,'\0');

	// send response packet
	return sendPacket(MESSAGE_COMMANDCOMPLETE);
}

bool sqlrprotocol_postgresql::sendEmptyQueryResponse() {
	
	// response packet data structure:
	//
	// data {
	// }

	// debug
	debugStart("EmptyQueryResponse");
	debugEnd();

	// build response packet
	resppacket.clear();

	// send response packet
	return sendPacket(MESSAGE_EMPTYQUERYRESPONSE);
}

bool sqlrprotocol_postgresql::parse() {

	// get an available cursor
	sqlrservercursor	*cursor=cont->getCursor();
	if (!cursor) {
		return sendErrorResponse("Out of cursors");
	}

	// request packet data structure:
	//
	// data {
	// 	char[]		cursor name
	// 	char[]		query
	// 	uint16_t	param count
	// 	
	// 	// param types...
	// 	uint32_t	type
	// 	uint32_t	type
	// 	...
	// }

	// parse request packet
	const unsigned char	*rp=reqpacket;
	const unsigned char	*rpend=rp+reqpacketsize;

	// get cursor name
	const char	*cursorname=(const char *)rp;
	while (*rp && rp!=rpend) {
		rp++;
	}
	if (rp==rpend) {
		return sendErrorResponse("Invalid request");
	}
	rp++;

	// get query
	const char	*query=(const char *)rp;
	while (*rp && rp!=rpend) {
		rp++;
	}
	if (rp==rpend) {
		return sendErrorResponse("Invalid request");
	}
	uint32_t	querylength=((const char *)rp)-query;
	rp++;
	
	// get param types
	uint16_t	paramcount;
	readBE(rp,&paramcount,&rp);
	uint32_t	*paramtypes=new uint32_t[paramcount];
	for (uint16_t i=0; i<paramcount; i++) {
		readBE(rp,&(paramtypes[i]),&rp);
	}

	// map cursor id to name
	cursormap.setValue(charstring::duplicate(cursorname),cursor->getId());

	// debug
	debugStart("Parse");
	if (getDebug()) {
		stdoutput.printf("	cursor name: %d\n",cursorname);
		stdoutput.printf("	cursor id: %d\n",cursor->getId());
		stdoutput.printf("	query length: %d\n",querylength);
		stdoutput.printf("	query: %.*s\n",querylength,query);
		stdoutput.printf("	param count: %d\n",paramcount);
		for (uint16_t i=0; i<paramcount; i++) {
			stdoutput.printf("		param %d type: %d\n",
							i,paramtypes[i]);
		}
	}
	debugEnd();

	// bounds checking
	if (querylength>maxquerysize) {
		return sendErrorResponse("Query is too large");
	}

	// copy the query into the cursor's query buffer
	char	*querybuffer=cont->getQueryBuffer(cursor);
	bytestring::copy(querybuffer,query,querylength);
	querybuffer[querylength]='\0';
	cont->setQueryLength(cursor,querylength);

	// FIXME: do something with param types?

	// prepare the query
	if (!cont->prepareQuery(cursor,cont->getQueryBuffer(cursor),
					cont->getQueryLength(cursor),
					true,true,true)) {
		return sendCursorError(cursor);
	}


	// response packet data structure
	//
	// data {
	// 	uint16_t	column count
	//  	column[] {
	//  		uint16_t	table name length
	//  		char[]		table name
	//  		uint16_t	column name length
	//  		char[]		column name
	//  		uint16_t	type
	//  		uint32_t	length
	//  		uint32_t	precision
	//  		uint32_t	scale
	//  		unsigned char	is nullable
	//  		unsigned char	is primary key
	//  		unsigned char	is unique
	//  		unsigned char	is part of key
	//  		unsigned char	is unsigned
	//  		unsigned char	is zero filled
	//  		unsigned char	is binary
	//  		unsigned char	is auto increment
	// 	}
	// }

	debugStart("ParseComplete");
	debugEnd();

	// build response packet
	resppacket.clear();

	// send response packet
	return sendPacket(MESSAGE_PARSECOMPLETE);
}

bool sqlrprotocol_postgresql::cursorExecute() {

	// The client would like to execute the previously prepared query.
	//
	// Since this protocol supports bind variables, we must also collect:
	// * input bind values
	// * ouput bind definitions
	// * input/output bind definitions and values

	// request packet data structure:
	//
	// data {
	// 	unsigned char	request type
	// 	uint16_t	cursor id
	// 	uint16_t	input bind count
	// 	input bind[] {
	// 		...
	// 	}
	// 	uint16_t	output bind count
	// 	output bind[] {
	// 		...
	// 	}
	// 	uint16_t	input/output bind count
	// 	input/output bind[] {
	// 		...
	// 	}
	// }

	// parse request packet
	const unsigned char	*rp=reqpacket;

	uint16_t	cursorid;

	rp++;
	readBE(rp,&cursorid,&rp);

	debugStart("cursor execute request");
	if (getDebug()) {
		stdoutput.printf("	cursor id: %d\n",cursorid);
	}

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(cursorid);
	if (!cursor) {
		return sendCursorNotOpenError();
	}

	// The "controller" provides a "memorypool" to store bind values.
	// It's basically a specialized heap that can be deleted all-at-once.
	// Clear that here.
	cont->getBindPool(cursor)->clear();

	// process binds
	if (!readInputBinds(cursor,rp,&rp) ||
		!readOutputBinds(cursor,rp,&rp) ||
		!readInputOutputBinds(cursor,rp,&rp)) {
		return sendTooManyBindsError();
	}

	debugEnd();

	// execute the query
	if (!cont->executeQuery(cursor,true,true,true,true)) {
		return sendCursorError(cursor);
	}


	// response packet data structure
	//
	// data {
	// 	uint64_t	affected rows
	// 	uint64_t	last insert id
	// 	output bind[] {
	// 		...
	// 	}
	// 	input/output bind[] {
	// 		...
	// 	}
	// }

	// set values to send
	uint64_t	affectedrows=0;
	uint64_t	lastinsertid=0;

	if (cont->knowsAffectedRows(cursor)) {
		affectedrows=cont->affectedRows(cursor);
	}
	cont->getLastInsertId(&lastinsertid);
	// NOTE; getLastInsertId can fail, but that usually just means that the
	// db doesn't support it.  So, rather than return an error, we'll just
	// leave id=0.

	// debug
	debugStart("cursor execute response");
	if (getDebug()) {
		stdoutput.printf("	affected rows: %lld\n",affectedrows);
		stdoutput.printf("	last insert id: %lld\n",lastinsertid);
	}
	debugEnd();

	// build response packet...
	resppacket.clear();
	writeBE(&resppacket,affectedrows);
	writeBE(&resppacket,lastinsertid);
	writeOutputBinds(cursor);
	writeInputOutputBinds(cursor);

	// send response packet
	return sendPacket(RESPONSE_CURSOR_EXECUTE);
}

bool sqlrprotocol_postgresql::readInputBinds(sqlrservercursor *cursor,
						const unsigned char *rp,
						const unsigned char **rpout) {

	// input bind structure
	//
	// input bind {
	// 	unsigned char	bind type
	// 	uint16_t	variable name size
	// 	char[]		variable name
	// 	if (type == null) {
	// 		(nothing)
	// 	}
	// 	if (type == int8) {
	// 		int8_t	value
	// 	}
	// 	if (type == int16) {
	// 		int16_t	value
	// 	}
	// 	if (type == int32) {
	// 		int32_t	value
	// 	}
	// 	if (type == int64) {
	// 		int64_t	value
	// 	}
	// 	if (type == float) {
	// 		float	value
	// 	}
	// 	if (type == double) {
	// 		double	value
	// 	}
	// 	if (type == time) {
	// 		char[]	fixed-length string representing time
	// 	}
	// 	if (type == date) {
	// 		char[]	fixed-length string representing date
	// 	}
	// 	if (type == datetime) {
	// 		char[]	fixed-length string representing date/time
	// 	}
	// 	if (type == blob/char/varchar/decimal) {
	// 		int32_t	value length
	// 		char[]	value
	// 	}
	// }

	// get the bind array...
	//
	// The controller provides an array of sqlrserverbindvar structures.
	//
	// All we have to do is run through the bind variables passed to us
	// and assign various bits of info to members of each struct in the
	// array. The controller will do whatever else is necessary to bind
	// them to the query prior to execution.
	sqlrserverbindvar	*inbinds=cont->getInputBinds(cursor);

	// get the bind pool
	memorypool		*bindpool=cont->getBindPool(cursor);

	// keep track of how many binds we find
	uint16_t	bindcount=0;
	for (;;) {

		// get the bind variable type
		unsigned char	nativebindtype;
		read(rp,&nativebindtype,&rp);

		// bail if we got the end-marker
		if (nativebindtype==SAMPLEDB_TYPE_END) {
			break;
		}

		debugStart("input bind",1);

		sqlrserverbindvar	*bv=&(inbinds[bindcount]);

		// get the bind variable name (and null terminate it)
		readBE(rp,(uint16_t *)(&bv->variablesize),&rp);
		bv->variable=(char *)bindpool->allocate(bv->variablesize+1);
		read(rp,bv->variable,bv->variablesize,&rp);
		bv->variable[bv->variablesize]='\0';

		// get the bind variable data
		switch (nativebindtype) {
			case SAMPLEDB_TYPE_NULL:
				bv->type=SQLRSERVERBINDVARTYPE_NULL;
				bv->isnull=cont->nullBindValue();
				break;
			case SAMPLEDB_TYPE_INT8:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				unsigned char	val;
				read(rp,&val,&rp);
				bv->value.integerval=(int8_t)val;
				bv->isnull=cont->nonNullBindValue();
				break;
				}
			case SAMPLEDB_TYPE_INT16:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				uint16_t	val;
				readBE(rp,&val,&rp);
				bv->value.integerval=(int16_t)val;
				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_INT32:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				uint32_t	val;
				readBE(rp,&val,&rp);
				bv->value.integerval=(int32_t)val;
				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_INT64:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				uint64_t	val;
				readBE(rp,&val,&rp);
				bv->value.integerval=(int64_t)val;
				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_FLOAT:
				{
				bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
				float	val;
				read(rp,&val,&rp);
				bv->value.doubleval.value=val;
				bv->value.doubleval.precision=0;
				bv->value.doubleval.scale=0;
				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_DOUBLE:
				{
				bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
				double	val;
				read(rp,&val,&rp);
				bv->value.doubleval.value=val;
				bv->value.doubleval.precision=0;
				bv->value.doubleval.scale=0;
				bv->isnull=cont->nonNullBindValue();
				break;
				}
			case SAMPLEDB_TYPE_TIME:
				{
				bv->type=SQLRSERVERBINDVARTYPE_DATE;

				// eg: "00:00:00.000000"
				char	val[16];
				read(rp,val,15,&rp);
				val[15]='\0';

				cont->parseDateTime(val,
						false,false,"/-.:",
						&bv->value.dateval.year,
						&bv->value.dateval.month,
						&bv->value.dateval.day,
						&bv->value.dateval.hour,
						&bv->value.dateval.minute,
						&bv->value.dateval.second,
						&bv->value.dateval.microsecond,
						&bv->value.dateval.isnegative);

				// invalidate non-time parts
				bv->value.dateval.year=-1;
				bv->value.dateval.month=-1;
				bv->value.dateval.day=-1;

				// invalidate unsupported parts
				bv->value.dateval.isnegative=false;
				bv->value.dateval.tz=NULL;

				// allocate scratch space
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					(char *)bindpool->allocate(
						bv->value.dateval.buffersize);

				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_DATE:
				{
				bv->type=SQLRSERVERBINDVARTYPE_DATE;

				// eg. "2001-01-01"
				char	val[11];
				read(rp,val,10,&rp);
				val[10]='\0';

				cont->parseDateTime(val,
						false,false,"/-.:",
						&bv->value.dateval.year,
						&bv->value.dateval.month,
						&bv->value.dateval.day,
						&bv->value.dateval.hour,
						&bv->value.dateval.minute,
						&bv->value.dateval.second,
						&bv->value.dateval.microsecond,
						&bv->value.dateval.isnegative);

				// invalidate non-date parts
				bv->value.dateval.hour=-1;
				bv->value.dateval.minute=-1;
				bv->value.dateval.second=-1;
				bv->value.dateval.microsecond=-1;

				// invalidate unsupported parts
				bv->value.dateval.isnegative=false;
				bv->value.dateval.tz=NULL;

				// allocate scratch space
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					(char *)bindpool->allocate(
						bv->value.dateval.buffersize);

				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_DATETIME:
				{
				bv->type=SQLRSERVERBINDVARTYPE_DATE;

				// eg. "2001-01-01 01:01:01.000000"
				char	val[27];
				read(rp,val,26,&rp);
				val[26]='\0';

				cont->parseDateTime(val,
						false,false,"/-.:",
						&bv->value.dateval.year,
						&bv->value.dateval.month,
						&bv->value.dateval.day,
						&bv->value.dateval.hour,
						&bv->value.dateval.minute,
						&bv->value.dateval.second,
						&bv->value.dateval.microsecond,
						&bv->value.dateval.isnegative);

				// invalidate unsupported parts
				bv->value.dateval.isnegative=false;
				bv->value.dateval.tz=NULL;

				// allocated scratch space
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					(char *)bindpool->allocate(
						bv->value.dateval.buffersize);

				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_BLOB:
				// (null-terminate in case it's really a clob)
				bv->type=SQLRSERVERBINDVARTYPE_BLOB;
				readBE(rp,&bv->valuesize,&rp);
				bv->value.stringval=
					(char *)bindpool->allocate(
							bv->valuesize+1);
				read(rp,bv->value.stringval,bv->valuesize,&rp);
				bv->value.stringval[bv->valuesize]='\0';
				bv->isnull=cont->nonNullBindValue();
				break;
			default:
				// handle all other types as
				// strings of some kind...
				// (be sure to null-terminate)
				bv->type=SQLRSERVERBINDVARTYPE_STRING;
				readBE(rp,&bv->valuesize,&rp);
				bv->value.stringval=
					(char *)bindpool->allocate(
							bv->valuesize+1);
				read(rp,bv->value.stringval,bv->valuesize,&rp);
				bv->value.stringval[bv->valuesize]='\0';
				bv->isnull=cont->nonNullBindValue();
				break;
		}

		if (getDebug()) {
			stdoutput.printf("		"
						"variable: %s\n",bv->variable);
			if (bv->type==SQLRSERVERBINDVARTYPE_NULL) {
				stdoutput.write("		"
						"type: NULL\n");
			} else if (bv->type==SQLRSERVERBINDVARTYPE_STRING) {
				stdoutput.write("		"
						"type: STRING\n");
				stdoutput.printf("		"
						"value: %s\n",
						bv->value.stringval);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_INTEGER) {
				stdoutput.write("		"
						"type: INTEGER\n");
				stdoutput.printf("		"
						"value: %lld\n",
						bv->value.integerval);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_DOUBLE) {
				stdoutput.write("		"
						"type: DOUBLE\n");
				stdoutput.printf("		"
						"value: %f (%d,%d)\n",
						bv->value.doubleval.value,
						bv->value.doubleval.precision,
						bv->value.doubleval.scale);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_DATE) {
				stdoutput.write("		"
						"type: DATE\n");
				stdoutput.printf("		"
						"value: ...\n");
			} else if (bv->type==SQLRSERVERBINDVARTYPE_BLOB) {
				stdoutput.write("		"
						"type: BLOB\n");
				stdoutput.printf("		"
						"value: ...\n");
			}
		}

		debugEnd(1);

		// increment bind count
		bindcount++;

		// bail if there were too many binds
		if (bindcount==maxbindcount) {
			return false;
		}
	}

	// set the bind count
	cont->setInputBindCount(cursor,bindcount);

	// pass position back out
	*rpout=rp;

	return true;
}

bool sqlrprotocol_postgresql::readOutputBinds(sqlrservercursor *cursor,
						const unsigned char *rp,
						const unsigned char **rpout) {

	// output bind structure
	//
	// output bind {
	// 	unsigned char	bind type
	// 	uint16_t	variable name size
	// 	char[]		variable name
	// 	if (type == blob/char/varchar/decimal) {
	// 		int32_t	value length
	// 	}
	// }

	// get the bind array
	//
	// The controller provides an array of sqlrserverbindvar structures.
	//
	// All we have to do is run through the bind variables passed to us
	// and assign various bits of info to members of each struct in the
	// array. The controller will do whatever else is necessary to bind
	// them to the query prior to execution.
	//
	// Since these are output binds, all we really have to do is set the
	// type and maybe initialize some things.
	sqlrserverbindvar	*outbinds=cont->getOutputBinds(cursor);

	// get the bind pool
	memorypool		*bindpool=cont->getBindPool(cursor);

	// keep track of how many binds we find
	uint16_t	bindcount=0;
	for (;;) {

		// get the bind variable type
		unsigned char	nativebindtype;
		read(rp,&nativebindtype,&rp);

		// bail if we got the end-marker
		if (nativebindtype==SAMPLEDB_TYPE_END) {
			break;
		}

		debugStart("output bind",1);

		sqlrserverbindvar	*bv=&(outbinds[bindcount]);

		// get the bind variable name (and null terminate it)
		readBE(rp,(uint16_t *)(&bv->variablesize),&rp);
		bv->variable=(char *)bindpool->allocate(bv->variablesize+1);
		read(rp,bv->variable,bv->variablesize,&rp);
		bv->variable[bv->variablesize]='\0';

		// keep track of the native bind type
		bv->nativetype=nativebindtype;

		// get the bind variable data
		switch (nativebindtype) {
			case SAMPLEDB_TYPE_NULL:
				bv->type=SQLRSERVERBINDVARTYPE_NULL;
				break;
			case SAMPLEDB_TYPE_INT8:
			case SAMPLEDB_TYPE_INT16:
			case SAMPLEDB_TYPE_INT32:
			case SAMPLEDB_TYPE_INT64:
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				break;
			case SAMPLEDB_TYPE_FLOAT:
			case SAMPLEDB_TYPE_DOUBLE:
				bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
				bv->value.doubleval.precision=0;
				bv->value.doubleval.scale=0;
				break;
			case SAMPLEDB_TYPE_TIME:
			case SAMPLEDB_TYPE_DATE:
			case SAMPLEDB_TYPE_DATETIME:
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

				// allocate scratch space
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					(char *)bindpool->allocate(
						bv->value.dateval.buffersize);
				break;
			case SAMPLEDB_TYPE_BLOB:
				bv->type=SQLRSERVERBINDVARTYPE_BLOB;
				readBE(rp,&bv->valuesize,&rp);
				bv->value.stringval=
					(char *)bindpool->allocate(
							bv->valuesize+1);
				bytestring::zero(bv->value.stringval,
							bv->valuesize+1);
				break;
			default:
				// handle all other types as
				// strings of some kind...
				bv->type=SQLRSERVERBINDVARTYPE_STRING;
				readBE(rp,&bv->valuesize,&rp);
				bv->value.stringval=
					(char *)bindpool->allocate(
							bv->valuesize+1);
				bytestring::zero(bv->value.stringval,
							bv->valuesize+1);
				break;
		}

		if (getDebug()) {
			stdoutput.printf("		"
						"variable: %s\n",bv->variable);
			if (bv->type==SQLRSERVERBINDVARTYPE_NULL) {
				stdoutput.write("		"
						"type: NULL\n");
			} else if (bv->type==SQLRSERVERBINDVARTYPE_STRING) {
				stdoutput.write("		"
						"type: STRING\n");
			} else if (bv->type==SQLRSERVERBINDVARTYPE_INTEGER) {
				stdoutput.write("		"
						"type: INTEGER\n");
			} else if (bv->type==SQLRSERVERBINDVARTYPE_DOUBLE) {
				stdoutput.write("		"
						"type: DOUBLE\n");
			} else if (bv->type==SQLRSERVERBINDVARTYPE_DATE) {
				stdoutput.write("		"
						"type: DATE\n");
			} else if (bv->type==SQLRSERVERBINDVARTYPE_BLOB) {
				stdoutput.write("		"
						"type: BLOB\n");
			}
		}

		debugEnd(1);

		// increment bind count
		bindcount++;

		// bail if there were too many binds
		if (bindcount==maxbindcount) {
			return false;
		}
	}

	// set the bind count
	cont->setOutputBindCount(cursor,bindcount);

	// pass position back out
	*rpout=rp;

	return true;
}

bool sqlrprotocol_postgresql::readInputOutputBinds(sqlrservercursor *cursor,
						const unsigned char *rp,
						const unsigned char **rpout) {

	// input/output bind structure
	//
	// input/output bind {
	// 	unsigned char	bind type
	// 	uint16_t	variable name size
	// 	char[]		variable name
	// 	if (type == null) {
	// 		(nothing)
	// 	}
	// 	if (type == int8) {
	// 		int8_t	value
	// 	}
	// 	if (type == int16) {
	// 		int16_t	value
	// 	}
	// 	if (type == int32) {
	// 		int32_t	value
	// 	}
	// 	if (type == int64) {
	// 		int64_t	value
	// 	}
	// 	if (type == float) {
	// 		float	value
	// 	}
	// 	if (type == double) {
	// 		double	value
	// 	}
	// 	if (type == time) {
	// 		char[]	fixed-length string representing time
	// 	}
	// 	if (type == date) {
	// 		char[]	fixed-length string representing date
	// 	}
	// 	if (type == datetime) {
	// 		char[]	fixed-length string representing date/time
	// 	}
	// 	if (type == blob/char/varchar/decimal) {
	// 		int32_t	value length
	// 		char[]	value
	// 	}
	// }

	// get the bind array
	//
	// The controller provides an array of sqlrserverbindvar structures.
	//
	// All we have to do is run through the bind variables passed to us
	// and assign various bits of info to members of each struct in the
	// array. The controller will do whatever else is necessary to bind
	// them to the query prior to execution.
	sqlrserverbindvar	*inoutbinds=cont->getInputOutputBinds(cursor);

	// get the bind pool
	memorypool		*bindpool=cont->getBindPool(cursor);

	// keep track of how many binds we find
	uint16_t	bindcount=0;
	for (;;) {

		// get the bind variable type
		unsigned char	nativebindtype;
		read(rp,&nativebindtype,&rp);

		// bail if we got the end-marker
		if (nativebindtype==SAMPLEDB_TYPE_END) {
			break;
		}

		debugStart("input/output bind",1);

		sqlrserverbindvar	*bv=&(inoutbinds[bindcount]);

		// get the bind variable name (and null terminate it)
		readBE(rp,(uint16_t *)(&bv->variablesize),&rp);
		bv->variable=(char *)bindpool->allocate(bv->variablesize+1);
		read(rp,bv->variable,bv->variablesize,&rp);
		bv->variable[bv->variablesize]='\0';

		// keep track of the native bind type
		bv->nativetype=nativebindtype;

		// get the bind variable data
		switch (nativebindtype) {
			case SAMPLEDB_TYPE_NULL:
				bv->type=SQLRSERVERBINDVARTYPE_NULL;
				bv->isnull=cont->nullBindValue();
				break;
			case SAMPLEDB_TYPE_INT8:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				unsigned char	val;
				read(rp,&val,&rp);
				bv->value.integerval=(int8_t)val;
				bv->isnull=cont->nonNullBindValue();
				break;
				}
			case SAMPLEDB_TYPE_INT16:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				uint16_t	val;
				readBE(rp,&val,&rp);
				bv->value.integerval=(int16_t)val;
				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_INT32:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				uint32_t	val;
				readBE(rp,&val,&rp);
				bv->value.integerval=(int32_t)val;
				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_INT64:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				uint64_t	val;
				readBE(rp,&val,&rp);
				bv->value.integerval=(int64_t)val;
				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_FLOAT:
				{
				bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
				float	val;
				read(rp,&val,&rp);
				bv->value.doubleval.value=val;
				bv->value.doubleval.precision=0;
				bv->value.doubleval.scale=0;
				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_DOUBLE:
				{
				bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
				double	val;
				read(rp,&val,&rp);
				bv->value.doubleval.value=val;
				bv->value.doubleval.precision=0;
				bv->value.doubleval.scale=0;
				bv->isnull=cont->nonNullBindValue();
				break;
				}
			case SAMPLEDB_TYPE_TIME:
				{
				bv->type=SQLRSERVERBINDVARTYPE_DATE;

				// eg: "00:00:00.000000"
				char	val[16];
				read(rp,val,15,&rp);
				val[15]='\0';

				cont->parseDateTime(val,
						false,false,"/-.:",
						&bv->value.dateval.year,
						&bv->value.dateval.month,
						&bv->value.dateval.day,
						&bv->value.dateval.hour,
						&bv->value.dateval.minute,
						&bv->value.dateval.second,
						&bv->value.dateval.microsecond,
						&bv->value.dateval.isnegative);

				// invalidate non-time parts
				bv->value.dateval.year=-1;
				bv->value.dateval.month=-1;
				bv->value.dateval.day=-1;

				// invalidate unsupported parts
				bv->value.dateval.isnegative=false;
				bv->value.dateval.tz=NULL;

				// allocate scratch space
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					(char *)bindpool->allocate(
						bv->value.dateval.buffersize);

				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_DATE:
				{
				bv->type=SQLRSERVERBINDVARTYPE_DATE;

				// eg. "2001-01-01"
				char	val[11];
				read(rp,val,10,&rp);
				val[10]='\0';

				cont->parseDateTime(val,
						false,false,"/-.:",
						&bv->value.dateval.year,
						&bv->value.dateval.month,
						&bv->value.dateval.day,
						&bv->value.dateval.hour,
						&bv->value.dateval.minute,
						&bv->value.dateval.second,
						&bv->value.dateval.microsecond,
						&bv->value.dateval.isnegative);

				// invalidate non-date parts
				bv->value.dateval.hour=-1;
				bv->value.dateval.minute=-1;
				bv->value.dateval.second=-1;
				bv->value.dateval.microsecond=-1;

				// invalidate unsupported parts
				bv->value.dateval.isnegative=false;
				bv->value.dateval.tz=NULL;

				// allocate scratch space
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					(char *)bindpool->allocate(
						bv->value.dateval.buffersize);

				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_DATETIME:
				{
				bv->type=SQLRSERVERBINDVARTYPE_DATE;

				// eg. "2001-01-01 01:01:01.000000"
				char	val[27];
				read(rp,val,26,&rp);
				val[26]='\0';

				cont->parseDateTime(val,
						false,false,"/-.:",
						&bv->value.dateval.year,
						&bv->value.dateval.month,
						&bv->value.dateval.day,
						&bv->value.dateval.hour,
						&bv->value.dateval.minute,
						&bv->value.dateval.second,
						&bv->value.dateval.microsecond,
						&bv->value.dateval.isnegative);

				// invalidate unsupported parts
				bv->value.dateval.isnegative=false;
				bv->value.dateval.tz=NULL;

				// allocated scratch space
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					(char *)bindpool->allocate(
						bv->value.dateval.buffersize);

				bv->isnull=cont->nonNullBindValue();
				}
				break;
			case SAMPLEDB_TYPE_BLOB:
				bv->type=SQLRSERVERBINDVARTYPE_BLOB;
				readBE(rp,&bv->valuesize,&rp);
				bv->value.stringval=
					(char *)bindpool->allocate(
							bv->valuesize);
				read(rp,bv->value.stringval,bv->valuesize,&rp);
				bv->isnull=cont->nonNullBindValue();
				break;
			default:
				// handle all other types as
				// strings of some kind...
				bv->type=SQLRSERVERBINDVARTYPE_STRING;
				readBE(rp,&bv->valuesize,&rp);
				bv->value.stringval=
					(char *)bindpool->allocate(
							bv->valuesize);
				read(rp,bv->value.stringval,bv->valuesize,&rp);
				bv->isnull=cont->nonNullBindValue();
				break;
		}

		if (getDebug()) {
			stdoutput.printf("		"
						"variable: %s\n",bv->variable);
			if (bv->type==SQLRSERVERBINDVARTYPE_NULL) {
				stdoutput.write("		"
						"type: NULL\n");
			} else if (bv->type==SQLRSERVERBINDVARTYPE_STRING) {
				stdoutput.write("		"
						"type: STRING\n");
				stdoutput.printf("		"
						"value: %s\n",
						bv->value.stringval);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_INTEGER) {
				stdoutput.write("		"
						"type: INTEGER\n");
				stdoutput.printf("		"
						"value: %lld\n",
						bv->value.integerval);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_DOUBLE) {
				stdoutput.write("		"
						"type: DOUBLE\n");
				stdoutput.printf("		"
						"value: %f (%d,%d)\n",
						bv->value.doubleval.value,
						bv->value.doubleval.precision,
						bv->value.doubleval.scale);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_DATE) {
				stdoutput.write("		"
						"type: DATE\n");
				stdoutput.printf("		"
						"value: ...\n");
			} else if (bv->type==SQLRSERVERBINDVARTYPE_BLOB) {
				stdoutput.write("		"
						"type: BLOB\n");
				stdoutput.printf("		"
						"value: ...\n");
			}
		}

		debugEnd(1);

		// increment bind count
		bindcount++;

		// bail if there were too many binds
		if (bindcount==maxbindcount) {
			return false;
		}
	}

	// set the bind count
	cont->setInputOutputBindCount(cursor,bindcount);

	// pass position back out
	*rpout=rp;

	return true;
}

void sqlrprotocol_postgresql::writeOutputBinds(sqlrservercursor *cursor) {

	// output bind structure {
	// 	unsigned char	bind type
	// 	uint16_t	variable name size
	// 	char[]		variable name
	// 	unsigned char	isnull;
	// 	if (type == null || isnull == true) {
	// 		(nothing)
	// 	}
	// 	if (type == int8) {
	// 		int8_t	value
	// 	}
	// 	if (type == int16) {
	// 		int16_t	value
	// 	}
	// 	if (type == int32) {
	// 		int32_t	value
	// 	}
	// 	if (type == int64) {
	// 		int64_t	value
	// 	}
	// 	if (type == float) {
	// 		float	value
	// 	}
	// 	if (type == double) {
	// 		double	value
	// 	}
	// 	if (type == time) {
	// 		char[]	fixed-length string representing time
	// 	}
	// 	if (type == date) {
	// 		char[]	fixed-length string representing date
	// 	}
	// 	if (type == datetime) {
	// 		char[]	fixed-length string representing date/time
	// 	}
	// 	if (type == blob/char/varchar/decimal) {
	// 		int32_t	value length
	// 		char[]	value
	// 	}
	// }

	// By now, the controller will have populated the members of the 
	// output and inputoutput binds arrays with whatever values we need
	// to send back to the client.
	//
	// Send them back...

	sqlrserverbindvar	*outbinds=cont->getOutputBinds(cursor);
	uint16_t		bindcount=cont->getOutputBindCount(cursor);

	for (uint16_t i=0; i<bindcount; i++) {

		sqlrserverbindvar	*bv=&(outbinds[i]);

		// get the native bind type
		unsigned char	nativebindtype=bv->nativetype;

		// send type and name
		write(&resppacket,nativebindtype);
		writeBE(&resppacket,*((uint16_t *)&bv->variablesize));
		write(&resppacket,bv->variable,bv->variablesize);

		// handle nulls
		if (bv->isnull==cont->nullBindValue()) {
			write(&resppacket,(unsigned char)1);
			nativebindtype=SAMPLEDB_TYPE_NULL;
		} else {
			write(&resppacket,(unsigned char)0);
		}

		switch (nativebindtype) {
			case SAMPLEDB_TYPE_NULL:
				break;
			case SAMPLEDB_TYPE_INT8:
				{
				int8_t	val=bv->value.integerval;
				write(&resppacket,*((unsigned char *)&val));
				break;
				}
			case SAMPLEDB_TYPE_INT16:
				{
				int16_t	val=bv->value.integerval;
				writeBE(&resppacket,*((uint16_t *)&val));
				}
				break;
			case SAMPLEDB_TYPE_INT32:
				{
				int32_t	val=bv->value.integerval;
				writeBE(&resppacket,*((uint32_t *)&val));
				}
				break;
			case SAMPLEDB_TYPE_INT64:
				{
				int64_t	val=bv->value.integerval;
				writeBE(&resppacket,*((uint64_t *)&val));
				}
				break;
			case SAMPLEDB_TYPE_FLOAT:
				{
				float	val=bv->value.doubleval.value;
				write(&resppacket,val);
				}
				break;
			case SAMPLEDB_TYPE_DOUBLE:
				{
				double	val=bv->value.doubleval.value;
				write(&resppacket,val);
				break;
				}
			case SAMPLEDB_TYPE_TIME:
				{
				char	val[16];
				charstring::printf(val,sizeof(val),
						"%02d:%02d:%02d.%06d",
						bv->value.dateval.hour,
						bv->value.dateval.minute,
						bv->value.dateval.second,
						bv->value.dateval.microsecond);
				write(&resppacket,val);
				}
				break;
			case SAMPLEDB_TYPE_DATE:
				{
				char	val[11];
				charstring::printf(val,sizeof(val),
						"%04d-%02d-%02d",
						bv->value.dateval.year,
						bv->value.dateval.month,
						bv->value.dateval.day);
				write(&resppacket,val);
				}
				break;
			case SAMPLEDB_TYPE_DATETIME:
				{
				char	val[27];
				charstring::printf(val,sizeof(val),
						"%04d-%02d-%02d "
						"%02d:%02d:%02d.%06d",
						bv->value.dateval.year,
						bv->value.dateval.month,
						bv->value.dateval.day,
						bv->value.dateval.hour,
						bv->value.dateval.minute,
						bv->value.dateval.second,
						bv->value.dateval.microsecond);
				write(&resppacket,val);
				}
				break;
			case SAMPLEDB_TYPE_BLOB:
				write(&resppacket,bv->valuesize);
				write(&resppacket,bv->value.stringval);
				break;
			default:
				// handle all other types as
				// strings of some kind...
				write(&resppacket,bv->valuesize);
				write(&resppacket,bv->value.stringval);
				break;
		}
	}
}

void sqlrprotocol_postgresql::writeInputOutputBinds(sqlrservercursor *cursor) {

	// input/output bind structure {
	// 	unsigned char	bind type
	// 	uint16_t	variable name size
	// 	char[]		variable name
	// 	unsigned char	isnull;
	// 	if (type == null || isnull == true) {
	// 		(nothing)
	// 	}
	// 	if (type == int8) {
	// 		int8_t	value
	// 	}
	// 	if (type == int16) {
	// 		int16_t	value
	// 	}
	// 	if (type == int32) {
	// 		int32_t	value
	// 	}
	// 	if (type == int64) {
	// 		int64_t	value
	// 	}
	// 	if (type == float) {
	// 		float	value
	// 	}
	// 	if (type == double) {
	// 		double	value
	// 	}
	// 	if (type == time) {
	// 		char[]	fixed-length string representing time
	// 	}
	// 	if (type == date) {
	// 		char[]	fixed-length string representing date
	// 	}
	// 	if (type == datetime) {
	// 		char[]	fixed-length string representing date/time
	// 	}
	// 	if (type == blob/char/varchar/decimal) {
	// 		int32_t	value length
	// 		char[]	value
	// 	}
	// }

	// By now, the controller will have populated the members of the 
	// output and inputoutput binds arrays with whatever values we need
	// to send back to the client.
	//
	// Send them back...

	sqlrserverbindvar	*inoutbinds=cont->getInputOutputBinds(cursor);
	uint16_t		bindcount=cont->getInputOutputBindCount(cursor);

	for (uint16_t i=0; i<bindcount; i++) {

		sqlrserverbindvar	*bv=&(inoutbinds[i]);

		// get the native bind type
		unsigned char	nativebindtype=bv->nativetype;

		// send type and name
		write(&resppacket,nativebindtype);
		writeBE(&resppacket,*((uint16_t *)&bv->variablesize));
		write(&resppacket,bv->variable,bv->variablesize);

		// handle nulls
		if (bv->isnull==cont->nullBindValue()) {
			write(&resppacket,(unsigned char)1);
			nativebindtype=SAMPLEDB_TYPE_NULL;
		} else {
			write(&resppacket,(unsigned char)0);
		}

		switch (nativebindtype) {
			case SAMPLEDB_TYPE_NULL:
				break;
			case SAMPLEDB_TYPE_INT8:
				{
				int8_t	val=bv->value.integerval;
				write(&resppacket,*((unsigned char *)&val));
				break;
				}
			case SAMPLEDB_TYPE_INT16:
				{
				int16_t	val=bv->value.integerval;
				writeBE(&resppacket,*((uint16_t *)&val));
				}
				break;
			case SAMPLEDB_TYPE_INT32:
				{
				int32_t	val=bv->value.integerval;
				writeBE(&resppacket,*((uint32_t *)&val));
				}
				break;
			case SAMPLEDB_TYPE_INT64:
				{
				int64_t	val=bv->value.integerval;
				writeBE(&resppacket,*((uint64_t *)&val));
				}
				break;
			case SAMPLEDB_TYPE_FLOAT:
				{
				float	val=bv->value.doubleval.value;
				write(&resppacket,val);
				}
				break;
			case SAMPLEDB_TYPE_DOUBLE:
				{
				double	val=bv->value.doubleval.value;
				write(&resppacket,val);
				break;
				}
			case SAMPLEDB_TYPE_TIME:
				{
				char	val[16];
				charstring::printf(val,sizeof(val),
						"%02d:%02d:%02d.%06d",
						bv->value.dateval.hour,
						bv->value.dateval.minute,
						bv->value.dateval.second,
						bv->value.dateval.microsecond);
				write(&resppacket,val);
				}
				break;
			case SAMPLEDB_TYPE_DATE:
				{
				char	val[11];
				charstring::printf(val,sizeof(val),
						"%04d-%02d-%02d",
						bv->value.dateval.year,
						bv->value.dateval.month,
						bv->value.dateval.day);
				write(&resppacket,val);
				}
				break;
			case SAMPLEDB_TYPE_DATETIME:
				{
				char	val[27];
				charstring::printf(val,sizeof(val),
						"%04d-%02d-%02d "
						"%02d:%02d:%02d.%06d",
						bv->value.dateval.year,
						bv->value.dateval.month,
						bv->value.dateval.day,
						bv->value.dateval.hour,
						bv->value.dateval.minute,
						bv->value.dateval.second,
						bv->value.dateval.microsecond);
				write(&resppacket,val);
				}
				break;
			case SAMPLEDB_TYPE_BLOB:
				write(&resppacket,bv->valuesize);
				write(&resppacket,bv->value.stringval);
				break;
			default:
				// handle all other types as
				// strings of some kind...
				write(&resppacket,bv->valuesize);
				write(&resppacket,bv->value.stringval);
				break;
		}
	}
}

bool sqlrprotocol_postgresql::cursorFetch() {

	// The client would like to fetch a row from the result set of the
	// previously executed query.

	// request packet data structure:
	//
	// data {
	// 	unsigned char	request type
	// 	uint16_t	cursor id
	// }

	// parse request packet
	const unsigned char	*rp=reqpacket;

	uint16_t	cursorid;

	rp++;
	readBE(rp,&cursorid,&rp);

	// debug
	debugStart("cursor fetch request");
	if (getDebug()) {
		stdoutput.printf("	cursor id: %d\n",cursorid);
	}
	debugEnd();

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(cursorid);
	if (!cursor) {
		return sendCursorNotOpenError();
	}

	// fetch the row
	bool	error;
	if (!cont->fetchRow(cursor,&error)) {
		return (error)?sendCursorError(cursor):
				sendEndOfResultSetResponse();
	}


	// response packet data structure
	//
	// data {
	// 	field [] {
	// 		type
	// 		data (format depends on type)
	// 	}
	// }

	debugStart("row response");

	// build response packet
	resppacket.clear();

	uint16_t	colcount=cont->colCount(cursor);
	for (uint16_t i=0; i<colcount; i++) {

		debugStart("field",1);

		// get the field
		const char	*field=NULL;
		uint64_t	fieldlength=0;
		bool		blob=0;
		bool		null=0;
		if (!cont->getField(cursor,i,&field,&fieldlength,&blob,&null)) {
			debugEnd(1);
			return false;
		}

		// send the field
		writeField(cursor,i,field,fieldlength,
			postgresqltypemap[cont->getColumnType(cursor,i)],
			blob,null);

		debugEnd(1);
	}

	debugEnd();

	// FIXME: kludgy
	cont->nextRow(cursor);

	// send response packet
	return sendPacket(RESPONSE_CURSOR_FETCH);
}

bool sqlrprotocol_postgresql::sendEndOfResultSetResponse() {

	// response packet data structure
	// 	(nothing)
	// }

	debugStart("end of result set response");
	debugEnd();

	// build response packet
	resppacket.clear();

	// send response packet
	return sendPacket(RESPONSE_END_OF_RESULT_SET);
}

void sqlrprotocol_postgresql::writeField(sqlrservercursor *cursor,
					uint16_t column,
					const char *field,
					uint64_t fieldlength,
					unsigned char columntype,
					bool blob,
					bool null) {

	// debug
	if (getDebug()) {
		if (blob) {
			stdoutput.write("		LOB\n");
		} else {
			stdoutput.printf("		\"%s\" (%d)\n",
							field,fieldlength);
		}
	}

	// handle nulls
	if (null) {
		write(&resppacket,(unsigned char)SAMPLEDB_TYPE_NULL);
		return;
	}

	// handle blobs
	if (blob) {
		writeBlobField(cursor,column);
		return;
	}

	// handle regular fields
	write(&resppacket,columntype);
	switch (columntype) {
		case SAMPLEDB_TYPE_INT8:
			write(&resppacket,
				(char)charstring::toInteger(field));
			break;
		case SAMPLEDB_TYPE_INT16:
			writeBE(&resppacket,
				(uint16_t)charstring::toInteger(field));
			break;
		case SAMPLEDB_TYPE_INT32:
			writeBE(&resppacket,
				(uint32_t)charstring::toInteger(field));
			break;
		case SAMPLEDB_TYPE_INT64:
			writeBE(&resppacket,
				(uint64_t)charstring::toInteger(field));
			break;
		case SAMPLEDB_TYPE_DOUBLE:
			{
			double		fval=charstring::toFloat(field);
			uint64_t	ival;
			bytestring::copy(&ival,&fval,sizeof(double));
			writeBE(&resppacket,ival);
			}
			break;
		case SAMPLEDB_TYPE_FLOAT:
			{
			float		fval=charstring::toFloat(field);
			uint32_t	ival;
			bytestring::copy(&ival,&fval,sizeof(float));
			writeBE(&resppacket,ival);
			}
			break;
		case SAMPLEDB_TYPE_TIME:
			{
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
			char	val[16];
			charstring::printf(val,sizeof(val),
						"%02d:%02d:%02d.%06d",
						hour,minute,second,usec);
			write(&resppacket,val);
			}
			break;
		case SAMPLEDB_TYPE_DATE:
			{
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
			char	val[11];
			charstring::printf(val,sizeof(val),
						"%04d-%02d-%02d",
						year,month,day);
			write(&resppacket,val);
			}
			break;
		case SAMPLEDB_TYPE_DATETIME:
			{
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
			char	val[27];
			charstring::printf(val,sizeof(val),
					"%04d-%02d-%02d %02d:%02d:%02d.%06d",
					year,month,day,hour,minute,second,usec);
			write(&resppacket,val);
			}
			break;
		case SAMPLEDB_TYPE_CHAR:
		case SAMPLEDB_TYPE_VARCHAR:
		case SAMPLEDB_TYPE_DECIMAL:
		case SAMPLEDB_TYPE_BLOB:
			// Why handle LOBs here?  The database connection module
			// might not correctly identify a LOB field as a LOB.
			// This happens with the mysql connection module when
			// it's using the traditional mysql API, which doesn't
			// handle LOBs differently from other data.  In cases
			// like that, the blob flag passed to getField() will be
			// false and this method will be called instead of
			// sendLobField().  So, this method has to handle LOBs
			// too.
			writeBE(&resppacket,fieldlength);
			write(&resppacket,field,fieldlength);
			break;
		default:
			break;
	}
}

#define MAX_BYTES_PER_CHAR	4

void sqlrprotocol_postgresql::writeBlobField(sqlrservercursor *cursor,
							uint16_t column) {

	// get blob length (send a null field if it fails)
	uint64_t	loblength;
	if (!cont->getLobFieldLength(cursor,column,&loblength)) {
		write(&resppacket,(unsigned char)SAMPLEDB_TYPE_NULL);
		cont->closeLobField(cursor,column);
		return;
	}

	// initialize sizes and status
	uint64_t	charstoread=sizeof(lobbuffer)/MAX_BYTES_PER_CHAR;
	uint64_t	charsread=0;
	uint64_t	offset=0;

	// send blob data...
	for (;;) {

		// read a segment from the lob
		if (cont->getLobFieldSegment(cursor,column,
					lobbuffer,sizeof(lobbuffer),
					offset,charstoread,&charsread) &&
					charsread) {

			// send type/length if we haven't already
			if (!offset) {
				write(&resppacket,
					(unsigned char)SAMPLEDB_TYPE_BLOB);
				write(&resppacket,loblength);
			}

			// send the segment we just got
			write(&resppacket,lobbuffer,charsread);

			// bump offset
			offset=offset+charstoread;

		} else {

			// if we failed to read a segment or read
			// an empty segment then we're done...

			// if we haven't sent anything yet,
			// then send a NULL field
			if (!offset) {
				write(&resppacket,
					(unsigned char)SAMPLEDB_TYPE_NULL);
			}

			cont->closeLobField(cursor,column);
			return;
		}
	}
}

bool sqlrprotocol_postgresql::cursorClose() {

	// The client would like to close the specified cursor.

	// request packet data structure:
	//
	// data {
	// 	unsigned char	request type
	// 	uint16_t	cursor id
	// }

	// parse request packet
	const unsigned char	*rp=reqpacket;

	uint16_t	cursorid;

	rp++;
	readBE(rp,&cursorid,&rp);

	// debug
	debugStart("cursor close request");
	if (getDebug()) {
		stdoutput.printf("	cursor id: %d\n",cursorid);
	}
	debugEnd();

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(cursorid);
	if (!cursor) {
		return sendCursorNotOpenError();
	}

	// close the cursor
	if (!cont->close(cursor)) {
		return sendErrorResponse("Failed to close cursor");
	}

	// mark the cursor available
	cont->setState(cursor,SQLRCURSORSTATE_AVAILABLE);

	// success
	return sendSuccessResponse();
}

bool sqlrprotocol_postgresql::sendCursorError(sqlrservercursor *cursor) {

	// get the cursor-level error
	const char	*errorstring;
	uint32_t	errorlength;
	int64_t		errnum;
	bool		liveconnection;
	cont->errorMessage(cursor,
				&errorstring,
				&errorlength,
				&errnum,
				&liveconnection);

	// send an error response packet
	return sendErrorResponse(errorstring,errorlength);
}

bool sqlrprotocol_postgresql::sendNotImplementedError() {
	return sendErrorResponse("ERROR","0A000","Feature not supported");
}

bool sqlrprotocol_postgresql::sendOutOfCursorsError() {
	return sendErrorResponse("Out of cursors");
}

bool sqlrprotocol_postgresql::sendCursorNotOpenError() {
	return sendErrorResponse("Cursor is not open");
}

bool sqlrprotocol_postgresql::sendTooManyBindsError() {
	return sendErrorResponse("Too many bind variables");
}

void sqlrprotocol_postgresql::debugRecvTypeError() {
	if (getDebug()) {
		stdoutput.printf("invalid packet type: 0x%02x\n",reqtype);
	}
}

void sqlrprotocol_postgresql::debugColumnType(unsigned char columntype) {
	stdoutput.write("		type: ");
	switch (columntype) {
		case SAMPLEDB_TYPE_NULL:
			stdoutput.write("SAMPLEDB_TYPE_NULL\n");
			break;
		case SAMPLEDB_TYPE_CHAR:
			stdoutput.write("SAMPLEDB_TYPE_CHAR\n");
			break;
		case SAMPLEDB_TYPE_VARCHAR:
			stdoutput.write("SAMPLEDB_TYPE_VARCHAR\n");
			break;
		case SAMPLEDB_TYPE_INT8:
			stdoutput.write("SAMPLEDB_TYPE_INT8\n");
			break;
		case SAMPLEDB_TYPE_INT16:
			stdoutput.write("SAMPLEDB_TYPE_INT16\n");
			break;
		case SAMPLEDB_TYPE_INT32:
			stdoutput.write("SAMPLEDB_TYPE_INT32\n");
			break;
		case SAMPLEDB_TYPE_INT64:
			stdoutput.write("SAMPLEDB_TYPE_INT64\n");
			break;
		case SAMPLEDB_TYPE_FLOAT:
			stdoutput.write("SAMPLEDB_TYPE_FLOAT\n");
			break;
		case SAMPLEDB_TYPE_DOUBLE:
			stdoutput.write("SAMPLEDB_TYPE_DOUBLE\n");
			break;
		case SAMPLEDB_TYPE_DECIMAL:
			stdoutput.write("SAMPLEDB_TYPE_DECIMAL\n");
			break;
		case SAMPLEDB_TYPE_DATE:
			stdoutput.write("SAMPLEDB_TYPE_DATE\n");
			break;
		case SAMPLEDB_TYPE_TIME:
			stdoutput.write("SAMPLEDB_TYPE_TIME\n");
			break;
		case SAMPLEDB_TYPE_DATETIME:
			stdoutput.write("SAMPLEDB_TYPE_DATETIME\n");
			break;
		case SAMPLEDB_TYPE_BLOB:
			stdoutput.write("SAMPLEDB_TYPE_BLOB\n");
			break;
		default:
			stdoutput.write("unknown SAMPLEDB_TYPE\n");
			break;
	}
}

void sqlrprotocol_postgresql::debugSystemError() {
	char	*err=error::getErrorString();
	stdoutput.printf("%s\n",err);
	delete[] err;
}

void sqlrprotocol_postgresql::readString(const unsigned char *rp,
					const unsigned char *rpend,
					stringbuffer *strb,
					const unsigned char **rpout) {
	// read until we hit a null or the end of the request
	while (*rp && rp!=rpend) {
		strb->append(*rp);
		rp++;
	}

	// bump past the null (if we didn't hit the end of the request)
	if (rp!=rpend) {
		rp++;
	}

	// return the current position
	*rpout=rp;
}


extern "C" {
	SQLRSERVER_DLLSPEC sqlrprotocol	*new_sqlrprotocol_postgresql(
						sqlrservercontroller *cont,
						sqlrprotocols *ps,
						domnode *parameters) {
		return new sqlrprotocol_postgresql(cont,ps,parameters);
	}
}
