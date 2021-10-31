// Copyright (c) 2017  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/bytebuffer.h>
#include <rudiments/character.h>
#include <rudiments/randomnumber.h>
#include <rudiments/datetime.h>
#include <rudiments/process.h>
#include <rudiments/error.h>

#include <fwpk/fwpkincludes.h>

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
							sqlrprotocols *ps,
							domnode *parameters);
		virtual	~sqlrprotocol_oracle();

		clientsessionexitstatus_t	clientSession(
							filedescriptor *cs);

	private:
		void	init();
		void	free();
		void	reInit();

		void	resetSendPacketBuffer(unsigned char packettype);
		bool	sendPacket();
		bool	sendPacket(bool flush);
		bool	recvPacket();
		void	readHost(const unsigned char *rp,
					uint16_t *value,
					const unsigned char **rpout);
		void	readHost(const unsigned char *rp,
					uint32_t *value,
					const unsigned char **rpout);
		bool	readMarker8(const unsigned char *rp,
					unsigned char expected,
					const unsigned char **rpout);
		bool	readMarker16(const unsigned char *rp,
					uint16_t expected,
					const unsigned char **rpout);
		bool	readMarker32(const unsigned char *rp,
					uint32_t expected,
					const unsigned char **rpout);
		bool	getNullTerminatedArray(const unsigned char *rp,
						const unsigned char *end,
						unsigned char **array,
						uint32_t *arraycount,
						const unsigned char **rpout);
		bool	getString(const unsigned char *rp,
						const unsigned char *end,
						char **string,
						const unsigned char **rpout);
		bool	getString(const unsigned char *rp,
						char **string,
						uint32_t length,
						const unsigned char **rpout);

		void	writeHost(bytebuffer *buffer, uint16_t value);
		void	writeHost(bytebuffer *buffer, uint32_t value);

		void	generateAuthSessionKey(uint16_t bytes);


		// handshake...
		bool	initialHandshake();
		bool	connect();
		bool	recvConnectRequest();
		bool	sendConnectResponse();
		bool	sendAccept();
		bool	sendAccept(const unsigned char *data,
						uint16_t datalength);
		bool	sendResend();
		bool	sendRedirect();
		bool	sendRefuse();

		bool	anoNegotiation();
		bool	recvAnoRequest();
		bool	getAnoServiceHeader(const unsigned char *rp,
						uint16_t *service,
						uint16_t *fieldcount,
						const unsigned char **rpout);
		bool	getSupervisorService(const unsigned char *rp,
						uint16_t fieldcount,
						const unsigned char **rpout);
		bool	getAuthenticationService(const unsigned char *rp,
						uint16_t fieldcount,
						const unsigned char **rpout);
		bool	getEncryptionService(const unsigned char *rp,
						uint16_t fieldcount,
						const unsigned char **rpout);
		bool	getCryptoChecksummingService(const unsigned char *rp,
						uint16_t fieldcount,
						const unsigned char **rpout);
		bool	getAnoVersionField(const unsigned char *rp,
						uint32_t *version,
						const unsigned char **rpout);
		bool	getAnoConnectionInfoField(const unsigned char *rp,
						uint32_t *pid,
						uint32_t *connectiontype,
						const unsigned char **rpout);
		bool	getAnoArrayField(const unsigned char *rp,
						uint16_t **array,
						uint32_t *arraycount,
						const unsigned char **rpout);
		bool	getAnoConstantField(const unsigned char *rp,
						uint16_t *constant,
						const unsigned char **rpout);
		bool	getAnoConstantField(const unsigned char *rp,
						unsigned char *constant,
						const unsigned char **rpout);
		bool	getAnoStatusField(const unsigned char *rp,
						uint16_t *status,
						const unsigned char **rpout);
		bool	getAnoUnknownField(const unsigned char *rp,
						const unsigned char **rpout);
		bool		sendAnoResponse();
		uint16_t	putSupervisorService();
		uint16_t	putAuthenticationService();
		uint16_t	putEncryptionService();
		uint16_t	putCryptoChecksummingService();
		uint16_t	putAnoServiceHeader(uint16_t type,
							uint16_t fieldcount);
		uint16_t	putAnoVersionField(uint32_t version);
		uint16_t	putAnoStatusField(uint16_t status);
		uint16_t	putAnoConstant(unsigned char constant);
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

		void	debugTtcCode(unsigned char ttccode);
		void	debugTtiFunction(unsigned char ttifunction);
		void	debugOptions(uint16_t options, uint16_t moreoptions);
		void	debugOptions(uint16_t options);
		void	debugCharacterSet(unsigned char characterset);
		void	debugStatusFlags(uint16_t statusflags);
		void	debugColumnType(const char *name, uint16_t columntype);
		void	debugColumnType(uint16_t columntype);
		void	debugSystemError();

		bool	getTtiFunction(const unsigned char *rp,
						unsigned char *ttifunction,
						const unsigned char **rpout);

		// open...
		bool	open(const unsigned char *rp);
		bool	sendOpenResponse(sqlrservercursor *cursor);

		// query...
		bool	query(const unsigned char *rp);
		bool	sendQueryResponse(sqlrservercursor *cursor);
		bool	query2(const unsigned char *rp);
		bool	sendQuery2Response(sqlrservercursor *cursor,
							bool binds);
		bool	bindParameters(sqlrservercursor *cursor,
							uint16_t pcount,
							uint16_t *ptypes);
		bool	query3(const unsigned char *rp);
		bool	sendQuery3Response(sqlrservercursor *cursor,
							uint16_t options);

		// execute...
		bool	execute(const unsigned char *rp);
		bool	sendExecuteResponse(sqlrservercursor *cursor);

		// fetch...
		bool	fetch(const unsigned char *rp);
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
						uint16_t columntypelen,
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
						uint64_t fieldlength,
						uint16_t columntype);
		void	putLobField(sqlrservercursor *cursor, uint32_t col);
		void	putError(const char *error);
		void	putError(const char *error, uint32_t errorlength);

		// close...
		bool	close(const unsigned char *rp);
		void	clearParams(sqlrservercursor *cursor);
		bool	sendCloseResponse(sqlrservercursor *cursor);

		// disconnect...
		bool	disconnect(const unsigned char *rp);
		bool	sendDisconnectResponse();

		// cancel...
		bool	cancel(const unsigned char *rp);

		// version
		bool	version(const unsigned char *rp);
		bool	sendVersionResponse();

		// occa
		bool	occa(const unsigned char *rp,
				const unsigned char **rpout);

		// logon unknown
		bool	logonUnknown(const unsigned char *rp);
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

		unsigned char	*ttiversions;
		uint32_t	ttiversioncount;
		unsigned char	ttiversion;

		char		*clientstring;
		const char	*serverstring;

		const unsigned char	*datatypes;
		uint16_t		datatypeslength;

		filedescriptor	*clientsock;

		bytebuffer	reqpacket;
		unsigned char	reqpackettype;

		memorypool	*resppacketpool;
		unsigned char	*resppacket;
		uint16_t	resppacketsize;
		unsigned char	resppackettype;

		randomnumber	r;
		uint32_t	seed;

		char		*username;
		char		*authsessionkey;
		char		*response;
		uint64_t	responselength;

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
					sqlrprotocols *ps,
					domnode *parameters) :
					sqlrprotocol(cont,ps,parameters) {

	clientsock=NULL;

	if (getDebug()) {
		stdoutput.write("parameters {\n");
		stdoutput.write("}\n");
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
	datatypeslength=0;

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
	clientsock->dontUseNaglesAlgorithm();
	clientsock->setSocketReadBufferSize(65536);
	clientsock->setSocketWriteBufferSize(65536);
	clientsock->setReadBufferSize(65536);
	clientsock->setWriteBufferSize(65536);

	reInit();

	bool	endsession=true;

	clientsessionexitstatus_t	status=CLIENTSESSIONEXITSTATUS_ERROR;
	if (initialHandshake()) {

		// loop, getting and executing commands
		bool	loop=true;
		const unsigned char	*rp=NULL;
		do {

			// get the tti function...
			unsigned char		ttifunction;
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
			//cont->setState(cursor,SQLRCURSORSTATE_AVAILABLE);

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

void sqlrprotocol_oracle::resetSendPacketBuffer(unsigned char packettype) {
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
	unsigned char	packetflags=0;
	uint16_t	headerchecksum=0;

	// overwrite the first 8 bytes of the reqpacket with the packet header
	reqpacket.setPosition(0);
	reqpacket.write(hostToBE(reqpacketsize));
	reqpacket.write(hostToBE(packetchecksum));
	reqpacket.write(reqpackettype);
	reqpacket.write(packetflags);
	reqpacket.write(hostToBE(headerchecksum));

	if (getDebug()) {
		stdoutput.write("send {\n");
		stdoutput.printf("	packet size: %d\n",reqpacketsize);
		stdoutput.printf("	packet checksum: %d\n",packetchecksum);
		stdoutput.printf("	packet type: %d\n",reqpackettype);
		stdoutput.printf("	packet flags: 0x%04x\n",packetflags);
		stdoutput.printf("	header checksum: %d\n",headerchecksum);
		debugHexDump(reqpacket.getBuffer()+8,reqpacketsize-8);
		stdoutput.write("}\n");
	}

	// send the packet
	if (clientsock->write(reqpacket.getBuffer(),
				reqpacket.getSize())!=
				(ssize_t)reqpacket.getSize()) {
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

	return true;
}

bool sqlrprotocol_oracle::recvPacket() {

	// size
	// 2 bytes (big endian)
	if (clientsock->read(&resppacketsize)!=sizeof(uint16_t)) {
		if (getDebug()) {
			stdoutput.write("read packet size failed\n");
			debugSystemError();
		}
		return false;
	}
	resppacketsize=beToHost(resppacketsize);

	// sanity check
	if (resppacketsize<8 || resppacketsize>sdu) {
		if (getDebug()) {
			stdoutput.printf("invalid packet size: %d\n",
							resppacketsize);
			debugSystemError();
		}
		return false;
	}

	// packet checksum
	// 2 bytes (big endian) (always 0)
	uint16_t	packetchecksum;
	if (clientsock->read(&packetchecksum)!=sizeof(uint16_t)) {
		if (getDebug()) {
			stdoutput.write("read packet checksum failed\n");
			debugSystemError();
		}
		return false;
	}
	packetchecksum=beToHost(packetchecksum);

	// packet type
	// 1 byte
	if (clientsock->read(&resppackettype)!=sizeof(unsigned char)) {
		if (getDebug()) {
			stdoutput.write("read packet type failed\n");
			debugSystemError();
		}
		return false;
	}

	// packet flags
	// 1 byte
	unsigned char	packetflags;
	if (clientsock->read(&packetflags)!=sizeof(unsigned char)) {
		if (getDebug()) {
			stdoutput.write("read packet flags failed\n");
			debugSystemError();
		}
		return false;
	}

	// header checksum
	// 2 bytes (big endian) (always 0)
	uint16_t	headerchecksum;
	if (clientsock->read(&headerchecksum)!=sizeof(uint16_t)) {
		if (getDebug()) {
			stdoutput.write("read header checksum failed\n");
			debugSystemError();
		}
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
		if (getDebug()) {
			stdoutput.write("read packet failed\n");
			debugSystemError();
		}
		return false;
	}

	if (getDebug()) {
		stdoutput.write("recv {\n");
		stdoutput.printf("	packet size: %d\n",resppacketsize+8);
		stdoutput.printf("	packet checksum: %d\n",packetchecksum);
		stdoutput.printf("	packet type: %d\n",resppackettype);
		stdoutput.printf("	packet flags: %d\n",packetflags);
		stdoutput.printf("	header checksum: %d\n",headerchecksum);
		debugHexDump(resppacket,resppacketsize);
		stdoutput.write("}\n");
	}

	return true;
}

void sqlrprotocol_oracle::readHost(const unsigned char *rp,
					uint16_t *value,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,sizeof(uint16_t));
	*rpout=rp+sizeof(uint16_t);
}

void sqlrprotocol_oracle::readHost(const unsigned char *rp,
					uint32_t *value,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,sizeof(uint32_t));
	*rpout=rp+sizeof(uint32_t);
}

bool sqlrprotocol_oracle::readMarker8(const unsigned char *rp,
					unsigned char expected,
					const unsigned char **rpout) {
	unsigned char	marker;
	return read(rp,&marker,"marker",expected,rpout);
}

bool sqlrprotocol_oracle::readMarker16(const unsigned char *rp,
					uint16_t expected,
					const unsigned char **rpout) {
	uint16_t	marker;
	return readBE(rp,&marker,"marker",expected,rpout);
}

bool sqlrprotocol_oracle::readMarker32(const unsigned char *rp,
					uint32_t expected,
					const unsigned char **rpout) {
	uint32_t	marker;
	return readBE(rp,&marker,"marker",expected,rpout);
}

bool sqlrprotocol_oracle::getNullTerminatedArray(const unsigned char *rp,
						const unsigned char *end,
						unsigned char **array,
						uint32_t *arraycount,
						const unsigned char **rpout) {
	*array=NULL;
	*arraycount=0;

	// count the array items
	const unsigned char	*start=rp;
	for (;;) {
		if (!(*rp)) {
			break;
		}
		if (rp==end) {
			if (getDebug()) {
				stdoutput.printf("bad null terminated array, "
						"no null terminator found\n");
			}
			return false;
		}
		(*arraycount)++;
		rp++;
	}

	// reset rp
	rp=start;

	// get the array items
	*array=new unsigned char[*arraycount];
	for (uint32_t i=0; i<*arraycount; i++) {
		read(rp,&((*array)[i]),&rp);
	}

	// skip the null terminator
	rp++;
	
	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getString(const unsigned char *rp,
					const unsigned char *end,
					char **string,
					const unsigned char **rpout) {
	*string=NULL;
	uint32_t	stringlength=0;

	// count the characters
	const unsigned char	*start=rp;
	for (;;) {
		if (!(*rp)) {
			break;
		}
		if (rp==end) {
			if (getDebug()) {
				stdoutput.printf("bad string, "
						"no null terminator found\n");
			}
			return false;
		}
		stringlength++;
		rp++;
	}

	// reset rp
	rp=start;

	// get the string
	*string=new char[stringlength+1];
	for (uint32_t i=0; i<stringlength; i++) {
		read(rp,&((*string)[i]),&rp);
	}
	(*string)[stringlength]='\0';

	// skip the null terminator
	rp++;
	
	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getString(const unsigned char *rp,
					char **string,
					uint32_t length,
					const unsigned char **rpout) {
	*string=charstring::duplicate((const char *)rp,length);
	*rpout=rp+length;
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
		r.generateNumber(&number);
		int32_t	scalednumber=randomnumber::scaleNumber(number,0,16);
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
		if (getDebug()) {
			stdoutput.printf("bad packet type %d, expected %d\n",
						resppackettype,PACKET_CONNECT);
		}
		return false;
	}

	const unsigned char	*rp=resppacket;

	uint16_t	protocolcharacteristics;
	uint16_t	maxpacketsbeforeack;
	uint16_t	one;
	uint16_t	connectdatalength;
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
	readBE(rp,&connectdatalength,&rp);
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

	if (getDebug()) {
		stdoutput.write("connect {\n");
		stdoutput.printf("	version: 0x%04x\n",connectversion);
		stdoutput.printf("	lowest supported version: 0x%04x\n",
							connectlowestversion);
		stdoutput.printf("	gso: 0x%04x\n",gso);
		stdoutput.printf("	sdu: %d\n",sdu);
		stdoutput.printf("	tdu: %d\n",tdu);
		stdoutput.printf("	protocol characteristics: 0x%04x\n",
						protocolcharacteristics);
		stdoutput.printf("	max packets before ack: %d\n",
						maxpacketsbeforeack);
		stdoutput.printf("	client is little endian: %d\n",
						(one==1));
		stdoutput.printf("	connect data length: %d\n",
						connectdatalength);
		stdoutput.printf("	connect data offset: %d\n",
						connectdataoffset);
		stdoutput.printf("	max connect data that "
					"can be received: %d\n",
					maxconnectdatathatcanbereceived);
		stdoutput.printf("	ANO flags: 0x%04x\n",anoflags);
		stdoutput.printf("	trace cross facility item 1: 0x%08x\n",
					tracecrossfacilityitem1);
		stdoutput.printf("	trace cross facility item 2: 0x%08x\n",
					tracecrossfacilityitem2);
		stdoutput.printf("	trace unique connection id 1: "
					"0x%016x\n",
					traceuniqueconnectionid1);
		stdoutput.printf("	trace unique connection id 2: "
					"0x%016x\n",
					traceuniqueconnectionid2);
		stdoutput.printf("	connect data: %*s\n",
					connectdatalength,connectdata);
		stdoutput.write("}\n");
	}

	return true;
}

bool sqlrprotocol_oracle::sendConnectResponse() {

	// look for a protocol version that we support
	// FIXME: we currently only support PROTOCOL_VERSION_8
	if (connectlowestversion<=PROTOCOL_VERSION_8) {
		connectversion=PROTOCOL_VERSION_8;
	} else {
		if (getDebug()) {
			stdoutput.printf("no supported connect "
						"protocol version found\n");
		}
		sendRefuse();
		return false;
	}

	return sendAccept();
}

bool sqlrprotocol_oracle::sendAccept() {
	return sendAccept(NULL,0);
}

bool sqlrprotocol_oracle::sendAccept(const unsigned char *data,
						uint16_t datalength) {

	// data offset:
	// (8 bytes for the header +
	//  16 bytes of this stuff +
	//  8 bytes of padding)
	uint16_t	dataoffset=32;
	// NOTE: 0x0801 returned by 8.0 server
	//gso=0x0801;
	uint64_t	padding=0;

	// debug
	if (getDebug()) {
		stdoutput.write("accept {\n");
		stdoutput.printf("	version: 0x%04x\n",connectversion);
		stdoutput.printf("	gso: 0x%04x\n",gso);
		stdoutput.printf("	sdu: %d\n",sdu);
		stdoutput.printf("	tdu: %d\n",tdu);
		stdoutput.printf("	data length: %d\n",datalength);
		stdoutput.printf("	data offset: %d\n",dataoffset);
		stdoutput.printf("	ANO flags: 0x%04x\n",anoflags);
		debugHexDump(data,datalength);
		stdoutput.write("}\n");
	}

	// build packet
	resetSendPacketBuffer(PACKET_ACCEPT);
	writeBE(&reqpacket,connectversion);
	writeBE(&reqpacket,gso);
	writeBE(&reqpacket,sdu);
	writeBE(&reqpacket,tdu);
	// writeLE?
	writeHost(&reqpacket,(uint16_t)1);
	writeBE(&reqpacket,datalength);
	writeBE(&reqpacket,dataoffset);
	writeBE(&reqpacket,anoflags);
	writeBE(&reqpacket,padding);
	reqpacket.append(data,datalength);

	return sendPacket(true);
}

bool sqlrprotocol_oracle::sendResend() {

	// debug
	if (getDebug()) {
		stdoutput.write("resend {\n");
		stdoutput.write("}\n");
	}

	// build packet
	resetSendPacketBuffer(PACKET_RESEND);

	return sendPacket(true);
}

bool sqlrprotocol_oracle::sendRedirect() {

	// debug
	if (getDebug()) {
		stdoutput.write("redirect {\n");
		stdoutput.write("}\n");
	}

	// build packet
	resetSendPacketBuffer(PACKET_REDIRECT);
	// FIXME: implement this...

	return sendPacket(true);
}

bool sqlrprotocol_oracle::sendRefuse() {

	// debug
	if (getDebug()) {
		stdoutput.write("refuse {\n");
		stdoutput.write("}\n");
	}

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
		if (getDebug()) {
			stdoutput.printf("bad packet type %d, expected %d\n",
						resppackettype,PACKET_DATA);
		}
		return false;
	}

	const unsigned char	*rp=resppacket;
	//const unsigned char	*end=rp+resppacketsize;

	// ano request header...
	uint16_t	dataflags;
	uint16_t	overalllength;
	uint16_t	servicecount;
	unsigned char	desiredoptionsflag;

	readBE(rp,&dataflags,&rp);
	if (!readMarker32(rp,0xdeadbeef,&rp)) {
		return false;
	}
	readBE(rp,&overalllength,&rp);
	readBE(rp,&anorequestversion,&rp);
	readBE(rp,&servicecount,&rp);
	read(rp,&desiredoptionsflag,&rp);

	if (getDebug()) {
		stdoutput.write("ano request header {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		stdoutput.printf("	overall length: %d\n",overalllength);
		stdoutput.printf("	version: 0x%08x\n",anorequestversion);
		stdoutput.printf("	service count %d\n",servicecount);
		stdoutput.printf("	desired options flag: 0x%02x\n",
							desiredoptionsflag);
	}

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
				if (getDebug()) {
					stdoutput.printf("bad ano service:"
								" %d\n",
								service);
				}
				break;
		}
		if (!success) {
			break;
		}
	}

	if (getDebug()) {
		stdoutput.write("}\n");
	}

	// bail if something failed
	if (!success) {
		return false;
	}

	return true;
}

bool sqlrprotocol_oracle::getAnoServiceHeader(const unsigned char *rp,
						uint16_t *service,
						uint16_t *fieldcount,
						const unsigned char **rpout) {

	readBE(rp,service,&rp);
	readBE(rp,fieldcount,&rp);
	if (!readMarker32(rp,0x00000000,&rp)) {
		return false;
	}

	if (getDebug()) {
		stdoutput.write("	ano service header {\n");
		stdoutput.printf("		service: %d\n",*service);
		stdoutput.printf("		field count: %d\n",*fieldcount);
		stdoutput.write("	}\n");
	}

	*rpout=rp;

	return true;
}


bool sqlrprotocol_oracle::getSupervisorService(const unsigned char *rp,
						uint16_t fieldcount,
						const unsigned char **rpout) {

	if (getDebug()) {
		stdoutput.write("	supervisor {\n");
	}

	uint32_t	pid;
	uint32_t	connectiontype;
	uint16_t	*drivers;
	uint32_t	drivercount;
	if (!getAnoVersionField(rp,&supervisorversion,&rp) ||
		!getAnoConnectionInfoField(rp,&pid,&connectiontype,&rp) ||
		!getAnoArrayField(rp,&drivers,&drivercount,&rp)) {
		delete[] drivers;
		return false;
	}

	if (getDebug()) {
		stdoutput.write("	}\n");
	}
	delete[] drivers;

	*rpout=rp;
	
	return true;
}

bool sqlrprotocol_oracle::getAuthenticationService(const unsigned char *rp,
						uint16_t fieldcount,
						const unsigned char **rpout) {

	if (getDebug()) {
		stdoutput.write("	authentication {\n");
	}

	uint16_t	constant;
	uint16_t	status;
	if (!getAnoVersionField(rp,&authenticationversion,&rp) ||
		!getAnoConstantField(rp,&constant,&rp) ||
		!getAnoStatusField(rp,&status,&rp)) {
		return false;
	}

	if (getDebug()) {
		stdoutput.write("	}\n");
	}

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getEncryptionService(const unsigned char *rp,
						uint16_t fieldcount,
						const unsigned char **rpout) {

	if (getDebug()) {
		stdoutput.write("	encryption {\n");
	}

	uint16_t	*drivers;
	uint32_t	drivercount;
	unsigned char	constant;
	if (!getAnoVersionField(rp,&encryptionversion,&rp) ||
		!getAnoArrayField(rp,&drivers,&drivercount,&rp)) {
		delete[] drivers;
		return false;
	}
	if (fieldcount>2) {
		if (!getAnoConstantField(rp,&constant,&rp)) {
			delete[] drivers;
			return false;
		} 
	}

	if (getDebug()) {
		stdoutput.write("	}\n");
	}
	delete[] drivers;

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getCryptoChecksummingService(
						const unsigned char *rp,
						uint16_t fieldcount,
						const unsigned char **rpout) {

	if (getDebug()) {
		stdoutput.write("	crypto-checksumming {\n");
	}

	uint16_t	*drivers;
	uint32_t	drivercount;
	if (!getAnoVersionField(rp,&cryptochecksummingversion,&rp) ||
		!getAnoArrayField(rp,&drivers,&drivercount,&rp)) {
		delete[] drivers;
		return false;
	}

	if (getDebug()) {
		stdoutput.write("	}\n");
	}
	delete[] drivers;

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getAnoVersionField(const unsigned char *rp,
						uint32_t *version,
						const unsigned char **rpout) {
	uint16_t	size;
	uint16_t	type;
	if (!readBE(rp,&size,"size",4,&rp) ||
		!readBE(rp,&type,"type",5,&rp)) {
		return false;
	}
	readBE(rp,version,&rp);

	if (getDebug()) {
		stdoutput.printf("		version: 0x%08x\n",*version);
		// 8.0 -> 10g send a version string, 11i+ sends all 0's
	}

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getAnoConnectionInfoField(
						const unsigned char *rp,
						uint32_t *pid,
						uint32_t *connectiontype,
						const unsigned char **rpout) {
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
		stdoutput.printf("		pid: %d\n",*pid);
		stdoutput.printf("		connection type: 0x%08x\n",
							*connectiontype);
	}

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getAnoArrayField(const unsigned char *rp,
						uint16_t **array,
						uint32_t *arraycount,
						const unsigned char **rpout) {

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
		// length.  Other times there's no deadbeef marker, and no
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

	if (getDebug()) {
		stdoutput.printf("		array count: %d\n",*arraycount);
	}

	// FIXME: sanity check on array count

	// get the array members
	if (*arraycount) {
		*array=new uint16_t[*arraycount];
		for (uint32_t i=0; i<*arraycount; i++) {
			readBE(rp,&((*array)[i]),&rp);
			if (getDebug()) {
				stdoutput.printf("		array[%d]: "
							"%d\n",i,(*array)[i]);
			}
		}
	}

	*rpout=rp;

	return true;
}


bool sqlrprotocol_oracle::getAnoConstantField(const unsigned char *rp,
						uint16_t *constant,
						const unsigned char **rpout) {
	uint16_t	size;
	uint16_t	type;
	if (!readBE(rp,&size,"size",2,&rp) ||
		!readBE(rp,&type,"type",3,&rp)) {
		return false;
	}
	readBE(rp,constant,&rp);

	if (getDebug()) {
		stdoutput.printf("		constant: 0x%04x\n",*constant);
	}

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getAnoConstantField(const unsigned char *rp,
						unsigned char *constant,
						const unsigned char **rpout) {
	uint16_t	size;
	uint16_t	type;
	if (!readBE(rp,&size,"size",1,&rp) ||
		!readBE(rp,&type,"type",2,&rp)) {
		return false;
	}
	read(rp,constant,&rp);

	if (getDebug()) {
		stdoutput.printf("		constant: 0x%02x\n",*constant);
	}

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getAnoStatusField(const unsigned char *rp,
						uint16_t *status,
						const unsigned char **rpout) {
	uint16_t	size;
	uint16_t	type;
	if (!readBE(rp,&size,"size",2,&rp) ||
		!readBE(rp,&type,"type",6,&rp)) {
		return false;
	}
	readBE(rp,status,&rp);

	if (getDebug()) {
		stdoutput.printf("		status: 0x%04x\n",*status);
	}

	*rpout=rp;

	return true;
}

bool sqlrprotocol_oracle::getAnoUnknownField(const unsigned char *rp,
						const unsigned char **rpout) {
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
	uint16_t	overalllength=13;
	uint64_t	overalllengthpos=0;
	uint32_t	version=anorequestversion;
	uint16_t	servicecount=0;
	uint64_t	servicecountpos=0;
	unsigned char	servicestobeused=0;

	writeBE(&reqpacket,dataflags);
	writeBE(&reqpacket,(uint32_t)0xdeadbeef);
	overalllengthpos=reqpacket.getPosition();
	writeBE(&reqpacket,overalllength);
	writeBE(&reqpacket,version);
	servicecountpos=reqpacket.getPosition();
	writeBE(&reqpacket,servicecount);
	write(&reqpacket,servicestobeused);

	if (getDebug()) {
		stdoutput.write("ano response header {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		stdoutput.printf("	version: 0x%08x\n",version);
		stdoutput.printf("	services to be used: %d\n",
							servicestobeused);
	}


	// services...
	overalllength+=putSupervisorService();
	servicecount++;
	overalllength+=putAuthenticationService();
	servicecount++;
	overalllength+=putEncryptionService();
	servicecount++;
	overalllength+=putCryptoChecksummingService();
	servicecount++;


	// backpatch the overall length and servicecount
	reqpacket.setPosition(overalllengthpos);
	reqpacket.write(hostToBE(overalllength));
	reqpacket.setPosition(servicecountpos);
	reqpacket.write(hostToBE(servicecount));

	if (getDebug()) {
		stdoutput.printf("	overall length: %d\n",overalllength);
		stdoutput.printf("	service count: %d\n",servicecount);
		stdoutput.write("}\n");
	}

	return sendPacket(true);
}

uint16_t sqlrprotocol_oracle::putSupervisorService() {

	if (getDebug()) {
		stdoutput.write("	supervisor {\n");
	}

	uint16_t drivers[]={0x0004,0x0001};

	uint16_t	length=putAnoServiceHeader(4,3)+
				putAnoVersionField(supervisorversion)+
				putAnoStatusField(0x001f)+
				putAnoArrayField(drivers,2);

	if (getDebug()) {
		stdoutput.write("	}\n");
	}
	return length;
}

uint16_t sqlrprotocol_oracle::putAuthenticationService() {

	if (getDebug()) {
		stdoutput.write("	authentication {\n");
	}

	uint16_t	length=putAnoServiceHeader(1,2)+
				putAnoVersionField(authenticationversion)+
				putAnoStatusField(0xfbff);

	if (getDebug()) {
		stdoutput.write("	}\n");
	}
	return length;
}

uint16_t sqlrprotocol_oracle::putEncryptionService() {

	if (getDebug()) {
		stdoutput.write("	encryption {\n");
	}

	uint16_t	length=putAnoServiceHeader(2,2)+
				putAnoVersionField(encryptionversion)+
				putAnoConstant((unsigned char)0);

	if (getDebug()) {
		stdoutput.write("	}\n");
	}
	return length;
}

uint16_t sqlrprotocol_oracle::putCryptoChecksummingService() {

	if (getDebug()) {
		stdoutput.write("	crypto-checksumming {\n");
	}

	uint16_t	length=putAnoServiceHeader(3,2)+
				putAnoVersionField(cryptochecksummingversion)+
				putAnoConstant((unsigned char)0);

	if (getDebug()) {
		stdoutput.write("	}\n");
	}
	return length;
}

uint16_t sqlrprotocol_oracle::putAnoServiceHeader(uint16_t service,
							uint16_t fieldcount) {

	if (getDebug()) {
		stdoutput.write("		ano service header {\n");
		stdoutput.printf("			service: %d\n",service);
		stdoutput.printf("			field count: %d\n",
								fieldcount);
		stdoutput.write("		}\n");
	}

	// service, field count, marker, return total length
	writeBE(&reqpacket,service);
	writeBE(&reqpacket,fieldcount);
	// FIXME; send something other than 0x00000000 if there was an error...
	writeBE(&reqpacket,(uint32_t)0x00000000);
	return 8;
}

uint16_t sqlrprotocol_oracle::putAnoVersionField(uint32_t version) {

	if (getDebug()) {
		stdoutput.printf("		version: 0x%08x\n",version);
	}

	// data length, field type, version, return total length
	writeBE(&reqpacket,(uint16_t)4);
	writeBE(&reqpacket,(uint16_t)5);
	writeBE(&reqpacket,version);
	return 8;
}

uint16_t sqlrprotocol_oracle::putAnoStatusField(uint16_t status) {

	if (getDebug()) {
		stdoutput.printf("		status: 0x%04x\n",status);
	}

	// data length, field type, status, return total length
	writeBE(&reqpacket,(uint16_t)2);
	writeBE(&reqpacket,(uint16_t)6);
	writeBE(&reqpacket,status);
	return 6;
}

uint16_t sqlrprotocol_oracle::putAnoConstant(unsigned char constant) {

	if (getDebug()) {
		stdoutput.printf("		constant: 0x%02x\n",constant);
	}

	// data length, field type, constant, return total length
	writeBE(&reqpacket,(uint16_t)1);
	writeBE(&reqpacket,(uint16_t)2);
	write(&reqpacket,constant);
	return 5;
}

uint16_t sqlrprotocol_oracle::putAnoArrayField(uint16_t *array,
						uint32_t arraycount) {

	// data length, field type
	uint16_t datalength=((arraycount)?(4+2+4+arraycount*2):1);
	writeBE(&reqpacket,(uint16_t)((arraycount)?(4+2+4+arraycount*2):1));
	writeBE(&reqpacket,(uint16_t)1);

	if (getDebug()) {
		stdoutput.printf("		arraycount: %d\n",arraycount);
	}

	if (arraycount) {

		// deadbeef marker, array marker,
		// array count, array members
		writeBE(&reqpacket,(uint32_t)0xdeadbeef);
		writeBE(&reqpacket,(uint16_t)0x003);
		writeBE(&reqpacket,arraycount);
		for (uint32_t i=0; i<arraycount; i++) {
			if (getDebug()) {
				stdoutput.printf("		"
						"array[%d]: %d\n",i,array[i]);
			}
			writeBE(&reqpacket,array[i]);
		}

	} else {
		// null terminator
		write(&reqpacket,(unsigned char)0x00);
	}

	// return total length
	return 4+datalength;
}

bool sqlrprotocol_oracle::ttiNegotiation() {
	return recvTtiRequest() && sendTtiResponse();
}

bool sqlrprotocol_oracle::recvTtiRequest() {

	if (!recvPacket()) {
		return false;
	}

	if (resppackettype!=PACKET_DATA) {
		if (getDebug()) {
			stdoutput.printf("bad packet type %d, expected %d\n",
						resppackettype,PACKET_DATA);
		}
		return false;
	}

	const unsigned char	*rp=resppacket;
	const unsigned char	*end=rp+resppacketsize;

	uint16_t	dataflags;
	unsigned char	ttccode;
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
		stdoutput.write("tti request {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		for (uint32_t i=0; i<ttiversioncount; i++) {
			stdoutput.printf("	version[%d]: %d\n",
						i,ttiversions[i]);
		}
		stdoutput.printf("	client string: \"%s\"\n",clientstring);
		stdoutput.write("}\n");
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
		if (getDebug()) {
			stdoutput.printf("no supported tti "
						"protocol version found\n");
		}
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
	unsigned char	ttccode=TTC_PROTOCOL_NEGOTIATION;

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
	write(&reqpacket,(unsigned char)0);
	write(&reqpacket,serverstring);
	write(&reqpacket,'\0');


	// charset...
	// FIXME: no idea what charset=1 means...
	uint16_t	charset=1;
	// FIXME: other charsets might have graph elements...
	uint16_t	charsetgraphelementcount=0;

	writeLE(&reqpacket,charset);
	write(&reqpacket,(unsigned char)0);
	writeLE(&reqpacket,charsetgraphelementcount);
	// FIXME: each charset graph element consists of 5 bytes...


	// fdo... (whatever that is)
	uint16_t	fdolength=100;
	uint32_t	fdodatalength=fdolength-4;
	// no idea what this means...
	unsigned char	part1[]={
		0x05, 0x0b, 0x0c, 0x03, 0x0c, 0x0c, 0x05, 0x04,
		0x05, 0x0d, 0x06, 0x09, 0x07, 0x08, 0x05, 0x0e,
		0x05, 0x06, 0x05, 0x0f, 0x02, 0xec, 0xeb, 0xed,
		0x05, 0x0a, 0x05, 0x05, 0x05, 0x05, 0x05
	};
	unsigned char	part1length=sizeof(part1);
	// no idea what this means...
	unsigned char	part2[]={
		0x08, 0x23, 0x43, 0x23, 0x23, 0x08, 0x11, 0x23,
		0x08, 0x11, 0x41, 0xb0, 0x23, 0x00, 0x83
	};
	unsigned char	part2length=sizeof(part2);

	writeBE(&reqpacket,fdolength);
	writeBE(&reqpacket,fdodatalength);
	write(&reqpacket,(unsigned char)1);
	write(&reqpacket,part1length);
	write(&reqpacket,part2length);
	reqpacket.append(part1,part1length);
	reqpacket.append(part2,part2length);


	// some charset related thing, or something...
	unsigned char	charsetthing[]={
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
		stdoutput.write("tti response {\n");
		stdoutput.printf("	data flags: 0x%04x\n",
							dataflags);
		debugTtcCode(ttccode);
		stdoutput.printf("	version: %d\n",ttiversion);
		stdoutput.printf("	server string: \"%s\"\n",serverstring);
		stdoutput.printf("	charset: 0x%02x\n",charset);
		stdoutput.write("}\n");
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
		if (getDebug()) {
			stdoutput.printf("bad packet type %d, expected %d\n",
						resppackettype,PACKET_DATA);
		}
		return false;
	}

	const unsigned char	*rp=resppacket;
	const unsigned char	*end=rp+resppacketsize;

	uint16_t	dataflags;
	unsigned char	ttccode;

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
	datatypeslength=end-rp;

	if (getDebug()) {
		stdoutput.write("datatype request {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		stdoutput.printf("	data types:\n");
		debugHexDump(datatypes,datatypeslength);
		stdoutput.write("}\n");
	}

	return true;
}

bool sqlrprotocol_oracle::sendDataTypeResponse() {

	// FIXME: 9i appears to want more than we send here
	// might have to do with the 0x02 in the request...

	resetSendPacketBuffer(PACKET_DATA);

	uint16_t	dataflags=0;
	unsigned char	ttccode=TTC_DATATYPE_NEGOTIATION;

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);
	// for now echo the requested data types
	reqpacket.append(datatypes,datatypeslength);

	if (getDebug()) {
		stdoutput.write("datatype response {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		stdoutput.printf("	data types:\n");
		debugHexDump(datatypes,datatypeslength);
		stdoutput.write("}\n");
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
		if (getDebug()) {
			stdoutput.printf("bad packet type %d, expected %d\n",
						resppackettype,PACKET_DATA);
		}
		return false;
	}

	const unsigned char	*rp=resppacket;
	const unsigned char	*end=rp+resppacketsize;

	uint16_t	dataflags;
	unsigned char	ttccode;
	unsigned char	ttifunction;
	unsigned char	seqnumber;
	unsigned char	unknown;

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

	unsigned char	userlength;
	char		*user;

	// no idea...
	for (uint16_t i=0; i<((stringsmatch)?4:2); i++) {
		read(rp,&unknown,&rp);
	}

	// user length...
	read(rp,&userlength,&rp);

	// no idea...
	for (uint16_t i=0; i<((stringsmatch)?23:7); i++) {
		read(rp,&unknown,&rp);
	}

	// user name...
	getString(rp,&user,userlength,&rp);

	if (getDebug()) {
		stdoutput.printf("authentication request (phase %d) {\n",
							(secondphase)?2:1);
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		debugTtiFunction(ttifunction);
		stdoutput.printf("	seq number: %d\n",seqnumber);
		stdoutput.printf("	user: %s\n",user);
	}

	do {

		unsigned char	fieldnamelength;
		uint32_t	fieldnamelengthlong;
		char		*fieldname;
		unsigned char	fieldlength;
		uint32_t	fieldlengthlong;
		char		*field;

		// field name...
		if (!stringsmatch) {
			read(rp,&unknown,&rp);
		}
		read(rp,&fieldnamelength,&rp);
		if (!stringsmatch) {
			read(rp,&fieldnamelength,&rp);
		} else {
			readBE(rp,&fieldnamelengthlong,&rp);
		}
		getString(rp,&fieldname,fieldnamelength,&rp);

		// field...
		if (!stringsmatch) {
			read(rp,&unknown,&rp);
		}
		read(rp,&fieldlength,&rp);
		if (!stringsmatch) {
			read(rp,&fieldlength,&rp);
		} else {
			readBE(rp,&fieldlengthlong,&rp);
		}
		getString(rp,&field,fieldlength,&rp);

		// no idea...
		if (!stringsmatch) {
			read(rp,&unknown,&rp);
		} else {
			read(rp,&unknown,&rp);
			read(rp,&unknown,&rp);
			read(rp,&unknown,&rp);
			read(rp,&unknown,&rp);
		}

		if (getDebug()) {
			stdoutput.printf("	%s: %s\n",fieldname,field);
		}

		// FIXME: do something with these...

	} while (rp<end);

	if (getDebug()) {
		stdoutput.write("}\n");
	}

	return true;
}

bool sqlrprotocol_oracle::sendAuthenticationChallenge() {

	resetSendPacketBuffer(PACKET_DATA);

	// FIXME: actually do auth, and send some
	// different response if auth failed

	uint16_t	dataflags=0;
	unsigned char	ttccode=TTC_OK;
	unsigned char	unknown1[]={
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

	if (getDebug()) {
		stdoutput.write("authentication challenge {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
	}

	// no idea
	write(&reqpacket,(unsigned char)1);
	write(&reqpacket,(unsigned char)0);

	// session key...
	generateAuthSessionKey(16);
authsessionkey=charstring::duplicate("64760F3160DCEF82");
	putAuthField("AUTH_SESSKEY",authsessionkey);

	// no idea
	reqpacket.append(unknown1,sizeof(unknown1));

	putGenericFooter();

	if (getDebug()) {
		stdoutput.write("}\n");
	}

	return sendPacket(true);
}

void sqlrprotocol_oracle::putAuthField(const char *fieldname,
						const char *field) {

	unsigned char	fieldnamelength=charstring::length(fieldname);
	unsigned char	fieldlength=charstring::length(field);

	// field name...
	write(&reqpacket,fieldnamelength);
	// no idea...
	write(&reqpacket,(unsigned char)0);
	write(&reqpacket,(unsigned char)0);
	write(&reqpacket,(unsigned char)0);
	// again...
	write(&reqpacket,fieldnamelength);
	write(&reqpacket,fieldname,fieldnamelength);

	// field...
	write(&reqpacket,fieldlength);
	// no idea...
	write(&reqpacket,(unsigned char)0);
	write(&reqpacket,(unsigned char)0);
	write(&reqpacket,(unsigned char)0);
	// again...
	if (fieldlength) {
		write(&reqpacket,fieldlength);
		write(&reqpacket,field,fieldlength);
	}
	// no idea...
	write(&reqpacket,(unsigned char)0);
	write(&reqpacket,(unsigned char)0);
	write(&reqpacket,(unsigned char)0);
	write(&reqpacket,(unsigned char)0);

	if (getDebug()) {
			stdoutput.printf("	%s: %s\n",fieldname,field);
	}
}

bool sqlrprotocol_oracle::sendAuthenticationResponse() {

	resetSendPacketBuffer(PACKET_DATA);

	// FIXME: actually do auth, and send some
	// different response if auth failed

	uint16_t	dataflags=0;

	writeBE(&reqpacket,dataflags);

	unsigned char	unknownheader[]={
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

	unsigned char	unknownfooter[]={
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

	if (getDebug()) {
		stdoutput.write("authentication response {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		stdoutput.write("}\n");
	}

	return sendPacket(true);
}

bool sqlrprotocol_oracle::open(const unsigned char *rp) {

	// sqlplus 8.0.5, 8i, 9i
	// call this to open a cursor
	// sqlplus 10g+ use query3

	sqlrservercursor	*cursor=cont->getCursor();
	if (!cursor) {
		if (getDebug()) {
			stdoutput.printf("couldn't get cursor\n");
		}
		return sendCursorNotOpenError();
	}

	uint16_t	cursorid=cont->getId(cursor);
hackcursorid=cursorid;
	
	// FIXME: decode this...

	if (getDebug()) {
		stdoutput.write("open request {\n");
		stdoutput.printf("	cursor id: %d\n",cursorid);
		stdoutput.write("}\n");
	}

	return sendOpenResponse(cursor);
}

bool sqlrprotocol_oracle::sendOpenResponse(sqlrservercursor *cursor) {

	resetSendPacketBuffer(PACKET_DATA);

	uint16_t	dataflags=0;
	unsigned char	ttccode=TTC_OK;
	unsigned char	unknown[]={
		0x01, 0x00, 0x00, 0x00, 0x09
	};

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);
	reqpacket.append(unknown,sizeof(unknown));

	if (getDebug()) {
		stdoutput.write("open response {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		stdoutput.write("}\n");
	}

	return sendPacket(true);
}

void sqlrprotocol_oracle::debugTtcCode(unsigned char ttccode) {
	stdoutput.write("	ttc code: ");
	switch (ttccode) {
		case TTC_PROTOCOL_NEGOTIATION:
			stdoutput.write("TTC_PROTOCOL_NEGOTIATION");
			break;
		case TTC_DATATYPE_NEGOTIATION:
			stdoutput.write("TTC_DATATYPE_NEGOTIATION");
			break;
		case TTC_TTI_FUNCTION:
			stdoutput.write("TTC_TTI_FUNCTION");
			break;
		case TTC_OK:
			stdoutput.write("TTC_OK");
			break;
		case TTC_EXTENDED_TTI_FUNCTION:
			stdoutput.write("TTC_EXTENDED_TTI_FUNCTION");
			break;
		case TTC_EXTPROC1:
			stdoutput.write("TTC_EXTPROC1");
			break;
		case TTC_EXTPROC2:
			stdoutput.write("TTC_EXTPROC2");
			break;
		default:
			stdoutput.write("UNKNOWN");
			break;
	}
	stdoutput.printf(" (0x%02x)\n",ttccode);
}

void sqlrprotocol_oracle::debugTtiFunction(unsigned char ttifunction) {
	stdoutput.write("	tti function: ");
	switch (ttifunction) {
		case TTI_OPEN:
			stdoutput.write("TTI_OPEN");
			break;
		case TTI_QUERY:
			stdoutput.write("TTI_QUERY");
			break;
		case TTI_EXECUTE:
			stdoutput.write("TTI_EXECUTE");
			break;
		case TTI_FETCH:
			stdoutput.write("TTI_FETCH");
			break;
		case TTI_CLOSE:
			stdoutput.write("TTI_CLOSE");
			break;
		case TTI_DISCONNECT:
			stdoutput.write("TTI_DISCONNECT");
			break;
		case TTI_AUTOCOMMIT_ON:
			stdoutput.write("TTI_AUTOCOMMIT_ON");
			break;
		case TTI_AUTOCOMMIT_OFF:
			stdoutput.write("TTI_AUTOCOMMIT_OFF");
			break;
		case TTI_COMMIT:
			stdoutput.write("TTI_COMMIT");
			break;
		case TTI_ROLLBACK:
			stdoutput.write("TTI_ROLLBACK");
			break;
		case TTI_CANCEL:
			stdoutput.write("TTI_CANCEL");
			break;
		case TTI_DESCRIBE:
			stdoutput.write("TTI_DESCRIBE");
			break;
		case TTI_STARTUP:
			stdoutput.write("TTI_STARTUP");
			break;
		case TTI_SHUTDOWN:
			stdoutput.write("TTI_SHUTDOWN");
			break;
		case TTI_VERSION:
			stdoutput.write("TTI_VERSION");
			break;
		case TTI_K2_TRANSACTIONS:
			stdoutput.write("TTI_K2_TRANSACTIONS");
			break;
		case TTI_QUERY2:
			stdoutput.write("TTI_QUERY2");
			break;
		case TTI_OSQL7:
			stdoutput.write("TTI_OSQL7");
			break;
		case TTI_OKOD:
			stdoutput.write("TTI_OKOD");
			break;
		case TTI_QUERY3:
			stdoutput.write("TTI_QUERY3");
			break;
		case TTI_LOB_OPERATIONS:
			stdoutput.write("TTI_LOB_OPERATIONS");
			break;
		case TTI_ODNY:
			stdoutput.write("TTI_ODNY");
			break;
		case TTI_TRANSACTION_END:
			stdoutput.write("TTI_TRANSACTION_END");
			break;
		case TTI_TRANSACTION_BEGIN:
			stdoutput.write("TTI_TRANSACTION_BEGIN");
			break;
		case TTI_OCCA:
			stdoutput.write("TTI_OCCA");
			break;
		case TTI_STARTUP2:
			stdoutput.write("TTI_STARTUP2");
			break;
		case TTI_LOGON_PRESENT_PWD:
			stdoutput.write("TTI_LOGON_PRESENT_PWD");
			break;
		case TTI_LOGON_PRESENT_USER:
			stdoutput.write("TTI_LOGON_PRESENT_USER");
			break;
		case TTI_LOGON_PRESENT_PWD_SEND_AUTH_PASSWORD:
			stdoutput.write(
				"TTI_LOGON_PRESENT_PWD_SEND_AUTH_PASSWORD");
			break;
		case TTI_LOGON_PRESENT_USER_REQ_AUTH_SESSKEY:
			stdoutput.write(
				"TTI_LOGON_PRESENT_USER_REQ_AUTH_SESSKEY");
			break;
		case TTI_DESCRIBE2:
			stdoutput.write("TTI_DESCRIBE2");
			break;
		case TTI_OOTCM:
			stdoutput.write("TTI_OOTCM");
			break;
		case TTI_OKPFC:
			stdoutput.write("TTI_OKPFC");
			break;
		case TTI_SWITCH_SESSION:
			stdoutput.write("TTI_SWITCH_SESSION");
			break;
		case TTI_CLOSE2:
			stdoutput.write("TTI_CLOSE2");
			break;
		case TTI_OSCID:
			stdoutput.write("TTI_OSCID");
			break;
		case TTI_OSKEYVAL:
			stdoutput.write("TTI_OSKEYVAL");
			break;
		default:
			stdoutput.write("UNKNOWN");
			break;
	}
	stdoutput.printf(" (0x%02x)\n",ttifunction);
}

void sqlrprotocol_oracle::debugOptions(uint16_t options,
					uint16_t moreoptions) {

	stdoutput.printf("	options: 0x%04x (",options);
	stdoutput.printBits(hostToBE(options));
	stdoutput.write(")\n");
	debugOptions(options);
	stdoutput.printf("	moreoptions: 0x%04x (",moreoptions);
	stdoutput.printBits(hostToBE(moreoptions));
	stdoutput.write(")\n");
	debugOptions(moreoptions);
}

void sqlrprotocol_oracle::debugOptions(uint16_t options) {
	if (options&OPTION_PARSE) {
		stdoutput.write("		OPTION_PARSE\n");
	}
	if (options&OPTION_BIND) {
		stdoutput.write("		OPTION_BIND\n");
	}
	if (options&OPTION_DEFINE) {
		stdoutput.write("		OPTION_DEFINE\n");
	}
	if (options&OPTION_EXECUTE) {
		stdoutput.write("		OPTION_EXECUTE\n");
	}
	if (options&OPTION_FETCH) {
		stdoutput.write("		OPTION_FETCH\n");
	}
	if (options&OPTION_CANCEL) {
		stdoutput.write("		OPTION_CANCEL\n");
	}
	if (options&OPTION_COMMIT) {
		stdoutput.write("		OPTION_COMMIT\n");
	}
	if (options&OPTION_EXACTFETCH) {
		stdoutput.write("		OPTION_EXACTFETCH\n");
	}
	if (options&OPTION_SNDIOV) {
		stdoutput.write("		OPTION_SNDIOV\n");
	}
	if (options&OPTION_NOPLSQL) {
		stdoutput.write("		OPTION_NOPLSQL\n");
	}
}

void sqlrprotocol_oracle::debugCharacterSet(unsigned char characterset) {
	stdoutput.printf("	character set: 0x%02x\n",
				(uint32_t)(0x000000ff&characterset));
}

void sqlrprotocol_oracle::debugStatusFlags(uint16_t statusflags) {
	stdoutput.write("	status flags:\n");
	stdoutput.write("		");
	stdoutput.printf("0x%04x\n",statusflags);
	stdoutput.write("		");
	stdoutput.printBits(statusflags);
	stdoutput.write("\n");
	/*if (statusflags&SERVER_STATUS_IN_TRANS) {
		stdoutput.write("		"
				"SERVER_STATUS_IN_TRANS\n");
	}*/
}

void sqlrprotocol_oracle::debugColumnType(const char *name,
						uint16_t columntype) {
	stdoutput.printf("		type: %s (0x%02x)\n",
				name,(uint32_t)(0x000000ff&columntype));
	debugColumnType(columntype);
}

void sqlrprotocol_oracle::debugColumnType(uint16_t columntype) {
	stdoutput.write("		");
	switch (columntype) {
		case ORACLE_TYPE_VARCHAR:
			stdoutput.write("ORACLE_TYPE_VARCHAR\n");
			break;
		case ORACLE_TYPE_NUMBER:
			stdoutput.write("ORACLE_TYPE_NUMBER\n");
			break;
		case ORACLE_TYPE_VARNUM:
			stdoutput.write("ORACLE_TYPE_VARNUM\n");
			break;
		case ORACLE_TYPE_LONG:
			stdoutput.write("ORACLE_TYPE_LONG\n");
			break;
		case ORACLE_TYPE_ROWID_DEPRECATED:
			stdoutput.write("ORACLE_TYPE_ROWID_DEPRECATED\n");
			break;
		case ORACLE_TYPE_DATE:
			stdoutput.write("ORACLE_TYPE_DATE\n");
			break;
		case ORACLE_TYPE_RAW:
			stdoutput.write("ORACLE_TYPE_RAW\n");
			break;
		case ORACLE_TYPE_LONG_RAW:
			stdoutput.write("ORACLE_TYPE_LONG_RAW\n");
			break;
		case ORACLE_TYPE_CHAR:
			stdoutput.write("ORACLE_TYPE_CHAR\n");
			break;
		case ORACLE_TYPE_RESULT_SET:
			stdoutput.write("ORACLE_TYPE_RESULT_SET\n");
			break;
		case ORACLE_TYPE_ROWID:
			stdoutput.write("ORACLE_TYPE_ROWID\n");
			break;
		case ORACLE_TYPE_NAMED_TYPE:
			stdoutput.write("ORACLE_TYPE_NAMED_TYPE\n");
			break;
		case ORACLE_TYPE_REF_TYPE:
			stdoutput.write("ORACLE_TYPE_REF_TYPE\n");
			break;
		case ORACLE_TYPE_CLOB:
			stdoutput.write("ORACLE_TYPE_CLOB\n");
			break;
		case ORACLE_TYPE_BLOB:
			stdoutput.write("ORACLE_TYPE_BLOB\n");
			break;
		case ORACLE_TYPE_BFILE:
			stdoutput.write("ORACLE_TYPE_BFILE\n");
			break;
		case ORACLE_TYPE_TIMESTAMP:
			stdoutput.write("ORACLE_TYPE_TIMESTAMP\n");
			break;
		case ORACLE_TYPE_TIMESTAMPTZ:
			stdoutput.write("ORACLE_TYPE_TIMESTAMPTZ\n");
			break;
		case ORACLE_TYPE_INTERVALYM:
			stdoutput.write("ORACLE_TYPE_INTERVALYM\n");
			break;
		case ORACLE_TYPE_INTERVALDS:
			stdoutput.write("ORACLE_TYPE_INTERVALDS\n");
			break;
		case ORACLE_TYPE_TIMESTAMPLTZ:
			stdoutput.write("ORACLE_TYPE_TIMESTAMPLTZ\n");
			break;
		case ORACLE_TYPE_PLSQL_INDEX_TABLE:
			stdoutput.write("ORACLE_TYPE_PLSQL_INDEX_TABLE\n");
			break;
		default:
			stdoutput.write("unknown ORACLE_TYPE\n");
			break;
	}
}

void sqlrprotocol_oracle::debugSystemError() {
	char	*err=error::getErrorString();
	stdoutput.printf("%s\n",err);
	delete[] err;
}

bool sqlrprotocol_oracle::getTtiFunction(const unsigned char *rp,
						unsigned char *ttifunction,
						const unsigned char **rpout) {

	if (!rp) {
		if (!recvPacket()) {
			return false;
		}

		if (resppackettype!=PACKET_DATA) {
			if (getDebug()) {
				stdoutput.printf(
					"bad packet type %d, expected %d\n",
					resppackettype,PACKET_DATA);
			}
			return false;
		}

		rp=resppacket;
	}

	const unsigned char	*rpin=rp;

	uint16_t	dataflags;
	unsigned char	ttccode;

	readBE(rp,&dataflags,&rp);
	if (!read(rp,&ttccode,"ttccode",TTC_TTI_FUNCTION,&rp) &&
		!read(rp,&ttccode,"ttccode",TTC_EXTENDED_TTI_FUNCTION,&rp)) {
		*rpout=rpin;
		return false;
	}
	read(rp,ttifunction,&rp);
	*rpout=rp;

	if (getDebug()) {
		stdoutput.write("get tti function {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		debugTtiFunction(*ttifunction);
		stdoutput.write("}\n");
	}
	
	return true;
}

bool sqlrprotocol_oracle::query(const unsigned char *rp) {

	// sqlplus 8.0.5, 8i, 9i
	// call this to prepare some initial queries
	// sqlplus 10g+ use query3

	// prepares the specified query

	// parse the request...
	uint16_t	options;
	uint16_t	moreoptions;
	uint16_t	cursorid;
	unsigned char	unknown3;
	unsigned char	unknown4;
	unsigned char	unknown5;
	uint16_t	querylen;
	unsigned char	unknown6;
	unsigned char	unknown7;
	const char	*query;

	readBE(rp,&options,&rp);
	readBE(rp,&moreoptions,&rp);
	readBE(rp,&cursorid,&rp);
	read(rp,&unknown3,&rp);
	read(rp,&unknown4,&rp);
	read(rp,&unknown5,&rp);
	readLE(rp,&querylen,&rp);
	read(rp,&unknown6,&rp);
	read(rp,&unknown7,&rp);
	query=(char *)rp;
cursorid=hackcursorid;

	if (getDebug()) {
		stdoutput.write("query request {\n");
		debugOptions(options,moreoptions);
		stdoutput.printf("	cursor id: %d\n",cursorid);
		stdoutput.printf("	unknown: %02x %02x %02x\n",
						unknown3,unknown4,unknown5);
		stdoutput.printf("	query length: %d\n",querylen);
		stdoutput.printf("	unknown: %02x %02x\n",
						unknown6,unknown7);
		stdoutput.printf("	query: \"%*s\"\n",querylen,query);
		stdoutput.write("}\n");
	}

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(cursorid);
	if (!cursor) {
		if (getDebug()) {
			stdoutput.printf("cursor id %d not found\n",cursorid);
		}
		return sendCursorNotOpenError();
	}

	// reset column type cache flag
	columntypescached[cont->getId(cursor)]=false;

	// bounds checking
	if (querylen>maxquerysize) {
		// FIXME: implement this
		//return sendErrPacket(1105,"Unknown error","24000");
		return false;
	}

	// copy the query into the cursor's query buffer
	char	*querybuffer=cont->getQueryBuffer(cursor);
	bytestring::copy(querybuffer,query,querylen);
	querybuffer[querylen]='\0';
	cont->setQueryLength(cursor,querylen);

	// prepare the query
	if (!cont->prepareQuery(cursor,cont->getQueryBuffer(cursor),
					cont->getQueryLength(cursor),
					true,true,true)) {
		if (getDebug()) {
			stdoutput.printf("prepare query failed\n");
		}
		return sendQueryError(cursor);
	}
	return sendQueryResponse(cursor);
}

bool sqlrprotocol_oracle::sendQueryResponse(sqlrservercursor *cursor) {

	resetSendPacketBuffer(PACKET_DATA);

	// FIXME: decode this...

	uint16_t	dataflags=0;
	// FIXME: not a valid ttccode type...
	unsigned char	ttccode=4;
	unsigned char unknown1[]={
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00
	};
	unsigned char unknown2[]={
		// not cursor id
		0x01, 0x00
	};
	unsigned char unknown3[]={
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
		stdoutput.write("query response {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		stdoutput.write("}\n");
	}

	return sendPacket(true);
}

bool sqlrprotocol_oracle::query2(const unsigned char *rp) {

	// sqlplus 8.0.5, 8i, 9i
	// call this to prepare/execute some initial queries
	// sqlplus 10g+ use query3
	// can apparently be used for fetch too

	// parse the request...
	uint16_t	options;
	uint16_t	moreoptions;
	uint16_t	cursorid;
	uint32_t	querylen;
	const char	*query;

	readBE(rp,&options,&rp);
	readBE(rp,&moreoptions,&rp);
	readBE(rp,&cursorid,&rp);
cursorid=hackcursorid;
	if (options&OPTION_PARSE) {
		// no idea...
		for (uint16_t i=0; i<7; i++) {
			unsigned char	unknown;
			read(rp,&unknown,&rp);
		}
		readLE(rp,&querylen,&rp);
		// no idea...
		for (uint16_t i=0; i<44; i++) {
			unsigned char	unknown;
			read(rp,&unknown,&rp);
		}
		query=(char *)rp;
		rp+=querylen;
	}
	// no idea...

	if (getDebug()) {
		stdoutput.write("query2 request {\n");
		debugOptions(options,moreoptions);
		stdoutput.printf("	cursor id: %d\n",cursorid);
		if (options&OPTION_PARSE) {
			stdoutput.printf("	query length: %d\n",
								querylen);
			stdoutput.printf("	query: \"%*s\"\n",
								querylen,query);
		}
		stdoutput.write("}\n");
	}

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(cursorid);
	if (!cursor) {
		if (getDebug()) {
			stdoutput.printf("cursor id %d not found\n",cursorid);
		}
		return sendCursorNotOpenError();
	}

	if (options&OPTION_PARSE) {

		// reset column type cache flag
		columntypescached[cont->getId(cursor)]=false;

		// bounds checking
		if (querylen>maxquerysize) {
			// FIXME: implement this
			//return sendErrPacket(1105,"Unknown error","24000");
			return false;
		}

		// copy the query into the cursor's query buffer
		char	*querybuffer=cont->getQueryBuffer(cursor);
		bytestring::copy(querybuffer,query,querylen);
		querybuffer[querylen]='\0';
		cont->setQueryLength(cursor,querylen);
	
		// prepare the query
		if (!cont->prepareQuery(cursor,
					cont->getQueryBuffer(cursor),
					cont->getQueryLength(cursor),
					true,true,true)) {
			if (getDebug()) {
				stdoutput.printf("prepare query failed\n");
			}
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
			if (getDebug()) {
				stdoutput.printf("execute query failed\n");
			}
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
	unsigned char	ttccode;

	if (binds) {

		dataflags=0;
		// FIXME: not a valid ttccode type...
		ttccode=11;

		// FIXME: decode this...

		unsigned char unknown[]={
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

		unsigned char unknown[]={

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

	if (getDebug()) {
		stdoutput.write("query2 response {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		stdoutput.write("}\n");
	}

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
			if (getDebug()) {
				stdoutput.printf(
					"bad packet type %d, expected %d\n",
					resppackettype,PACKET_DATA);
			}
			return false;
		}

		if (i>maxbindcount) {
			continue;
		}

		const unsigned char	*rp=resppacket;

		uint16_t	dataflags;
		unsigned char	unknown;

		readBE(rp,&dataflags,&rp);
		read(rp,&unknown,&rp);


		sqlrserverbindvar	*bv=&(inbinds[i]);

		// the bind variable name should be something like :1, :2, etc.
		bv->variable=bindvarnames[i];
		bv->variablesize=charstring::length(bv->variable);

		if (getDebug()) {
			stdoutput.printf("bind %d {\n",i);
			stdoutput.printf("	data flags: 0x%04x\n",
								dataflags);
			stdoutput.printf("	variable: %s\n",bv->variable);
		}

		// FIXME: handle nulls
		if (false) {
			bv->type=SQLRSERVERBINDVARTYPE_NULL;
			bv->isnull=cont->nullBindValue();
			if (getDebug()) {
				stdoutput.write("	type: NULL\n");
				stdoutput.write("	isnull: true\n");
				stdoutput.write("}\n");
			}
			continue;
		}

		// handle non-nulls
		switch (ptypes[i]) {
			/*case MYSQL_TYPE_TINY:
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				bv->value.integerval=*((char *)rp);
				bv->isnull=cont->nonNullBindValue();
				rp+=sizeof(char);
				break;
			case MYSQL_TYPE_SHORT:
				{
				bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
				uint16_t	val;
				bytestring::copy(&val,rp,sizeof(uint16_t));
				val=leToHost((uint16_t)val);
				bv->value.integerval=(int16_t)val;
				bv->isnull=cont->nonNullBindValue();
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
				bv->isnull=cont->nonNullBindValue();
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
				bv->isnull=cont->nonNullBindValue();
				rp+=sizeof(int64_t);
				}
				break;
			case MYSQL_TYPE_FLOAT:
				bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
				bytestring::copy(&bv->value.doubleval.value,
							rp,sizeof(float));
				bv->value.doubleval.precision=0;
				bv->value.doubleval.scale=0;
				bv->isnull=cont->nonNullBindValue();
				rp+=sizeof(float);
				break;
			case MYSQL_TYPE_DOUBLE:
				bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
				bytestring::copy(&bv->value.doubleval.value,
							rp,sizeof(double));
				bv->value.doubleval.precision=0;
				bv->value.doubleval.scale=0;
				bv->isnull=cont->nonNullBindValue();
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
				bv->isnull=cont->nonNullBindValue();
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					new char[bv->value.dateval.buffersize];

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
					if (len>8) {
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
				bv->isnull=cont->nonNullBindValue();
				bv->value.dateval.buffersize=64;
				bv->value.dateval.buffer=
					new char[bv->value.dateval.buffersize];

				char	len=*((char *)rp);
				rp+=sizeof(char);

				if (len) {
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
					new char[bv->value.dateval.buffersize];

				char	len=*((char *)rp);
				rp+=sizeof(char);

				if (len) {
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
				bv->isnull=cont->nonNullBindValue();
				rp+=bv->valuesize;
				break;
			*/
			case ORACLE_TYPE_VARCHAR:
			// (for all other types, assume varchar)
			default:
				{
				unsigned char	len;
				read(rp,&len,&rp);

				bv->type=SQLRSERVERBINDVARTYPE_STRING;
				bv->valuesize=len;
				bv->value.stringval=charstring::duplicate(
							(const char *)rp,
							bv->valuesize);
				bv->isnull=cont->nonNullBindValue();
				rp+=bv->valuesize;
				break;
				}
		}

		if (getDebug()) {
			stdoutput.write("	");
			if (bv->type==SQLRSERVERBINDVARTYPE_STRING) {
				stdoutput.write("type: STRING\n");
				stdoutput.printf("	value: %s\n",
						bv->value.stringval);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_INTEGER) {
				stdoutput.write("type: INTEGER\n");
				stdoutput.printf("	value: %lld\n",
						bv->value.integerval);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_DOUBLE) {
				stdoutput.write("type: DOUBLE\n");
				stdoutput.printf("	value: %f (%d,%d)\n",
						bv->value.doubleval.value,
						bv->value.doubleval.precision,
						bv->value.doubleval.scale);
			} else if (bv->type==SQLRSERVERBINDVARTYPE_DATE) {
				// FIXME: print date...
			}
			stdoutput.printf("	value size: %d\n",
							bv->valuesize);
			stdoutput.write("	isnull: false\n");
			stdoutput.write("}\n");
		}
	}

	return true;
}

bool sqlrprotocol_oracle::query3(const unsigned char *rp) {

	// all versions call this to open/prepare/describe/execute most queries
	// can apparently be used for fetch too

	// parse the request...
	uint16_t	options;
	uint16_t	moreoptions;
	uint16_t	cursorid;
	uint32_t	querylen;
	const char	*query;

	readBE(rp,&options,&rp);
	readBE(rp,&moreoptions,&rp);
cursorid=hackcursorid;
	// no idea...
	for (uint16_t i=0; i<9; i++) {
		unsigned char	unknown;
		read(rp,&unknown,&rp);
	}
	readLE(rp,&querylen,&rp);
	// no idea...
	for (uint16_t i=0; i<56; i++) {
		unsigned char	unknown;
		read(rp,&unknown,&rp);
	}
	query=(char *)rp;
	rp+=querylen;
	// no idea...

	if (getDebug()) {
		stdoutput.write("query3 request {\n");
		debugOptions(options,moreoptions);
		stdoutput.printf("	cursor id: %d\n",cursorid);
		stdoutput.printf("	query length: %d\n",querylen);
		stdoutput.printf("	query: \"%*s\"\n",querylen,query);
		stdoutput.write("}\n");
	}

	// get the requested cursor
	sqlrservercursor	*cursor;
	if (cursorid==65535) {
		cursor=cont->getCursor();
		if (!cursor) {
			if (getDebug()) {
				stdoutput.printf("couldn't get cursor\n");
			}
			return sendCursorNotOpenError();
		}
		cursorid=cont->getId(cursor);
hackcursorid=cursorid;
		if (getDebug()) {
			stdoutput.write("open request {\n");
			stdoutput.printf("	cursor id: %d\n",cursorid);
			stdoutput.write("}\n");
		}
	} else {
		cursor=cont->getCursor(cursorid);
		if (!cursor) {
			if (getDebug()) {
				stdoutput.printf(
					"cursor id %d not found\n",cursorid);
			}
			return sendCursorNotOpenError();
		}
	}

	if (options&OPTION_PARSE) {

		// reset column type cache flag
		columntypescached[cont->getId(cursor)]=false;

		// bounds checking
		if (querylen>maxquerysize) {
			// FIXME: implement this
			//return sendErrPacket(1105,"Unknown error","24000");
			return false;
		}

		// copy the query into the cursor's query buffer
		char	*querybuffer=cont->getQueryBuffer(cursor);
		bytestring::copy(querybuffer,query,querylen);
		querybuffer[querylen]='\0';
		cont->setQueryLength(cursor,querylen);
	
		// prepare the query
		if (!cont->prepareQuery(cursor,
					cont->getQueryBuffer(cursor),
					cont->getQueryLength(cursor),
					true,true,true)) {
			if (getDebug()) {
				stdoutput.printf("prepare query failed\n");
			}
			return sendQueryError(cursor);
		}
	}

	if (options&OPTION_EXECUTE) {

		// execute the query
		if (!cont->executeQuery(cursor,true,true,true,true)) {
			if (getDebug()) {
				stdoutput.printf("execute query failed\n");
			}
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
	unsigned char	ttccode;

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

			const unsigned char	unknown3[]={
				0x06, 0x02, 0x8C
			};
			reqpacket.append(unknown3,sizeof(unknown3));

			// column count
			write(&reqpacket,(unsigned char)colcount);

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

				if (getDebug()) {
					stdoutput.write("query3 response "
								"row {\n");
				}

				putRow(cursor,colcount,true);

				if (getDebug()) {
					stdoutput.write("}\n");
				}

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

		const unsigned char	unknown1[]={
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
		write(&reqpacket,(unsigned char)((colcount)?3:47));

		const unsigned char	unknown2[]={
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
			const unsigned char	unknown3[]={
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00
			};
			reqpacket.append(unknown3,sizeof(unknown3));
		}

		if (getDebug()) {
			stdoutput.write("query3 response (footer) {\n");
			stdoutput.printf("	data flags: 0x%04x\n",
								dataflags);
			debugTtcCode(ttccode);
			stdoutput.write("}\n");
		}
	}

	return sendPacket(true);
}

bool sqlrprotocol_oracle::execute(const unsigned char *rp) {

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
		stdoutput.write("execute request {\n");
		debugOptions(options,moreoptions);
		stdoutput.printf("	cursor id: %d\n",cursorid);
		stdoutput.write("}\n");
	}

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(cursorid);
	if (!cursor) {
		if (getDebug()) {
			stdoutput.printf("cursor id %d not found\n",cursorid);
		}
		return sendCursorNotOpenError();
	}

	// execute the query
	if (!cont->executeQuery(cursor,true,true,true,true)) {
		if (getDebug()) {
			stdoutput.printf("execute query failed\n");
		}
		return sendQueryError(cursor);
	}

	return sendExecuteResponse(cursor);
}

bool sqlrprotocol_oracle::sendExecuteResponse(sqlrservercursor *cursor) {

	resetSendPacketBuffer(PACKET_DATA);

	// FIXME: decode this...

	uint16_t	dataflags=0;
	// FIXME: not a valid ttccode type...
	unsigned char	ttccode=4;
	unsigned char	unknown[]={
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

	if (getDebug()) {
		stdoutput.write("execute response {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		stdoutput.write("}\n");
	}

	return sendPacket(true);
}

bool sqlrprotocol_oracle::fetch(const unsigned char *rp) {

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
		stdoutput.printf("fetch request {\n");
		debugOptions(options,moreoptions);
		stdoutput.printf("}\n");
	}

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(cursorid);
	if (!cursor) {
		if (getDebug()) {
			stdoutput.printf("cursor id %d not found\n",cursorid);
		}
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
				unsigned char	ttccode=16;

				writeBE(&reqpacket,dataflags);
				write(&reqpacket,ttccode);

				if (getDebug()) {
					stdoutput.write("fetch response "
							"header {\n");
					stdoutput.printf("	data flags: "
							"0x%04x\n",dataflags);
					debugTtcCode(ttccode);
					stdoutput.write("}\n");
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
				const unsigned char	unknown[]={
					0x00, 0x00,
				};
				reqpacket.append(unknown,sizeof(unknown));
			}

			// always appears to be the same...
			const unsigned char	unknown2[]={
				0x06, 0x02,
			};
			reqpacket.append(unknown2,sizeof(unknown2));

			// FIXME: this varies, but it's not clear with what
			const unsigned char	unknown3[]={
				0x8C
			};
			reqpacket.append(unknown3,sizeof(unknown3));

			write(&reqpacket,(unsigned char)colcount);

			// always appears to be the same...
			const unsigned char	unknown4[]={
				0x00, 0x00, 0x00,
				0x01, 0x00, 0x00, 0x00
			};
			reqpacket.append(unknown4,sizeof(unknown4));
		}

		// no idea...
		write(&reqpacket,(unsigned char)7);

		if (getDebug()) {
			stdoutput.printf("fetch response row {\n");
		}

		putRow(cursor,colcount,exactfetch);

		if (getDebug()) {
			stdoutput.write("}\n");
		}

		// FIXME: kludgy
		cont->nextRow(cursor);

		rowsfetched++;

	} while (rowsfetched<rowstofetch);

	if (rowsfetched) {

		if (exactfetch) {
			const unsigned char	unknown5[]={
				// ???
				0x08, 0x04, 0x00
			};

			const unsigned char	unknown6[][1]={
				// 1 columns
				{0xA6},
				// 2 columns
				{0xA5}
			};

			const unsigned char	unknown7[]={
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
	
			const unsigned char	unknown8[][4]={
				// 1 column
				{0x11, 0x00, 0x03, 0x00},
				// 2 columns
				{0x0E, 0x00, 0x03, 0x00}
			};
	
			const unsigned char	unknown9[]={
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
			const unsigned char	unknown10[]={
				0x27
			};
	
			const unsigned char	unknown11[]={
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
	
			const unsigned char	unknown[]={
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
	if (getDebug()) {
		stdoutput.write("cache column definitions {\n");
		if (!colcount) {
			stdoutput.write("	no columns\n");
		}
	}

	uint16_t	curid=cont->getId(cursor);

	if (columntypescached[curid]) {
		if (getDebug()) {
			stdoutput.write("	already cached\n");
			stdoutput.write("}\n");
		}
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
					cont->getColumnTypeNameLength(cursor,i),
					cont->getColumnScale(cursor,i));
		if (getDebug()) {
			stdoutput.printf("	%s: %d\n",
				cont->getColumnTypeName(cursor,i),ct[i]);
		}
	}

	columntypescached[curid]=true;

	if (getDebug()) {
		stdoutput.write("}\n");
	}
}

void sqlrprotocol_oracle::putColumnDefinitions(sqlrservercursor *cursor,
							uint32_t colcount,
							bool query3) {

	unsigned char	lengthtotal=0;
	for (uint32_t i=0; i<colcount; i++) {
		lengthtotal+=cont->getColumnLength(cursor,i);
	}
	uint32_t	constant=51;

	write(&reqpacket,lengthtotal);
	writeBE(&reqpacket,colcount);
	writeBE(&reqpacket,constant);

	if (getDebug()) {
		stdoutput.write("column definitions header {\n");
		stdoutput.printf("	length total: %d\n",lengthtotal);
		stdoutput.printf("	column count: %d\n",colcount);
		stdoutput.printf("	constant: %d\n",constant);
		stdoutput.write("}\n");
		stdoutput.write("column definitions {\n");
	}

	for (uint32_t i=0; i<colcount; i++) {
		putColumnDefinition(cursor,i,query3);
	}

	if (getDebug()) {
		stdoutput.printf("}\n");
	}
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
	unsigned char	marker1=1;
	// no idea - 128 for char/varchar, 00 for numeric
	unsigned char	marker2=128;
	unsigned char	precision=cont->getColumnPrecision(cursor,column);
	unsigned char	scale=cont->getColumnScale(cursor,column);
	// 16 for non-integer decimal, otherwise actual length
	unsigned char	length=cont->getColumnLength(cursor,column);
	unsigned char	unknown1[]={
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
	unsigned char	alias=1;
	const char	*name=cont->getColumnName(cursor,column);
	uint32_t	namelength=cont->getColumnNameLength(cursor,column);
	// no idea
	uint32_t	marker4=0;
	// no idea
	uint32_t	marker5=0;

	write(&reqpacket,marker1);
	write(&reqpacket,(unsigned char)columntype);
	write(&reqpacket,marker2);
	write(&reqpacket,precision);
	write(&reqpacket,scale);
	write(&reqpacket,length);
	reqpacket.append(unknown1,sizeof(unknown1));
	// yes, twice
	writeBE(&reqpacket,marker3);
	writeBE(&reqpacket,marker3);
	writeBE(&reqpacket,nullable);
	// yes, twice
	if (query3) {
		write(&reqpacket,(unsigned char)namelength);
		write(&reqpacket,(unsigned char)namelength);
	} else {
		write(&reqpacket,alias);
		write(&reqpacket,alias);
	}
	writeBE(&reqpacket,namelength);
	write(&reqpacket,name,namelength);
	writeBE(&reqpacket,marker4);
	writeBE(&reqpacket,marker5);

	if (getDebug()) {
		stdoutput.printf("	column %d {\n",column);
		stdoutput.printf("		marker1: %d\n",marker1);
		debugColumnType(columntypestring,columntype);
		stdoutput.printf("		marker2: %d\n",marker2);
		stdoutput.printf("		precision: %d\n",precision);
		stdoutput.printf("		scale: %d\n",scale);
		stdoutput.printf("		length: %d\n",length);
		stdoutput.printf("		marker3: %d\n",marker3);
		stdoutput.printf("		nullable: %d\n",nullable);
		stdoutput.printf("		name length: %ld\n",namelength);
		stdoutput.printf("		name: %s\n",name);
		stdoutput.printf("		marker4: %d\n",marker4);
		stdoutput.printf("		marker5: %d\n",marker5);
		stdoutput.printf("	}\n");
	}
}

uint16_t sqlrprotocol_oracle::getColumnType(const char *columntypestring,
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
	const unsigned char	unknown[]={
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
	dtsince.initialize("12/15/2012 11:15:00 EST");
	datetime	dt;
	dt.getSystemDateAndTime();
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
	uint64_t	fieldlength;
	bool		blob;
	bool		null;

	// put the fields
	for (uint32_t i=0; i<colcount; i++) {

		if (getDebug()) {
			stdoutput.printf("	col %d {\n",i);
			debugColumnType(ct[i]);
		}

		// get the field (again)
		fieldlength=0;
		blob=false;
		null=false;
		if (!cont->getField(cursor,i,&field,&fieldlength,&blob,&null)) {
			// FIXME: handle error
		}

		// put the field
		if (blob) {
			if (getDebug()) {
				stdoutput.write("		LOB\n");
			}
			putLobField(cursor,i);
		} else if (!null) {
			if (getDebug()) {
				stdoutput.printf("		\"%s\" (%d)\n",
							field,fieldlength);
			}
			putField(field,fieldlength,ct[i]);

			// no idea
			if (terminator) {
				if (i==colcount-1) {
					writeBE(&reqpacket,(uint16_t)0);
				} else {
					writeBE(&reqpacket,(uint32_t)0);
				}
			}
		}

		if (getDebug()) {
			stdoutput.write("	}\n");
		}
	}
}

void sqlrprotocol_oracle::putField(const char *field,
					uint64_t fieldlength,
					uint16_t columntype) {

	switch (columntype) {
		case ORACLE_TYPE_CHAR:
		case ORACLE_TYPE_VARCHAR:
			// what about fields longer than 255 chars?
			write(&reqpacket,(unsigned char)fieldlength);
			write(&reqpacket,field,fieldlength);
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
			if (getDebug()) {
				stdoutput.printf("unknown column type: %d\n",
								columntype);
			}
			break;
	}
}

#define MAX_BYTES_PER_CHAR	4

void sqlrprotocol_oracle::putLobField(sqlrservercursor *cursor, uint32_t col) {

	// Get lob length.  If this fails, send a NULL field.
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
	return putError(error,charstring::length(error));
}

void sqlrprotocol_oracle::putError(const char *error, uint32_t errorlength) {

	// if we hit the end of the result set then we need to send
	// ORA-01403; no data found

	uint16_t	dataflags=0;
	unsigned char	ttccode=TTI_EXECUTE;

	const unsigned char	unknown1[]={
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

	const unsigned char	unknown2[]={
		0x0A
	};

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);
	reqpacket.append(unknown1,sizeof(unknown1));
	write(&reqpacket,(unsigned char)errorlength);
	reqpacket.append(error,errorlength);
	reqpacket.append(unknown2,sizeof(unknown2));

	if (getDebug()) {
		stdoutput.write("error response {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		stdoutput.printf("	error: %*s\n",errorlength,error);
		stdoutput.write("}\n");
	}
}

bool sqlrprotocol_oracle::close(const unsigned char *rp) {

	uint16_t	cursorid;

	// FIXME: decode this...
cursorid=hackcursorid;

	if (getDebug()) {
		stdoutput.printf("close request {\n");
		stdoutput.printf("	cursor id: %d\n",cursorid);
		stdoutput.printf("}\n");
	}

	// get the requested cursor
	sqlrservercursor	*cursor=cont->getCursor(cursorid);
	if (!cursor) {
		if (getDebug()) {
			stdoutput.printf("cursor id %d not found\n",cursorid);
		}
		return sendCursorNotOpenError();
	}

	clearParams(cursor);
	pcounts[cont->getId(cursor)]=0;
	cont->abort(cursor);
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
		} else if (bv->type==SQLRSERVERBINDVARTYPE_DATE) {
			delete[] bv->value.dateval.buffer;
		}
	}
	cont->setInputBindCount(cursor,0);
}

bool sqlrprotocol_oracle::sendCloseResponse(sqlrservercursor *cursor) {

	resetSendPacketBuffer(PACKET_DATA);

	uint16_t	dataflags=0;
	// FIXME: not a valid ttccode type...
	unsigned char	ttccode=9;

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);

	if (getDebug()) {
		stdoutput.printf("close response {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		stdoutput.printf("}\n");
	}

	return sendPacket(true);
}

bool sqlrprotocol_oracle::disconnect(const unsigned char *rp) {

	// no idea
	unsigned char	unknown;
	read(rp,&unknown,&rp);

	if (getDebug()) {
		stdoutput.printf("disconnect request {\n");
		stdoutput.printf("	unknown: 0x%02x\n",unknown);
		stdoutput.printf("}\n");
	}

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
	unsigned char	ttccode=9;

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);

	if (getDebug()) {
		stdoutput.printf("disconnect response {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		stdoutput.printf("}\n");
	}

	return sendPacket(true);
}

bool sqlrprotocol_oracle::cancel(const unsigned char *rp) {
	// FIXME: implement this
	return false;
}

bool sqlrprotocol_oracle::version(const unsigned char *rp) {
	
	// FIXME: decode this...

	if (getDebug()) {
		stdoutput.write("version request {\n");
		stdoutput.write("}\n");
	}

	return sendVersionResponse();
}

bool sqlrprotocol_oracle::sendVersionResponse() {

	resetSendPacketBuffer(PACKET_DATA);

	uint16_t	dataflags=0;
	unsigned char	ttccode=TTC_OK;
	// FIXME: get this from the db
	const char	*serverversion=
			"Oracle8 Release 8.0.5.0.0 - Production\n"
			"PL/SQL Release 8.0.5.0.0 - Production";
	unsigned char	unknown[]={
		0x50, 0x00, 0x08, 0x09
	};

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);
	// writeLE?
	writeHost(&reqpacket,(uint16_t)charstring::length(serverversion));
	write(&reqpacket,serverversion);
	reqpacket.append(unknown,sizeof(unknown));

	if (getDebug()) {
		stdoutput.write("version response {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		stdoutput.printf("	server version:\n%s\n",serverversion);
		stdoutput.write("}\n");
	}

	return sendPacket(true);
}

bool sqlrprotocol_oracle::occa(const unsigned char *rp,
				const unsigned char **rpout) {

	// no idea what this is at all,
	// and it doesn't appear to have a response
	
	// FIXME: decode this...
	unsigned char	unknown[11];
	for (uint16_t i=0; i<sizeof(unknown); i++) {
		read(rp,&(unknown[i]),&rp);
	}

	*rpout=rp;

	if (getDebug()) {
		stdoutput.write("occa {\n");
		debugHexDump(unknown,sizeof(unknown));
		stdoutput.write("}\n");
	}

	return true;
}

bool sqlrprotocol_oracle::logonUnknown(const unsigned char *rp) {

	// no idea

	// FIXME: decode this...

	if (getDebug()) {
		stdoutput.write("logon unknown request {\n");
		stdoutput.write("}\n");
	}

	return sendLogonUnknownResponse();
}

bool sqlrprotocol_oracle::sendLogonUnknownResponse() {

	resetSendPacketBuffer(PACKET_DATA);

	uint16_t	dataflags=0;
	unsigned char	ttccode=TTC_OK;

	// no idea
	unsigned char	unknown[]={
		0x0C,
		0x00, 0x00, 0x00, 0x67,
		0x70, 0x00,
		0x00, 0x00, 0x00, 0x09
	};

	writeBE(&reqpacket,dataflags);
	write(&reqpacket,ttccode);
	reqpacket.append(unknown,sizeof(unknown));

	if (getDebug()) {
		stdoutput.write("logon unknown response {\n");
		stdoutput.printf("	data flags: 0x%04x\n",dataflags);
		debugTtcCode(ttccode);
		stdoutput.write("}\n");
	}

	return sendPacket(true);
}


void sqlrprotocol_oracle::putGenericFooter() {

	// no idea...

	// 8i server sends this to 8i client
	//
	// 8.0.5 server doesn't send it to 8.0.5 client,
	// but 8.0.5 client tolerates it
	unsigned char	footer[]={
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
	uint32_t	errorlength;
	int64_t		errnum;
	bool		liveconnection;
	cont->errorMessage(cursor,
				&errorstring,
				&errorlength,
				&errnum,
				&liveconnection);
stdoutput.printf("%*s\n",errorlength,errorstring);
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
						sqlrprotocols *ps,
						domnode *parameters) {
		return new sqlrprotocol_oracle(cont,ps,parameters);
	}
}
