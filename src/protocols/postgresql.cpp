// Copyright (c) 1999-2019  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/character.h>
#include <rudiments/bytebuffer.h>
#include <rudiments/process.h>
#include <rudiments/randomnumber.h>
#include <rudiments/error.h>

// request/response packet types
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
#define MESSAGE_BINDCOMPLETE		'2'
#define MESSAGE_EXECUTE			'E'
#define MESSAGE_SYNC			'S'
#define MESSAGE_DESCRIBE		'D'
#define MESSAGE_NODATA			'n'
#define MESSAGE_CLOSE			'C'
#define MESSAGE_CLOSECOMPLETE		'3'
#define MESSAGE_TERMINATE		'X'


// auth types
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

// (error) field types
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

// sqlrelay-column-type to postgresql-column-type map.
#if 0
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
#endif


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

		bool	query();
		bool	emptyQuery(const char *query);
		bool	sendQueryResult(sqlrservercursor *cursor,
						bool sendrowdescription);
		bool	sendResultSet(sqlrservercursor *cursor,
							uint16_t colcount);
		bool	sendRowDescription(sqlrservercursor *cursor,
							uint16_t colcount);
		bool	sendDataRow(sqlrservercursor *cursor,
							uint16_t colcount);
		bool	sendCommandComplete(sqlrservercursor *cursor);
		bool	sendEmptyQueryResponse();

		bool	parse();
		bool	bind();
		bool	describe();
		bool	sendNoData();
		bool	execute();
		bool	sync();
		bool	close();

		bool	sendCursorError(sqlrservercursor *cursor);
		bool	sendNotImplementedError();
		bool	sendOutOfCursorsError();
		bool	sendCursorNotOpenError();
		bool	sendTooManyBindsError();

		void	debugRecvTypeError();
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

		char		**bindvarnames;
		int16_t		*bindvarnamesizes;

		char		lobbuffer[32768];

		dictionary<char *, sqlrservercursor *>	stmtcursormap;
		dictionary<char *, sqlrservercursor *>	portalcursormap;
};


sqlrprotocol_postgresql::sqlrprotocol_postgresql(sqlrservercontroller *cont,
					sqlrprotocols *ps,
					domnode *parameters) :
					sqlrprotocol(cont,ps,parameters) {

	clientsock=NULL;

	reqpacketsize=0;
	reqpacket=NULL;
	reqtype=MESSAGE_NULL;

	rand.setSeed(randomnumber::getSeed());

	maxquerysize=cont->getConfig()->getMaxQuerySize();
	maxbindcount=cont->getConfig()->getMaxBindCount();

	bindvarnames=new char *[maxbindcount];
	bindvarnamesizes=new int16_t[maxbindcount];
	for (uint16_t i=0; i<maxbindcount; i++) {
		charstring::printf(&bindvarnames[i],"$%d",i+1);
		bindvarnamesizes[i]=charstring::length(bindvarnames[i]);
	}

	init();
}

sqlrprotocol_postgresql::~sqlrprotocol_postgresql() {

	for (uint16_t i=0; i<maxbindcount; i++) {
		delete[] bindvarnames[i];
	}
	delete[] bindvarnames;

	free();
	delete[] reqpacket;
}

void sqlrprotocol_postgresql::init() {
	user=NULL;
	password=NULL;
	database=NULL;
	replication=NULL;
}

void sqlrprotocol_postgresql::free() {
	delete[] user;
	delete[] password;
	delete[] database;
	delete[] replication;
	options.clearAndArrayDelete();
}


clientsessionexitstatus_t sqlrprotocol_postgresql::clientSession(
						filedescriptor *cs) {

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
			if (!recvPacket()) {
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
				case MESSAGE_BIND:
					loop=bind();
					break;
				case MESSAGE_DESCRIBE:
					loop=describe();
					break;
				case MESSAGE_EXECUTE:
					loop=execute();
					break;
				case MESSAGE_SYNC:
					loop=sync();
					break;
				case MESSAGE_CLOSE:
					loop=close();
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
		authenticate() &&
		// FIXME: send BackendKeyData and ParameterStatus here...
		sendReadyForQuery();
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

	// fail if the user wasn't sent (it's required)
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

	// respond with the error

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

	// clear binds
	cont->getBindPool(cursor)->clear();
	cont->setInputBindCount(cursor,0);

	// FIXME: There could be multiple queries.  If so, and if not in a
	// transaction, then we need to start one and commit/rollback it later.
	bool	result=false;
	if (emptyQuery(query)) {
		result=sendEmptyQueryResponse();
	} else {
		if (cont->prepareQuery(cursor,query,querylength,
						true,true,true) &&
			cont->executeQuery(cursor,true,true,true,true)) {
			result=sendQueryResult(cursor,true);
		} else {
			result=sendCursorError(cursor);
		}
	}

	// release the cursor
	cont->setState(cursor,SQLRCURSORSTATE_AVAILABLE);

	return (result)?sendReadyForQuery():false;
}

bool sqlrprotocol_postgresql::emptyQuery(const char *query) {
	return !(cont->skipWhitespaceAndComments(query)[0]);
}

bool sqlrprotocol_postgresql::sendQueryResult(sqlrservercursor *cursor,
						bool sendrowdescription) {
	uint16_t	colcount=cont->colCount(cursor);
	if (colcount) {
		if (sendrowdescription &&
			!sendRowDescription(cursor,colcount)) {
			return false;
		}
		return sendResultSet(cursor,colcount);
	}
	return sendCommandComplete(cursor);
}

bool sqlrprotocol_postgresql::sendResultSet(sqlrservercursor *cursor,
							uint16_t colcount) {

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
		write(&resppacket,cont->getColumnName(cursor,i));
		write(&resppacket,'\0');

		// table oid (or 0 if not known)
		const char	*tablename=cont->getColumnTable(cursor,i);
		uint32_t	tableoid=0;
		if (charstring::isNumber(tablename)) {
			// The postgresql backend returns oid's unless
			// tablemangling=lookup is set.  If we get a number
			// for the table name, then assume the backend is
			// returning oid's.
			tableoid=charstring::toInteger(tablename);
		}
		writeBE(&resppacket,tableoid);

		// column "attribute number" (or 0 if not known)
		// FIXME: no idea what this even is
		writeBE(&resppacket,(uint16_t)0);

		// data type oid (or 0 if not known)
		const char	*coltypename=cont->getColumnTypeName(cursor,i);
		uint32_t	coltypeoid=0;
		if (charstring::isNumber(coltypename)) {
			// The postgresql backend returns oid's unless
			// typemangling=yes/lookup is set.  If we get a number
			// for the type name, then assume the backend is
			// returning oid's.
			coltypeoid=charstring::toInteger(coltypename);
		} else {
			// FIXME: map column types to oid's
		}
		writeBE(&resppacket,coltypeoid);

		// data type size
		// FIXME: should be negative for variable-width types
		writeBE(&resppacket,(uint16_t)cont->getColumnLength(cursor,i));

		// type modifier
		// FIXME: no idea what this even is
		writeBE(&resppacket,(uint32_t)0);

		// format code text=0, binary=1
		// for now, we always return text, even if binary was requested
		writeBE(&resppacket,(uint16_t)0);

		
		if (getDebug()) {
			stdoutput.printf("	column %d {\n",i);
			stdoutput.printf("		name: %s\n",
						cont->getColumnName(cursor,i));
			stdoutput.printf("		table name: %s\n",
								tablename);
			stdoutput.printf("		table oid: %d\n",
								tableoid);
			stdoutput.printf("		attribute number: 0\n");
			stdoutput.printf("		column type name: %s\n",
								coltypename);
			stdoutput.printf("		data type oid: %d\n",
								coltypeoid);
			stdoutput.printf("		data type size: %d\n",
					cont->getColumnLength(cursor,i));
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
	// 	char[]		stmt name
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

	// get stmt name
	const char	*stmtname=(const char *)rp;
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
	if (paramcount>maxbindcount) {
		return sendTooManyBindsError();
	}
	uint32_t	*paramtypes=new uint32_t[paramcount];
	for (uint16_t i=0; i<paramcount; i++) {
		readBE(rp,&(paramtypes[i]),&rp);
	}

	// map stmt -> cursor
	stmtcursormap.setValue(charstring::duplicate(stmtname),cursor);

	// debug
	debugStart("Parse");
	if (getDebug()) {
		stdoutput.printf("	stmt name: %s\n",stmtname);
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

	// FIXME: do something with param types?
	delete[] paramtypes;

	// bounds checking
	if (querylength>maxquerysize) {
		return sendErrorResponse("Query is too large");
	}

	// copy the query into the cursor's query buffer
	char	*querybuffer=cont->getQueryBuffer(cursor);
	bytestring::copy(querybuffer,query,querylength);
	querybuffer[querylength]='\0';
	cont->setQueryLength(cursor,querylength);

	// clear binds
	cont->getBindPool(cursor)->clear();
	cont->setInputBindCount(cursor,0);

	// prepare the query
	if (!cont->prepareQuery(cursor,cont->getQueryBuffer(cursor),
					cont->getQueryLength(cursor),
					true,true,true)) {
		return sendCursorError(cursor);
	}


	// response packet data structure
	//
	// data {
	// }

	debugStart("ParseComplete");
	debugEnd();

	// build response packet
	resppacket.clear();

	// send response packet
	return sendPacket(MESSAGE_PARSECOMPLETE);
}

bool sqlrprotocol_postgresql::bind() {

	// request packet data structure:
	//
	// data {
	// 	char[]		portal name
	//	char[]		stmt name
	//	uint16_t	param format code count
	//	uint16_t[]	param formats (0=text, 1=binary)
	//	uint16_t	param value count
	//
	//	// param values...
	//	int32_t		value length (-1 = null)
	//	byte[]		parameter value
	//
	//	uint16_t	result format code count
	//	uint16_t[]	result formats (0=text, 1=binary)
	// }

	debugStart("Bind");

	// parse request packet
	const unsigned char	*rp=reqpacket;
	const unsigned char	*rpend=rp+reqpacketsize;

	stringbuffer	portal;
	stringbuffer	stmtname;
	readString(rp,rpend,&portal,&rp);
	readString(rp,rpend,&stmtname,&rp);
	
	// get the requested cursor
	sqlrservercursor	*cursor=NULL;
	if (!stmtcursormap.getValue((char *)stmtname.getString(),&cursor)) {
		// FIXME: invalid cursor error...
	}

	// map portal -> cursor
	portalcursormap.setValue(
		charstring::duplicate(portal.getString()),cursor);

	// get and clear the bind pool
	memorypool		*bindpool=cont->getBindPool(cursor);
	bindpool->clear();

	// get the input binds
	sqlrserverbindvar	*inbinds=cont->getInputBinds(cursor);

	// debug
	if (getDebug()) {
		stdoutput.printf("	portal name: %s\n",portal.getString());
		stdoutput.printf("	stmt name: %s\n",stmtname.getString());
		stdoutput.printf("	cursor id: %d\n",cursor->getId());
	}

	// param format codes...
	uint16_t	paramformatcodecount;
	readBE(rp,&paramformatcodecount,&rp);
	if (paramformatcodecount>maxbindcount) {
		return sendTooManyBindsError();
	}
	uint16_t	*paramformatcodes=NULL;
	if (paramformatcodecount) {
		// FIXME: use the bind pool
		paramformatcodes=new uint16_t[paramformatcodecount];
		for (uint16_t i=0; i<paramformatcodecount; i++) {
			readBE(rp,&(paramformatcodes[i]),&rp);
		}
	}

	// debug
	if (getDebug()) {
		stdoutput.printf("	param format codes: (%d) ",
							paramformatcodecount);
		for (uint16_t i=0; i<paramformatcodecount; i++) {
			stdoutput.printf("%d",paramformatcodes[i]);
		}
		stdoutput.write('\n');
	}

	// param values...
	uint16_t	paramvaluecount;
	readBE(rp,&paramvaluecount,&rp);
	if (paramvaluecount>maxbindcount) {
		return sendTooManyBindsError();
	}
	// debug
	if (getDebug()) {
		stdoutput.printf("	param value count: %d\n",
							paramvaluecount);
	}
	for (uint16_t i=0; i<paramvaluecount; i++) {

		sqlrserverbindvar	*bv=&(inbinds[i]);

		if (getDebug()) {
			stdoutput.printf("	param %d {\n",i);
		}

		// get the variable name
		bv->variable=bindvarnames[i];
		bv->variablesize=bindvarnamesizes[i];

		if (getDebug()) {
			stdoutput.printf("		name: %s\n",
							bv->variable);
		}

		if (!paramformatcodecount || !paramformatcodes[i]) {

			// text parameter...

			// get length/null-indicator
			uint32_t	paramlength;
			readBE(rp,&paramlength,&rp);

			if (getDebug()) {
				stdoutput.printf("		"
						"format: text\n");
				stdoutput.printf("		"
						"length: %d\n",paramlength);
			}

			if (paramlength==(uint32_t)-1) {

				// bind null
				bv->type=SQLRSERVERBINDVARTYPE_NULL;
				bv->isnull=cont->nullBindValue();

				if (getDebug()) {
					stdoutput.printf("		"
							"value: (null)\n");
				}

			} else {

				// bind string
				bv->type=SQLRSERVERBINDVARTYPE_STRING;
				bv->valuesize=paramlength;
				bv->value.stringval=
					(char *)bindpool->allocate(
							bv->valuesize+1);
				read(rp,bv->value.stringval,bv->valuesize,&rp);
				bv->value.stringval[bv->valuesize]='\0';
				bv->isnull=cont->nonNullBindValue();

				if (getDebug()) {
					stdoutput.printf("		"
							"value: %s\n",
							bv->value.stringval);
				}
			}

		} else {

			if (getDebug()) {
				stdoutput.printf("		"
						"format: binary\n");
			}

			// FIXME: binary parameter...
		}

		debugEnd(1);
	}

	// set the bind count
	cont->setInputBindCount(cursor,paramvaluecount);

	// result format codes...
	// FIXME: what do we do with these?
	uint16_t	resultformatcodecount;
	readBE(rp,&resultformatcodecount,&rp);
	uint16_t	*resultformatcodes=NULL;
	if (resultformatcodecount) {
		// FIXME: use the bind pool
		resultformatcodes=new uint16_t[resultformatcodecount];
		for (uint16_t i=0; i<resultformatcodecount; i++) {
			readBE(rp,&(resultformatcodes[i]),&rp);
		}
	}

	// debug
	if (getDebug()) {
		stdoutput.printf("	result format codes: (%d) ",
							resultformatcodecount);
		for (uint16_t i=0; i<resultformatcodecount; i++) {
			stdoutput.printf("%d",resultformatcodes[i]);
		}
		stdoutput.write('\n');
	}
	debugEnd();

	delete[] paramformatcodes;
	delete[] resultformatcodes;

	// response packet data structure
	//
	// data {
	// }

	debugStart("BindComplete");
	debugEnd();

	// build response packet
	resppacket.clear();

	// send response packet
	return sendPacket(MESSAGE_BINDCOMPLETE);
}

bool sqlrprotocol_postgresql::describe() {

	// request packet data structure:
	//
	// data {
	//	char		S (stmt) or P (portal)
	//	char[]		stmt/portal name
	// }

	// parse request packet
	const unsigned char	*rp=reqpacket;
	const unsigned char	*rpend=rp+reqpacketsize;

	char	sorp;
	read(rp,&sorp,&rp);

	stringbuffer	name;
	readString(rp,rpend,&name,&rp);
	
	// decide whether to use stmt/portal -> cursor map
	dictionary<char *, sqlrservercursor *>	*dict=
			(sorp=='S')?&stmtcursormap:&portalcursormap;

	// get the requested cursor
	sqlrservercursor	*cursor=NULL;
	if (!dict->getValue((char *)name.getString(),&cursor)) {
		// FIXME: invalid cursor error...
	}

	// debug
	debugStart("Describe");
	if (getDebug()) {
		stdoutput.printf("	S or P: %c\n",sorp);
		stdoutput.printf("	name: %s\n",name.getString());
		stdoutput.printf("	cursor id: %d\n",cursor->getId());
	}
	debugEnd();

	// return RowDescription or NoData if the statement will not return rows
	// (If there are no columns, then there can't be any rows)
	uint16_t	colcount=cont->colCount(cursor);
	return (colcount)?sendRowDescription(cursor,colcount):sendNoData();
}

bool sqlrprotocol_postgresql::sendNoData() {
	
	// response packet data structure:
	//
	// data {
	// }

	// debug
	debugStart("NoData");
	debugEnd();

	// build response packet
	resppacket.clear();

	// send response packet
	return sendPacket(MESSAGE_NODATA);
}

bool sqlrprotocol_postgresql::execute() {

	// request packet data structure:
	//
	// data {
	//	char[]		portal name
	//	uint32_t	max number of rows to return
	// }

	// parse request packet
	const unsigned char	*rp=reqpacket;
	const unsigned char	*rpend=rp+reqpacketsize;

	stringbuffer	portal;
	readString(rp,rpend,&portal,&rp);
	
	uint32_t	maxrows;
	readBE(rp,&maxrows,&rp);

	// get the requested cursor
	sqlrservercursor	*cursor=NULL;
	if (!portalcursormap.getValue((char *)portal.getString(),&cursor)) {
		// FIXME: invalid cursor error...
	}

	debugStart("Execute");
	if (getDebug()) {
		stdoutput.printf("	portal name: %s\n",portal.getString());
		stdoutput.printf("	cursor id: %d\n",cursor->getId());
		stdoutput.printf("	max rows: %d\n",maxrows);
	}
	debugEnd();

	// execute the query
	if (!cont->executeQuery(cursor,true,true,true,true)) {
		return sendCursorError(cursor);
	}

	if (emptyQuery(cont->getQueryBuffer(cursor))) {
		return sendEmptyQueryResponse();
	}

	return sendQueryResult(cursor,false);
}

bool sqlrprotocol_postgresql::sync() {

	// request packet data structure:
	//
	// data {
	// }

	// parse request packet (nothing to do)

	// debug
	debugStart("Sync");
	debugEnd();

	// The docs say:
	//
	// This parameterless message causes the backend to close the current
	// transaction if it's not inside a BEGIN/COMMIT transaction block
	// ("close" meaning to commit if no error, or roll back if error).
	//
	// However, we'll be in an autocommit state if we're not inside of a
	// transaction block.  So, we don't need to commit/rollback, the
	// backend will automatically do that for us.

	// send response packet
	return sendReadyForQuery();
}

bool sqlrprotocol_postgresql::close() {

	// The client would like to close the specified cursor.

	// request packet data structure:
	//
	// data {
	//	char		S (stmt) or P (portal)
	//	char[]		stmt/portal name
	// }

	// parse request packet
	const unsigned char	*rp=reqpacket;
	const unsigned char	*rpend=rp+reqpacketsize;

	char	sorp;
	read(rp,&sorp,&rp);

	stringbuffer	name;
	readString(rp,rpend,&name,&rp);

	// decide whether to use stmt/portal -> cursor map
	dictionary<char *, sqlrservercursor *>	*dict=
			(sorp=='S')?&stmtcursormap:&portalcursormap;
	
	// get the requested cursor
	sqlrservercursor	*cursor=NULL;
	if (!dict->getValue((char *)name.getString(),&cursor)) {
		// FIXME: invalid cursor error...
	}

	// debug
	debugStart("Close");
	if (getDebug()) {
		stdoutput.printf("	S or P: %c\n",sorp);
		stdoutput.printf("	name: %s\n",name.getString());
		stdoutput.printf("	cursor id: %d\n",cursor->getId());
	}
	debugEnd();

	// remove stmt/portal -> cursor mapping
	dict->removeAndArrayDeleteKey((char *)name.getString());

	// mark the cursor available
	cont->setState(cursor,SQLRCURSORSTATE_AVAILABLE);

	debugStart("CloseComplete");
	debugEnd();

	// build response packet
	resppacket.clear();

	// send response packet
	return sendPacket(MESSAGE_CLOSECOMPLETE);
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
		stdoutput.printf("invalid packet type: %c\n",reqtype);
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
