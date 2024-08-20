// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/memorypool.h>
#include <rudiments/charstring.h>
#include <rudiments/bytebuffer.h>
#include <rudiments/inetsocketclient.h>

#define LAN_HEADER_SIZE	52

// kinds of messages
#define COPKIND_ASSIGN		1
#define COPKIND_REASSIGN	2
#define COPKIND_CONNECT		3
#define COPKIND_RECONNECT	4
#define COPKIND_START		5
#define COPKIND_CONTINUE	6
#define COPKIND_ABORT		7
#define COPKIND_LOGOFF		8
#define COPKIND_TEST		9
#define COPKIND_CFG		10
#define COPKIND_AUTHMETHODS	11
#define COPKIND_SSOREQ		12
#define COPKIND_ELICITDATA	13
#define COPKIND_DEFAULTCONNECT	254
#define COPKIND_DIRECT		255

class SQLRSERVER_DLLSPEC sqlrauth_teradata_sidechannel : public sqlrauth {
	public:
		sqlrauth_teradata_sidechannel(sqlrservercontroller *cont,
							domnode *parameters);
		~sqlrauth_teradata_sidechannel();
		const char	*auth(sqlrcredentials *cred);
	private:
		bool	recvMessageFromClient();

		bool	passthrough();
		bool	forwardClientMessageToBackend();
		bool	recvMessageFromBackend();
		bool	forwardBackendMessageToClient();

		void	copyOut(byte_t *rp,
					byte_t *value,
					byte_t **rpout);
		void	copyOut(byte_t *rp,
					char *value,
					size_t size,
					byte_t **rpout);
		void	copyOut(byte_t *rp,
					byte_t *value,
					size_t size,
					byte_t **rpout);
		void	copyOut(byte_t *rp,
					char16_t *value,
					size_t size,
					byte_t **rpout);
		void	copyOut(byte_t *rp,
					float *value,
					byte_t **rpout);
		void	copyOut(byte_t *rp,
					double *value,
					byte_t **rpout);
		void	copyOutLE(byte_t *rp,
					uint16_t *value,
					byte_t **rpout);
		void	copyOutBE(byte_t *rp,
					uint16_t *value,
					byte_t **rpout);
		void	copyOutLE(byte_t *rp,
					uint32_t *value,
					byte_t **rpout);
		void	copyOutBE(byte_t *rp,
					uint32_t *value,
					byte_t **rpout);
		void	copyOutLE(byte_t *rp,
					uint64_t *value,
					byte_t **rpout);
		void	copyOutBE(byte_t *rp,
					uint64_t *value,
					byte_t **rpout);

		const char	*host;
		uint16_t	port;

		filedescriptor	*clientsock;

		bytebuffer	sendheader;
		bytebuffer	senddata;

		memorypool	*clientrecvmessagepool;
		byte_t		*clientrecvheader;
		byte_t		*clientrecvdata;
		uint32_t	clientrecvdatasize;

		memorypool	*sidechannelrecvmessagepool;
		byte_t		*sidechannelrecvheader;
		byte_t		*sidechannelrecvdata;
		uint32_t	sidechannelrecvdatasize;

		byte_t		messagekind;
		uint32_t	sessionno;
		byte_t		requestauth[8];
		uint32_t	requestno;

		inetsocketclient	isc;
};

sqlrauth_teradata_sidechannel::sqlrauth_teradata_sidechannel(
					sqlrservercontroller *cont,
					domnode *parameters) :
					sqlrauth(cont,parameters) {

	host=parameters->getAttributeValue("host");
	port=charstring::convertToInteger(parameters->getAttributeValue("port"));

	clientsock=NULL;

	clientrecvmessagepool=new memorypool(1024,1024,10240);
	clientrecvheader=NULL;
	clientrecvdata=NULL;
	clientrecvdatasize=0;

	sidechannelrecvmessagepool=new memorypool(1024,1024,10240);
	sidechannelrecvheader=NULL;
	sidechannelrecvdata=NULL;
	sidechannelrecvdatasize=0;

	messagekind=0;
	sessionno=0;
	requestno=0;
}

sqlrauth_teradata_sidechannel::~sqlrauth_teradata_sidechannel() {
	delete clientrecvmessagepool;
	delete sidechannelrecvmessagepool;
}

const char *sqlrauth_teradata_sidechannel::auth(sqlrcredentials *cred) {

	clientsock=((sqlrteradatacredentials *)cred)->getClientFileDescriptor();

	isc.close();
	isc.setHost(host);
	isc.setPort(port);
	// FIXME: buffering
	if (!isc.connect()) {
		// FIXME: display/report error
		return NULL;
	}

	const char	*retval=NULL;
	bool		loop=true;
	while (loop) {

		if (!recvMessageFromClient()) {
			break;
		}

		switch (messagekind) {
			case COPKIND_CFG:
				debugStart("copkind_cfg");
				debugWrite("...");
				debugEnd();
				if (!passthrough()) {
					loop=false;
				}
				break;
			case COPKIND_ASSIGN:
				debugStart("copkind_assign");
				debugWrite("...");
				debugEnd();
				if (!passthrough()) {
					loop=false;
				}
				break;
			case COPKIND_SSOREQ:
				debugStart("copkind_ssoreq");
				debugWrite("...");
				debugEnd();
				if (!passthrough()) {
					loop=false;
				}
				break;
			case COPKIND_CONNECT:
				debugStart("copkind_connect");
				debugWrite("...");
				debugEnd();
				if (passthrough()) {
					retval="";
				}
				loop=false;
				break;
			default:
				loop=false;
				break;
		}
	}

	// NOTE: Don't isc.close() here.
	//
	// Fastload connects multiple times and ties the sessions together.
	// If we close this session after auth, then when fastload connects
	// again, it won't be able to find this session.
	//
	// Instead, we'll just leave each session open until the next auth
	// attempt.

	return retval;
}

bool sqlrauth_teradata_sidechannel::recvMessageFromClient() {

	clientrecvmessagepool->clear();

	// receive lan header
	clientrecvheader=clientrecvmessagepool->allocate(LAN_HEADER_SIZE);
	if (clientsock->read(clientrecvheader,LAN_HEADER_SIZE)!=
							LAN_HEADER_SIZE) {
		debugWrite("read header from client failed");
		return false;
	}

	// lan header fields
	byte_t		version;
	byte_t		messageclass;
	uint16_t	highordermessagesize;
	byte_t		bytevar;
	uint16_t	wordvar;
	uint16_t	lowordermessagesize;
	uint16_t 	resforexpan[3];
	uint16_t	corrtag[2];
	byte_t		gtwbyte;
	byte_t		hostcharset;
	byte_t		spare[14];

	// copy out values from lan header
	byte_t	*ptr=clientrecvheader;
	copyOut(ptr,&version,&ptr);
	copyOut(ptr,&messageclass,&ptr);
	copyOut(ptr,&messagekind,&ptr);
	copyOutBE(ptr,&highordermessagesize,&ptr);
	copyOut(ptr,&bytevar,&ptr);
	copyOutBE(ptr,&wordvar,&ptr);
	copyOutBE(ptr,&lowordermessagesize,&ptr);
	// FIXME: net-to-host these?
	copyOut(ptr,(byte_t *)resforexpan,sizeof(resforexpan),&ptr);
	// FIXME: net-to-host these?
	copyOut(ptr,(byte_t *)corrtag,sizeof(corrtag),&ptr);
	copyOutBE(ptr,&sessionno,&ptr);
	copyOut(ptr,(byte_t *)requestauth,sizeof(requestauth),&ptr);
	copyOutBE(ptr,&requestno,&ptr);
	copyOut(ptr,&gtwbyte,&ptr);
	copyOut(ptr,&hostcharset,&ptr);
	copyOut(ptr,(byte_t *)spare,sizeof(spare),&ptr);

	clientrecvdatasize=(((uint32_t)highordermessagesize)<<16)|
					((uint32_t)lowordermessagesize);

	if (getDebug()) {
		debugStart("client recv header");
		debugWrite("version: %d",(int)version);
		debugWrite("class: %d",(int)messageclass);
		debugWrite("kind: %d",(int)messagekind);
		debugWrite("high order message size: %d",
						(int)highordermessagesize);
		debugWrite("bytevar: %d",(int)bytevar);
		debugWrite("wordvar: %d",(int)wordvar);
		debugWrite("low order message size: %d",
						(int)lowordermessagesize);
		stringbuffer	b;
		b.append("res for expan: ");
		b.safePrint((byte_t *)resforexpan,sizeof(resforexpan));
		debugWrite(b.getString());
		b.clear();
		b.write("correleation tag: ");
		b.safePrint((byte_t *)corrtag,sizeof(corrtag));
		debugWrite(b.getString());
		debugWrite("session no: %d",(int)sessionno);
		debugWrite("request auth: "
					"%03d.%03d.%03d.%03d."
					"%03d.%03d.%03d.%03d",
					requestauth[0],
					requestauth[1],
					requestauth[2],
					requestauth[3],
					requestauth[4],
					requestauth[5],
					requestauth[6],
					requestauth[7]);
		debugWrite("request no: %d",(int)requestno);
		debugWrite("gateway byte: %d",(int)gtwbyte);
		debugWrite("host charset: %d",(int)hostcharset);
		debugWrite("clientrecvdatasize: %d",
						(int)clientrecvdatasize);
		debugHexDump(clientrecvheader,LAN_HEADER_SIZE);
		debugEnd();
	}


	// receive lan data
	clientrecvdata=clientrecvmessagepool->allocate(clientrecvdatasize);
	if (clientsock->read(clientrecvdata,clientrecvdatasize)!=
						(ssize_t)clientrecvdatasize) {
		debugWrite("read data from client failed");
		return false;
	}

	debugStart("client recv data");
	debugHexDump(clientrecvdata,clientrecvdatasize);
	debugEnd();

	return true;
}

bool sqlrauth_teradata_sidechannel::passthrough() {
	return forwardClientMessageToBackend() &&
		recvMessageFromBackend() &&
		forwardBackendMessageToClient();
}

bool sqlrauth_teradata_sidechannel::forwardClientMessageToBackend() {

	// pass whatever we received from the client through to the sidechannel

	debugStart("sidechannel send header");
	debugWrite("size: %d",LAN_HEADER_SIZE);
	debugHexDump(clientrecvheader,LAN_HEADER_SIZE);
	debugEnd();

	debugStart("sidechannel send data");
	debugWrite("size: %d",clientrecvdatasize);
	debugHexDump(clientrecvdata,clientrecvdatasize);
	debugEnd();

	if (isc.write(clientrecvheader,LAN_HEADER_SIZE)!=LAN_HEADER_SIZE) {
		debugWrite("send client header to sidechannel failed");
		return false;
	}
	if (isc.write(clientrecvdata,clientrecvdatasize)!=
					(ssize_t)clientrecvdatasize) {
		debugWrite("send client data to sidechannel failed");
		return false;
	}
	return true;
}

bool sqlrauth_teradata_sidechannel::recvMessageFromBackend() {

	sidechannelrecvmessagepool->clear();

	// receive lan header
	sidechannelrecvheader=
		sidechannelrecvmessagepool->allocate(LAN_HEADER_SIZE);
	if (isc.read(sidechannelrecvheader,LAN_HEADER_SIZE)!=LAN_HEADER_SIZE) {
		debugWrite("read header failed");
		isc.close();
		return false;
	}

	// copy out values from lan header
	uint16_t	lowordermessagesize;
	uint16_t	highordermessagesize;
	byte_t		*ptr=sidechannelrecvheader;
	// skip version, message class
	ptr=ptr+sizeof(byte_t)+
		sizeof(byte_t);
	messagekind=*ptr;
	// skip message kind
	ptr=ptr+sizeof(byte_t);
	// high order message size
	bytestring::copy(&highordermessagesize,ptr,sizeof(uint16_t));
	highordermessagesize=
		filedescriptor::convertNetToHost(highordermessagesize);
	// skip high order message size, bytevar, wordvar
	ptr=ptr+sizeof(uint16_t)+
		sizeof(byte_t)+
		sizeof(uint16_t);
	// low order message size
	bytestring::copy(&lowordermessagesize,ptr,sizeof(uint16_t));
	lowordermessagesize=
		filedescriptor::convertNetToHost(lowordermessagesize);

	// build the total size
	sidechannelrecvdatasize=(((uint32_t)highordermessagesize)<<16)|
				((uint32_t)lowordermessagesize);

	debugStart("sidechannel recv header");
	debugWrite("high order message size: %d",(int)highordermessagesize);
	debugWrite("low order message size: %d",(int)lowordermessagesize);
	debugWrite("data size: %d",(int)sidechannelrecvdatasize);
	debugHexDump(sidechannelrecvheader,LAN_HEADER_SIZE);
	debugEnd();

	// build one big packet...
	sidechannelrecvdata=
		sidechannelrecvmessagepool->allocate(sidechannelrecvdatasize);

	// receive lan data
	if (isc.read(sidechannelrecvdata,sidechannelrecvdatasize)!=
					(ssize_t)sidechannelrecvdatasize) {
		debugWrite("sidechannel recv data failed");
		isc.close();
		return false;
	}

	debugStart("sidechannel recv data");
	debugHexDump(sidechannelrecvdata,sidechannelrecvdatasize);
	debugEnd();

	return true;
}

bool sqlrauth_teradata_sidechannel::forwardBackendMessageToClient() {

	// send whatever we received from the sidechannel to the client

	debugStart("client send header");
	debugWrite("size: %d",LAN_HEADER_SIZE);
	debugHexDump(sidechannelrecvheader,LAN_HEADER_SIZE);
	debugEnd();

	debugStart("client send data");
	debugWrite("size: %d",sidechannelrecvdatasize);
	debugHexDump(sidechannelrecvdata,sidechannelrecvdatasize);
	debugEnd();

	if (clientsock->write(sidechannelrecvheader,
				LAN_HEADER_SIZE)!=LAN_HEADER_SIZE) {
		debugWrite("clientsock write failed");
		return false;
	}
	if (clientsock->write(sidechannelrecvdata,
				sidechannelrecvdatasize)!=
				(ssize_t)sidechannelrecvdatasize) {
		debugWrite("clientsock write failed");
		return false;
	}
	clientsock->flushWriteBuffer(-1,-1);
	return true;
}

void sqlrauth_teradata_sidechannel::copyOut(byte_t *rp,
						byte_t *value,
						byte_t **rpout) {
	*value=*rp;
	*rpout=rp+sizeof(byte_t);
}

void sqlrauth_teradata_sidechannel::copyOut(byte_t *rp,
						char *value,
						size_t size,
						byte_t **rpout) {
	bytestring::copy(value,rp,size);
	*rpout=rp+size;
}

void sqlrauth_teradata_sidechannel::copyOut(byte_t *rp,
						byte_t *value,
						size_t size,
						byte_t **rpout) {
	bytestring::copy(value,rp,size);
	*rpout=rp+size;
}

void sqlrauth_teradata_sidechannel::copyOut(byte_t *rp,
						char16_t *value,
						size_t size,
						byte_t **rpout) {
	bytestring::copy(value,rp,size*sizeof(char16_t));
	*rpout=rp+size*sizeof(char16_t);
}

void sqlrauth_teradata_sidechannel::copyOut(byte_t *rp,
						float *value,
						byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(float));
	*rpout=rp+sizeof(float);
}

void sqlrauth_teradata_sidechannel::copyOut(byte_t *rp,
						double *value,
						byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(double));
	*rpout=rp+sizeof(double);
}


void sqlrauth_teradata_sidechannel::copyOutLE(byte_t *rp,
						uint16_t *value,
						byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint16_t));
	*value=filedescriptor::convertLittleEndianToHost(*value);
	*rpout=rp+sizeof(uint16_t);
}

void sqlrauth_teradata_sidechannel::copyOutBE(byte_t *rp,
						uint16_t *value,
						byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint16_t));
	*value=filedescriptor::convertNetToHost(*value);
	*rpout=rp+sizeof(uint16_t);
}

void sqlrauth_teradata_sidechannel::copyOutLE(byte_t *rp,
						uint32_t *value,
						byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint32_t));
	*value=filedescriptor::convertLittleEndianToHost(*value);
	*rpout=rp+sizeof(uint32_t);
}

void sqlrauth_teradata_sidechannel::copyOutBE(byte_t *rp,
						uint32_t *value,
						byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint32_t));
	*value=filedescriptor::convertNetToHost(*value);
	*rpout=rp+sizeof(uint32_t);
}

void sqlrauth_teradata_sidechannel::copyOutLE(byte_t *rp,
						uint64_t *value,
						byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint64_t));
	*value=filedescriptor::convertLittleEndianToHost(*value);
	*rpout=rp+sizeof(uint64_t);
}

void sqlrauth_teradata_sidechannel::copyOutBE(byte_t *rp,
						uint64_t *value,
						byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint64_t));
	*value=filedescriptor::convertNetToHost(*value);
	*rpout=rp+sizeof(uint64_t);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_teradata_sidechannel(
						sqlrservercontroller *cont,
						domnode *parameters) {
		return new sqlrauth_teradata_sidechannel(cont,parameters);
	}
}
