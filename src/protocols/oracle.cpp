// Copyright (c) 2017  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/bytebuffer.h>
#include <rudiments/character.h>
#include <rudiments/randomnumber.h>
#include <rudiments/datetime.h>
#include <rudiments/process.h>
#include <rudiments/error.h>

// packet types
#define	PACKET_CONNECT		1
#define	PACKET_ACCEPT 		2
#define	PACKET_ACKNOWLEDGE	3
#define	PACKET_REFUSE		4
#define	PACKET_REDIRECT		5
#define	PACKET_DATA		6
#define	PACKET_NULL		7
#define	PACKET_ABORT		9
#define	PACKET_RESEND		11
#define	PACKET_MARKER		12
#define	PACKET_ATTENTION	13
#define	PACKET_CONTROL_INFO	14

// protocol versions
#define PROTOCOL_VERSION_7		0x0134
#define PROTOCOL_VERSION_8		0x0136
#define PROTOCOL_VERSION_9		0x0138
#define PROTOCOL_VERSION_10		0x0139
#define PROTOCOL_VERSION_11		0x013A
#define PROTOCOL_VERSION_12		0x013B

// authentication adapter table "nautab" supports:
// SECURID
// KERBEROS5
// IDENTIX
// RADIUS
// don't know the id's for each though

// encryption types (from 8i trace)
#define ENC_NONE	0
#define ENC_RC4_40	1
#define ENC_RC4_56	8
#define	ENC_DES		2
#define ENC_DES40	3
#define ENC_RC4_256	6
#define ENC_RC4_128	10
#define ENC_3DES168	12
#define ENC_3DES112	11

// checksumming types (from 8i trace)
#define	CS_NONE		0
#define	CS_SHA1		3
#define CS_MD5		1

// two task common (ttc) types
#define TTC_PROTOCOL_NEGOTIATION	0x01
#define TTC_DATATYPE_NEGOTIATION	0x02
#define TTC_TTI_FUNCTION		0x03
#define TTC_OK				0x08
#define TTC_EXTENDED_TTI_FUNCTION	0x11
#define TTC_EXTPROC1			0x20
#define TTC_EXTPROC2			0x44

// two task interface (tti) functions
#define TTI_OPEN		0x02
#define TTI_QUERY		0x03
#define TTI_EXECUTE		0x04
#define TTI_FETCH		0x05
#define TTI_CLOSE		0x08
#define TTI_DISCONNECT		0x09
#define TTI_AUTOCOMMIT_ON	0x0C
#define TTI_AUTOCOMMIT_OFF	0x0D
#define TTI_COMMIT		0x0E
#define TTI_ROLLBACK		0x0F
#define TTI_CANCEL		0x14
#define TTI_DESCRIBE		0x2B
#define TTI_STARTUP		0x30
#define TTI_SHUTDOWN		0x31
#define TTI_VERSION		0x3B
#define TTI_K2_TRANSACTIONS	0x43
#define TTI_QUERY2		0x47
#define TTI_OSQL7		0x4A
#define TTI_OKOD		0x5C
#define TTI_QUERY3		0x5E
#define TTI_LOB_OPERATIONS	0x60
#define TTI_ODNY		0x62
#define TTI_TRANSACTION_END	0x67
#define TTI_TRANSACTION_BEGIN	0x68
#define TTI_OCCA		0x69
#define TTI_STARTUP2		0x6D
#define TTI_LOGON_PRESENT_PWD	0x51
#define TTI_LOGON_PRESENT_USER	0x52
#define TTI_LOGON_UNKNOWN	0x54
#define TTI_LOGON_PRESENT_PWD_SEND_AUTH_PASSWORD	0x73
#define TTI_LOGON_PRESENT_USER_REQ_AUTH_SESSKEY		0x76
#define TTI_DESCRIBE2		0x77
#define TTI_OOTCM		0x7F
#define TTI_OKPFC		0x8B
#define TTI_SWITCH_SESSION	0x6B
#define TTI_CLOSE2		0x78
#define TTI_OSCID		0x87
#define TTI_OSKEYVAL		0x9A

// options
#define OPTION_PARSE		(1<<0) // 1
#define OPTION_BIND		(1<<3) // 8
#define OPTION_DEFINE		(1<<4) // 16
#define OPTION_EXECUTE		(1<<5) // 32
#define OPTION_FETCH		(1<<6) // 64
#define OPTION_CANCEL		(1<<7) // 128
#define OPTION_COMMIT		(1<<8) // 254
#define OPTION_EXACTFETCH	(1<<9) // 512
#define OPTION_SNDIOV		(1<<10) // 1024
#define OPTION_NOPLSQL		(1<<15) // 32768

// data types
#define DATA_TYPE_STRING	0
#define DATA_TYPE_UB2ARRAY	1
#define DATA_TYPE_UB1		2
#define DATA_TYPE_UB2		3
#define DATA_TYPE_UB4		4
#define DATA_TYPE_VERSION	5
#define DATA_TYPE_STATUS	6

// column types
#define ORACLE_TYPE_VARCHAR		1
#define ORACLE_TYPE_NUMBER		2
#define ORACLE_TYPE_VARNUM		6
#define ORACLE_TYPE_LONG		8
#define ORACLE_TYPE_ROWID_DEPRECATED	11
#define ORACLE_TYPE_DATE		12
#define ORACLE_TYPE_RAW			23
#define ORACLE_TYPE_LONG_RAW		24
#define ORACLE_TYPE_CHAR		96
#define ORACLE_TYPE_RESULT_SET		102
#define ORACLE_TYPE_ROWID		104
#define ORACLE_TYPE_NAMED_TYPE		109
#define ORACLE_TYPE_REF_TYPE		111
#define ORACLE_TYPE_CLOB		112
#define ORACLE_TYPE_BLOB		113
#define ORACLE_TYPE_BFILE		114
#define ORACLE_TYPE_TIMESTAMP		180
#define ORACLE_TYPE_TIMESTAMPTZ		181
#define ORACLE_TYPE_INTERVALYM		182
#define ORACLE_TYPE_INTERVALDS		183
#define ORACLE_TYPE_TIMESTAMPLTZ	231
#define ORACLE_TYPE_PLSQL_INDEX_TABLE	998
#define ORACLE_TYPE_FIXED_CHAR		999


static uint16_t	oracletypemap[]={
	// "UNKNOWN"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// addded by freetds
	// "CHAR"
	(uint16_t)ORACLE_TYPE_CHAR,
	// "INT"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "SMALLINT"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "TINYINT"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "MONEY"
	(uint16_t)ORACLE_TYPE_VARNUM,
	// "DATETIME"
	(uint16_t)ORACLE_TYPE_TIMESTAMP,
	// "NUMERIC"
	(uint16_t)ORACLE_TYPE_VARNUM,
	// "DECIMAL"
	(uint16_t)ORACLE_TYPE_VARNUM,
	// "SMALLDATETIME"
	(uint16_t)ORACLE_TYPE_TIMESTAMP,
	// "SMALLMONEY"
	(uint16_t)ORACLE_TYPE_VARNUM,
	// "IMAGE"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "BINARY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "BIT"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "REAL"
	(uint16_t)ORACLE_TYPE_VARNUM,
	// "FLOAT"
	(uint16_t)ORACLE_TYPE_VARNUM,
	// "TEXT"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "VARCHAR"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "VARBINARY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "LONGCHAR"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "LONGBINARY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "LONG"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "ILLEGAL"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "SENSITIVITY"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "BOUNDARY"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "VOID"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "USHORT"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// added by lago
	// "UNDEFINED"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "DOUBLE"
	(uint16_t)ORACLE_TYPE_VARNUM,
	// "DATE"
	(uint16_t)ORACLE_TYPE_DATE,
	// "TIME"
	(uint16_t)ORACLE_TYPE_DATE,
	// "TIMESTAMP"
	(uint16_t)ORACLE_TYPE_TIMESTAMP,
	// added by msql
	// "UINT"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "LASTREAL"
	(uint16_t)ORACLE_TYPE_VARNUM,
	// added by oracle
	// "STRING"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "VARSTRING"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "LONGLONG"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "MEDIUMINT"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "YEAR"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "NEWDATE"
	(uint16_t)ORACLE_TYPE_DATE,
	// "NULL"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "ENUM"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "SET"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "TINYBLOB"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "MEDIUMBLOB"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "LONGBLOB"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "BLOB"
	(uint16_t)ORACLE_TYPE_BLOB,
	// added by oracle
	// "VARCHAR2"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "NUMBER"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "ROWID"
	(uint16_t)ORACLE_TYPE_ROWID,
	// "RAW"
	(uint16_t)ORACLE_TYPE_RAW,
	// "LONG_RAW"
	(uint16_t)ORACLE_TYPE_RAW,
	// "MLSLABEL"
	(uint16_t)ORACLE_TYPE_RAW,
	// "CLOB"
	(uint16_t)ORACLE_TYPE_CLOB,
	// "BFILE"
	(uint16_t)ORACLE_TYPE_BFILE,
	// added by odbc
	// "BIGINT"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "INTEGER"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "LONGVARBINARY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "LONGVARCHAR"
	(uint16_t)ORACLE_TYPE_BLOB,
	// added by db2
	// "GRAPHIC"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "VARGRAPHIC"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "LONGVARGRAPHIC"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "DBCLOB"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "DATALINK"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "USER_DEFINED_TYPE"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "SHORT_DATATYPE"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "TINY_DATATYPE"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// added by firebird
	// "D_FLOAT"
	(uint16_t)ORACLE_TYPE_VARNUM,
	// "ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "QUAD"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "INT64"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "DOUBLE PRECISION"
	(uint16_t)ORACLE_TYPE_VARNUM,
	// added by postgresql
	// "BOOL"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "BYTEA"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "NAME"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "INT8"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "INT2"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "INT2VECTOR"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "INT4"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "REGPROC"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "OID"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "TID"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "XID"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "CID"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "OIDVECTOR"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "SMGR"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "POINT"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "LSEG"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "PATH"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "BOX"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "POLYGON"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "LINE"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "LINE_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "FLOAT4"
	(uint16_t)ORACLE_TYPE_VARNUM,
	// "FLOAT8"
	(uint16_t)ORACLE_TYPE_VARNUM,
	// "ABSTIME"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "RELTIME"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "TINTERVAL"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "CIRCLE"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "CIRCLE_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "MONEY_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "MACADDR"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "INET"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "CIDR"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "BOOL_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "BYTEA_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "CHAR_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "NAME_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "INT2_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "INT2VECTOR_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "INT4_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "REGPROC_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "TEXT_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "OID_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "TID_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "XID_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "CID_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "OIDVECTOR_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "BPCHAR_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "VARCHAR_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "INT8_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "POINT_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "LSEG_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "PATH_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "BOX_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "FLOAT4_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "FLOAT8_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "ABSTIME_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "RELTIME_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "TINTERVAL_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "POLYGON_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "ACLITEM"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "ACLITEM_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "MACADDR_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "INET_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "CIDR_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "BPCHAR"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "TIMESTAMP_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "DATE_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "TIME_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "TIMESTAMPTZ"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "TIMESTAMPTZ_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "INTERVAL"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "INTERVAL_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "NUMERIC_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "TIMETZ"
	(uint16_t)ORACLE_TYPE_TIMESTAMPTZ,
	// "TIMETZ_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "BIT_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "VARBIT"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "VARBIT_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "REFCURSOR"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "REFCURSOR_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "REGPROCEDURE"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "REGOPER"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "REGOPERATOR"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "REGCLASS"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "REGTYPE"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "REGPROCEDURE_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "REGOPER_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "REGOPERATOR_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "REGCLASS_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "REGTYPE_ARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "RECORD"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "CSTRING"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "ANY"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "ANYARRAY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "TRIGGER"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "LANGUAGE_HANDLER"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "INTERNAL"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "OPAQUE"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "ANYELEMENT"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "PG_TYPE"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "PG_ATTRIBUTE"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "PG_PROC"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "PG_CLASS"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// none added by sqlite
	// added by sqlserver
	// "UBIGINT"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "UNIQUEIDENTIFIER"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// added by informix
	// "SMALLFLOAT"
	(uint16_t)ORACLE_TYPE_VARNUM,
	// "BYTE"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "BOOLEAN"
	(uint16_t)ORACLE_TYPE_NUMBER,
	// "TINYTEXT"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "MEDIUMTEXT"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "LONGTEXT"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "JSON"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "GEOMETRY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "SDO_GEOMETRY"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "NCHAR"
	(uint16_t)ORACLE_TYPE_CHAR,
	// "NVARCHAR"
	(uint16_t)ORACLE_TYPE_VARCHAR,
	// "NTEXT"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "XML"
	(uint16_t)ORACLE_TYPE_BLOB,
	// "DATETIMEOFFSET"
	(uint16_t)ORACLE_TYPE_TIMESTAMP
};

enum oraclelisttype_t {
	ORACLELISTTYPE_DATABASE_LIST=0,
	ORACLELISTTYPE_TABLE_LIST,
	ORACLELISTTYPE_COLUMN_LIST
};

class SQLRSERVER_DLLSPEC sqlrprotocol_oracle : public sqlrprotocol {
	public:
		sqlrprotocol_oracle(sqlrservercontroller *cont,
							domnode *parameters);
		virtual	~sqlrprotocol_oracle();

		clientsessionexitstatus_t	clientSession(
							filedescriptor *cs);

	private:
		void	init();
		void	free();
		void	reInit();

		void	resetSendPacketBuffer(byte_t packettype);
		bool	sendPacket();
		bool	sendPacket(bool flush);
		bool	recvPacket();
		void	readHost(const byte_t *rp,
					uint16_t *value,
					const byte_t **rpout);
		void	readHost(const byte_t *rp,
					uint32_t *value,
					const byte_t **rpout);
		bool	readMarker8(const byte_t *rp,
					byte_t expected,
					const byte_t **rpout);
		bool	readMarker16(const byte_t *rp,
					uint16_t expected,
					const byte_t **rpout);
		bool	readMarker32(const byte_t *rp,
					uint32_t expected,
					const byte_t **rpout);
		bool	getNullTerminatedArray(const byte_t *rp,
						const byte_t *end,
						byte_t **array,
						uint32_t *arraycount,
						const byte_t **rpout);
		bool	getString(const byte_t *rp,
						const byte_t *end,
						char **string,
						const byte_t **rpout);
		bool	getString(const byte_t *rp,
						char **string,
						uint32_t size,
						const byte_t **rpout);

		void	writeHost(bytebuffer *buffer, uint16_t value);
		void	writeHost(bytebuffer *buffer, uint32_t value);

		void	generateAuthSessionKey(uint16_t bytes);


		// handshake...
		bool	initialHandshake();
		bool	connect();
		bool	recvConnectRequest();
		bool	sendConnectResponse();
		bool	sendAccept();
		bool	sendAccept(const byte_t *data, uint16_t datasize);
		bool	sendResend();
		bool	sendRedirect();
		bool	sendRefuse();

		bool	anoNegotiation();
		bool	recvAnoRequest();
		bool	getAnoServiceHeader(const byte_t *rp,
						uint16_t *service,
						uint16_t *fieldcount,
						const byte_t **rpout);
		bool	getSupervisorService(const byte_t *rp,
						uint16_t fieldcount,
						const byte_t **rpout);
		bool	getAuthenticationService(const byte_t *rp,
						uint16_t fieldcount,
						const byte_t **rpout);
		bool	getEncryptionService(const byte_t *rp,
						uint16_t fieldcount,
						const byte_t **rpout);
		bool	getCryptoChecksummingService(const byte_t *rp,
						uint16_t fieldcount,
						const byte_t **rpout);
		bool	getAnoVersionField(const byte_t *rp,
						uint32_t *version,
						const byte_t **rpout);
		bool	getAnoConnectionInfoField(const byte_t *rp,
						uint32_t *pid,
						uint32_t *connectiontype,
						const byte_t **rpout);
		bool	getAnoArrayField(const byte_t *rp,
						uint16_t **array,
						uint32_t *arraycount,
						const byte_t **rpout);
		bool	getAnoConstantField(const byte_t *rp,
						uint16_t *constant,
						const byte_t **rpout);
		bool	getAnoConstantField(const byte_t *rp,
						byte_t *constant,
						const byte_t **rpout);
		bool	getAnoStatusField(const byte_t *rp,
						uint16_t *status,
						const byte_t **rpout);
		bool	getAnoUnknownField(const byte_t *rp,
						const byte_t **rpout);
		bool		sendAnoResponse();
		uint16_t	putSupervisorService();
		uint16_t	putAuthenticationService();
		uint16_t	putEncryptionService();
		uint16_t	putCryptoChecksummingService();
		uint16_t	putAnoServiceHeader(uint16_t type,
							uint16_t fieldcount);
		uint16_t	putAnoVersionField(uint32_t version);
		uint16_t	putAnoStatusField(uint16_t status);
		uint16_t	putAnoConstant(byte_t constant);
		uint16_t	putAnoArrayField(uint16_t *array,
							uint32_t arraycount);

		bool	ttiNegotiation();
		bool	recvTtiRequest();
		bool	sendTtiResponse();
		void	putTti6Response();
		void	putTti5Response();
		void	putTti4Response();
		void	putTti3Response();
		void	putTti2Response();
		void	putTti1Response();

		bool	dataTypeNegotiation();
		bool	recvDataTypeRequest();
		bool	sendDataTypeResponse();

		bool	authenticate();
		bool	recvAuthenticationRequest(bool secondphase);
		bool	sendAuthenticationChallenge();
		void	putAuthField(const char *fieldname, const char *field);
		bool	sendAuthenticationResponse();

		void	debugTtcCode(byte_t ttccode);
		void	debugTtiFunction(byte_t ttifunction);
		void	debugOptions(uint16_t options, uint16_t moreoptions);
		void	debugOptions(uint16_t options);
		void	debugCharacterSet(byte_t characterset);
		void	debugStatusFlags(uint16_t statusflags);
		void	debugColumnType(const char *name, uint16_t columntype);
		void	debugColumnType(uint16_t columntype);
		void	debugSystemError();

		bool	getTtiFunction(const byte_t *rp,
						byte_t *ttifunction,
						const byte_t **rpout);

		// open...
		bool	open(const byte_t *rp);
		bool	sendOpenResponse(sqlrservercursor *cursor);

		// query...
		bool	query(const byte_t *rp);
		bool	sendQueryResponse(sqlrservercursor *cursor);
		bool	query2(const byte_t *rp);
		bool	sendQuery2Response(sqlrservercursor *cursor,
							bool binds);
		bool	bindParameters(sqlrservercursor *cursor,
							uint16_t pcount,
							uint16_t *ptypes);
		bool	query3(const byte_t *rp);
		bool	sendQuery3Response(sqlrservercursor *cursor,
							uint16_t options);

		// execute...
		bool	execute(const byte_t *rp);
		bool	sendExecuteResponse(sqlrservercursor *cursor);

		// fetch...
		bool	fetch(const byte_t *rp);
		bool	sendFetchResponse(sqlrservercursor *cursor,
							bool parse,
							bool define,
							bool sndiov,
							bool exactfetch);
		void	cacheColumnDefinitions(sqlrservercursor *cursor,
							uint32_t colcount);
		void	putColumnDefinitions(sqlrservercursor *cursor,
							uint32_t colcount,
							bool query3);
		void	putColumnDefinition(sqlrservercursor *cursor,
							uint32_t column,
							bool query3);
		uint16_t	getColumnType(const char *columntypestring,
						uint16_t columntypesize,
						uint32_t scale);
		uint16_t	getColumnFlags(sqlrservercursor *cursor,
						uint32_t column,
						uint16_t sqlrcolumntype,
						uint16_t columntype,
						const char *columntypestring);
		uint16_t	getColumnFlags(sqlrservercursor *cursor,
						uint16_t sqlrcolumntype,
						uint16_t columntype,
						const char *columntypestring,
						bool isnullable,
						bool isprimarykey,
						bool isunique,
						bool ispartofkey,
						bool isunsigned,
						bool iszerofilled,
						bool isbinary,
						bool isautoincrement);
		void	putIov();
		void	putRow(sqlrservercursor *cursor,
						uint32_t colcount,
						bool terminator);
		void	putField(const char *field,
						uint64_t fieldsize,
						uint16_t columntype);
		void	putLobField(sqlrservercursor *cursor, uint32_t col);
		void	putError(const char *error);
		void	putError(const char *error, uint32_t errorsize);

		// close...
		bool	close(const byte_t *rp);
		void	clearParams(sqlrservercursor *cursor);
		bool	sendCloseResponse(sqlrservercursor *cursor);

		// disconnect...
		bool	disconnect(const byte_t *rp);
		bool	sendDisconnectResponse();

		// cancel...
		bool	cancel(const byte_t *rp);

		// version
		bool	version(const byte_t *rp);
		bool	sendVersionResponse();

		// occa
		bool	occa(const byte_t *rp, const byte_t **rpout);

		// logon unknown
		bool	logonUnknown(const byte_t *rp);
		bool	sendLogonUnknownResponse();

		void	putGenericFooter();

		bool	sendQueryError(sqlrservercursor *cursor);
		bool	sendCursorNotOpenError();
		bool	sendNotImplementedError();

		uint16_t	connectversion;
		uint16_t	connectlowestversion;
		uint16_t	gso;
		uint16_t	anoflags;

		uint16_t	sdu;
		uint16_t	tdu;

		uint32_t	anorequestversion;
		uint32_t	supervisorversion;
		uint32_t	authenticationversion;
		uint32_t	encryptionversion;
		uint32_t	cryptochecksummingversion;

		byte_t		*ttiversions;
		uint32_t	ttiversioncount;
		byte_t		ttiversion;

		char		*clientstring;
		const char	*serverstring;

		const byte_t	*datatypes;
		uint16_t	datatypessize;

		filedescriptor	*clientsock;

		bytebuffer	reqpacket;
		byte_t		reqpackettype;

		memorypool	*resppacketpool;
		byte_t		*resppacket;
		uint16_t	resppacketsize;
		byte_t		resppackettype;

		randomnumber	r;
		//uint32_t	seed;

		char		*username;
		char		*authsessionkey;
		char		*response;
		//uint64_t	responsesize;

		uint16_t	maxcursorcount;
		uint32_t	maxquerysize;
		uint16_t	maxbindcount;

		char		**bindvarnames;

		char		lobbuffer[32768];

		uint16_t	*pcounts;
		uint16_t	**ptypes;
		bool		*columntypescached;
		uint16_t	**columntypes;
};

sqlrprotocol_oracle::sqlrprotocol_oracle(sqlrservercontroller *cont,
					domnode *parameters) :
					sqlrprotocol(cont,parameters) {

	clientsock=NULL;

	if (getDebug()) {
		debugStart("parameters");
		debugEnd();
	}

	r.setSeed(randomnumber::getSeed());

	resppacketpool=new memorypool(1024,1024,10240);

	maxcursorcount=cont->getConfig()->getMaxCursors();
	maxquerysize=cont->getConfig()->getMaxQuerySize();
	maxbindcount=cont->getConfig()->getMaxBindCount();

	bindvarnames=new char *[maxbindcount];
	for (uint16_t i=0; i<maxbindcount; i++) {
		charstring::printf(&bindvarnames[i],":%d",i+1);
	}

	pcounts=new uint16_t[maxcursorcount];
	ptypes=new uint16_t *[maxcursorcount];
	columntypescached=new bool[maxcursorcount];
	columntypes=new uint16_t *[maxcursorcount];
	for (uint16_t i=0; i<maxcursorcount; i++) {
		pcounts[i]=0;
		ptypes[i]=new uint16_t[maxbindcount];
		columntypescached[i]=false;
		if (cont->getMaxColumnCount()) {
			columntypes[i]=new uint16_t[cont->getMaxColumnCount()];
		} else {
			columntypes[i]=NULL;
		}
	}

	init();
}

sqlrprotocol_oracle::~sqlrprotocol_oracle() {
	free();

	for (uint16_t i=0; i<maxbindcount; i++) {
		delete[] bindvarnames[i];
	}
	delete[] bindvarnames;

	for (uint16_t i=0; i<maxcursorcount; i++) {
		delete[] ptypes[i];
		delete[] columntypes[i];
	}
	delete[] pcounts;
	delete[] ptypes;
	delete[] columntypes;

	delete resppacketpool;
}

void sqlrprotocol_oracle::init() {

	sdu=4086;
	tdu=32767;

	connectversion=0;
	connectlowestversion=0;
	gso=0;
	anoflags=0;

	anorequestversion=0;
	supervisorversion=0;
	authenticationversion=0;
	encryptionversion=0;
	cryptochecksummingversion=0;

	ttiversions=NULL;
	ttiversioncount=0;
	ttiversion=0;

	clientstring=NULL;
	serverstring=NULL;

	datatypes=NULL;
	datatypessize=0;

	resppacket=NULL;
	resppacketsize=0;
	username=NULL;
	authsessionkey=NULL;
	response=NULL;
}

void sqlrprotocol_oracle::free() {

	delete[] ttiversions;
	ttiversions=NULL;

	delete[] username;
	delete[] authsessionkey;
	delete[] response;
	resppacketpool->clear();

	delete[] clientstring;
}

void sqlrprotocol_oracle::reInit() {
	free();
	init();
}

uint16_t hackcursorid=65535;

clientsessionexitstatus_t sqlrprotocol_oracle::clientSession(
						filedescriptor *cs) {

	clientsock=cs;

	// set up the socket
	clientsock->setNaglesAlgorithmEnabled(false);
	clientsock->setSocketReadBufferSize(65536);
	clientsock->setSocketWriteBufferSize(65536);
	clientsock->setReadBufferSize(65536);
	clientsock->setWriteBufferSize(65536);

	reInit();

	bool	endsession=true;

	clientsessionexitstatus_t	status=CLIENTSESSIONEXITSTATUS_ERROR;
	if (initialHandshake()) {

		// loop, getting and executing commands
		bool		loop=true;
		const byte_t	*rp=NULL;
		do {

			// get the tti function...
			byte_t		ttifunction;
			if (!getTtiFunction(rp,&ttifunction,&rp)) {
				break;
			}

			// do the appropriate thing...
			switch (ttifunction) {
				case TTI_OPEN:
					loop=open(rp);
					rp=NULL;
					break;
				case TTI_QUERY:
					loop=query(rp);
					rp=NULL;
					break;
				case TTI_QUERY2:
					loop=query2(rp);
					rp=NULL;
					break;
				case TTI_QUERY3:
					loop=query3(rp);
					rp=NULL;
					break;
				case TTI_EXECUTE:
					loop=execute(rp);
					rp=NULL;
					break;
				case TTI_FETCH:
					loop=fetch(rp);
					rp=NULL;
					break;
				case TTI_CLOSE:
				case TTI_CLOSE2:
					loop=close(rp);
					rp=NULL;
					break;
				case TTI_DISCONNECT:
					loop=disconnect(rp);
					rp=NULL;
					break;
				case TTI_AUTOCOMMIT_ON:
					loop=false;
					break;
				case TTI_AUTOCOMMIT_OFF:
					loop=false;
					break;
				case TTI_COMMIT:
					loop=false;
					break;
				case TTI_ROLLBACK:
					loop=false;
					break;
				case TTI_CANCEL:
					loop=false;
					break;
				case TTI_DESCRIBE:
				case TTI_DESCRIBE2:
					loop=false;
					break;
				case TTI_STARTUP:
				case TTI_STARTUP2:
					loop=false;
					break;
				case TTI_SHUTDOWN:
					loop=false;
					break;
				case TTI_VERSION:
					loop=version(rp);
					rp=NULL;
					break;
				case TTI_K2_TRANSACTIONS:
					loop=false;
					break;
				case TTI_OSQL7:
					loop=false;
					break;
				case TTI_OKOD:
					loop=false;
					break;
				case TTI_LOB_OPERATIONS:
					loop=false;
					break;
				case TTI_ODNY:
					loop=false;
					break;
				case TTI_TRANSACTION_END:
					loop=false;
					break;
				case TTI_TRANSACTION_BEGIN:
					loop=false;
					break;
				case TTI_OCCA:
					loop=occa(rp,&rp);
					break;
				case TTI_LOGON_PRESENT_PWD:
					loop=false;
					break;
				case TTI_LOGON_PRESENT_USER:
					loop=false;
					break;
				case TTI_LOGON_UNKNOWN:
					loop=logonUnknown(rp);
					rp=NULL;
					break;
				case TTI_LOGON_PRESENT_PWD_SEND_AUTH_PASSWORD:
					loop=false;
					break;
				case TTI_LOGON_PRESENT_USER_REQ_AUTH_SESSKEY:
					loop=false;
					break;
				case TTI_OOTCM:
					loop=false;
					break;
				case TTI_OKPFC:
					loop=false;
					break;
				case TTI_SWITCH_SESSION:
					// FIXME: 8.0.5 uses this to get the
					// server version for some reason...
					loop=version(rp);
					rp=NULL;
					break;
				case TTI_OSCID:
					loop=false;
					break;
				case TTI_OSKEYVAL:
					loop=false;
					break;
				default:
					// FIXME: bad options...
					loop=false;
					break;
			}

			// release the cursor
			// FIXME: kludgy
			//cont->release(cursor);

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

void sqlrprotocol_oracle::resetSendPacketBuffer(byte_t packettype) {
	reqpacket.clear();
	reqpacket.append((uint64_t)0);
	reqpackettype=packettype;
}

bool sqlrprotocol_oracle::sendPacket() {
	return sendPacket(false);
}

bool sqlrprotocol_oracle::sendPacket(bool flush) {

	uint16_t	reqpacketsize=(uint16_t)reqpacket.getSize();
	uint16_t	packetchecksum=0;
	byte_t		packetflags=0;
	uint16_t	headerchecksum=0;

	// overwrite the first 8 bytes of the reqpacket with the packet header
	reqpacket.setPositionRelativeToBeginning(0);
	reqpacket.write(hostToBE(reqpacketsize));
	reqpacket.write(hostToBE(packetchecksum));
	reqpacket.write(reqpackettype);
	reqpacket.write(packetflags);
	reqpacket.write(hostToBE(headerchecksum));

	if (getDebug()) {
		debugStart("send");
		debugWrite("packet size: %d",reqpacketsize);
		debugWrite("packet checksum: %d",packetchecksum);
		debugWrite("packet type: %d",reqpackettype);
		debugWrite("packet flags: 0x%04x",packetflags);
		debugWrite("header checksum: %d",headerchecksum);
		debugHexDump(reqpacket.getBuffer()+8,reqpacketsize-8);
		debugEnd();
	}

	// send the packet
	if (clientsock->write(reqpacket.getBuffer(),
				reqpacket.getSize())!=
				(ssize_t)reqpacket.getSize()) {
		debugWrite("write packet data failed");
		debugSystemError();
		return false;
	}

	if (flush) {
		clientsock->flushWriteBuffer(-1,-1);
		debugWrite("send packet flush...");
	} else {
		debugWrite("no flush...");
	}

	return true;
}

bool sqlrprotocol_oracle::recvPacket() {

	// size
	// 2 bytes (big endian)
	if (clientsock->read(&resppacketsize)!=sizeof(uint16_t)) {
		debugWrite("read packet size failed");
		debugSystemError();
		return false;
	}
	resppacketsize=beToHost(resppacketsize);

	// sanity check
	if (resppacketsize<8 || resppacketsize>sdu) {
		debugWrite("invalid packet size: %d",resppacketsize);
		debugSystemError();
		return false;
	}

	// packet checksum
	// 2 bytes (big endian) (always 0)
	uint16_t	packetchecksum;
	if (clientsock->read(&packetchecksum)!=sizeof(uint16_t)) {
		debugWrite("read packet checksum failed");
		debugSystemError();
		return false;
	}
	packetchecksum=beToHost(packetchecksum);

	// packet type
	// 1 byte
	if (clientsock->read(&resppackettype)!=sizeof(byte_t)) {
		debugWrite("read packet type failed");
		debugSystemError();
		return false;
	}

	// packet flags
	// 1 byte
	byte_t	packetflags;
	if (clientsock->read(&packetflags)!=sizeof(byte_t)) {
		debugWrite("read packet flags failed");
		debugSystemError();
		return false;
	}

	// header checksum
	// 2 bytes (big endian) (always 0)
	uint16_t	headerchecksum;
	if (clientsock->read(&headerchecksum)!=sizeof(uint16_t)) {
		debugWrite("read header checksum failed");
		debugSystemError();
		return false;
	}
	headerchecksum=beToHost(headerchecksum);

	// we've already received 8 bytes...
	resppacketsize-=8;

	// reallocate recv buffer
	resppacketpool->clear();
	resppacket=resppacketpool->allocate(resppacketsize);

	// packet
	if (clientsock->read(resppacket,resppacketsize)!=resppacketsize) {
		debugWrite("read packet failed");
		debugSystemError();
		return false;
	}

	if (getDebug()) {
		debugStart("recv");
		debugWrite("packet size: %d",resppacketsize+8);
		debugWrite("packet checksum: %d",packetchecksum);
		debugWrite("packet type: %d",resppackettype);
		debugWrite("packet flags: %d",packetflags);
		debugWrite("header checksum: %d",headerchecksum);
		debugHexDump(resppacket,resppacketsize);
		debugEnd();
	}

	return true;
}

void sqlrprotocol_oracle::readHost(const byte_t *rp,
					uint16_t *value,
					const byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint16_t));
	*rpout=rp+sizeof(uint16_t);
}

void sqlrprotocol_oracle::readHost(const byte_t *rp,
					uint32_t *value,
					const byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint32_t));
	*rpout=rp+sizeof(uint32_t);
}

bool sqlrprotocol_oracle::readMarker8(const byte_t *rp,
					byte_t expected,
					const byte_t **rpout) {
	byte_t	marker;
	return read(rp,&marker,"marker",expected,rpout);
}

bool sqlrprotocol_oracle::readMarker16(const byte_t *rp,
					uint16_t expected,
					const byte_t **rpout) {
	uint16_t	marker;
	return readBE(rp,&marker,"marker",expected,rpout);
}

bool sqlrprotocol_oracle::readMarker32(const byte_t *rp,
					uint32_t expected,
					const byte_t **rpout) {
	uint32_t	marker;
	return readBE(rp,&marker,"marker",expected,rpout);
}

bool sqlrprotocol_oracle::getNullTerminatedArray(const byte_t *rp,
						const byte_t *end,
						byte_t **array,
						uint32_t *arraycount,
						const byte_t **rpout) {
	*array=NULL;
	*arraycount=0;

	// count the array items
	const byte_t	*start=rp;
	for (;;) {
		if (!(*rp)) {
			break;
		}
		if (rp==end) {
			debugWrite("bad null terminated array, "
					"no null terminator found");
			return false;
		}
		(*arraycount)++;
		rp++;
	}

	// reset rp
	rp=start;

	// get the array items
	*array=new byte_t[*arraycount];
	for (uint32_t i=0; i<*arraycount; i++) {
		read(rp,&((*array)[i]),&rp);
	}

	// skip the null terminator
	rp++;
	
	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getString(const byte_t *rp,
					const byte_t *end,
					char **string,
					const byte_t **rpout) {
	*string=NULL;
	uint32_t	stringsize=0;

	// count the bytes
	const byte_t	*start=rp;
	for (;;) {
		if (!(*rp)) {
			break;
		}
		if (rp==end) {
			debugWrite("bad string, "
					"no null terminator found");
			return false;
		}
		stringsize++;
		rp++;
	}

	// reset rp
	rp=start;

	// get the string
	*string=new char[stringsize+1];
	for (uint32_t i=0; i<stringsize; i++) {
		read(rp,&((*string)[i]),&rp);
	}
	(*string)[stringsize]='\0';

	// skip the null terminator
	rp++;
	
	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getString(const byte_t *rp,
					char **string,
					uint32_t size,
					const byte_t **rpout) {
	*string=charstring::duplicate((const char *)rp,size);
	*rpout=rp+size;
	return true;
}

void sqlrprotocol_oracle::writeHost(bytebuffer *buffer, uint16_t value) {
	buffer->append(value);
}

void sqlrprotocol_oracle::writeHost(bytebuffer *buffer, uint32_t value) {
	buffer->append(value);
}

void sqlrprotocol_oracle::generateAuthSessionKey(uint16_t bytes) {

	// random bytes
	stringbuffer	str;
	uint32_t	number;
	for (uint16_t i=0; i<bytes; i++) {
		r.generate(&number);
		int32_t	scalednumber=randomnumber::scale(number,0,16);
		str.append((char)(scalednumber+((scalednumber<10)?'0':'A'-10)));
	}
	delete[] authsessionkey;
	authsessionkey=str.detachString();
}

bool sqlrprotocol_oracle::initialHandshake() {
	return connect() &&
		anoNegotiation() &&
		ttiNegotiation() &&
		dataTypeNegotiation() &&
		authenticate();
}

bool sqlrprotocol_oracle::connect() {
	return recvConnectRequest() &&
		// the database always requests a resend here, for some reason
		sendResend() &&
		recvConnectRequest() &&
		sendConnectResponse();
}

bool sqlrprotocol_oracle::recvConnectRequest() {

	if (!recvPacket()) {
		return false;
	}

	if (resppackettype!=PACKET_CONNECT) {
		debugWrite("bad packet type %d, expected %d",
					resppackettype,PACKET_CONNECT);
		return false;
	}

	const byte_t	*rp=resppacket;

	uint16_t	protocolcharacteristics;
	uint16_t	maxpacketsbeforeack;
	uint16_t	one;
	uint16_t	connectdatasize;
	uint16_t	connectdataoffset;
	uint32_t	maxconnectdatathatcanbereceived;
	uint32_t	tracecrossfacilityitem1;
	uint32_t	tracecrossfacilityitem2;
	uint64_t	traceuniqueconnectionid1;
	uint64_t	traceuniqueconnectionid2;

	readBE(rp,&connectversion,&rp);
	readBE(rp,&connectlowestversion,&rp);
	readBE(rp,&gso,&rp);
	readBE(rp,&sdu,&rp);
	readBE(rp,&tdu,&rp);
	readBE(rp,&protocolcharacteristics,&rp);
	readBE(rp,&maxpacketsbeforeack,&rp);
	readHost(rp,&one,&rp);
	readBE(rp,&connectdatasize,&rp);
	readBE(rp,&connectdataoffset,&rp);
	readBE(rp,&maxconnectdatathatcanbereceived,&rp);
	readBE(rp,&anoflags,&rp);
	readBE(rp,&tracecrossfacilityitem1,&rp);
	readBE(rp,&tracecrossfacilityitem2,&rp);
	readBE(rp,&traceuniqueconnectionid1,&rp);
	readBE(rp,&traceuniqueconnectionid2,&rp);

	// connect data
	const char	*connectdata=
			(const char *)resppacket+(connectdataoffset-8);

	debugStart("connect");
	debugWrite("version: 0x%04x",connectversion);
	debugWrite("lowest supported version: 0x%04x",connectlowestversion);
	debugWrite("gso: 0x%04x",gso);
	debugWrite("sdu: %d",sdu);
	debugWrite("tdu: %d",tdu);
	debugWrite("protocol characteristics: 0x%04x",protocolcharacteristics);
	debugWrite("max packets before ack: %d",maxpacketsbeforeack);
	debugWrite("client is little endian: %d",(one==1));
	debugWrite("connect data size: %d",connectdatasize);
	debugWrite("connect data offset: %d",connectdataoffset);
	debugWrite("max connect data that can be received: %d",
					maxconnectdatathatcanbereceived);
	debugWrite("ANO flags: 0x%04x",anoflags);
	debugWrite("trace cross facility item 1: 0x%08x",
					tracecrossfacilityitem1);
	debugWrite("trace cross facility item 2: 0x%08x",
					tracecrossfacilityitem2);
	debugWrite("trace unique connection id 1: 0x%016x",
					traceuniqueconnectionid1);
	debugWrite("trace unique connection id 2: 0x%016x",
					traceuniqueconnectionid2);
	debugWrite("connect data: %*s",connectdatasize,connectdata);
	debugEnd();

	return true;
}

bool sqlrprotocol_oracle::sendConnectResponse() {

	// look for a protocol version that we support
	// FIXME: we currently only support PROTOCOL_VERSION_8
	if (connectlowestversion<=PROTOCOL_VERSION_8) {
		connectversion=PROTOCOL_VERSION_8;
	} else {
		debugWrite("no supported connect protocol version found");
		sendRefuse();
		return false;
	}

	return sendAccept();
}

bool sqlrprotocol_oracle::sendAccept() {
	return sendAccept(NULL,0);
}

bool sqlrprotocol_oracle::sendAccept(const byte_t *data, uint16_t datasize) {

	// data offset:
	// (8 bytes for the header +
	//  16 bytes of this stuff +
	//  8 bytes of padding)
	uint16_t	dataoffset=32;
	// NOTE: 0x0801 returned by 8.0 server
	//gso=0x0801;
	uint64_t	padding=0;

	// debug
	debugStart("accept");
	debugWrite("version: 0x%04x",connectversion);
	debugWrite("gso: 0x%04x",gso);
	debugWrite("sdu: %d",sdu);
	debugWrite("tdu: %d",tdu);
	debugWrite("data size: %d",datasize);
	debugWrite("data offset: %d",dataoffset);
	debugWrite("ANO flags: 0x%04x",anoflags);
	debugHexDump(data,datasize);
	debugEnd();

	// build packet
	resetSendPacketBuffer(PACKET_ACCEPT);
	writeBE(&reqpacket,connectversion);
	writeBE(&reqpacket,gso);
	writeBE(&reqpacket,sdu);
	writeBE(&reqpacket,tdu);
	// writeLE?
	writeHost(&reqpacket,(uint16_t)1);
	writeBE(&reqpacket,datasize);
	writeBE(&reqpacket,dataoffset);
	writeBE(&reqpacket,anoflags);
	writeBE(&reqpacket,padding);
	reqpacket.append(data,datasize);

	return sendPacket(true);
}

bool sqlrprotocol_oracle::sendResend() {

	// debug
	debugStart("resend");
	debugEnd();

	// build packet
	resetSendPacketBuffer(PACKET_RESEND);

	return sendPacket(true);
}

bool sqlrprotocol_oracle::sendRedirect() {

	// debug
	debugStart("redirect");
	debugEnd();

	// build packet
	resetSendPacketBuffer(PACKET_REDIRECT);
	// FIXME: implement this...

	return sendPacket(true);
}

bool sqlrprotocol_oracle::sendRefuse() {

	// debug
	debugStart("refuse");
	debugEnd();

	// build packet
	resetSendPacketBuffer(PACKET_REFUSE);
	// FIXME: implement this...

	return sendPacket(true);
}

bool sqlrprotocol_oracle::anoNegotiation() {
	return recvAnoRequest() && sendAnoResponse();
}

bool sqlrprotocol_oracle::recvAnoRequest() {

	if (!recvPacket()) {
		return false;
	}

	if (resppackettype!=PACKET_DATA) {
		debugWrite("bad packet type %d, expected %d",
						resppackettype,PACKET_DATA);
		return false;
	}

	const byte_t	*rp=resppacket;
	//const byte_t	*end=rp+resppacketsize;

	// ano request header...
	uint16_t	dataflags;
	uint16_t	overallsize;
	uint16_t	servicecount;
	byte_t		desiredoptionsflag;

	readBE(rp,&dataflags,&rp);
	if (!readMarker32(rp,0xdeadbeef,&rp)) {
		return false;
	}
	readBE(rp,&overallsize,&rp);
	readBE(rp,&anorequestversion,&rp);
	readBE(rp,&servicecount,&rp);
	read(rp,&desiredoptionsflag,&rp);

	debugStart("ano request header");
	debugWrite("data flags: 0x%04x",dataflags);
	debugWrite("overall size: %d",overallsize);
	debugWrite("version: 0x%08x",anorequestversion);
	debugWrite("service count %d",servicecount);
	debugWrite("desired options flag: 0x%02x",desiredoptionsflag);

	// service count ...
	bool	success=true;
	for (uint16_t i=0; i<servicecount; i++) {

		// reset success flag...
		success=false;

		uint16_t	service;
		uint16_t	fieldcount;
		if (!getAnoServiceHeader(rp,&service,&fieldcount,&rp)) {
			break;
		}

		switch (service) {
			case 4:
				success=getSupervisorService(
							rp,fieldcount,&rp);
				break;
			case 1:
				success=getAuthenticationService(
							rp,fieldcount,&rp);
				break;
			case 2:
				success=getEncryptionService(
							rp,fieldcount,&rp);
				break;
			case 3:
				success=getCryptoChecksummingService(
							rp,fieldcount,&rp);
				break;
			default:
				debugWrite("bad ano service: %d",service);
				break;
		}
		if (!success) {
			break;
		}
	}

	debugEnd();

	// bail if something failed
	if (!success) {
		return false;
	}

	return true;
}

bool sqlrprotocol_oracle::getAnoServiceHeader(const byte_t *rp,
						uint16_t *service,
						uint16_t *fieldcount,
						const byte_t **rpout) {

	readBE(rp,service,&rp);
	readBE(rp,fieldcount,&rp);
	if (!readMarker32(rp,0x00000000,&rp)) {
		return false;
	}

	debugStart("ano service header");
	debugWrite("service: %d",*service);
	debugWrite("field count: %d",*fieldcount);
	debugEnd();

	*rpout=rp;

	return true;
}


bool sqlrprotocol_oracle::getSupervisorService(const byte_t *rp,
						uint16_t fieldcount,
						const byte_t **rpout) {

	debugStart("supervisor");

	uint32_t	pid;
	uint32_t	connectiontype;
	uint16_t	*drivers=NULL;
	uint32_t	drivercount;
	if (!getAnoVersionField(rp,&supervisorversion,&rp) ||
		!getAnoConnectionInfoField(rp,&pid,&connectiontype,&rp) ||
		!getAnoArrayField(rp,&drivers,&drivercount,&rp)) {
		delete[] drivers;
		debugEnd();
		return false;
	}

	delete[] drivers;

	debugEnd();

	*rpout=rp;
	
	return true;
}

bool sqlrprotocol_oracle::getAuthenticationService(const byte_t *rp,
						uint16_t fieldcount,
						const byte_t **rpout) {

	debugStart("authentication");

	uint16_t	constant;
	uint16_t	status;
	if (!getAnoVersionField(rp,&authenticationversion,&rp) ||
		!getAnoConstantField(rp,&constant,&rp) ||
		!getAnoStatusField(rp,&status,&rp)) {
		debugEnd();
		return false;
	}

	debugEnd();

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getEncryptionService(const byte_t *rp,
						uint16_t fieldcount,
						const byte_t **rpout) {

	debugWrite("encryption");

	uint16_t	*drivers=NULL;
	uint32_t	drivercount;
	byte_t		constant;
	if (!getAnoVersionField(rp,&encryptionversion,&rp) ||
		!getAnoArrayField(rp,&drivers,&drivercount,&rp)) {
		delete[] drivers;
		debugEnd();
		return false;
	}
	if (fieldcount>2) {
		if (!getAnoConstantField(rp,&constant,&rp)) {
			delete[] drivers;
			debugEnd();
			return false;
		} 
	}

	delete[] drivers;

	debugEnd();

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getCryptoChecksummingService(
						const byte_t *rp,
						uint16_t fieldcount,
						const byte_t **rpout) {

	debugStart("crypto-checksumming");

	uint16_t	*drivers=NULL;
	uint32_t	drivercount;
	if (!getAnoVersionField(rp,&cryptochecksummingversion,&rp) ||
		!getAnoArrayField(rp,&drivers,&drivercount,&rp)) {
		delete[] drivers;
		debugEnd();
		return false;
	}

	delete[] drivers;

	debugEnd();

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getAnoVersionField(const byte_t *rp,
						uint32_t *version,
						const byte_t **rpout) {
	uint16_t	size;
	uint16_t	type;
	if (!readBE(rp,&size,"size",4,&rp) ||
		!readBE(rp,&type,"type",5,&rp)) {
		return false;
	}
	readBE(rp,version,&rp);

	if (getDebug()) {
		debugWrite("version: 0x%08x",*version);
		// 8.0 -> 10g send a version string, 11i+ sends all 0's
	}

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getAnoConnectionInfoField(
						const byte_t *rp,
						uint32_t *pid,
						uint32_t *connectiontype,
						const byte_t **rpout) {
	uint16_t	size;
	uint16_t	type;
	if (!readBE(rp,&size,"size",8,&rp) ||
		!readBE(rp,&type,"type",1,&rp)) {
		return false;
	}
	readBE(rp,pid,&rp);
	readBE(rp,connectiontype,&rp);

	// NOTE: We consistently get 0x1788dda1 or 0x1784574b for the
	// connection type, but 8.0.5 (at least) consistently sends 0x1784574b
	// to the real db.

	if (getDebug()) {
		debugWrite("pid: %d",*pid);
		debugWrite("connection type: 0x%08x",*connectiontype);
	}

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getAnoArrayField(const byte_t *rp,
						uint16_t **array,
						uint32_t *arraycount,
						const byte_t **rpout) {

	*array=NULL;
	*arraycount=0;

	// get the total field size
	uint16_t	size;
	readBE(rp,&size,&rp);

	// get the field type, should be 1 (UB2Array)
	uint16_t	type;
	if (!readBE(rp,&type,"type",1,&rp)) {
		return false;
	}

	// if size was 1, then there is just a null terminator
	// skip it and bail
	if (size==1) {
		*rpout=rp+1;
		return true;
	}

	// look for a deadbeef marker, followed by an array marker
	if (!readMarker32(rp,0xdeadbeef,&rp) ||
		!readMarker16(rp,0x0003,&rp)) {

		// FIXME: Sometimes there's no deadbeef marker, just an array
		// marker, and the next 4 bytes are definitely NOT the array
		// size.  Other times there's no deadbeef marker, and no
		// array marker anywhere in the rest of the field.  In both
		// cases, the rest of the field contains data, but I have no
		// idea how to interpret it.
		//
		// For now, we'll just return NULL/0 but we won't fail outright.
		*rpout=rp+size;
		return true;
	}
	
	// get the array count
	readBE(rp,arraycount,&rp);

	debugWrite("array count: %d",*arraycount);

	// FIXME: sanity check on array count

	// get the array members
	if (*arraycount) {
		*array=new uint16_t[*arraycount];
		for (uint32_t i=0; i<*arraycount; i++) {
			readBE(rp,&((*array)[i]),&rp);
			debugWrite("array[%d]: %d",i,(*array)[i]);
		}
	}

	*rpout=rp;

	return true;
}


bool sqlrprotocol_oracle::getAnoConstantField(const byte_t *rp,
						uint16_t *constant,
						const byte_t **rpout) {
	uint16_t	size;
	uint16_t	type;
	if (!readBE(rp,&size,"size",2,&rp) ||
		!readBE(rp,&type,"type",3,&rp)) {
		return false;
	}
	readBE(rp,constant,&rp);

	debugWrite("constant: 0x%04x",*constant);

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getAnoConstantField(const byte_t *rp,
						byte_t *constant,
						const byte_t **rpout) {
	uint16_t	size;
	uint16_t	type;
	if (!readBE(rp,&size,"size",1,&rp) ||
		!readBE(rp,&type,"type",2,&rp)) {
		return false;
	}
	read(rp,constant,&rp);

	debugWrite("constant: 0x%02x",*constant);

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getAnoStatusField(const byte_t *rp,
						uint16_t *status,
						const byte_t **rpout) {
	uint16_t	size;
	uint16_t	type;
	if (!readBE(rp,&size,"size",2,&rp) ||
		!readBE(rp,&type,"type",6,&rp)) {
		return false;
	}
	readBE(rp,status,&rp);

	debugWrite("status: 0x%04x",*status);

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getAnoUnknownField(const byte_t *rp,
						const byte_t **rpout) {
	uint16_t	size;
	uint16_t	type;
	readBE(rp,&size,&rp);
	readBE(rp,&type,&rp);
	rp+=size;

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::sendAnoResponse() {

	resetSendPacketBuffer(PACKET_DATA);

	// ano response header...
	uint16_t	dataflags=0;
	uint16_t	overallsize=13;
	uint64_t	overallsizepos=0;
	uint32_t	version=anorequestversion;
	uint16_t	servicecount=0;
	uint64_t	servicecountpos=0;
	byte_t		servicestobeused=0;

	writeBE(&reqpacket,dataflags);
	writeBE(&reqpacket,(uint32_t)0xdeadbeef);
	overallsizepos=reqpacket.getPosition();
	writeBE(&reqpacket,overallsize);
	writeBE(&reqpacket,version);
	servicecountpos=reqpacket.getPosition();
	writeBE(&reqpacket,servicecount);
	write(&reqpacket,servicestobeused);

	debugStart("ano response header");
	debugWrite("data flags: 0x%04x",dataflags);
	debugWrite("version: 0x%08x",version);
	debugWrite("services to be used: %d",servicestobeused);


	// services...
	overallsize+=putSupervisorService();
	servicecount++;
	overallsize+=putAuthenticationService();
	servicecount++;
	overallsize+=putEncryptionService();
	servicecount++;
	overallsize+=putCryptoChecksummingService();
	servicecount++;


	// backpatch the overall size and servicecount
	reqpacket.setPositionRelativeToBeginning(overallsizepos);
	reqpacket.write(hostToBE(overallsize));
	reqpacket.setPositionRelativeToBeginning(servicecountpos);
	reqpacket.write(hostToBE(servicecount));

	debugWrite("overall size: %d",overallsize);
	debugWrite("service count: %d",servicecount);
	debugEnd();

	return sendPacket(true);
}

uint16_t sqlrprotocol_oracle::putSupervisorService() {

	debugStart("supervisor");

	uint16_t drivers[]={0x0004,0x0001};

	uint16_t	size=putAnoServiceHeader(4,3)+
				putAnoVersionField(supervisorversion)+
				putAnoStatusField(0x001f)+
				putAnoArrayField(drivers,2);

	debugEnd();
	return size;
}

uint16_t sqlrprotocol_oracle::putAuthenticationService() {

	debugStart("authentication");

	uint16_t	size=putAnoServiceHeader(1,2)+
				putAnoVersionField(authenticationversion)+
				putAnoStatusField(0xfbff);

	debugEnd();
	return size;
}

uint16_t sqlrprotocol_oracle::putEncryptionService() {

	debugStart("encryption");

	uint16_t	size=putAnoServiceHeader(2,2)+
				putAnoVersionField(encryptionversion)+
				putAnoConstant((byte_t)0);

	debugEnd();
	return size;
}

uint16_t sqlrprotocol_oracle::putCryptoChecksummingService() {

	debugStart("crypto-checksumming");

	uint16_t	size=putAnoServiceHeader(3,2)+
				putAnoVersionField(cryptochecksummingversion)+
				putAnoConstant((byte_t)0);

	debugEnd();
	return size;
}

uint16_t sqlrprotocol_oracle::putAnoServiceHeader(uint16_t service,
							uint16_t fieldcount) {

	debugStart("ano service header");
	debugWrite("service: %d",service);
	debugWrite("field count: %d",fieldcount);
	debugEnd();

	// service, field count, marker, return total size
	writeBE(&reqpacket,service);
	writeBE(&reqpacket,fieldcount);
	// FIXME; send something other than 0x00000000 if there was an error...
	writeBE(&reqpacket,(uint32_t)0x00000000);
	return 8;
}

uint16_t sqlrprotocol_oracle::putAnoVersionField(uint32_t version) {

	debugWrite("version: 0x%08x",version);

	// data size, field type, version, return total size
	writeBE(&reqpacket,(uint16_t)4);
	writeBE(&reqpacket,(uint16_t)5);
	writeBE(&reqpacket,version);
	return 8;
}

uint16_t sqlrprotocol_oracle::putAnoStatusField(uint16_t status) {

	debugWrite("status: 0x%04x",status);

	// data size, field type, status, return total size
	writeBE(&reqpacket,(uint16_t)2);
	writeBE(&reqpacket,(uint16_t)6);
	writeBE(&reqpacket,status);
	return 6;
}

uint16_t sqlrprotocol_oracle::putAnoConstant(byte_t constant) {

	debugWrite("constant: 0x%02x",constant);

	// data size, field type, constant, return total size
	writeBE(&reqpacket,(uint16_t)1);
	writeBE(&reqpacket,(uint16_t)2);
	write(&reqpacket,constant);
	return 5;
}

uint16_t sqlrprotocol_oracle::putAnoArrayField(uint16_t *array,
						uint32_t arraycount) {

	// data size, field type
	uint16_t datasize=((arraycount)?(4+2+4+arraycount*2):1);
	writeBE(&reqpacket,(uint16_t)((arraycount)?(4+2+4+arraycount*2):1));
	writeBE(&reqpacket,(uint16_t)1);

	debugWrite("arraycount: %d",arraycount);

	if (arraycount) {

		// deadbeef marker, array marker,
		// array count, array members
		writeBE(&reqpacket,(uint32_t)0xdeadbeef);
		writeBE(&reqpacket,(uint16_t)0x003);
		writeBE(&reqpacket,arraycount);
		for (uint32_t i=0; i<arraycount; i++) {
			debugWrite("array[%d]: %d",i,array[i]);
			writeBE(&reqpacket,array[i]);
		}

	} else {
		// null terminator
		write(&reqpacket,(byte_t)0x00);
	}

	// return total size
	return 4+datasize;
}

bool sqlrprotocol_oracle::ttiNegotiation() {
	return recvTtiRequest() && sendTtiResponse();
}

bool sqlrprotocol_oracle::recvTtiRequest() {

	if (!recvPacket()) {
		return false;
	}

	if (resppackettype!=PACKET_DATA) {
		debugWrite("bad packet type %d, expected %d",
						resppackettype,PACKET_DATA);
		return false;
	}

	const byte_t	*rp=resppacket;
	const byte_t	*end=rp+resppacketsize;

	uint16_t	dataflags;
	byte_t		ttccode;
	delete[] clientstring;

	readBE(rp,&dataflags,&rp);
	if (!read(rp,&ttccode,"ttccode",TTC_PROTOCOL_NEGOTIATION,&rp) ||
		!getNullTerminatedArray(rp,end,
					&ttiversions,
					&ttiversioncount,&rp) ||
		!getString(rp,end,&clientstring,&rp)) {
		return false;
	}

	if (getDebug()) {
		debugStart("tti request");
		debugWrite("data flags: 0x%04x",dataflags);
		debugTtcCode(ttccode);
		for (uint32_t i=0; i<ttiversioncount; i++) {
			debugWrite("version[%d]: %d",i,ttiversions[i]);
		}
		debugWrite("client string: \"%s\"",clientstring);
		debugEnd();
	}

	return true;
}

bool sqlrprotocol_oracle::sendTtiResponse() {

	// look for a ttiversion that we support
	// FIXME: we currently only support TTI 5
	ttiversion=0;
	for (uint32_t i=0; i<ttiversioncount; i++) {
		if (ttiversions[i]==5) {
			ttiversion=ttiversions[i];
			break;
		}
	}
	if (!ttiversion) {
		debugWrite("no supported tti protocol version found");
		// FIXME: send refuse?
		return false;
	}

	resetSendPacketBuffer(PACKET_DATA);

	switch (ttiversion) {
		case 6:
			putTti6Response();
			break;
		case 5:
			putTti5Response();
			break;
		case 4:
			putTti4Response();
			break;
		case 3:
			putTti3Response();
			break;
		case 2:
			putTti2Response();
			break;
		case 1:
			putTti1Response();
			break;
	}

	return sendPacket(true);
}

void sqlrprotocol_oracle::putTti6Response() {

	// oracle 8i+ supports TTI 6 (and lower)
	// FIXME: implement this...
}

void sqlrprotocol_oracle::putTti5Response() {

	// oracle 8.0 supports TTI 5 (and lower)

	uint16_t	dataflags=0;
	byte_t		ttccode=TTC_PROTOCOL_NEGOTIATION;

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);

	// protocol version and server flags...
	//
	// sent by:
	// * 8.0.5 server on redhat 5.2 x86
	//serverstring="Linuxi386/Linux-2.0.34 ";
	// NOTE: Without the space at the end, the client sends a marker after
	// the first phase of authentication.
	//
	// sent by:
	// * 8i server on redhat 6.2 x86
	// * 9i server on redhat 9 x86
	// * 10g server on fedora 5 x86
	// * 11g server on fedora 12 x86
	//serverstring="Linuxi386/Linux-2.0.34-8.1.0";
	//
	// sent by:
	// * 12c on fedora 19 x64
	//serverstring="x86_64/Linux 2.4.xx";
	//
	// NOTE: No datatype negotiation is sent if the client's string is
	// echoed.
	serverstring=clientstring;

	write(&reqpacket,ttiversion);
	write(&reqpacket,(byte_t)0);
	write(&reqpacket,serverstring);
	write(&reqpacket,'\0');


	// charset...
	// FIXME: no idea what charset=1 means...
	uint16_t	charset=1;
	// FIXME: other charsets might have graph elements...
	uint16_t	charsetgraphelementcount=0;

	writeLE(&reqpacket,charset);
	write(&reqpacket,(byte_t)0);
	writeLE(&reqpacket,charsetgraphelementcount);
	// FIXME: each charset graph element consists of 5 bytes...


	// fdo... (whatever that is)
	uint16_t	fdosize=100;
	uint32_t	fdodatasize=fdosize-4;
	// no idea what this means...
	byte_t	part1[]={
		0x05, 0x0b, 0x0c, 0x03, 0x0c, 0x0c, 0x05, 0x04,
		0x05, 0x0d, 0x06, 0x09, 0x07, 0x08, 0x05, 0x0e,
		0x05, 0x06, 0x05, 0x0f, 0x02, 0xec, 0xeb, 0xed,
		0x05, 0x0a, 0x05, 0x05, 0x05, 0x05, 0x05
	};
	byte_t	part1size=sizeof(part1);
	// no idea what this means...
	byte_t	part2[]={
		0x08, 0x23, 0x43, 0x23, 0x23, 0x08, 0x11, 0x23,
		0x08, 0x11, 0x41, 0xb0, 0x23, 0x00, 0x83
	};
	byte_t	part2size=sizeof(part2);

	writeBE(&reqpacket,fdosize);
	writeBE(&reqpacket,fdodatasize);
	write(&reqpacket,(byte_t)1);
	write(&reqpacket,part1size);
	write(&reqpacket,part2size);
	reqpacket.append(part1,part1size);
	reqpacket.append(part2,part2size);


	// some charset related thing, or something...
	byte_t	charsetthing[]={
		0x00, 0x01, 0x00,
		0x01, 0x03,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	
	reqpacket.append(charsetthing,sizeof(charsetthing));

	if (getDebug()) {
		debugStart("tti response");
		debugWrite("data flags: 0x%04x",dataflags);
		debugTtcCode(ttccode);
		debugWrite("version: %d",ttiversion);
		debugWrite("server string: \"%s\"",serverstring);
		debugWrite("charset: 0x%02x",charset);
		debugEnd();
	}
}

void sqlrprotocol_oracle::putTti4Response() {

	// oracle ??? supports TTI 4 (and lower)
	// FIXME: implement this...
}

void sqlrprotocol_oracle::putTti3Response() {

	// oracle ??? supports TTI 3 (and lower)
	// FIXME: implement this...
}

void sqlrprotocol_oracle::putTti2Response() {

	// oracle ??? supports TTI 2 (and lower)
	// FIXME: implement this...
}

void sqlrprotocol_oracle::putTti1Response() {

	// oracle ??? supports TTI 1 (and lower)
	// FIXME: implement this...
}

bool sqlrprotocol_oracle::dataTypeNegotiation() {
	return recvDataTypeRequest() && sendDataTypeResponse();
}

bool sqlrprotocol_oracle::recvDataTypeRequest() {

	if (!recvPacket()) {
		return false;
	}

	if (resppackettype!=PACKET_DATA) {
		debugWrite("bad packet type %d, expected %d",
					resppackettype,PACKET_DATA);
		return false;
	}

	const byte_t	*rp=resppacket;
	const byte_t	*end=rp+resppacketsize;

	uint16_t	dataflags;
	byte_t		ttccode;

	readBE(rp,&dataflags,&rp);
	if (!read(rp,&ttccode,"ttccode",TTC_DATATYPE_NEGOTIATION,&rp) ||
		!readMarker16(rp,0x0100,&rp) ||
		!readMarker16(rp,0x0100,&rp)) {
		return false;
	} 

	// 9i sends 0x02, all others send 0x00
	if (!readMarker8(rp,0x00,&rp) &&
		!readMarker8(rp,0x02,&rp)) {
		return false;
	}

	// NOTE: When talking to the db directly, 8.0.5 sends/recieves almost
	// nothing, but when talking to relay it sends/receives a ton of stuff.
	// It's not exctly clear what triggers this.
	datatypes=rp;
	datatypessize=end-rp;

	if (getDebug()) {
		debugStart("datatype request");
		debugWrite("data flags: 0x%04x",dataflags);
		debugTtcCode(ttccode);
		debugWrite("data types:");
		debugHexDump(datatypes,datatypessize);
		debugEnd();
	}

	return true;
}

bool sqlrprotocol_oracle::sendDataTypeResponse() {

	// FIXME: 9i appears to want more than we send here
	// might have to do with the 0x02 in the request...

	resetSendPacketBuffer(PACKET_DATA);

	uint16_t	dataflags=0;
	byte_t		ttccode=TTC_DATATYPE_NEGOTIATION;

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);
	// for now echo the requested data types
	reqpacket.append(datatypes,datatypessize);

	if (getDebug()) {
		debugStart("datatype response");
		debugWrite("data flags: 0x%04x",dataflags);
		debugTtcCode(ttccode);
		debugWrite("data types:");
		debugHexDump(datatypes,datatypessize);
		debugEnd();
	}

	return sendPacket(true);
}

bool sqlrprotocol_oracle::authenticate() {
	return recvAuthenticationRequest(false) &&
		sendAuthenticationChallenge() &&
		recvAuthenticationRequest(true) &&
		sendAuthenticationResponse();
}

bool sqlrprotocol_oracle::recvAuthenticationRequest(bool secondphase) {
	
	if (!recvPacket()) {
		return false;
	}

	if (resppackettype!=PACKET_DATA) {
		debugWrite("bad packet type %d, expected %d",
						resppackettype,PACKET_DATA);
		return false;
	}

	const byte_t	*rp=resppacket;
	const byte_t	*end=rp+resppacketsize;

	uint16_t	dataflags;
	byte_t		ttccode;
	byte_t		ttifunction;
	byte_t		seqnumber;
	byte_t		unknown;

	readBE(rp,&dataflags,&rp);
	if (!read(rp,&ttccode,"ttccode",TTC_TTI_FUNCTION,&rp)) {
		return false;
	}
	if (!secondphase) {
		if (!read(rp,&ttifunction,"ttifunction",
				TTI_LOGON_PRESENT_USER_REQ_AUTH_SESSKEY,&rp) &&
			!read(rp,&ttifunction,"ttifunction",
				TTI_LOGON_PRESENT_USER,&rp)) {
			return false;
		}
	} else {
		if (!read(rp,&ttifunction,"ttifunction",
				TTI_LOGON_PRESENT_PWD_SEND_AUTH_PASSWORD,&rp) &&
			!read(rp,&ttifunction,"ttifunction",
				TTI_LOGON_PRESENT_PWD,&rp)) {
			return false;
		}
	}
	read(rp,&seqnumber,&rp);

	// FIXME: This protocol appears to vary, depending on whether the
	// clientstring/serverstring match.  I suspect that it's not really
	// based on that though, but rather, on derived capabilities of the
	// different versions, or something...
	bool	stringsmatch=!charstring::compare(clientstring,serverstring);

	byte_t	usersize;
	char	*user;

	// no idea...
	for (uint16_t i=0; i<((stringsmatch)?4:2); i++) {
		read(rp,&unknown,&rp);
	}

	// user size...
	read(rp,&usersize,&rp);

	// no idea...
	for (uint16_t i=0; i<((stringsmatch)?23:7); i++) {
		read(rp,&unknown,&rp);
	}

	// user name...
	getString(rp,&user,usersize,&rp);

	debugStart("authentication request (phase %d)",(secondphase)?2:1);
	debugWrite("data flags: 0x%04x",dataflags);
	debugTtcCode(ttccode);
	debugTtiFunction(ttifunction);
	debugWrite("seq number: %d",seqnumber);
	debugWrite("user: %s",user);

	do {

		byte_t		fieldnamesize;
		uint32_t	fieldnamesizelong;
		char		*fieldname;
		byte_t		fieldsize;
		uint32_t	fieldsizelong;
		char		*field;

		// field name...
		if (!stringsmatch) {
			read(rp,&unknown,&rp);
		}
		read(rp,&fieldnamesize,&rp);
		if (!stringsmatch) {
			read(rp,&fieldnamesize,&rp);
		} else {
			readBE(rp,&fieldnamesizelong,&rp);
		}
		getString(rp,&fieldname,fieldnamesize,&rp);

		// field...
		if (!stringsmatch) {
			read(rp,&unknown,&rp);
		}
		read(rp,&fieldsize,&rp);
		if (!stringsmatch) {
			read(rp,&fieldsize,&rp);
		} else {
			readBE(rp,&fieldsizelong,&rp);
		}
		getString(rp,&field,fieldsize,&rp);

		// no idea...
		if (!stringsmatch) {
			read(rp,&unknown,&rp);
		} else {
			read(rp,&unknown,&rp);
			read(rp,&unknown,&rp);
			read(rp,&unknown,&rp);
			read(rp,&unknown,&rp);
		}

		debugWrite("%s: %s",fieldname,field);

		// FIXME: do something with these...

	} while (rp<end);

	debugEnd();

	return true;
}

bool sqlrprotocol_oracle::sendAuthenticationChallenge() {

	resetSendPacketBuffer(PACKET_DATA);

	// FIXME: actually do auth, and send some
	// different response if auth failed

	uint16_t	dataflags=0;
	byte_t		ttccode=TTC_OK;
	byte_t		unknown1[]={
		0x04,

		// varies, sometimes 0x01
		0x00,

		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

		// varies, sometimes 0x40
		0x00,

		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

		// varies, sometimes 0x02, 0x00
		0x00, 0x02,

		0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};


	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);

	debugStart("authentication challenge");
	debugWrite("data flags: 0x%04x",dataflags);
	debugTtcCode(ttccode);

	// no idea
	write(&reqpacket,(byte_t)1);
	write(&reqpacket,(byte_t)0);

	// session key...
	generateAuthSessionKey(16);
authsessionkey=charstring::duplicate("64760F3160DCEF82");
	putAuthField("AUTH_SESSKEY",authsessionkey);

	// no idea
	reqpacket.append(unknown1,sizeof(unknown1));

	putGenericFooter();

	debugEnd();

	return sendPacket(true);
}

void sqlrprotocol_oracle::putAuthField(const char *fieldname,
						const char *field) {

	byte_t	fieldnamesize=charstring::getLength(fieldname);
	byte_t	fieldsize=charstring::getLength(field);

	// field name...
	write(&reqpacket,fieldnamesize);
	// no idea...
	write(&reqpacket,(byte_t)0);
	write(&reqpacket,(byte_t)0);
	write(&reqpacket,(byte_t)0);
	// again...
	write(&reqpacket,fieldnamesize);
	write(&reqpacket,fieldname,fieldnamesize);

	// field...
	write(&reqpacket,fieldsize);
	// no idea...
	write(&reqpacket,(byte_t)0);
	write(&reqpacket,(byte_t)0);
	write(&reqpacket,(byte_t)0);
	// again...
	if (fieldsize) {
		write(&reqpacket,fieldsize);
		write(&reqpacket,field,fieldsize);
	}
	// no idea...
	write(&reqpacket,(byte_t)0);
	write(&reqpacket,(byte_t)0);
	write(&reqpacket,(byte_t)0);
	write(&reqpacket,(byte_t)0);

	debugWrite("%s: %s",fieldname,field);
}

bool sqlrprotocol_oracle::sendAuthenticationResponse() {

	resetSendPacketBuffer(PACKET_DATA);

	// FIXME: actually do auth, and send some
	// different response if auth failed

	uint16_t	dataflags=0;

	writeBE(&reqpacket,dataflags);

	byte_t	unknownheader[]={
		0x08, 0x08, 0x00
	};
	reqpacket.append(unknownheader,sizeof(unknownheader));

	putAuthField("AUTH_VERSION_STRING","- Production");
	putAuthField("AUTH_VERSION_SQL","12");
	putAuthField("AUTH_XACTION_TRAITS","3");
	putAuthField("AUTH_VERSION_NO","134238208");
	putAuthField("AUTH_VERSION_STATUS","0");
	putAuthField("AUTH_CAPABILITY_TABLE","");
	putAuthField("AUTH_SESSION_ID","9");
	putAuthField("AUTH_SERIAL_NUM","1981");

	byte_t	unknownfooter[]={
		0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00
	};
	reqpacket.append(unknownfooter,sizeof(unknownfooter));

	putGenericFooter();

	debugStart("authentication response");
	debugWrite("data flags: 0x%04x",dataflags);
	debugEnd();

	return sendPacket(true);
}

bool sqlrprotocol_oracle::open(const byte_t *rp) {

	// sqlplus 8.0.5, 8i, 9i
	// call this to open a cursor
	// sqlplus 10g+ use query3

	sqlrservercursor	*cursor=cont->getCursor();
	if (!cursor) {
		debugWrite("couldn't get cursor");
		return sendCursorNotOpenError();
	}

	uint16_t	cursorid=cont->getId(cursor);
hackcursorid=cursorid;
	
	// FIXME: decode this...

	debugStart("open request");
	debugWrite("cursor id: %d",cursorid);
	debugEnd();

	return sendOpenResponse(cursor);
}

bool sqlrprotocol_oracle::sendOpenResponse(sqlrservercursor *cursor) {

	resetSendPacketBuffer(PACKET_DATA);

	uint16_t	dataflags=0;
	byte_t		ttccode=TTC_OK;
	byte_t		unknown[]={
		0x01, 0x00, 0x00, 0x00, 0x09
	};

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);
	reqpacket.append(unknown,sizeof(unknown));

	debugStart("open response");
	debugWrite("data flags: 0x%04x",dataflags);
	debugTtcCode(ttccode);
	debugEnd();

	return sendPacket(true);
}

void sqlrprotocol_oracle::debugTtcCode(byte_t ttccode) {
	if (!getDebug()) {
		return;
	}
	const char	*code=NULL;
	switch (ttccode) {
		case TTC_PROTOCOL_NEGOTIATION:
			code="TTC_PROTOCOL_NEGOTIATION";
			break;
		case TTC_DATATYPE_NEGOTIATION:
			code="TTC_DATATYPE_NEGOTIATION";
			break;
		case TTC_TTI_FUNCTION:
			code="TTC_TTI_FUNCTION";
			break;
		case TTC_OK:
			code="TTC_OK";
			break;
		case TTC_EXTENDED_TTI_FUNCTION:
			code="TTC_EXTENDED_TTI_FUNCTION";
			break;
		case TTC_EXTPROC1:
			code="TTC_EXTPROC1";
			break;
		case TTC_EXTPROC2:
			code="TTC_EXTPROC2";
			break;
		default:
			code="UNKNOWN";
			break;
	}
	debugWrite("ttc code: (0x%02x) %s",ttccode,code);
}

void sqlrprotocol_oracle::debugTtiFunction(byte_t ttifunction) {
	if (!getDebug()) {
		return;
	}
	const char	*func=NULL;
	switch (ttifunction) {
		case TTI_OPEN:
			func="TTI_OPEN";
			break;
		case TTI_QUERY:
			func="TTI_QUERY";
			break;
		case TTI_EXECUTE:
			func="TTI_EXECUTE";
			break;
		case TTI_FETCH:
			func="TTI_FETCH";
			break;
		case TTI_CLOSE:
			func="TTI_CLOSE";
			break;
		case TTI_DISCONNECT:
			func="TTI_DISCONNECT";
			break;
		case TTI_AUTOCOMMIT_ON:
			func="TTI_AUTOCOMMIT_ON";
			break;
		case TTI_AUTOCOMMIT_OFF:
			func="TTI_AUTOCOMMIT_OFF";
			break;
		case TTI_COMMIT:
			func="TTI_COMMIT";
			break;
		case TTI_ROLLBACK:
			func="TTI_ROLLBACK";
			break;
		case TTI_CANCEL:
			func="TTI_CANCEL";
			break;
		case TTI_DESCRIBE:
			func="TTI_DESCRIBE";
			break;
		case TTI_STARTUP:
			func="TTI_STARTUP";
			break;
		case TTI_SHUTDOWN:
			func="TTI_SHUTDOWN";
			break;
		case TTI_VERSION:
			func="TTI_VERSION";
			break;
		case TTI_K2_TRANSACTIONS:
			func="TTI_K2_TRANSACTIONS";
			break;
		case TTI_QUERY2:
			func="TTI_QUERY2";
			break;
		case TTI_OSQL7:
			func="TTI_OSQL7";
			break;
		case TTI_OKOD:
			func="TTI_OKOD";
			break;
		case TTI_QUERY3:
			func="TTI_QUERY3";
			break;
		case TTI_LOB_OPERATIONS:
			func="TTI_LOB_OPERATIONS";
			break;
		case TTI_ODNY:
			func="TTI_ODNY";
			break;
		case TTI_TRANSACTION_END:
			func="TTI_TRANSACTION_END";
			break;
		case TTI_TRANSACTION_BEGIN:
			func="TTI_TRANSACTION_BEGIN";
			break;
		case TTI_OCCA:
			func="TTI_OCCA";
			break;
		case TTI_STARTUP2:
			func="TTI_STARTUP2";
			break;
		case TTI_LOGON_PRESENT_PWD:
			func="TTI_LOGON_PRESENT_PWD";
			break;
		case TTI_LOGON_PRESENT_USER:
			func="TTI_LOGON_PRESENT_USER";
			break;
		case TTI_LOGON_PRESENT_PWD_SEND_AUTH_PASSWORD:
			func="TTI_LOGON_PRESENT_PWD_SEND_AUTH_PASSWORD";
			break;
		case TTI_LOGON_PRESENT_USER_REQ_AUTH_SESSKEY:
			func="TTI_LOGON_PRESENT_USER_REQ_AUTH_SESSKEY";
			break;
		case TTI_DESCRIBE2:
			func="TTI_DESCRIBE2";
			break;
		case TTI_OOTCM:
			func="TTI_OOTCM";
			break;
		case TTI_OKPFC:
			func="TTI_OKPFC";
			break;
		case TTI_SWITCH_SESSION:
			func="TTI_SWITCH_SESSION";
			break;
		case TTI_CLOSE2:
			func="TTI_CLOSE2";
			break;
		case TTI_OSCID:
			func="TTI_OSCID";
			break;
		case TTI_OSKEYVAL:
			func="TTI_OSKEYVAL";
			break;
		default:
			func="UNKNOWN";
			break;
	}
	debugWrite("tti function: (0x%02x) %s",ttifunction,func);
}

void sqlrprotocol_oracle::debugOptions(uint16_t options,
					uint16_t moreoptions) {
	if (!getDebug()) {
		return;
	}

	debugStart("options");
	debugWrite("0x%04x",options);
	stringbuffer	b;
	b.printBits(hostToBE(options));
	debugWrite(b.getString());
	debugOptions(options);
	debugEnd();

	debugStart("moreoptions");
	debugWrite("0x%04x",moreoptions);
	b.clear();
	b.printBits(hostToBE(moreoptions));
	debugWrite(b.getString());
	debugOptions(moreoptions);
	debugEnd();
}

void sqlrprotocol_oracle::debugOptions(uint16_t options) {
	if (options&OPTION_PARSE) {
		debugWrite("OPTION_PARSE");
	}
	if (options&OPTION_BIND) {
		debugWrite("OPTION_BIND");
	}
	if (options&OPTION_DEFINE) {
		debugWrite("OPTION_DEFINE");
	}
	if (options&OPTION_EXECUTE) {
		debugWrite("OPTION_EXECUTE");
	}
	if (options&OPTION_FETCH) {
		debugWrite("OPTION_FETCH");
	}
	if (options&OPTION_CANCEL) {
		debugWrite("OPTION_CANCEL");
	}
	if (options&OPTION_COMMIT) {
		debugWrite("OPTION_COMMIT");
	}
	if (options&OPTION_EXACTFETCH) {
		debugWrite("OPTION_EXACTFETCH");
	}
	if (options&OPTION_SNDIOV) {
		debugWrite("OPTION_SNDIOV");
	}
	if (options&OPTION_NOPLSQL) {
		debugWrite("OPTION_NOPLSQL");
	}
}

void sqlrprotocol_oracle::debugCharacterSet(byte_t characterset) {
	if (!getDebug()) {
		return;
	}
	debugWrite("character set: 0x%02x",(uint32_t)(0x000000ff&characterset));
}

void sqlrprotocol_oracle::debugStatusFlags(uint16_t statusflags) {
	if (!getDebug()) {
		return;
	}
	debugStart("status flags");
	debugWrite("0x%04x",statusflags);
	stringbuffer	b;
	b.printBits(statusflags);
	debugWrite(b.getString());
	/*if (statusflags&SERVER_STATUS_IN_TRANS) {
		debugWrite("SERVER_STATUS_IN_TRANS");
	}*/
	debugEnd();
}

void sqlrprotocol_oracle::debugColumnType(const char *name,
						uint16_t columntype) {
	if (!getDebug()) {
		return;
	}
	debugWrite("type: %s (0x%02x)",name,(uint32_t)(0x000000ff&columntype));
	debugColumnType(columntype);
}

void sqlrprotocol_oracle::debugColumnType(uint16_t columntype) {
	if (!getDebug()) {
		return;
	}
	switch (columntype) {
		case ORACLE_TYPE_VARCHAR:
			debugWrite("ORACLE_TYPE_VARCHAR");
			break;
		case ORACLE_TYPE_NUMBER:
			debugWrite("ORACLE_TYPE_NUMBER");
			break;
		case ORACLE_TYPE_VARNUM:
			debugWrite("ORACLE_TYPE_VARNUM");
			break;
		case ORACLE_TYPE_LONG:
			debugWrite("ORACLE_TYPE_LONG");
			break;
		case ORACLE_TYPE_ROWID_DEPRECATED:
			debugWrite("ORACLE_TYPE_ROWID_DEPRECATED");
			break;
		case ORACLE_TYPE_DATE:
			debugWrite("ORACLE_TYPE_DATE");
			break;
		case ORACLE_TYPE_RAW:
			debugWrite("ORACLE_TYPE_RAW");
			break;
		case ORACLE_TYPE_LONG_RAW:
			debugWrite("ORACLE_TYPE_LONG_RAW");
			break;
		case ORACLE_TYPE_CHAR:
			debugWrite("ORACLE_TYPE_CHAR");
			break;
		case ORACLE_TYPE_RESULT_SET:
			debugWrite("ORACLE_TYPE_RESULT_SET");
			break;
		case ORACLE_TYPE_ROWID:
			debugWrite("ORACLE_TYPE_ROWID");
			break;
		case ORACLE_TYPE_NAMED_TYPE:
			debugWrite("ORACLE_TYPE_NAMED_TYPE");
			break;
		case ORACLE_TYPE_REF_TYPE:
			debugWrite("ORACLE_TYPE_REF_TYPE");
			break;
		case ORACLE_TYPE_CLOB:
			debugWrite("ORACLE_TYPE_CLOB");
			break;
		case ORACLE_TYPE_BLOB:
			debugWrite("ORACLE_TYPE_BLOB");
			break;
		case ORACLE_TYPE_BFILE:
			debugWrite("ORACLE_TYPE_BFILE");
			break;
		case ORACLE_TYPE_TIMESTAMP:
			debugWrite("ORACLE_TYPE_TIMESTAMP");
			break;
		case ORACLE_TYPE_TIMESTAMPTZ:
			debugWrite("ORACLE_TYPE_TIMESTAMPTZ");
			break;
		case ORACLE_TYPE_INTERVALYM:
			debugWrite("ORACLE_TYPE_INTERVALYM");
			break;
		case ORACLE_TYPE_INTERVALDS:
			debugWrite("ORACLE_TYPE_INTERVALDS");
			break;
		case ORACLE_TYPE_TIMESTAMPLTZ:
			debugWrite("ORACLE_TYPE_TIMESTAMPLTZ");
			break;
		case ORACLE_TYPE_PLSQL_INDEX_TABLE:
			debugWrite("ORACLE_TYPE_PLSQL_INDEX_TABLE");
			break;
		default:
			debugWrite("unknown ORACLE_TYPE");
			break;
	}
}

void sqlrprotocol_oracle::debugSystemError() {
	char	*err=error::getErrorString();
	debugWrite(err);
	delete[] err;
}

bool sqlrprotocol_oracle::getTtiFunction(const byte_t *rp,
						byte_t *ttifunction,
						const byte_t **rpout) {

	if (!rp) {
		if (!recvPacket()) {
			return false;
		}

		if (resppackettype!=PACKET_DATA) {
			debugWrite("bad packet type %d, expected %d",
						resppackettype,PACKET_DATA);
			return false;
		}

		rp=resppacket;
	}

	const byte_t	*rpin=rp;

	uint16_t	dataflags;
	byte_t		ttccode;

	readBE(rp,&dataflags,&rp);
	if (!read(rp,&ttccode,"ttccode",TTC_TTI_FUNCTION,&rp) &&
		!read(rp,&ttccode,"ttccode",TTC_EXTENDED_TTI_FUNCTION,&rp)) {
		*rpout=rpin;
		return false;
	}
	read(rp,ttifunction,&rp);
	*rpout=rp;

	if (getDebug()) {
		debugStart("get tti function");
		debugWrite("data flags: 0x%04x",dataflags);
		debugTtcCode(ttccode);
		debugTtiFunction(*ttifunction);
		debugEnd();
	}
	
	return true;
}

bool sqlrprotocol_oracle::query(const byte_t *rp) {

	// sqlplus 8.0.5, 8i, 9i
	// call this to prepare some initial queries
	// sqlplus 10g+ use query3

	// prepares the specified query

	// parse the request...
	uint16_t	options;
	uint16_t	moreoptions;
	uint16_t	cursorid;
	byte_t		unknown3;
	byte_t		unknown4;
	byte_t		unknown5;
	uint16_t	querysize;
	byte_t		unknown6;
	byte_t		unknown7;
	const char	*query;

	readBE(rp,&options,&rp);
	readBE(rp,&moreoptions,&rp);
	readBE(rp,&cursorid,&rp);
	read(rp,&unknown3,&rp);
	read(rp,&unknown4,&rp);
	read(rp,&unknown5,&rp);
	readLE(rp,&querysize,&rp);
	read(rp,&unknown6,&rp);
	read(rp,&unknown7,&rp);
	query=(char *)rp;
cursorid=hackcursorid;

	debugStart("query request");
	debugOptions(options,moreoptions);
	debugWrite("cursor id: %d",cursorid);
	debugWrite("unknown: %02x %02x %02x",unknown3,unknown4,unknown5);
	debugWrite("query size: %d",querysize);
	debugWrite("unknown: %02x %02x",unknown6,unknown7);
	debugWrite("query: \"%*s\"",querysize,query);
	debugEnd();

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(cursorid);
	if (!cursor) {
		debugWrite("cursor id %d not found",cursorid);
		return sendCursorNotOpenError();
	}

	// reset column type cache flag
	columntypescached[cont->getId(cursor)]=false;

	// bounds checking
	if (querysize>maxquerysize) {
		// FIXME: implement this
		//return sendErrPacket(1105,"Unknown error","24000");
		return false;
	}

	// copy the query into the cursor's query buffer
	char	*querybuffer=cont->getQueryBuffer(cursor);
	bytestring::copy(querybuffer,query,querysize);
	querybuffer[querysize]='\0';
	cont->setQuerySize(cursor,querysize);

	// prepare the query
	if (!cont->prepareQuery(cursor,cont->getQueryBuffer(cursor),
					cont->getQuerySize(cursor),
					true,true,true)) {
		debugWrite("prepare query failed");
		return sendQueryError(cursor);
	}
	return sendQueryResponse(cursor);
}

bool sqlrprotocol_oracle::sendQueryResponse(sqlrservercursor *cursor) {

	resetSendPacketBuffer(PACKET_DATA);

	// FIXME: decode this...

	uint16_t	dataflags=0;
	// FIXME: not a valid ttccode type...
	byte_t	ttccode=4;
	byte_t unknown1[]={
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00
	};
	byte_t unknown2[]={
		// not cursor id
		0x01, 0x00
	};
	byte_t unknown3[]={
		0x11, 0x00, 0x03, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);
	reqpacket.append(unknown1,sizeof(unknown1));
	reqpacket.append(unknown2,sizeof(unknown2));
	reqpacket.append(unknown3,sizeof(unknown3));

	putGenericFooter();

	if (getDebug()) {
		debugStart("query response");
		debugWrite("data flags: 0x%04x",dataflags);
		debugTtcCode(ttccode);
		debugEnd();
	}

	return sendPacket(true);
}

bool sqlrprotocol_oracle::query2(const byte_t *rp) {

	// sqlplus 8.0.5, 8i, 9i
	// call this to prepare/execute some initial queries
	// sqlplus 10g+ use query3
	// can apparently be used for fetch too

	// parse the request...
	uint16_t	options;
	uint16_t	moreoptions;
	uint16_t	cursorid;
	uint32_t	querysize=0;
	const char	*query=NULL;

	readBE(rp,&options,&rp);
	readBE(rp,&moreoptions,&rp);
	readBE(rp,&cursorid,&rp);
cursorid=hackcursorid;
	if (options&OPTION_PARSE) {
		// no idea...
		for (uint16_t i=0; i<7; i++) {
			byte_t	unknown;
			read(rp,&unknown,&rp);
		}
		readLE(rp,&querysize,&rp);
		// no idea...
		for (uint16_t i=0; i<44; i++) {
			byte_t	unknown;
			read(rp,&unknown,&rp);
		}
		query=(char *)rp;
		rp+=querysize;
	}
	// no idea...

	if (getDebug()) {
		debugStart("query2 request");
		debugOptions(options,moreoptions);
		debugWrite("cursor id: %d",cursorid);
		if (options&OPTION_PARSE) {
			debugWrite("query size: %d",querysize);
			debugWrite("query: \"%*s\"",querysize,query);
		}
		debugEnd();
	}

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(cursorid);
	if (!cursor) {
		debugWrite("cursor id %d not found",cursorid);
		return sendCursorNotOpenError();
	}

	if (options&OPTION_PARSE) {

		// reset column type cache flag
		columntypescached[cont->getId(cursor)]=false;

		// bounds checking
		if (querysize>maxquerysize) {
			// FIXME: implement this
			//return sendErrPacket(1105,"Unknown error","24000");
			return false;
		}

		// copy the query into the cursor's query buffer
		char	*querybuffer=cont->getQueryBuffer(cursor);
		bytestring::copy(querybuffer,query,querysize);
		querybuffer[querysize]='\0';
		cont->setQuerySize(cursor,querysize);
	
		// prepare the query
		if (!cont->prepareQuery(cursor,
					cont->getQueryBuffer(cursor),
					cont->getQuerySize(cursor),
					true,true,true)) {
			debugWrite("prepare query failed");
			return sendQueryError(cursor);
		}
	}

	if (options&OPTION_BIND) {

		if (!sendQuery2Response(cursor,true)) {
			return false;
		}

		// FIXME: get these somehow...
		uint16_t	pcount=1;
		uint16_t	ptypes[]={ORACLE_TYPE_VARCHAR};
		
		if (!bindParameters(cursor,pcount,ptypes)) {
			return false;
		}
	}

	if (options&OPTION_EXECUTE) {

		// execute the query
		if (!cont->executeQuery(cursor,true,true,true,true)) {
			debugWrite("execute query failed");
			return sendQueryError(cursor);
		}
	}

	if (options&OPTION_COMMIT) {
		// FIXME: commit...
	}

	return sendQuery2Response(cursor,false);
}

bool sqlrprotocol_oracle::sendQuery2Response(sqlrservercursor *cursor,
								bool binds) {

	resetSendPacketBuffer(PACKET_DATA);

	uint16_t	dataflags;
	byte_t		ttccode;

	if (binds) {

		dataflags=0;
		// FIXME: not a valid ttccode type...
		ttccode=11;

		// FIXME: decode this...

		byte_t unknown[]={
			0x05, 0xFE, 0x01, 0x00, 0x00,
			0x00, 0x01, 0x00, 0x00, 0x00, 0x20
		};

		writeBE(&reqpacket,dataflags);
		write(&reqpacket,ttccode);
		reqpacket.append(unknown,sizeof(unknown));

	} else {

		dataflags=0;
		ttccode=TTC_OK;

		// FIXME: decode this...

		byte_t unknown[]={

			// varies...
			0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
			0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x01,
			0x00, 0x00, 0x00

			// it's this from 8i on redhat 6.2,
			// though the 8i client accepts the above...
			/*
 			0x02, 0x00, 0x79, 0xE6, 0x19 0x00, 0x00, 0x00,
			0x00, 0x00, 0x04, 0x01, 0x00 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00 0x00, 0x01, 0x00,
			0x00, 0x00, 0x03, 0x00, 0x40 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00 0x09, 0x00, 0x00,
			0x01, 0x00, 0x00
			*/
		};

		writeBE(&reqpacket,dataflags);
		write(&reqpacket,ttccode);
		reqpacket.append(unknown,sizeof(unknown));
	}

	putGenericFooter();

	debugStart("query2 response");
	debugWrite("data flags: 0x%04x",dataflags);
	debugTtcCode(ttccode);
	debugEnd();

	return sendPacket(true);
}

bool sqlrprotocol_oracle::bindParameters(sqlrservercursor *cursor,
							uint16_t pcount,
							uint16_t *ptypes) {

	cont->setInputBindCount(cursor,
				(pcount<=maxbindcount)?pcount:maxbindcount);
	sqlrserverbindvar	*inbinds=cont->getInputBinds(cursor);

	for (uint16_t i=0; i<pcount; i++) {

		if (!recvPacket()) {
			return false;
		}

		if (resppackettype!=PACKET_DATA) {
			debugWrite("bad packet type %d, expected %d",
						resppackettype,PACKET_DATA);
			return false;
		}

		if (i>maxbindcount) {
			continue;
		}

		const byte_t	*rp=resppacket;

		uint16_t	dataflags;
		byte_t		unknown;

		readBE(rp,&dataflags,&rp);
		read(rp,&unknown,&rp);


		sqlrserverbindvar	*bv=&(inbinds[i]);

		// the bind variable name should be something like :1, :2, etc.
		bv->variable=bindvarnames[i];
		bv->variablesize=charstring::getLength(bv->variable);

		debugStart("bind %d",i);
		debugWrite("data flags: 0x%04x",dataflags);
		debugWrite("variable: %s",bv->variable);

		// FIXME: handle nulls
		if (false) {
			bv->type=SQLRSERVERBINDVARTYPE_NULL;
			bv->isnull=cont->getNullBindValue();
			debugWrite("type: NULL");
			debugWrite("isnull: true");
			debugEnd();
			continue;
		}

		// handle non-nulls
		switch (ptypes[i]) {
			/*case MYSQL_TYPE_TINY:
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				bv->value.integerval=*((char *)rp);
				bv->isnull=cont->getNonNullBindValue();
				rp+=sizeof(char);
				break;
			case MYSQL_TYPE_SHORT:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				uint16_t	val;
				bytestring::copy(&val,rp,sizeof(uint16_t));
				val=leToHost((uint16_t)val);
				bv->value.integerval=(int16_t)val;
				bv->isnull=cont->getNonNullBindValue();
				rp+=sizeof(int16_t);
				}
				break;
			case MYSQL_TYPE_LONG:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				uint32_t	val;
				bytestring::copy(&val,rp,sizeof(uint32_t));
				val=leToHost((uint32_t)val);
				bv->value.integerval=(int32_t)val;
				bv->isnull=cont->getNonNullBindValue();
				rp+=sizeof(int32_t);
				}
				break;
			case MYSQL_TYPE_LONGLONG:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				uint64_t	val;
				bytestring::copy(&val,rp,sizeof(uint64_t));
				val=leToHost((uint64_t)val);
				bv->value.integerval=(int64_t)val;
				bv->isnull=cont->getNonNullBindValue();
				rp+=sizeof(int64_t);
				}
				break;
			case MYSQL_TYPE_FLOAT:
				bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
				bytestring::copy(&bv->value.doubleval.value,
							rp,sizeof(float));
				bv->value.doubleval.precision=0;
				bv->value.doubleval.scale=0;
				bv->isnull=cont->getNonNullBindValue();
				rp+=sizeof(float);
				break;
			case MYSQL_TYPE_DOUBLE:
				bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
				bytestring::copy(&bv->value.doubleval.value,
							rp,sizeof(double));
				bv->value.doubleval.precision=0;
				bv->value.doubleval.scale=0;
				bv->isnull=cont->getNonNullBindValue();
				rp+=sizeof(double);
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
				bv->isnull=cont->getNonNullBindValue();
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					new char[bv->value.dateval.buffersize];

				char	size=*((char *)rp);
				rp+=sizeof(char);

				if (size) {
					char	isneg=*((char *)rp);
					bv->value.dateval.isnegative=isneg;
					rp+=sizeof(char);

					int32_t	days;
					bytestring::copy(&days,
							rp,sizeof(int32_t));
					bv->value.dateval.day=
						leToHost((uint32_t)days);
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
					if (size>8) {
						int32_t	ms;
						bytestring::copy(&ms,
							rp,sizeof(int32_t));
						bv->value.dateval.
							microsecond=
							leToHost((uint32_t)ms);
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
				bv->isnull=cont->getNonNullBindValue();
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					new char[bv->value.dateval.buffersize];

				char	size=*((char *)rp);
				rp+=sizeof(char);

				if (size) {
					int16_t	year;
					bytestring::copy(&year,
							rp,sizeof(int16_t));
					bv->value.dateval.year=
						leToHost((uint16_t)year);
					rp+=sizeof(int16_t);
					bv->value.dateval.month=*((char *)rp);
					rp+=sizeof(char);
					bv->value.dateval.day=*((char *)rp);
					rp+=sizeof(char);

					// ignore time parts
					if (size>4) {
						rp+=3*sizeof(char);
						if (size>7) {
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
				bv->isnull=cont->getNonNullBindValue();
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					new char[bv->value.dateval.buffersize];

				char	size=*((char *)rp);
				rp+=sizeof(char);

				if (size) {
					int16_t	year;
					bytestring::copy(&year,
							rp,sizeof(int16_t));
					bv->value.dateval.year=
						leToHost((uint16_t)year);
					rp+=sizeof(int16_t);
					bv->value.dateval.month=*((char *)rp);
					rp+=sizeof(char);
					bv->value.dateval.day=*((char *)rp);
					rp+=sizeof(char);
					if (size>4) {
						bv->value.dateval.hour=
								*((char *)rp);
						rp+=sizeof(char);
						bv->value.dateval.minute=
								*((char *)rp);
						rp+=sizeof(char);
						bv->value.dateval.second=
								*((char *)rp);
						rp+=sizeof(char);
						if (size>7) {
							int32_t	ms;
							bytestring::copy(&ms,
							rp,sizeof(int32_t));
							bv->value.dateval.
								microsecond=
							leToHost((uint32_t)ms);
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
				bv->valuesize=getLenEncInt(rp,&rp);
				bv->value.stringval=charstring::duplicate(
							(const char *)rp,
							bv->valuesize);
				bv->isnull=cont->getNonNullBindValue();
				rp+=bv->valuesize;
				break;
			*/
			case ORACLE_TYPE_VARCHAR:
			// (for all other types, assume varchar)
			default:
				{
				byte_t	size;
				read(rp,&size,&rp);

				bv->type=SQLRSERVERBINDVARTYPE_STRING;
				bv->valuesize=size;
				bv->value.stringval=charstring::duplicate(
							(const char *)rp,
							bv->valuesize);
				bv->isnull=cont->getNonNullBindValue();
				rp+=bv->valuesize;
				break;
				}
		}

		if (getDebug()) {
			if (bv->type==SQLRSERVERBINDVARTYPE_STRING) {
				debugWrite("type: STRING");
				debugWrite("value: %s",
						bv->value.stringval);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_INTEGER) {
				debugWrite("type: INTEGER");
				debugWrite("value: %lld",
						bv->value.integerval);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_DOUBLE) {
				debugWrite("type: DOUBLE");
				debugWrite("value: %f (%d,%d)",
						bv->value.doubleval.value,
						bv->value.doubleval.precision,
						bv->value.doubleval.scale);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_DATE) {
				// FIXME: print date...
			}
			debugWrite("value size: %d",bv->valuesize);
			debugWrite("isnull: false");
			debugEnd();
		}
	}

	return true;
}

bool sqlrprotocol_oracle::query3(const byte_t *rp) {

	// all versions call this to open/prepare/describe/execute most queries
	// can apparently be used for fetch too

	// parse the request...
	uint16_t	options;
	uint16_t	moreoptions;
	uint16_t	cursorid;
	uint32_t	querysize;
	const char	*query;

	readBE(rp,&options,&rp);
	readBE(rp,&moreoptions,&rp);
cursorid=hackcursorid;
	// no idea...
	for (uint16_t i=0; i<9; i++) {
		byte_t	unknown;
		read(rp,&unknown,&rp);
	}
	readLE(rp,&querysize,&rp);
	// no idea...
	for (uint16_t i=0; i<56; i++) {
		byte_t	unknown;
		read(rp,&unknown,&rp);
	}
	query=(char *)rp;
	rp+=querysize;
	// no idea...

	if (getDebug()) {
		debugStart("query3 request");
		debugOptions(options,moreoptions);
		debugWrite("cursor id: %d",cursorid);
		debugWrite("query size: %dn",querysize);
		debugWrite("query: \"%*s\"",querysize,query);
		debugEnd();
	}

	// get the requested cursor
	sqlrservercursor	*cursor;
	if (cursorid==65535) {
		cursor=cont->getCursor();
		if (!cursor) {
			debugWrite("couldn't get cursor");
			return sendCursorNotOpenError();
		}
		cursorid=cont->getId(cursor);
hackcursorid=cursorid;
		debugStart("open request");
		debugWrite("cursor id: %d",cursorid);
		debugEnd();
	} else {
		cursor=cont->getCursor(cursorid);
		if (!cursor) {
			debugWrite("cursor id %d not found",cursorid);
			return sendCursorNotOpenError();
		}
	}

	if (options&OPTION_PARSE) {

		// reset column type cache flag
		columntypescached[cont->getId(cursor)]=false;

		// bounds checking
		if (querysize>maxquerysize) {
			// FIXME: implement this
			//return sendErrPacket(1105,"Unknown error","24000");
			return false;
		}

		// copy the query into the cursor's query buffer
		char	*querybuffer=cont->getQueryBuffer(cursor);
		bytestring::copy(querybuffer,query,querysize);
		querybuffer[querysize]='\0';
		cont->setQuerySize(cursor,querysize);
	
		// prepare the query
		if (!cont->prepareQuery(cursor,
					cont->getQueryBuffer(cursor),
					cont->getQuerySize(cursor),
					true,true,true)) {
			debugWrite("prepare query failed");
			return sendQueryError(cursor);
		}
	}

	if (options&OPTION_EXECUTE) {

		// execute the query
		if (!cont->executeQuery(cursor,true,true,true,true)) {
			debugWrite("execute query failed");
			return sendQueryError(cursor);
		}
	}

	if (options&OPTION_COMMIT) {
		// FIXME: commit...
	}

	return sendQuery3Response(cursor,options);
}

bool sqlrprotocol_oracle::sendQuery3Response(sqlrservercursor *cursor,
							uint16_t options) {

	resetSendPacketBuffer(PACKET_DATA);

	uint32_t	colcount=cont->colCount(cursor);
	cacheColumnDefinitions(cursor,colcount);

	uint16_t	dataflags=0;
	byte_t		ttccode;

	bool	putfooter=true;

	if (colcount) {

		if (options&OPTION_PARSE) {

			dataflags=0;
			// FIXME: not a valid ttccode type...
			ttccode=16;

			writeBE(&reqpacket,dataflags);
			write(&reqpacket,ttccode);

			putColumnDefinitions(cursor,colcount,true);

			putIov();

			const byte_t	unknown3[]={
				0x06, 0x02, 0x8C
			};
			reqpacket.append(unknown3,sizeof(unknown3));

			// column count
			write(&reqpacket,(byte_t)colcount);

			// no idea
			writeBE(&reqpacket,(uint32_t)1);
			writeBE(&reqpacket,(uint32_t)7);
		}

		if (options&OPTION_FETCH) {

			// FIXME: get this from the client somehow
			uint32_t rowstofetch=1;

			// for each row...
			uint32_t rowsfetched=0;
			do {

				// fetch a row
				bool	error;
				if (!cont->fetchRow(cursor,&error)) {
					if (error) {
						// FIXME: handle error
					}
					break;
				}

				debugStart("query3 response row");

				putRow(cursor,colcount,true);

				debugEnd();

				// FIXME: kludgy
				cont->nextRow(cursor);

				rowsfetched++;

			} while (rowsfetched<rowstofetch);

			if (!rowsfetched) {

				// if we hit the end of the result set then we
				// need to send ORA-01403; no data found
				putError("ORA-01403: no data found");
				putfooter=false;
			}
		}
	}

	if (putfooter) {

		dataflags=0;
		ttccode=TTC_OK;

		writeBE(&reqpacket,dataflags);
		write(&reqpacket,ttccode);

		const byte_t	unknown1[]={
			0x04, 0x00, 0x01, 0x5D, 0x16,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x01, 0x00,

			// varies
			0x00,

			0x00
		};
		reqpacket.append(unknown1,sizeof(unknown1));

		// ???
		write(&reqpacket,(byte_t)((colcount)?3:47));

		const byte_t	unknown2[]={
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00,

			// varies (sequence?)
			0x1A,

			0x00, 0x00, 0x01, 0x00, 0x00, 0x00
		};
		reqpacket.append(unknown2,sizeof(unknown2));

		if (!colcount) {
			const byte_t	unknown3[]={
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00
			};
			reqpacket.append(unknown3,sizeof(unknown3));
		}

		if (getDebug()) {
			debugStart("query3 response (footer)");
			debugWrite("data flags: 0x%04x",dataflags);
			debugTtcCode(ttccode);
			debugEnd();
		}
	}

	return sendPacket(true);
}

bool sqlrprotocol_oracle::execute(const byte_t *rp) {

	// sqlplus 8.0.5 (at least)
	// call this to execute a commit at the end of initialization

	// parse the request...
	uint16_t	options;
	uint16_t	moreoptions;
	uint16_t	cursorid;

	// FIXME: decode this...
	readBE(rp,&options,&rp);
	readBE(rp,&moreoptions,&rp);
	readBE(rp,&cursorid,&rp);
cursorid=hackcursorid;

	if (getDebug()) {
		debugStart("execute request");
		debugOptions(options,moreoptions);
		debugWrite("cursor id: %d",cursorid);
		debugEnd();
	}

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(cursorid);
	if (!cursor) {
		debugWrite("cursor id %d not found",cursorid);
		return sendCursorNotOpenError();
	}

	// execute the query
	if (!cont->executeQuery(cursor,true,true,true,true)) {
		debugWrite("execute query failed");
		return sendQueryError(cursor);
	}

	return sendExecuteResponse(cursor);
}

bool sqlrprotocol_oracle::sendExecuteResponse(sqlrservercursor *cursor) {

	resetSendPacketBuffer(PACKET_DATA);

	// FIXME: decode this...

	uint16_t	dataflags=0;
	// FIXME: not a valid ttccode type...
	byte_t	ttccode=4;
	byte_t	unknown[]={
		0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
		0x00, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);
	reqpacket.append(unknown,sizeof(unknown));

	debugStart("execute response");
	debugWrite("data flags: 0x%04x",dataflags);
	debugTtcCode(ttccode);
	debugEnd();

	return sendPacket(true);
}

bool sqlrprotocol_oracle::fetch(const byte_t *rp) {

	// all versions call this to fetch

	// fetches the specified number of rows

	// parse the request...
	uint16_t	options;
	uint16_t	moreoptions;
	uint16_t	cursorid;

	// FIXME: decode this...
	readBE(rp,&options,&rp);
	readBE(rp,&moreoptions,&rp);
cursorid=hackcursorid;

	if (getDebug()) {
		debugStart("fetch request");
		debugOptions(options,moreoptions);
		debugEnd();
	}

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(cursorid);
	if (!cursor) {
		debugWrite("cursor id %d not found",cursorid);
		return sendCursorNotOpenError();
	}

	return sendFetchResponse(cursor,
				(options&OPTION_PARSE),
				(options&OPTION_DEFINE),
				(options&OPTION_SNDIOV),
				(options&OPTION_EXACTFETCH));
}

bool sqlrprotocol_oracle::sendFetchResponse(sqlrservercursor *cursor,
							bool parse,
							bool define,
							bool sndiov,
							bool exactfetch) {

	resetSendPacketBuffer(PACKET_DATA);

	uint32_t	colcount=cont->colCount(cursor);
	cacheColumnDefinitions(cursor,colcount);

	// FIXME: get this from the client somehow
	uint32_t rowstofetch=1;

	// for each row...
	uint32_t rowsfetched=0;
	do {

		// fetch a row
		bool	error;
		if (!cont->fetchRow(cursor,&error)) {
			if (error) {
				// FIXME: handle error
			}
			break;
		}

		// ok, so there is at least one row...
		// send various headers and column definitions
		if (!rowsfetched) {

			// FIXME: the headers/col-defs appear to be very
			// different when sent from 8i

			if (define) {

				uint16_t	dataflags=0;
				// FIXME: not a valid ttccode type...
				byte_t		ttccode=16;

				writeBE(&reqpacket,dataflags);
				write(&reqpacket,ttccode);

				if (getDebug()) {
					debugStart("fetch response header");
					debugWrite("data flags: 0x%04x",
								dataflags);
					debugTtcCode(ttccode);
					debugEnd();
				}
			}

			// send column definitions...
			if (define) {
				putColumnDefinitions(cursor,colcount,false);
			}

			// send "iov" (whatever that is)...
			if (sndiov) {
				putIov();
			} else {
				const byte_t	unknown[]={
					0x00, 0x00,
				};
				reqpacket.append(unknown,sizeof(unknown));
			}

			// always appears to be the same...
			const byte_t	unknown2[]={
				0x06, 0x02,
			};
			reqpacket.append(unknown2,sizeof(unknown2));

			// FIXME: this varies, but it's not clear with what
			const byte_t	unknown3[]={
				0x8C
			};
			reqpacket.append(unknown3,sizeof(unknown3));

			write(&reqpacket,(byte_t)colcount);

			// always appears to be the same...
			const byte_t	unknown4[]={
				0x00, 0x00, 0x00,
				0x01, 0x00, 0x00, 0x00
			};
			reqpacket.append(unknown4,sizeof(unknown4));
		}

		// no idea...
		write(&reqpacket,(byte_t)7);

		debugStart("fetch response row");

		putRow(cursor,colcount,exactfetch);

		debugEnd();

		// FIXME: kludgy
		cont->nextRow(cursor);

		rowsfetched++;

	} while (rowsfetched<rowstofetch);

	if (rowsfetched) {

		if (exactfetch) {
			const byte_t	unknown5[]={
				// ???
				0x08, 0x04, 0x00
			};

			const byte_t	unknown6[][1]={
				// 1 columns
				{0xA6},
				// 2 columns
				{0xA5}
			};

			const byte_t	unknown7[]={
				0x5C,
	
				// error code?
				0x16, 0x00,
	
				// ???
				0x00, 0x00, 0x00, 0x00,
				0x01,
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x04,
				0x01, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x01, 0x00
			};
	
			const byte_t	unknown8[][4]={
				// 1 column
				{0x11, 0x00, 0x03, 0x00},
				// 2 columns
				{0x0E, 0x00, 0x03, 0x00}
			};
	
			const byte_t	unknown9[]={
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00,
				0x00,
		
				// ???
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00
			};
	
			// this apears to increment with each response
			const byte_t	unknown10[]={
				0x27
			};
	
			const byte_t	unknown11[]={
 				0x00, 0x00, 0x01,
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00,
				0x00
			};
	
			reqpacket.append(unknown5,sizeof(unknown5));
			reqpacket.append(unknown6[colcount-1],
						sizeof(unknown6[colcount-1]));
			reqpacket.append(unknown7,sizeof(unknown7));
			reqpacket.append(unknown8[colcount-1],
						sizeof(unknown8[colcount-1]));
			reqpacket.append(unknown9,sizeof(unknown9));
			reqpacket.append(unknown10,sizeof(unknown10));
			reqpacket.append(unknown11,sizeof(unknown11));
	
		} else {
	
			const byte_t	unknown[]={
				0x04, 0x01, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
				0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00,
				0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
			};
	
			reqpacket.append(unknown,sizeof(unknown));
		}

	} else {

		// if we hit the end of the result set then we need to send
		// ORA-01403; no data found
		putError("ORA-01403: no data found");
	}

	putGenericFooter();
	
	return sendPacket(true);
}

void sqlrprotocol_oracle::cacheColumnDefinitions(sqlrservercursor *cursor,
							uint32_t colcount) {
	debugStart("cache column definitions");
	if (!colcount) {
		debugWrite("no columns");
	}

	uint16_t	curid=cont->getId(cursor);

	if (columntypescached[curid]) {
		debugWrite("already cached");
		debugEnd();
		return;
	}

	if (!cont->getMaxColumnCount()) {
		delete[] columntypes[curid];
		if (colcount) {
			columntypes[curid]=new uint16_t[colcount];
		} else {
			columntypes[curid]=NULL;
		}
	}

	uint16_t	*ct=columntypes[curid];

	for (uint32_t i=0; i<colcount; i++) {
		ct[i]=getColumnType(cont->getColumnTypeName(cursor,i),
					cont->getColumnTypeNameSize(cursor,i),
					cont->getColumnScale(cursor,i));
		debugWrite("%s: %d",cont->getColumnTypeName(cursor,i),ct[i]);
	}

	columntypescached[curid]=true;

	debugEnd();
}

void sqlrprotocol_oracle::putColumnDefinitions(sqlrservercursor *cursor,
							uint32_t colcount,
							bool query3) {

	byte_t	sizetotal=0;
	for (uint32_t i=0; i<colcount; i++) {
		sizetotal+=cont->getColumnSize(cursor,i);
	}
	uint32_t	constant=51;

	write(&reqpacket,sizetotal);
	writeBE(&reqpacket,colcount);
	writeBE(&reqpacket,constant);

	debugStart("column definitions header");
	debugWrite("size total: %d",sizetotal);
	debugWrite("column count: %d",colcount);
	debugWrite("constant: %d",constant);
	debugEnd();

	debugStart("column definitions");

	for (uint32_t i=0; i<colcount; i++) {
		putColumnDefinition(cursor,i,query3);
	}

	debugEnd();
}

void sqlrprotocol_oracle::putColumnDefinition(sqlrservercursor *cursor,
							uint32_t column,
							bool query3) {
	uint16_t	curid=cont->getId(cursor);

	//uint16_t	sqlrcolumntype=cont->getColumnType(cursor,column);
	const char	*columntypestring=
				cont->getColumnTypeName(cursor,column);
	uint16_t	columntype=columntypes[curid][column];
	/*uint16_t	columnflags=getColumnFlags(cursor,column,
							sqlrcolumntype,
							columntype,
							columntypestring);*/

	// no idea
	byte_t	marker1=1;
	// no idea - 128 for char/varchar, 00 for numeric
	byte_t	marker2=128;
	byte_t	precision=cont->getColumnPrecision(cursor,column);
	byte_t	scale=cont->getColumnScale(cursor,column);
	// 16 for non-integer decimal, otherwise actual size
	byte_t	size=cont->getColumnSize(cursor,column);
	byte_t	unknown1[]={
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00
	};
	// no idea - 1 for char/varchar, 0 for numeric
	uint16_t	marker3=1;
	uint16_t	nullable=(cont->getColumnIsNullable(cursor,column))?1:0;
	// 1 for select from table
	// 2 for select from table with alias
	// 3 for select from dual
	// 4 for select from dual with alias
	byte_t		alias=1;
	const char	*name=cont->getColumnName(cursor,column);
	uint32_t	namesize=cont->getColumnNameSize(cursor,column);
	// no idea
	uint32_t	marker4=0;
	// no idea
	uint32_t	marker5=0;

	write(&reqpacket,marker1);
	write(&reqpacket,(byte_t)columntype);
	write(&reqpacket,marker2);
	write(&reqpacket,precision);
	write(&reqpacket,scale);
	write(&reqpacket,size);
	reqpacket.append(unknown1,sizeof(unknown1));
	// yes, twice
	writeBE(&reqpacket,marker3);
	writeBE(&reqpacket,marker3);
	writeBE(&reqpacket,nullable);
	// yes, twice
	if (query3) {
		write(&reqpacket,(byte_t)namesize);
		write(&reqpacket,(byte_t)namesize);
	} else {
		write(&reqpacket,alias);
		write(&reqpacket,alias);
	}
	writeBE(&reqpacket,namesize);
	write(&reqpacket,name,namesize);
	writeBE(&reqpacket,marker4);
	writeBE(&reqpacket,marker5);

	debugStart("column %d",column);
	debugWrite("marker1: %d",marker1);
	debugColumnType(columntypestring,columntype);
	debugWrite("marker2: %d",marker2);
	debugWrite("precision: %d",precision);
	debugWrite("scale: %d",scale);
	debugWrite("size: %d",size);
	debugWrite("marker3: %d",marker3);
	debugWrite("nullable: %d",nullable);
	debugWrite("name size: %ld",namesize);
	debugWrite("name: %s",name);
	debugWrite("marker4: %d",marker4);
	debugWrite("marker5: %d",marker5);
	debugEnd();
}

uint16_t sqlrprotocol_oracle::getColumnType(const char *columntypestring,
						uint16_t columntypesize,
						uint32_t scale) {

	// sometimes column types have parentheses, like CHAR(40)
	const char	*leftparen=charstring::findFirst(columntypestring,"(");
	if (leftparen) {
		columntypesize=leftparen-columntypestring;
	}

	const char * const 	*datatypestring=cont->dataTypeStrings();

	for (uint32_t index=0; datatypestring[index]; index++) {

		// compare "columntypesize" bytes but also make sure that the
		// byte afterward is a NULL, we don't want "DATE" to match
		// "DATETIME" for example
		if (!charstring::compareIgnoringCase(
					datatypestring[index],
					columntypestring,
					columntypesize) &&
				datatypestring[index][columntypesize]=='\0') {

			uint16_t	retval=oracletypemap[index];

			// Some DB's, like oracle, don't distinguish between
			// decimal and integer types, they just have a numeric
			// field which may or may not have decimal points.
			// Those fields types get translated to "decimal"
			// but if there are 0 decimal points, then we need to
			// translate them to an integer type here.
			// FIXME:
			/*if ((retval==MYSQL_TYPE_DECIMAL ||
				retval==MYSQL_TYPE_NEWDECIMAL) && !scale) {
				retval=MYSQL_TYPE_LONG;
			}*/

			// Some DB's, like oracle, don't have separate DATE
			// and DATETIME types.  Rather, a DATE can store the
			// date and time, but which components it reports
			// depends on something like the NLS_DATE_FORMAT.  By
			// default, we map DATE to MYSQL_TYPE_DATE, but we also
			// provide the option of mapping it to
			// MYSQL_TYPE_DATETIME.
			// FIXME:
			/*if (retval==MYSQL_TYPE_DATE && datetodatetime) {
				retval=MYSQL_TYPE_DATETIME;
			}*/
			return retval;
		}
	}
	// FIXME:
	//return MYSQL_TYPE_NULL;
	return ORACLE_TYPE_VARCHAR;
}
		
uint16_t sqlrprotocol_oracle::getColumnFlags(sqlrservercursor *cursor,
						uint32_t column,
						uint16_t sqlrcolumntype,
						uint16_t columntype,
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

uint16_t sqlrprotocol_oracle::getColumnFlags(sqlrservercursor *cursor,
						uint16_t sqlrcolumntype,
						uint16_t columntype,
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
	/*if (!isnullable) {
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
	}*/
	return flags;
}

void sqlrprotocol_oracle::putIov() {

	// always appears to be the same...
	const byte_t	unknown[]={
		0x07, 0x00, 0x00, 0x00,
		// timestamp?  if so, it's
		// 12/21/1973 10:13:14 EST
		0x07, 0x78, 0x75, 0x0A
	};
	reqpacket.append(unknown,sizeof(unknown));

	// this appears to be a seconds-since timestamp
	// what's the significance of this date?
	// it's suspiciously close to my 8.0.5
	// software/db install date,
	// which was Oct 4, 2012.
	datetime	dtsince;
	dtsince.init("12/15/2012 11:15:00 EST");
	datetime	dt;
	dt.initFromSystemDateTime();
	uint32_t	timestamp=dt.getEpoch()-dtsince.getEpoch();
	writeBE(&reqpacket,timestamp);
}

void sqlrprotocol_oracle::putRow(sqlrservercursor *cursor,
						uint32_t colcount,
						bool terminator) {

	// get the column types
	uint16_t	*ct=columntypes[cont->getId(cursor)];

	// field pointers
	const char	*field;
	uint64_t	fieldsize;
	bool		lob;
	bool		null;

	// put the fields
	for (uint32_t i=0; i<colcount; i++) {

		debugStart("col %d",i);
		debugColumnType(ct[i]);

		// get the field (again)
		fieldsize=0;
		lob=false;
		null=false;
		if (!cont->getField(cursor,i,&field,&fieldsize,&lob,&null)) {
			// FIXME: handle error
		}

		// put the field
		if (lob) {
			debugWrite("LOB");
			putLobField(cursor,i);
		} else if (!null) {
			debugWrite("\"%s\" (%d)",field,fieldsize);
			putField(field,fieldsize,ct[i]);

			// no idea
			if (terminator) {
				if (i==colcount-1) {
					writeBE(&reqpacket,(uint16_t)0);
				} else {
					writeBE(&reqpacket,(uint32_t)0);
				}
			}
		}

		debugEnd();
	}
}

void sqlrprotocol_oracle::putField(const char *field,
					uint64_t fieldsize,
					uint16_t columntype) {

	switch (columntype) {
		case ORACLE_TYPE_CHAR:
		case ORACLE_TYPE_VARCHAR:
			// what about fields longer than 255 chars?
			write(&reqpacket,(byte_t)fieldsize);
			write(&reqpacket,field,fieldsize);
			break;
		case ORACLE_TYPE_NUMBER:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_VARNUM:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_LONG:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_ROWID_DEPRECATED:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_DATE:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_RAW:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_LONG_RAW:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_RESULT_SET:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_ROWID:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_NAMED_TYPE:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_REF_TYPE:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_CLOB:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_BLOB:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_BFILE:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_TIMESTAMP:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_TIMESTAMPTZ:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_INTERVALYM:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_INTERVALDS:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_TIMESTAMPLTZ:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_PLSQL_INDEX_TABLE:
			// FIXME: implement this
			break;
		case ORACLE_TYPE_FIXED_CHAR:
			// FIXME: implement this
			break;
		default:
			debugWrite("unknown column type: %d",columntype);
			break;
	}
}

#define MAX_BYTES_PER_CHAR	4

void sqlrprotocol_oracle::putLobField(sqlrservercursor *cursor, uint32_t col) {

	// Get lob size.  If this fails, send a NULL field.
	uint64_t	loblength;
	if (!cont->getLobFieldLength(cursor,col,&loblength)) {
		// send NULL as 0xfb
		reqpacket.append((char)0xfb);
		cont->closeLobField(cursor,col);
		return;
	}

	// for lobs of 0 length
	if (!loblength) {
		writeLenEncInt(&reqpacket,0);
		cont->closeLobField(cursor,col);
		return;
	}

	// initialize sizes and status
	uint64_t	charstoread=sizeof(lobbuffer)/MAX_BYTES_PER_CHAR;
	uint64_t	charsread=0;
	uint64_t	offset=0;
	bool		start=true;

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
				reqpacket.append((char)0xfb);
			}
			cont->closeLobField(cursor,col);
			return;

		} else {

			// if we haven't started sending yet, then do that now
			if (start) {
				writeLenEncInt(&reqpacket,loblength);
				start=false;
			}

			// put the segment we just got
			reqpacket.append(lobbuffer,charsread);

			offset=offset+charstoread;
		}
	}
}

void sqlrprotocol_oracle::putError(const char *error) {
	putError(error,charstring::getLength(error));
}

void sqlrprotocol_oracle::putError(const char *error, uint32_t errorsize) {

	// if we hit the end of the result set then we need to send
	// ORA-01403; no data found

	uint16_t	dataflags=0;
	byte_t		ttccode=TTI_EXECUTE;

	const byte_t	unknown1[]={
		0x00, 0x00, 0x00, 0x00, 0x7B,
		0x05, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
		0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,

		// varies... sequence?
		0x10,

		0x00, 0x00, 0x01, 0x00, 0x00, 0x00
	};

	const byte_t	unknown2[]={
		0x0A
	};

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);
	reqpacket.append(unknown1,sizeof(unknown1));
	write(&reqpacket,(byte_t)errorsize);
	reqpacket.append(error,errorsize);
	reqpacket.append(unknown2,sizeof(unknown2));

	debugStart("error response");
	debugWrite("data flags: 0x%04x",dataflags);
	debugTtcCode(ttccode);
	debugWrite("error: %*s",errorsize,error);
	debugEnd();
}

bool sqlrprotocol_oracle::close(const byte_t *rp) {

	uint16_t	cursorid;

	// FIXME: decode this...
cursorid=hackcursorid;

	debugStart("close request");
	debugWrite("cursor id: %d",cursorid);
	debugEnd();

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(cursorid);
	if (!cursor) {
		debugWrite("cursor id %d not found",cursorid);
		return sendCursorNotOpenError();
	}

	clearParams(cursor);
	pcounts[cont->getId(cursor)]=0;
	cont->abort(cursor);
	cont->release(cursor);
hackcursorid=65535;

	return sendCloseResponse(cursor);
}

void sqlrprotocol_oracle::clearParams(sqlrservercursor *cursor) {

	uint16_t		pcount=cont->getInputBindCount(cursor);
	sqlrserverbindvar	*inbinds=cont->getInputBinds(cursor);

	for (uint16_t i=0; i<pcount; i++) {
		sqlrserverbindvar	*bv=&(inbinds[i]);
		if (bv->type==SQLRSERVERBINDVARTYPE_STRING ||
			bv->type==SQLRSERVERBINDVARTYPE_BLOB) {
			delete[] bv->value.stringval;
		}
	}
	cont->setInputBindCount(cursor,0);
}

bool sqlrprotocol_oracle::sendCloseResponse(sqlrservercursor *cursor) {

	resetSendPacketBuffer(PACKET_DATA);

	uint16_t	dataflags=0;
	// FIXME: not a valid ttccode type...
	byte_t		ttccode=9;

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);

	debugStart("close response");
	debugWrite("data flags: 0x%04x",dataflags);
	debugTtcCode(ttccode);
	debugEnd();

	return sendPacket(true);
}

bool sqlrprotocol_oracle::disconnect(const byte_t *rp) {

	// no idea
	byte_t	unknown;
	read(rp,&unknown,&rp);

	debugStart("disconnect request");
	debugWrite("unknown: 0x%02x",unknown);
	debugEnd();

#if 0
	// FIXME: do something?
	uint16_t	cursorid=hackcursorid;
	hackcursorid=65535;
#endif

	return sendDisconnectResponse();
}

bool sqlrprotocol_oracle::sendDisconnectResponse() {

	resetSendPacketBuffer(PACKET_DATA);

	uint16_t	dataflags=0;
	// FIXME: not a valid ttccode type...
	byte_t		ttccode=9;

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);

	debugStart("disconnect response");
	debugWrite("data flags: 0x%04x",dataflags);
	debugTtcCode(ttccode);
	debugEnd();

	return sendPacket(true);
}

bool sqlrprotocol_oracle::cancel(const byte_t *rp) {
	// FIXME: implement this
	return false;
}

bool sqlrprotocol_oracle::version(const byte_t *rp) {
	
	// FIXME: decode this...

	debugStart("version request");
	debugEnd();

	return sendVersionResponse();
}

bool sqlrprotocol_oracle::sendVersionResponse() {

	resetSendPacketBuffer(PACKET_DATA);

	uint16_t	dataflags=0;
	byte_t		ttccode=TTC_OK;
	// FIXME: get this from the db
	const char	*serverversion=
			"Oracle8 Release 8.0.5.0.0 - Production\n"
			"PL/SQL Release 8.0.5.0.0 - Production";
	byte_t		unknown[]={
		0x50, 0x00, 0x08, 0x09
	};

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);
	// writeLE?
	writeHost(&reqpacket,(uint16_t)charstring::getLength(serverversion));
	write(&reqpacket,serverversion);
	reqpacket.append(unknown,sizeof(unknown));

	debugStart("version response");
	debugWrite("data flags: 0x%04x",dataflags);
	debugTtcCode(ttccode);
	debugWrite("server version:");
	debugWrite("%s",serverversion);
	debugEnd();

	return sendPacket(true);
}

bool sqlrprotocol_oracle::occa(const byte_t *rp, const byte_t **rpout) {

	// no idea what this is at all,
	// and it doesn't appear to have a response
	
	// FIXME: decode this...
	byte_t	unknown[11];
	for (uint16_t i=0; i<sizeof(unknown); i++) {
		read(rp,&(unknown[i]),&rp);
	}

	*rpout=rp;

	debugStart("occa");
	debugHexDump(unknown,sizeof(unknown));
	debugEnd();

	return true;
}

bool sqlrprotocol_oracle::logonUnknown(const byte_t *rp) {

	// no idea

	// FIXME: decode this...

	debugStart("logon unknown request");
	debugEnd();

	return sendLogonUnknownResponse();
}

bool sqlrprotocol_oracle::sendLogonUnknownResponse() {

	resetSendPacketBuffer(PACKET_DATA);

	uint16_t	dataflags=0;
	byte_t		ttccode=TTC_OK;

	// no idea
	byte_t	unknown[]={
		0x0C,
		0x00, 0x00, 0x00, 0x67,
		0x70, 0x00,
		0x00, 0x00, 0x00, 0x09
	};

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);
	reqpacket.append(unknown,sizeof(unknown));

	debugStart("logon unknown response");
	debugWrite("data flags: 0x%04x",dataflags);
	debugTtcCode(ttccode);
	debugEnd();

	return sendPacket(true);
}


void sqlrprotocol_oracle::putGenericFooter() {

	// no idea...

	// 8i server sends this to 8i client
	//
	// 8.0.5 server doesn't send it to 8.0.5 client,
	// but 8.0.5 client tolerates it
	byte_t	footer[]={
		0x00,
		0x36, 0x01, 0x00, 0x00, 0xA0, 0x0D, 0x6C, 0x09,
		0xC0, 0x54, 0x6C, 0x09, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	reqpacket.append(footer,sizeof(footer));
}

bool sqlrprotocol_oracle::sendQueryError(sqlrservercursor *cursor) {

	// FIXME: implement this;

	const char	*errorstring;
	uint32_t	errorsize;
	int64_t		errnum;
	bool		liveconnection;
	cont->getError(cursor,
			&errorstring,
			&errorsize,
			&errnum,
			&liveconnection);
	debugWrite("%*s",errorsize,errorstring);
	return false;
}

bool sqlrprotocol_oracle::sendCursorNotOpenError() {
	// FIXME: implement this;
	return false;
}

bool sqlrprotocol_oracle::sendNotImplementedError() {
	// FIXME: implement this;
	return false;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrprotocol	*new_sqlrprotocol_oracle(
						sqlrservercontroller *cont,
						domnode *parameters) {
		return new sqlrprotocol_oracle(cont,parameters);
	}
}
