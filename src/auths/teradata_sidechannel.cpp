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
			sqlrauth_teradata_sidechannel(
						sqlrservercontroller *cont,
						sqlrauths *auths,
						sqlrpwdencs *sqlrpe,
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
					size_t length,
					byte_t **rpout);
		void	copyOut(byte_t *rp,
					byte_t *value,
					size_t length,
					byte_t **rpout);
		void	copyOut(byte_t *rp,
					char16_t *value,
					size_t length,
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

		void	debugHexDump(const byte_t *data, uint64_t size);

		const char	*host;
		uint16_t	port;

		bool	debug;

		filedescriptor	*clientsock;

		bytebuffer	sendheader;
		bytebuffer	senddata;

		memorypool	*clientrecvmessagepool;
		byte_t		*clientrecvheader;
		byte_t		*clientrecvdata;
		uint32_t	clientrecvdatalength;

		memorypool	*sidechannelrecvmessagepool;
		byte_t		*sidechannelrecvheader;
		byte_t		*sidechannelrecvdata;
		uint32_t	sidechannelrecvdatalength;

		byte_t		messagekind;
		uint32_t	sessionno;
		byte_t		requestauth[8];
		uint32_t	requestno;

		inetsocketclient	isc;
};

sqlrauth_teradata_sidechannel::sqlrauth_teradata_sidechannel(
					sqlrservercontroller *cont,
					sqlrauths *auths,
					sqlrpwdencs *sqlrpe,
					domnode *parameters) :
					sqlrauth(cont,auths,sqlrpe,parameters) {

	debug=cont->getConfig()->getDebugAuths();

	host=parameters->getAttributeValue("host");
	port=charstring::convertToInteger(parameters->getAttributeValue("port"));

	clientsock=NULL;

	clientrecvmessagepool=new memorypool(1024,1024,10240);
	clientrecvheader=NULL;
	clientrecvdata=NULL;
	clientrecvdatalength=0;

	sidechannelrecvmessagepool=new memorypool(1024,1024,10240);
	sidechannelrecvheader=NULL;
	sidechannelrecvdata=NULL;
	sidechannelrecvdatalength=0;

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
				if (debug) {
					stdoutput.write("copkind_cfg {\n");
					stdoutput.write("...\n");
					stdoutput.write("}\n");
				}
				if (!passthrough()) {
					loop=false;
				}
				break;
			case COPKIND_ASSIGN:
				if (debug) {
					stdoutput.write("copkind_assign {\n");
					stdoutput.write("...\n");
					stdoutput.write("}\n");
				}
				if (!passthrough()) {
					loop=false;
				}
				break;
			case COPKIND_SSOREQ:
				if (debug) {
					stdoutput.write("copkind_ssoreq {\n");
					stdoutput.write("...\n");
					stdoutput.write("}\n");
				}
				if (!passthrough()) {
					loop=false;
				}
				break;
			case COPKIND_CONNECT:
				if (debug) {
					stdoutput.write("copkind_connect {\n");
					stdoutput.write("...\n");
					stdoutput.write("}\n");
				}
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
		if (debug) {
			stdoutput.write("read header from client failed\n");
		}
		return false;
	}

	// lan header fields
	byte_t		version;
	byte_t		messageclass;
	uint16_t	highordermessagelength;
	byte_t		bytevar;
	uint16_t	wordvar;
	uint16_t	lowordermessagelength;
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
	copyOutBE(ptr,&highordermessagelength,&ptr);
	copyOut(ptr,&bytevar,&ptr);
	copyOutBE(ptr,&wordvar,&ptr);
	copyOutBE(ptr,&lowordermessagelength,&ptr);
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

	clientrecvdatalength=(((uint32_t)highordermessagelength)<<16)|
					((uint32_t)lowordermessagelength);

	if (debug) {
		stdoutput.write("client recv header {\n");
		stdoutput.printf("	version: %d\n",(int)version);
		stdoutput.printf("	class: %d\n",(int)messageclass);
		stdoutput.printf("	kind: %d\n",(int)messagekind);
		stdoutput.printf("	high order message length: %d\n",
						(int)highordermessagelength);
		stdoutput.printf("	bytevar: %d\n",(int)bytevar);
		stdoutput.printf("	wordvar: %d\n",(int)wordvar);
		stdoutput.printf("	low order message length: %d\n",
						(int)lowordermessagelength);
		stdoutput.write("	res for expan: ");
		stdoutput.safePrint((byte_t *)resforexpan,sizeof(resforexpan));
		stdoutput.write('\n');
		stdoutput.write("	correleation tag: ");
		stdoutput.safePrint((byte_t *)corrtag,sizeof(corrtag));
		stdoutput.write('\n');
		stdoutput.printf("	session no: %d\n",(int)sessionno);
		stdoutput.printf("	request auth: "
					"%03d.%03d.%03d.%03d."
					"%03d.%03d.%03d.%03d\n",
					requestauth[0],
					requestauth[1],
					requestauth[2],
					requestauth[3],
					requestauth[4],
					requestauth[5],
					requestauth[6],
					requestauth[7]);
		stdoutput.printf("	request no: %d\n",(int)requestno);
		stdoutput.printf("	gateway byte: %d\n",(int)gtwbyte);
		stdoutput.printf("	host charset: %d\n",(int)hostcharset);
		stdoutput.printf("	clientrecvdatalength: %d\n",
						(int)clientrecvdatalength);
		stdoutput.write('\n');
		debugHexDump(clientrecvheader,LAN_HEADER_SIZE);
		stdoutput.write("}\n");
	}


	// receive lan data
	clientrecvdata=clientrecvmessagepool->allocate(clientrecvdatalength);
	if (clientsock->read(clientrecvdata,clientrecvdatalength)!=
						(ssize_t)clientrecvdatalength) {
		if (debug) {
			stdoutput.write("read data from client failed\n");
		}
		return false;
	}

	if (debug) {
		stdoutput.write("client recv data {\n");
		debugHexDump(clientrecvdata,clientrecvdatalength);
		stdoutput.write("}\n");
	}

	return true;
}

bool sqlrauth_teradata_sidechannel::passthrough() {
	return forwardClientMessageToBackend() &&
		recvMessageFromBackend() &&
		forwardBackendMessageToClient();
}

bool sqlrauth_teradata_sidechannel::forwardClientMessageToBackend() {

	// pass whatever we received from the client through to the sidechannel
	if (debug) {
		stdoutput.write("sidechannel send header {\n");
		stdoutput.printf("	length: %d\n",LAN_HEADER_SIZE);
		debugHexDump(clientrecvheader,LAN_HEADER_SIZE);
		stdoutput.write("}\n");
		stdoutput.write("sidechannel send data {\n");
		stdoutput.printf("	length: %d\n",clientrecvdatalength);
		debugHexDump(clientrecvdata,clientrecvdatalength);
		stdoutput.write("}\n");
	}
	if (isc.write(clientrecvheader,LAN_HEADER_SIZE)!=LAN_HEADER_SIZE) {
		if (debug) {
			stdoutput.write("send client header "
					"to sidechannel failed\n");
		}
		return false;
	}
	if (isc.write(clientrecvdata,clientrecvdatalength)!=
					(ssize_t)clientrecvdatalength) {
		if (debug) {
			stdoutput.write("send client data "
					"to sidechannel failed\n");
		}
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
		if (debug) {
			stdoutput.write("read header failed\n");
		}
		isc.close();
		return false;
	}

	// copy out values from lan header
	uint16_t	lowordermessagelength;
	uint16_t	highordermessagelength;
	byte_t		*ptr=sidechannelrecvheader;
	// skip version, message class
	ptr=ptr+sizeof(byte_t)+
		sizeof(byte_t);
	messagekind=*ptr;
	// skip message kind
	ptr=ptr+sizeof(byte_t);
	// high order message length
	bytestring::copy(&highordermessagelength,ptr,sizeof(uint16_t));
	highordermessagelength=
		filedescriptor::convertNetToHost(highordermessagelength);
	// skip high order message length, bytevar, wordvar
	ptr=ptr+sizeof(uint16_t)+
		sizeof(byte_t)+
		sizeof(uint16_t);
	// low order message length
	bytestring::copy(&lowordermessagelength,ptr,sizeof(uint16_t));
	lowordermessagelength=
		filedescriptor::convertNetToHost(lowordermessagelength);

	// build the total length
	sidechannelrecvdatalength=(((uint32_t)highordermessagelength)<<16)|
				((uint32_t)lowordermessagelength);

	if (debug) {
		stdoutput.write("sidechannel recv header {\n");
		stdoutput.printf("	high order message length: %d\n",
						(int)highordermessagelength);
		stdoutput.printf("	low order message length: %d\n",
						(int)lowordermessagelength);
		stdoutput.printf("	data length: %d\n",
						(int)sidechannelrecvdatalength);
		stdoutput.write('\n');
		debugHexDump(sidechannelrecvheader,LAN_HEADER_SIZE);
		stdoutput.write("}\n");
	}

	// build one big packet...
	sidechannelrecvdata=
		sidechannelrecvmessagepool->allocate(sidechannelrecvdatalength);

	// receive lan data
	if (isc.read(sidechannelrecvdata,sidechannelrecvdatalength)!=
					(ssize_t)sidechannelrecvdatalength) {
		if (debug) {
			stdoutput.write("sidechannel recv data failed\n");
		}
		isc.close();
		return false;
	}

	if (debug) {
		stdoutput.write("sidechannel recv data {\n");
		debugHexDump(sidechannelrecvdata,sidechannelrecvdatalength);
		stdoutput.write("}\n");
	}

	return true;
}

bool sqlrauth_teradata_sidechannel::forwardBackendMessageToClient() {

	// send whatever we received from the sidechannel to the client
	if (debug) {
		stdoutput.write("client send header {\n");
		stdoutput.printf("	length: %d\n",LAN_HEADER_SIZE);
		debugHexDump(sidechannelrecvheader,LAN_HEADER_SIZE);
		stdoutput.write("}\n");
		stdoutput.write("client send data {\n");
		stdoutput.printf("	length: %d\n",sidechannelrecvdatalength);
		debugHexDump(sidechannelrecvdata,sidechannelrecvdatalength);
		stdoutput.write("}\n");
	}
	if (clientsock->write(sidechannelrecvheader,
				LAN_HEADER_SIZE)!=LAN_HEADER_SIZE) {
		if (debug) {
			stdoutput.write("clientsock write failed\n");
		}
		return false;
	}
	if (clientsock->write(sidechannelrecvdata,
				sidechannelrecvdatalength)!=
				(ssize_t)sidechannelrecvdatalength) {
		if (debug) {
			stdoutput.write("clientsock write failed\n");
		}
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
						size_t length,
						byte_t **rpout) {
	bytestring::copy(value,rp,length);
	*rpout=rp+length;
}

void sqlrauth_teradata_sidechannel::copyOut(byte_t *rp,
						byte_t *value,
						size_t length,
						byte_t **rpout) {
	bytestring::copy(value,rp,length);
	*rpout=rp+length;
}

void sqlrauth_teradata_sidechannel::copyOut(byte_t *rp,
						char16_t *value,
						size_t length,
						byte_t **rpout) {
	bytestring::copy(value,rp,length*sizeof(char16_t));
	*rpout=rp+length*sizeof(char16_t);
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

void sqlrauth_teradata_sidechannel::debugHexDump(const byte_t *data,
							uint64_t size) {
	if (!size) {
		return;
	}
	stdoutput.write("	");
	for (uint64_t i=0; i<size; i++) {
		stdoutput.printf("%02x  ",data[i]);
		if (!((i+1)%8)) {
			stdoutput.write("   ");
		}
		if (!((i+1)%16)) {
			stdoutput.write("\n	");
		}
	}
	stdoutput.write("\n	");
	for (uint64_t i=0; i<size; i++) {
		if (data[i]>=' ' && data[i]<='~') {
			stdoutput.printf("%c   ",data[i]);
		} else {
			stdoutput.write(".   ");
		}
		if (!((i+1)%8)) {
			stdoutput.write("   ");
		}
		if (!((i+1)%16)) {
			stdoutput.write("\n	");
		}
	}
	stdoutput.write('\n');
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_teradata_sidechannel(
						sqlrservercontroller *cont,
						sqlrauths *auths,
						sqlrpwdencs *sqlrpe,
						domnode *parameters) {
		return new sqlrauth_teradata_sidechannel(cont,auths,
							sqlrpe,parameters);
	}
}
