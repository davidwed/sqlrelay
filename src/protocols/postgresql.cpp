// Copyright (c) 1999-2019  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/character.h>
#include <rudiments/bytebuffer.h>
#include <rudiments/process.h>
#include <rudiments/randomnumber.h>
#include <rudiments/file.h>
#include <rudiments/error.h>

#include <datatypes.h>

// request/response packet types
#define MESSAGE_NULL			0x00
#define MESSAGE_AUTHENTICATION		'R'
#define MESSAGE_PASSWORD		'p'
#define MESSAGE_ERRORRESPONSE		'E'
#define MESSAGE_BACKENDKEYDATA		'K'
#define MESSAGE_PARAMETERSTATUS		'S'
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
		bool	handleTlsRequest();
		void	parseOptions(const char *opts);
		bool	sendStartupMessageResponse();
		bool	sendAuthenticationCleartextPassword();
		bool	sendAuthenticationMD5Password();
		bool	recvPasswordMessage();
		bool	authenticate();
		bool	sendAuthenticationOk();
		bool	sendBackendKeyData();
		bool	sendStartupParameterStatuses();
		bool	sendParameterStatus(const char *name,
						const char *value);
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
		void	getQuery(const char *query,
						const char **start,
						const char **end);
		const char	*skipWhitespace(const char *str);
		bool	sendQueryResult(sqlrservercursor *cursor,
						bool sendrowdescription,
						uint32_t maxrows);
		bool	sendResultSet(sqlrservercursor *cursor,
							uint16_t colcount,
							uint32_t maxrows);
		bool	sendRowDescription(sqlrservercursor *cursor,
							uint16_t colcount);
		uint32_t	getColumnTypeOid(uint16_t coltype);
		bool	sendDataRow(sqlrservercursor *cursor,
							uint16_t colcount);
		bool	sendCommandComplete(sqlrservercursor *cursor);
		bool	sendEmptyQueryResponse();

		bool	parse();
		bool	bind();
		void	bindTextParameter(const unsigned char *rp,
						uint32_t paramlength,
						memorypool *bindpool,
						sqlrserverbindvar *bv,
						const unsigned char **rpout);
		bool	bindBinaryParameter(const unsigned char *rp,
						uint32_t oid,
						uint32_t paramlength,
						memorypool *bindpool,
						sqlrserverbindvar *bv,
						const unsigned char **rpout);
		bool	describe();
		bool	sendNoData();
		bool	execute();
		bool	emptyQuery(const char *query);
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

		stringbuffer	sv;
		char		*serverencoding;
		char		*clientencoding;
		char		*applicationname;
		char		*issuperuser;
		char		*sessionauth;
		char		*datestyle;
		char		*intervalstyle;
		char		*timezone;
		char		*integerdatetimes;
		char		*stdconfstr;

		char		*user;
		char		*password;
		char		*database;
		char		*replication;
		dictionary<char *, char *>	options;

		const char	*authmethod;
		randomnumber	rand;
		uint32_t	salt;
		uint32_t	secretkey;

		uint32_t	maxquerysize;
		uint16_t	maxbindcount;

		char		**bindvarnames;
		int16_t		*bindvarnamesizes;

		dictionary<char *, sqlrservercursor *>	stmtcursormap;
		dictionary<char *, sqlrservercursor *>	portalcursormap;
		dictionary<sqlrservercursor *, uint32_t *>	paramoids;
		dictionary<sqlrservercursor *, bool>		executeflag;
};


sqlrprotocol_postgresql::sqlrprotocol_postgresql(sqlrservercontroller *cont,
					sqlrprotocols *ps,
					domnode *parameters) :
					sqlrprotocol(cont,ps,parameters) {

	clientsock=NULL;

	serverencoding=NULL;
	clientencoding=NULL;
	applicationname=NULL;
	issuperuser=NULL;
	sessionauth=NULL;
	datestyle=NULL;
	intervalstyle=NULL;
	timezone=NULL;
	integerdatetimes=NULL;
	stdconfstr=NULL;

	stmtcursormap.setManageArrayKeys(true);
	portalcursormap.setManageArrayKeys(true);
	options.setManageArrayKeys(true);
	options.setManageArrayValues(true);
	paramoids.setManageArrayValues(true);

	authmethod="postgresql_md5";
	const char	*pwds=parameters->getAttributeValue("passwords");
	if (!charstring::compareIgnoringCase(pwds,"cleartext")) {
		authmethod="postgresql_cleartext";
	}

	if (getDebug()) {
		debugStart("parameters");
		stdoutput.printf("	authmethod: %s\n",authmethod);
		if (useTls()) {
			stdoutput.printf("	tls: yes\n");
			stdoutput.printf("	tls version: %s\n",
				getTlsContext()->getProtocolVersion());
			stdoutput.printf("	tls cert: %s\n",
				getTlsContext()->getCertificateChainFile());
			stdoutput.printf("	tls key: %s\n",
				getTlsContext()->getPrivateKeyFile());
			stdoutput.printf("	tls password: %s\n",
				getTlsContext()->getPrivateKeyPassword());
			stdoutput.printf("	tls validate: %d\n",
				getTlsContext()->getValidatePeer());
			stdoutput.printf("	tls ca: %s\n",
				getTlsContext()->getCertificateAuthority());
			stdoutput.printf("	tls ciphers: %s\n",
				getTlsContext()->getCiphers());
			stdoutput.printf("	tls depth: %d\n",
				getTlsContext()->getValidationDepth());
		} else {
			stdoutput.printf("	tls: no\n");
		}
		debugEnd();
	}

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

	delete[] serverencoding;
	delete[] clientencoding;
	delete[] applicationname;
	delete[] issuperuser;
	delete[] sessionauth;
	delete[] datestyle;
	delete[] intervalstyle;
	delete[] timezone;
	delete[] integerdatetimes;
	delete[] stdconfstr;
}

void sqlrprotocol_postgresql::init() {
	user=NULL;
	password=NULL;
	database=NULL;
	replication=NULL;
	salt=0;
	secretkey=0;
}

void sqlrprotocol_postgresql::free() {
	delete[] user;
	delete[] password;
	delete[] database;
	delete[] replication;
	options.clear();
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
		stmtcursormap.clear();
		portalcursormap.clear();
		executeflag.clear();
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
	if (getDebug()) {
		debugStart("recv");
		stdoutput.printf("	type: %c\n",reqtype);
		stdoutput.printf("	size: %d\n",reqpacketsize);
		debugHexDump(reqpacket,reqpacketsize);
		debugEnd();
	}

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
	if (getDebug()) {
		debugStart("send");
		if (type!=MESSAGE_NULL) {
			stdoutput.printf("	type: %c\n",type);
		} else {
			stdoutput.printf("	type: (null)\n");
		}
		stdoutput.printf("	size: %d\n",resppacket.getSize());
		debugHexDump(resppacket.getBuffer(),resppacket.getSize());
		debugEnd();
	}

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
		sendBackendKeyData() &&
		sendStartupParameterStatuses() &&
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

	bool	usingtls=false;

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

			// FIXME: support SSL

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

			// Yes=S, No=N
			const char	*response=(useTls())?"S":"N";
			debugStart(response);
			debugEnd();

			// return a single byte
			if (clientsock->write(response[0])!=sizeof(char)) {
				if (getDebug()) {
					stdoutput.printf(
						"write SSL %s failed\n",
						response);
					debugSystemError();
				}
				return false;
			}
			clientsock->flushWriteBuffer(-1,-1);

			// handle TLS request
			if (useTls()) {
				if (!handleTlsRequest()) {
					return false;
				}
				usingtls=true;
			}

		} else {

			if (useTls() && !usingtls) {
				sendErrorResponse(
					"SSL Error","88P01",
					(getTlsContext()->getValidatePeer())?
						"TLS mutual auth required":
						"TLS required");
				return false;
			}

			if (protocolversion!=196608) {
				// FIXME: NegotiateProtocolVersion
				sendErrorResponse("FATAL",
							"88P01",
							"Invalid protocol");
				return false;
			}

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
	if (getDebug()) {
		debugStart("StartupMessage");
		stdoutput.printf("	protocol version: %d\n",
							protocolversion);
		stdoutput.printf("	user: %s\n",user);
		stdoutput.printf("	database: %s\n",database);
		stdoutput.printf("	replication: %s\n",replication);
		linkedlist<char *>	*keys=options.getKeys();
		for (listnode<char *> *key=keys->getFirst();
						key; key=key->getNext()) {
			stdoutput.printf("	%s: %s\n",
					key->getValue(),
					options.getValue(key->getValue()));
		}
		debugEnd();
	}

	return true;
}

bool sqlrprotocol_postgresql::handleTlsRequest() {

	debugStart("tls");

	clientsock->setSecurityContext(getTlsContext());
	getTlsContext()->setFileDescriptor(clientsock);

	if (!getTlsContext()->accept()) {

		if (getDebug()) {
			stdoutput.printf("	accept failed: %s\n",
					getTlsContext()->getErrorString());
		}
		debugEnd();

		// FIXME: the client doesn't appear to receive this...
		sendErrorResponse("SSL Error","88P01",
					getTlsContext()->getErrorString());
		return false;
	}

	if (getDebug()) {
		stdoutput.printf("	accept success\n");
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
		sendErrorResponse("FATAL","88P01","user required");
		return false;
	}

	return (!charstring::compare(authmethod,"postgresql_md5"))?
				sendAuthenticationMD5Password():
				sendAuthenticationCleartextPassword();
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
	if (getDebug()) {
		debugStart("AuthenticationCleartextPassword");
		stdoutput.printf("	auth type: %d\n",authtype);
		debugEnd();
	}

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
	//	byte[4]		salt
	// }

	// set values to send
	uint32_t	authtype=AUTH_MD5;
	rand.generateNumber(&salt);

	// debug
	if (getDebug()) {
		debugStart("AuthenticationMD5Password");
		stdoutput.printf("	auth type: %d\n",authtype);
		stdoutput.printf("	salt: %d\n",salt);
		debugEnd();
	}

	// build response packet
	resppacket.clear();
	writeBE(&resppacket,authtype);
	write(&resppacket,(unsigned char *)&salt,sizeof(salt));

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
	if (getDebug()) {
		debugStart("PasswordMessage");
		stdoutput.printf("	password: %s\n",password);
		debugEnd();
	}

	return true;
}

bool sqlrprotocol_postgresql::authenticate() {

	// build auth credentials
	sqlrpostgresqlcredentials	cred;
	cred.setUser(user);
	cred.setPassword(password);
	cred.setPasswordLength(charstring::length(password));
	cred.setMethod(authmethod);
	cred.setSalt(salt);

	// authenticate
	bool	retval=cont->auth(&cred);

	// debug
	if (getDebug()) {
		debugStart("authenticate");
		stdoutput.printf("	auth %s\n",(retval)?"success":"failed");
		debugEnd();
	}

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
	if (getDebug()) {
		debugStart("AuthenticationOk");
		stdoutput.printf("	success: %d\n",success);
		debugEnd();
	}

	// build response packet
	resppacket.clear();
	writeBE(&resppacket,success);

	// send response packet
	return sendPacket(MESSAGE_AUTHENTICATION);
}

bool sqlrprotocol_postgresql::sendBackendKeyData() {

	// response packet data structure:
	//
	// data {
	//	uint32_t	process id
	//	uint32_t	secret key
	// }

	// set values to send
	uint32_t	pid=process::getProcessId();
	rand.generateNumber(&secretkey);

	// debug
	if (getDebug()) {
		debugStart("BackendKeyData");
		stdoutput.printf("	process id: %d\n",pid);
		stdoutput.printf("	secret key: %d\n",secretkey);
		debugEnd();
	}

	// build response packet
	resppacket.clear();
	writeBE(&resppacket,pid);
	writeBE(&resppacket,secretkey);

	// send response packet
	return sendPacket(MESSAGE_BACKENDKEYDATA);
}

bool sqlrprotocol_postgresql::sendStartupParameterStatuses() {

	if (!sv.getSize()) {

		// get the dbversion
		const char	*dbtype=cont->identify();
		const char	*dbversion=cont->dbVersion();
		if (!charstring::compare(dbtype,"postgresql")) {

			// massage the dbversion
			const char	*ptr=dbversion;
			char		*major=NULL;
			char		*minor=NULL;
			char		*patch=NULL;
			if (charstring::length(dbversion)==5) {
				major=charstring::duplicate(ptr,1);
				ptr++;
			} else {
				major=charstring::duplicate(ptr,2);
				ptr+=2;
			}
			minor=charstring::duplicate(ptr,2);
			ptr+=2;
			patch=charstring::duplicate(ptr,2);
			sv.append(major)->append('.');
			sv.append(charstring::toInteger(minor))->append('.');
			sv.append(charstring::toInteger(patch));

			// get other variables
			const char	*vars[]={
				"server_encoding",
				"client_encoding",
				"application_name",
				"is_superuser",
				"session_authorization",
				"DateStyle",
				"IntervalStyle",
				"TimeZone",
				"integer_datetimes",
				"standard_conforming_strings",
				NULL
			};
			char	**vals[]={
				&serverencoding,
				&clientencoding,
				&applicationname,
				&issuperuser,
				&sessionauth,
				&datestyle,
				&intervalstyle,
				&timezone,
				&integerdatetimes,
				&stdconfstr,
				NULL
			};
			uint16_t	i=0;
			stringbuffer	q;
			const char	*field;
			uint64_t	fieldlength;
			bool		blob;
			bool		null;
			bool		error;
			sqlrservercursor	*cursor=cont->getCursor();
			for (const char **var=vars; *var; var++) {
				q.append("show ")->append(*var);
				if (!cursor ||
					!cont->prepareQuery(cursor,
							q.getString(),
							q.getStringLength()) ||
					!cont->executeQuery(cursor) ||
					!cont->fetchRow(cursor,&error) ||
					!cont->getField(cursor,0,
							&field,
							&fieldlength,
							&blob,
							&null)) {
					field="";
					fieldlength=0;
				}
				*vals[i]=charstring::duplicate(field,
								fieldlength);
				i++;
				q.clear();
			}
			if (cursor) {
				cont->setState(cursor,
						SQLRCURSORSTATE_AVAILABLE);
			}

		} else {
			// FIXME: handle other db's too
			sv.append(dbversion);

			// FIXME: get these from the database (somehow)... 
			serverencoding=charstring::duplicate("UTF8");
			clientencoding=charstring::duplicate("UTF8");
			applicationname=charstring::duplicate("");
			issuperuser=charstring::duplicate("off");
			sessionauth=charstring::duplicate("");
			datestyle=charstring::duplicate("ISO, MDY");
			intervalstyle=charstring::duplicate("postgres");
			timezone=charstring::duplicate("US/Eastern");
			integerdatetimes=charstring::duplicate("on");
			stdconfstr=charstring::duplicate("on");
		}
	}

	return sendParameterStatus("server_version",sv.getString()) &&
		sendParameterStatus("server_encoding",serverencoding) &&
		sendParameterStatus("client_encoding",clientencoding) &&
		sendParameterStatus("application_name",applicationname) &&
		sendParameterStatus("is_superuser",issuperuser) &&
		sendParameterStatus("session_authorization",sessionauth) &&
		sendParameterStatus("DateStyle",datestyle) &&
		sendParameterStatus("IntervalStyle",intervalstyle) &&
		sendParameterStatus("TimeZone",timezone) &&
		sendParameterStatus("integer_datetimes",integerdatetimes) &&
		sendParameterStatus("standard_conforming_strings",stdconfstr);
}

bool sqlrprotocol_postgresql::sendParameterStatus(const char *name,
							const char *value) {

	// response packet data structure:
	//
	// data {
	//	char[]	name
	//	char[]	value
	// }

	// debug
	if (getDebug()) {
		debugStart("ParameterStatus");
		stdoutput.printf("	name: %s\n",name);
		stdoutput.printf("	value: %s\n",value);
		debugEnd();
	}

	// build response packet
	resppacket.clear();
	write(&resppacket,name);
	write(&resppacket,'\0');
	write(&resppacket,value);
	write(&resppacket,'\0');

	// send response packet
	return sendPacket(MESSAGE_PARAMETERSTATUS);
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
	if (getDebug()) {
		debugStart("ReadyForQuery");
		stdoutput.printf("	tx block status: %c\n",txblockstatus);
		debugEnd();
	}

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
	if (getDebug()) {
		debugStart("error");
		stdoutput.printf("	field type: S\n");
		stdoutput.printf("	string: %s\n",severity);
		stdoutput.printf("	field type: C\n");
		stdoutput.printf("	string: %s\n",sqlstate);
		stdoutput.printf("	field type: M\n");
		stdoutput.printf("	string: %.*s\n",errorstringlength,
							errorstring);
		stdoutput.printf("	field type: (null)\n");
		debugEnd();
	}

	// build response packet
	resppacket.clear();

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
	if (getDebug()) {
		debugStart("query");
		stdoutput.printf("	cursor id: %d\n",cursor->getId());
		stdoutput.printf("	query length: %d\n",querylength);
		stdoutput.printf("	queries: %.*s\n",querylength,query);
		debugEnd();
	}

	// clear binds
	cont->getBindPool(cursor)->clear();
	cont->setInputBindCount(cursor,0);

	// there could be multiple queries, process them individually...
	bool	result=false;
	bool	newtx=false;
	bool	first=true;
	bool	error=false;
	for (;;) {

		// get the query
		const char	*start=NULL;
		const char	*end=NULL;
		getQuery(query,&start,&end);
		query=start;
		querylength=end-start;

		// if there's more than 1 query, then
		// we may need to begin a transaction
		if (first) {
			newtx=(*end &&
				cont->skipWhitespaceAndComments(end+1)[0] &&
				!cont->inTransaction());
			if (newtx) {
				debugStart("begin");
				debugEnd();
				cont->begin();
			}
			first=false;
		}

		if (getDebug()) {
			debugStart("individual query");
			stdoutput.printf("	query: %.*s\n",
							querylength,query);
			debugEnd();
		}

		// prepare/execute the query...
		if (!querylength) {
			result=sendEmptyQueryResponse();
		} else {
			if (cont->prepareQuery(cursor,query,querylength,
							true,true,true) &&
				cont->executeQuery(cursor,
							true,true,true,true)) {
				result=sendQueryResult(cursor,true,0);
			} else {
				result=sendCursorError(cursor);
				error=true;
				break;
			}
		}

		// stop executing queries if:
		// * a network error occurred
		// * a sql error occurred
		// * if we just executed the final query
		if (!result || error || !*end) {
			break;
		}

		// next...
		query=skipWhitespace(end+1);

		// catch if *end was a ; followed by a NULL or whitespace
		if (!*query) {
			break;
		}
	}

	// if we had to start a new transaction, then complete it here
	if (newtx) {
		if (error) {
			debugStart("rollback");
			debugEnd();
			cont->rollback();
		} else {
			debugStart("commit");
			debugEnd();
			cont->commit();
		}
	}

	// release the cursor
	cont->setState(cursor,SQLRCURSORSTATE_AVAILABLE);

	return (result)?sendReadyForQuery():false;
}

void sqlrprotocol_postgresql::getQuery(const char *query,
					const char **start,
					const char **end) {

	*start=cont->skipWhitespaceAndComments(query);

	bool	inquotes=false;
	char	quote='\0';
	const char *ch=*start;
	while (*ch) {
		if (!inquotes) {
			if (*ch=='\'' || *ch=='"' || *ch=='`') {
				inquotes=true;
				quote=*ch;
			} else if (*ch==';') {
				break;
			}
		} else {
			if (*ch==quote) {
				inquotes=false;
			}
		}
		ch++;
	}
	*end=ch;
}

const char *sqlrprotocol_postgresql::skipWhitespace(const char *str) {
	// FIXME: push this up to sqlrservercontroller
	while (*str && character::isWhitespace(*str)) {
		str++;
	}
	return str;
}

bool sqlrprotocol_postgresql::sendQueryResult(sqlrservercursor *cursor,
						bool sendrowdescription,
						uint32_t maxrows) {
	uint16_t	colcount=cont->colCount(cursor);
	if (colcount) {
		if (sendrowdescription &&
			!sendRowDescription(cursor,colcount)) {
			return false;
		}
		return sendResultSet(cursor,colcount,maxrows);
	}
	return sendCommandComplete(cursor);
}

bool sqlrprotocol_postgresql::sendResultSet(sqlrservercursor *cursor,
							uint16_t colcount,
							uint32_t maxrows) {

	uint32_t	fetched=0;
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

		fetched++;
		if (maxrows && fetched==maxrows) {
			break;
		}
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
		const char	*columnname=cont->getColumnName(cursor,i);
		write(&resppacket,columnname);
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
		// I think this is attnum in the pg_attribute table for the
		// row that corresponds to this column.  I think that
		// corresponds to the 1-based index of the column in its table,
		// except for system columns, which have (arbitrary) negative
		// numbers.
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
			coltypeoid=getColumnTypeOid(
					cont->getColumnType(cursor,i));
		}
		writeBE(&resppacket,coltypeoid);

		// data type size and modifier
		uint16_t	datatypesize=cont->getColumnLength(cursor,i);
		uint32_t	datatypemodifier=(uint32_t)-1;
		// For various types (I'm sure I'll discover others later),
		// return -1 for the size and return the size in the modifier
		if (coltypeoid==1042 || coltypeoid==1043) {
			datatypemodifier=datatypesize;
			datatypesize=(uint16_t)-1;
		}
		writeBE(&resppacket,datatypesize);
		writeBE(&resppacket,datatypemodifier);

		// format code text=0, binary=1
		// for now, we always return text, even if binary was requested
		writeBE(&resppacket,(uint16_t)0);

		
		if (getDebug()) {
			stdoutput.printf("	column %d {\n",i);
			stdoutput.printf("		name: %s\n",
							columnname);
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
							datatypesize);
			stdoutput.printf("		type modifier: %d\n",
							datatypemodifier);
			stdoutput.printf("		format code: 0\n");
			debugEnd(1);
		}
	}

	debugEnd();

	// send response packet
	return sendPacket(MESSAGE_ROWDESCRIPTION);
}

uint32_t sqlrprotocol_postgresql::getColumnTypeOid(uint16_t coltype) {

	// FIXME: use a type map
	switch (coltype) {
		case BOOL_DATATYPE:
			return 16; //bool
		case BYTEA_DATATYPE:
			return 17; //bytea
		case CHAR_DATATYPE:
			return 18; //char
		case NAME_DATATYPE:
			return 19; //name
		case INT8_DATATYPE:
			return 20; //int8
		case INT2_DATATYPE:
			return 21; //int2
		case INT2VECTOR_DATATYPE:
			return 22; //int2vector
		case INT4_DATATYPE:
			return 23; //int4
		case REGPROC_DATATYPE:
			return 24; //regproc
		case TEXT_DATATYPE:
			return 25; //text
		case OID_DATATYPE:
			return 26; //oid
		case TID_DATATYPE:
			return 27; //tid
		case XID_DATATYPE:
			return 28; //xid
		case CID_DATATYPE:
			return 29; //cid
		case OIDVECTOR_DATATYPE:
			return 30; //oidvector
		case PG_TYPE_DATATYPE:
			return 71; //pg_type
		case PG_ATTRIBUTE_DATATYPE:
			return 75; //pg_attribute
		case PG_PROC_DATATYPE:
			return 81; //pg_proc
		case PG_CLASS_DATATYPE:
			return 83; //pg_class
		case SMGR_DATATYPE:
			return 210; //smgr
		case POINT_DATATYPE:
			return 600; //point
		case LSEG_DATATYPE:
			return 601; //lseg
		case PATH_DATATYPE:
			return 602; //path
		case BOX_DATATYPE:
			return 603; //box
		case POLYGON_DATATYPE:
			return 604; //polygon
		case LINE_DATATYPE:
			return 628; //line
		case _LINE_DATATYPE:
			return 629; //_line
		case _CIDR_DATATYPE:
			return 651; //_cidr
		case FLOAT4_DATATYPE:
			return 700; //float4
		case FLOAT8_DATATYPE:
			return 701; //float8
		case ABSTIME_DATATYPE:
			return 702; //abstime
		case RELTIME_DATATYPE:
			return 703; //reltime
		case TINTERVAL_DATATYPE:
			return 704; //tinterval
		case CIRCLE_DATATYPE:
			return 718; //circle
		case _CIRCLE_DATATYPE:
			return 719; //_circle
		case MONEY_DATATYPE:
			return 790; //money
		case _MONEY_DATATYPE:
			return 791; //_money
		case MACADDR_DATATYPE:
			return 829; //macaddr
		case INET_DATATYPE:
			return 869; //inet
		case CIDR_DATATYPE:
			return 650; //cidr
		case _BOOL_DATATYPE:
			return 1000; //_bool
		case _BYTEA_DATATYPE:
			return 1001; //_bytea
		case _CHAR_DATATYPE:
			return 1002; //_char
		case _NAME_DATATYPE:
			return 1003; //_name
		case _INT2_DATATYPE:
			return 1005; //_int2
		case _INT2VECTOR_DATATYPE:
			return 1006; //_int2vector
		case _INT4_DATATYPE:
			return 1007; //_int4
		case _REGPROC_DATATYPE:
			return 1008; //_regproc
		case _TEXT_DATATYPE:
			return 1009; //_text
		case _TID_DATATYPE:
			return 1010; //_tid
		case _XID_DATATYPE:
			return 1011; //_xid
		case _CID_DATATYPE:
			return 1012; //_cid
		case _OIDVECTOR_DATATYPE:
			return 1013; //_oidvector
		case _BPCHAR_DATATYPE:
			return 1014; //_bpchar
		case _VARCHAR_DATATYPE:
			return 1015; //_varchar
		case _INT8_DATATYPE:
			return 1016; //_int8
		case _POINT_DATATYPE:
			return 1017; //_point
		case _LSEG_DATATYPE:
			return 1018; //_lseg
		case _PATH_DATATYPE:
			return 1019; //_path
		case _BOX_DATATYPE:
			return 1020; //_box
		case _FLOAT4_DATATYPE:
			return 1021; //_float4
		case _FLOAT8_DATATYPE:
			return 1022; //_float8
		case _ABSTIME_DATATYPE:
			return 1023; //_abstime
		case _RELTIME_DATATYPE:
			return 1024; //_reltime
		case _TINTERVAL_DATATYPE:
			return 1025; //_tinterval
		case _POLYGON_DATATYPE:
			return 1027; //_polygon
		case _OID_DATATYPE:
			return 1028; //_oid
		case ACLITEM_DATATYPE:
			return 1033; //aclitem
		case _ACLITEM_DATATYPE:
			return 1034; //_aclitem
		case _MACADDR_DATATYPE:
			return 1040; //_macaddr
		case _INET_DATATYPE:
			return 1041; //_inet
		case BPCHAR_DATATYPE:
			return 1042; //bpchar
		case VARCHAR_DATATYPE:
			return 1043; //varchar
		case DATE_DATATYPE:
			return 1082; //date
		case TIME_DATATYPE:
			return 1083; //time
		case TIMESTAMP_DATATYPE:
			return 1114; //timestamp
		case _TIMESTAMP_DATATYPE:
			return 1115; //_timestamp
		case _DATE_DATATYPE:
			return 1182; //_date
		case _TIME_DATATYPE:
			return 1183; //_time
		case TIMESTAMPTZ_DATATYPE:
			return 1184; //timestamptz
		case _TIMESTAMPTZ_DATATYPE:
			return 1185; //_timestamptz
		case INTERVAL_DATATYPE:
			return 1186; //interval
		case _INTERVAL_DATATYPE:
			return 1187; //_interval
		case _NUMERIC_DATATYPE:
			return 1231; //_numeric
		case TIMETZ_DATATYPE:
			return 1266; //timetz
		case _TIMETZ_DATATYPE:
			return 1270; //_timetz
		case BIT_DATATYPE:
			return 1560; //bit
		case _BIT_DATATYPE:
			return 1561; //_bit
		case VARBIT_DATATYPE:
			return 1562; //varbit
		case _VARBIT_DATATYPE:
			return 1563; //_varbit
		case NUMERIC_DATATYPE:
			return 1700; //numeric
		case REFCURSOR_DATATYPE:
			return 1790; //refcursor
		case _REFCURSOR_DATATYPE:
			return 2201; //_refcursor
		case REGPROCEDURE_DATATYPE:
			return 2202; //regprocedure
		case REGOPER_DATATYPE:
			return 2203; //regoper
		case REGOPERATOR_DATATYPE:
			return 2204; //regoperator
		case REGCLASS_DATATYPE:
			return 2205; //regclass
		case REGTYPE_DATATYPE:
			return 2206; //regtype
		case _REGPROCEDURE_DATATYPE:
			return 2207; //_regprocedure
		case _REGOPER_DATATYPE:
			return 2208; //_regoper
		case _REGOPERATOR_DATATYPE:
			return 2209; //_regoperator
		case _REGCLASS_DATATYPE:
			return 2210; //_regclass
		case _REGTYPE_DATATYPE:
			return 2211; //_regtype
		case RECORD_DATATYPE:
			return 2249; //record
		case CSTRING_DATATYPE:
			return 2275; //cstring
		case ANY_DATATYPE:
			return 2276; //any
		case ANYARRAY_DATATYPE:
			return 2277; //anyarray
		case VOID_DATATYPE:
			return 2278; //void
		case TRIGGER_DATATYPE:
			return 2279; //trigger
		case LANGUAGE_HANDLER_DATATYPE:
			return 2280; //language_handler
		case INTERNAL_DATATYPE:
			return 2281; //internal
		case OPAQUE_DATATYPE:
			return 2282; //opaque
		case ANYELEMENT_DATATYPE:
			return 2283; //anyelement
		case UNKNOWN_DATATYPE:
		default:
			return 705; //unknown
	}
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

			// FIXME: currently, we only support text format, but
			// we should send binary if the client requested it
			// (in the resultformatcodes???)

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
	const char	*end=charstring::findFirstOrEnd(q,' ');
	if (*end && !charstring::compareIgnoringCase(end+1,"table",5)) {
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
	if (getDebug()) {
		debugStart("CommandComplete");
		stdoutput.printf("	commandtag: %s\n",
					commandtag.getString());
		debugEnd();
	}

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
	
	// get the requested cursor (or an available one)
	sqlrservercursor	*cursor=NULL;
	if (!stmtcursormap.getValue((char *)stmtname,&cursor)) {

		// get an available cursor
		cursor=cont->getCursor();
		if (!cursor) {
			return sendErrorResponse("Out of cursors");
		}

		// map stmt -> cursor
		stmtcursormap.setValue(charstring::duplicate(stmtname),cursor);
	}

	// set the execute flag true
	// (see execute() method for more info on this)
	executeflag.setValue(cursor,true);

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
	paramoids.remove(cursor);
	paramoids.setValue(cursor,paramtypes);

	// debug
	if (getDebug()) {
		debugStart("Parse");
		stdoutput.printf("	stmt name: %s\n",stmtname);
		stdoutput.printf("	cursor id: %d\n",cursor->getId());
		stdoutput.printf("	query length: %d\n",querylength);
		stdoutput.printf("	query: %.*s\n",querylength,query);
		stdoutput.printf("	param count: %d\n",paramcount);
		for (uint16_t i=0; i<paramcount; i++) {
			stdoutput.printf("	param %d type: %d\n",
							i,paramtypes[i]);
		}
		debugEnd();
	}

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
		return sendErrorResponse("ERROR","26000",
					"Invalid statement name");
	}

	// map portal -> cursor
	portalcursormap.setValue(
		charstring::duplicate(portal.getString()),cursor);

	// get and clear the bind pool
	memorypool		*bindpool=cont->getBindPool(cursor);
	bindpool->clear();

	// (re)set the execute flag
	// (see execute() method for more info on this)
	executeflag.setValue(cursor,true);

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
	uint32_t	*oids=NULL;
	if (paramformatcodecount) {
		paramformatcodes=new uint16_t[paramformatcodecount];
		for (uint16_t i=0; i<paramformatcodecount; i++) {
			readBE(rp,&(paramformatcodes[i]),&rp);
		}
		oids=paramoids.getValue(cursor);
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

		// get length/null-indicator
		uint32_t	paramlength;
		readBE(rp,&paramlength,&rp);

		if (getDebug()) {
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

		} else if (!paramformatcodecount || !paramformatcodes[i]) {
			if (getDebug()) {
				stdoutput.printf("		"
						"format: text\n");
			}
			bindTextParameter(rp,paramlength,bindpool,bv,&rp);
		} else {
			if (getDebug()) {
				stdoutput.printf("		"
						"format: binary\n");
			}
			if (!bindBinaryParameter(rp,oids[i],
						paramlength,bindpool,bv,&rp)) {
				return false;
			}
		}

		debugEnd(1);
	}

	delete[] paramformatcodes;

	// set the bind count
	cont->setInputBindCount(cursor,paramvaluecount);

	// result format codes...
	// FIXME: do something with these...
	uint16_t	resultformatcodecount;
	readBE(rp,&resultformatcodecount,&rp);
	uint16_t	*resultformatcodes=NULL;
	if (resultformatcodecount) {
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

void sqlrprotocol_postgresql::bindTextParameter(const unsigned char *rp,
						uint32_t paramlength,
						memorypool *bindpool,
						sqlrserverbindvar *bv,
						const unsigned char **rpout) {

	// bind string
	bv->type=SQLRSERVERBINDVARTYPE_STRING;
	bv->valuesize=paramlength;
	bv->value.stringval=(char *)bindpool->allocate(bv->valuesize+1);
	read(rp,bv->value.stringval,bv->valuesize,rpout);
	bv->value.stringval[bv->valuesize]='\0';
	bv->isnull=cont->nonNullBindValue();

	if (getDebug()) {
		stdoutput.printf("		"
				"value: %s\n",bv->value.stringval);
	}
}

bool sqlrprotocol_postgresql::bindBinaryParameter(const unsigned char *rp,
						uint32_t oid,
						uint32_t paramlength,
						memorypool *bindpool,
						sqlrserverbindvar *bv,
						const unsigned char **rpout) {

	if (getDebug()) {
		stdoutput.printf("		oid: %d\n",oid);
	}

	bv->valuesize=0;
	bv->isnull=cont->nonNullBindValue();

	switch (oid) {
		case 16: //bool
		case 1000: //_bool
			{
			unsigned char	value=0;
			bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
			read(rp,&value,rpout);
			bv->value.integerval=value;
			if (getDebug()) {
				stdoutput.printf("		"
						"value: %lld\n",
						bv->value.integerval);
			}
			}
			break;
		case 21: //int2
		case 1005: //_int2
			{
			int16_t	value=0;
			bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
			readBE(rp,(uint16_t *)&value,rpout);
			bv->value.integerval=value;
			if (getDebug()) {
				stdoutput.printf("		"
						"value: %lld\n",
						bv->value.integerval);
			}
			}
			break;
		case 23: //int4
		case 1007: //_int4
			{
			int32_t	value=0;
			bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
			readBE(rp,(uint32_t *)&value,rpout);
			bv->value.integerval=value;
			if (getDebug()) {
				stdoutput.printf("		"
						"value: %lld\n",
						bv->value.integerval);
			}
			}
			break;
		case 20: //int8
		case 1016: //_int8
			{
			int64_t	value=0;
			bv->type=SQLRSERVERBINDVARTYPE_INTEGER;
			readBE(rp,(uint64_t *)&value,rpout);
			bv->value.integerval=value;
			if (getDebug()) {
				stdoutput.printf("		"
						"value: %lld\n",
						bv->value.integerval);
			}
			}
			break;
		case 700: //float4
		case 1021: //_float4
			{
			bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
			uint32_t	val;
			readBE(rp,&val,rpout);
			float		value;
			bytestring::copy(&value,&val,sizeof(val));
			bv->value.doubleval.value=value;
			bv->value.doubleval.precision=0;
			bv->value.doubleval.scale=0;
			if (getDebug()) {
				stdoutput.printf("		"
						"value: %f\n",
						bv->value.doubleval.value);
			}
			}
			break;
		case 701: //float8
		case 1022: //_float8
			{
			bv->type=SQLRSERVERBINDVARTYPE_DOUBLE;
			uint64_t	val;
			readBE(rp,&val,rpout);
			bytestring::copy(&bv->value.doubleval.value,
							&val,sizeof(val));
			bv->value.doubleval.precision=0;
			bv->value.doubleval.scale=0;
			if (getDebug()) {
				stdoutput.printf("		"
						"value: %f\n",
						bv->value.doubleval.value);
			}
			}
			break;
		case 18: //char
		case 1002: //_char
		case 1014: //_bpchar
		case 1015: //_varchar
		case 1042: //bpchar
		case 1043: //varchar
			bindTextParameter(rp,paramlength,bindpool,bv,rpout);
			break;
		case 25: //text
		case 1009: //_text
			{
			bv->type=SQLRSERVERBINDVARTYPE_CLOB;
			bv->valuesize=paramlength;
			bv->value.stringval=
				(char *)bindpool->allocate(bv->valuesize+1);
			read(rp,bv->value.stringval,bv->valuesize,rpout);
			bv->value.stringval[bv->valuesize]='\0';
			bv->isnull=cont->nonNullBindValue();
			if (getDebug()) {
				stdoutput.printf("		"
						"value: %s\n",
						bv->value.stringval);
			}
			}
			break;
		case 17: //bytea
		case 1001: //_bytea
			{
			bv->type=SQLRSERVERBINDVARTYPE_BLOB;
			bv->valuesize=paramlength;
			bv->value.stringval=
				(char *)bindpool->allocate(bv->valuesize);
			read(rp,bv->value.stringval,bv->valuesize,rpout);
			bv->isnull=cont->nonNullBindValue();
			if (getDebug()) {
				stdoutput.printf("		value: ");
				stdoutput.safePrint(bv->value.stringval,
							bv->valuesize);
				stdoutput.printf("\n");
			}
			}
			break;
		case 1700: //numeric
		case 1231: //_numeric
			{
			uint16_t	ndigits;
			uint16_t	weight;
			uint16_t	sign;
			uint16_t	dscale;
			readBE(rp,&ndigits,&rp);
			readBE(rp,&weight,&rp);
			readBE(rp,&sign,&rp);
			readBE(rp,&dscale,&rp);
			stringbuffer	str;
			if (sign) {
				str.append('-');
			}
			for (uint16_t i=0; i<ndigits; i++) {
				uint16_t	digit;
				readBE(rp,&digit,&rp);
				if (!i) {
					str.append(digit);
				} else {
					str.writeFormatted("%04d",digit);
				}
			}
			
			bv->type=SQLRSERVERBINDVARTYPE_STRING;
			bv->valuesize=str.getSize();
			bv->value.stringval=(char *)bindpool->
						allocate(str.getSize()+1);
			charstring::copy(bv->value.stringval,
						str.getString(),
						str.getSize());
			bv->value.stringval[str.getSize()]='\0';
			bv->isnull=cont->nonNullBindValue();

			if (getDebug()) {
				stdoutput.printf("		"
						"ndigits: %hd\n",ndigits);
				stdoutput.printf("		"
						"weight: %hd\n",weight);
				stdoutput.printf("		"
						"sign: %hd\n",sign);
				stdoutput.printf("		"
						"dscale: %hd\n",dscale);
				stdoutput.printf("		"
						"value: %.*s\n",
						bv->valuesize,
						bv->value.stringval);
			}
			}
			break;
		case 1082: //date
		case 1182: //_date
			// FIXME: support this
			// 4 bytes, number of days since 4713BC
		case 1083: //time
		case 1183: //_time
			// FIXME: support this
			// 8 bytes, microseconds since midnight
		case 1266: //timetz
		case 1270: //_timetz
			// FIXME: support this
			// 8 bytes, microseconds since midnight (+tz?)
		case 1114: //timestamp
		case 1115: //_timestamp
			// FIXME: support this
			// 8 bytes, microseconds since 4713BC
		case 1184: //timestamptz
		case 1185: //_timestamptz
			// FIXME: support this
			// 8 bytes, microseconds since 4713BC (+tz?)


		// the rest of these are probably rare...
		case 19: //name
		case 22: //int2vector
		case 24: //regproc
		case 26: //oid
		case 27: //tid
		case 28: //xid
		case 29: //cid
		case 30: //oidvector
		case 71: //pg_type
		case 75: //pg_attribute
		case 81: //pg_proc
		case 83: //pg_class
		case 210: //smgr
		case 600: //point
		case 601: //lseg
		case 602: //path
		case 603: //box
		case 604: //polygon
		case 628: //line
		case 629: //_line
		case 651: //_cidr
		case 702: //abstime
		case 703: //reltime
		case 704: //tinterval
		case 718: //circle
		case 719: //_circle
		case 790: //money
		case 791: //_money
		case 829: //macaddr
		case 869: //inet
		case 650: //cidr
		case 1003: //_name
		case 1006: //_int2vector
		case 1008: //_regproc
		case 1010: //_tid
		case 1011: //_xid
		case 1012: //_cid
		case 1013: //_oidvector
		case 1017: //_point
		case 1018: //_lseg
		case 1019: //_path
		case 1020: //_box
		case 1023: //_abstime
		case 1024: //_reltime
		case 1025: //_tinterval
		case 1027: //_polygon
		case 1028: //_oid
		case 1033: //aclitem
		case 1034: //_aclitem
		case 1040: //_macaddr
		case 1041: //_inet
		case 1186: //interval
		case 1187: //_interval
		case 1560: //bit
		case 1561: //_bit
		case 1562: //varbit
		case 1563: //_varbit
		case 1790: //refcursor
		case 2201: //_refcursor
		case 2202: //regprocedure
		case 2203: //regoper
		case 2204: //regoperator
		case 2205: //regclass
		case 2206: //regtype
		case 2207: //_regprocedure
		case 2208: //_regoper
		case 2209: //_regoperator
		case 2210: //_regclass
		case 2211: //_regtype
		case 2249: //record
		case 2275: //cstring
		case 2276: //any
		case 2277: //anyarray
		case 2278: //void
		case 2279: //trigger
		case 2280: //language_handler
		case 2281: //internal
		case 2282: //opaque
		case 2283: //anyelement
		case 705: //unknown
		default:
			debugEnd(1);
			debugEnd();
			stringbuffer	err;
			err.append("parameter oid ");
			err.append(oid);
			err.append(" not supported");
			return sendErrorResponse(err.getString());
	}
	return true;
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
		return sendErrorResponse("ERROR","26000",
					"Invalid statement/portal name");
	}

	// debug
	if (getDebug()) {
		debugStart("Describe");
		stdoutput.printf("	S or P: %c\n",sorp);
		stdoutput.printf("	name: %s\n",name.getString());
		stdoutput.printf("	cursor id: %d\n",cursor->getId());
		debugEnd();
	}

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
		return sendErrorResponse("ERROR","26000","Invalid portal name");
	}

	// get the execute flag...
	// If the client passes a non-zero maxrows, and fetches all of them,
	// then it will re-issue an execute command to fetch more rows.  In
	// that case, we don't want to actually re-execute the query, just
	// send more rows.  So, to handle this, we set the execute flag to
	// true during prepare/bind phases, and false below.  If we get
	// multiple execute commands without prepare/bind in between, then
	// the flag will remain false, and we'll know to just fetch and return
	// rows, rather than actually re-execute the query.
	bool	exec=executeflag.getValue(cursor);

	if (getDebug()) {
		debugStart("Execute");
		stdoutput.printf("	portal name: %s\n",portal.getString());
		stdoutput.printf("	cursor id: %d\n",cursor->getId());
		stdoutput.printf("	max rows: %d\n",maxrows);
		if (exec) {
			stdoutput.printf("	(actually executing)\n");
		} else {
			stdoutput.printf("	(just fetching more rows)\n");
		}
		debugEnd();
	}

	// only execute the query if the flag is set
	if (exec) {

		// set the execute flag false
		executeflag.setValue(cursor,false);

		// handle an empty query
		if (emptyQuery(cont->getQueryBuffer(cursor))) {
			return sendEmptyQueryResponse();
		}

		// execute the query
		if (!cont->executeQuery(cursor,true,true,true,true)) {
			return sendCursorError(cursor);
		}
	}
	return sendQueryResult(cursor,false,maxrows);
}

bool sqlrprotocol_postgresql::emptyQuery(const char *query) {
	return !(cont->skipWhitespaceAndComments(query)[0]);
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
		return sendErrorResponse("ERROR","26000",
					"Invalid statement/portal name");
	}

	// debug
	if (getDebug()) {
		debugStart("Close");
		stdoutput.printf("	S or P: %c\n",sorp);
		stdoutput.printf("	name: %s\n",name.getString());
		stdoutput.printf("	cursor id: %d\n",cursor->getId());
		debugEnd();
	}

	// remove stmt/portal -> cursor mapping
	dict->remove((char *)name.getString());

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
