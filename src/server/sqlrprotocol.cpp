// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/bytestring.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/process.h>

class sqlrprotocolprivate {
	friend class sqlrprotocol;
	private:
		sqlrprotocols		*_ps;
		domnode			*_parameters;
		bool			_bigendian;
		bool			_debug;
};

sqlrprotocol::sqlrprotocol(sqlrservercontroller *cont,
				sqlrprotocols *ps,
				domnode *parameters) {
	pvt=new sqlrprotocolprivate;
	this->cont=cont;
	pvt->_ps=ps;
	pvt->_parameters=parameters;
	pvt->_bigendian=false;
	pvt->_debug=cont->getConfig()->getDebugProtocols();
}

sqlrprotocol::~sqlrprotocol() {
	delete pvt;
}

gsscontext *sqlrprotocol::getGSSContext() {
	return NULL;
}

tlscontext *sqlrprotocol::getTLSContext() {
	return NULL;
}

sqlrprotocols *sqlrprotocol::getProtocols() {
	return pvt->_ps;
}

domnode *sqlrprotocol::getParameters() {
	return pvt->_parameters;
}

void sqlrprotocol::setProtocolIsBigEndian(bool bigendian) {
	bigendian=pvt->_bigendian;
}

bool sqlrprotocol::getProtocolIsBigEndian() {
	return pvt->_bigendian;
}

void sqlrprotocol::copyOut(const unsigned char *rp,
					char *value,
					const unsigned char **rpout) {
	*value=(char)*rp;
	*rpout=rp+sizeof(char);
}

bool sqlrprotocol::copyOut(const unsigned char *rp,
					char *value,
					const char *name,
					char expected,
					const unsigned char **rpout) {
	copyOut(rp,value,rpout);
	if (*value!=expected) {
		if (pvt->_debug) {
			stdoutput.printf("bad %s 0x%02x, expected 0x%02x\n",
							name,*value,expected);
		}
		*rpout=rp;
		return false;
	}
	return true;
}

void sqlrprotocol::copyOut(const unsigned char *rp,
					unsigned char *value,
					const unsigned char **rpout) {
	*value=*rp;
	*rpout=rp+sizeof(unsigned char);
}

bool sqlrprotocol::copyOut(const unsigned char *rp,
					unsigned char *value,
					const char *name,
					unsigned char expected,
					const unsigned char **rpout) {
	copyOut(rp,value,rpout);
	if (*value!=expected) {
		if (pvt->_debug) {
			stdoutput.printf("bad %s 0x%02x, expected 0x%02x\n",
							name,*value,expected);
		}
		*rpout=rp;
		return false;
	}
	return true;
}

void sqlrprotocol::copyOut(const unsigned char *rp,
					char *value,
					size_t length,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,length);
	*rpout=rp+length;
}

void sqlrprotocol::copyOut(const unsigned char *rp,
					unsigned char *value,
					size_t length,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,length);
	*rpout=rp+length;
}

void sqlrprotocol::copyOut(const unsigned char *rp,
					char16_t *value,
					size_t length,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,length*sizeof(char16_t));
	*rpout=rp+length*sizeof(char16_t);
}

void sqlrprotocol::copyOut(const unsigned char *rp,
					float *value,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,sizeof(float));
	*rpout=rp+sizeof(float);
}

void sqlrprotocol::copyOut(const unsigned char *rp,
					double *value,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,sizeof(double));
	*rpout=rp+sizeof(double);
}

void sqlrprotocol::copyOut(const unsigned char *rp,
					uint16_t *value,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,sizeof(uint16_t));
	*value=toHost(*value);
	*rpout=rp+sizeof(uint16_t);
}


void sqlrprotocol::copyOutLE(const unsigned char *rp,
					uint16_t *value,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,sizeof(uint16_t));
	*value=leToHost(*value);
	*rpout=rp+sizeof(uint16_t);
}

bool sqlrprotocol::copyOutLE(const unsigned char *rp,
					uint16_t *value,
					const char *name,
					uint16_t expected,
					const unsigned char **rpout) {
	copyOutLE(rp,value,rpout);
	if (*value!=expected) {
		if (getDebug()) {
			stdoutput.printf("bad %s 0x%04x, expected 0x%04x\n",
							name,*value,expected);
		}
		*rpout=rp;
		return false;
	}
	return true;
}

void sqlrprotocol::copyOutBE(const unsigned char *rp,
					uint16_t *value,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,sizeof(uint16_t));
	*value=beToHost(*value);
	*rpout=rp+sizeof(uint16_t);
}

bool sqlrprotocol::copyOutBE(const unsigned char *rp,
					uint16_t *value,
					const char *name,
					uint16_t expected,
					const unsigned char **rpout) {
	copyOutBE(rp,value,rpout);
	if (*value!=expected) {
		if (pvt->_debug) {
			stdoutput.printf("bad %s 0x%04x, expected 0x%04x\n",
							name,*value,expected);
		}
		*rpout=rp;
		return false;
	}
	return true;
}

void sqlrprotocol::copyOut(const unsigned char *rp,
					uint32_t *value,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,sizeof(uint32_t));
	*value=toHost(*value);
	*rpout=rp+sizeof(uint32_t);
}

void sqlrprotocol::copyOutLE(const unsigned char *rp,
					uint32_t *value,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,sizeof(uint32_t));
	*value=leToHost(*value);
	*rpout=rp+sizeof(uint32_t);
}

bool sqlrprotocol::copyOutLE(const unsigned char *rp,
					uint32_t *value,
					const char *name,
					uint32_t expected,
					const unsigned char **rpout) {
	copyOutLE(rp,value,rpout);
	if (*value!=expected) {
		if (getDebug()) {
			stdoutput.printf("bad %s 0x%08x, expected 0x%08x\n",
							name,*value,expected);
		}
		*rpout=rp;
		return false;
	}
	return true;
}

void sqlrprotocol::copyOutBE(const unsigned char *rp,
					uint32_t *value,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,sizeof(uint32_t));
	*value=beToHost(*value);
	*rpout=rp+sizeof(uint32_t);
}

bool sqlrprotocol::copyOutBE(const unsigned char *rp,
					uint32_t *value,
					const char *name,
					uint32_t expected,
					const unsigned char **rpout) {
	copyOutBE(rp,value,rpout);
	if (*value!=expected) {
		if (pvt->_debug) {
			stdoutput.printf("bad %s 0x%08x, expected 0x%08x\n",
							name,*value,expected);
		}
		*rpout=rp;
		return false;
	}
	return true;
}

void sqlrprotocol::copyOut(const unsigned char *rp,
					uint64_t *value,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,sizeof(uint64_t));
	*value=toHost(*value);
	*rpout=rp+sizeof(uint64_t);
}

void sqlrprotocol::copyOutLE(const unsigned char *rp,
					uint64_t *value,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,sizeof(uint64_t));
	*value=leToHost(*value);
	*rpout=rp+sizeof(uint64_t);
}

bool sqlrprotocol::copyOutLE(const unsigned char *rp,
					uint64_t *value,
					const char *name,
					uint64_t expected,
					const unsigned char **rpout) {
	copyOutLE(rp,value,rpout);
	if (*value!=expected) {
		if (getDebug()) {
			stdoutput.printf("bad %s 0x%016x, expected 0x%016x\n",
							name,*value,expected);
		}
		*rpout=rp;
		return false;
	}
	return true;
}

void sqlrprotocol::copyOutBE(const unsigned char *rp,
					uint64_t *value,
					const unsigned char **rpout) {
	bytestring::copy(value,rp,sizeof(uint64_t));
	*value=beToHost(*value);
	*rpout=rp+sizeof(uint64_t);
}

bool sqlrprotocol::copyOutBE(const unsigned char *rp,
					uint64_t *value,
					const char *name,
					uint64_t expected,
					const unsigned char **rpout) {
	copyOutBE(rp,value,rpout);
	if (*value!=expected) {
		if (pvt->_debug) {
			stdoutput.printf("bad %s 0x%016x, expected 0x%016x\n",
							name,*value,expected);
		}
		*rpout=rp;
		return false;
	}
	return true;
}

uint64_t sqlrprotocol::copyOutLenEncInt(const unsigned char *rp,
					const unsigned char **rpout) {
	uint64_t	retval=0;
	switch (*rp) {
		case 0xfe:
			{
			uint64_t	val;
			bytestring::copy(&val,rp+1,sizeof(uint64_t));
			retval=val;
			*rpout=rp+9;
			}
		case 0xfd:
			{
			uint32_t	val=0;
			unsigned char	*valbytes=(unsigned char *)&val;
			rp++;
			valbytes[3]=*rp;
			rp++;
			valbytes[2]=*rp;
			rp++;
			valbytes[1]=*rp;
			rp++;
			valbytes[0]=0;
			val=beToHost(val);
			retval=(uint64_t)val;
			*rpout=rp;
			}
		case 0xfc:
			{
			uint16_t	val;
			bytestring::copy(&val,rp+1,sizeof(uint16_t));
			retval=val;
			*rpout=rp+3;
			}
		default:
			// *rp should be <= 0xfb at this point
			retval=*rp;
			*rpout=rp+1;
	}
	return retval;
}

void sqlrprotocol::copyIn(bytebuffer *buffer, char value) {
	buffer->append(value);
}

void sqlrprotocol::copyIn(bytebuffer *buffer, unsigned char value) {
	buffer->append(value);
}

void sqlrprotocol::copyIn(bytebuffer *buffer, const char *value) {
	copyIn(buffer,value,charstring::length(value));
}

void sqlrprotocol::copyIn(bytebuffer *buffer, const char *value,
							size_t length) {
	buffer->append(value,length);
}

void sqlrprotocol::copyIn(bytebuffer *buffer, const unsigned char *value,
							size_t length) {
	buffer->append(value,length);
}

void sqlrprotocol::copyIn(bytebuffer *buffer, float value) {
	buffer->append(value);
}

void sqlrprotocol::copyIn(bytebuffer *buffer, double value) {
	buffer->append(value);
}

void sqlrprotocol::copyIn(bytebuffer *buffer, uint16_t value) {
	buffer->append(hostTo(value));
}

void sqlrprotocol::copyInLE(bytebuffer *buffer, uint16_t value) {
	buffer->append(hostToLE(value));
}

void sqlrprotocol::copyInBE(bytebuffer *buffer, uint16_t value) {
	buffer->append(hostToBE(value));
}

void sqlrprotocol::copyIn(bytebuffer *buffer, uint32_t value) {
	buffer->append(hostTo(value));
}

void sqlrprotocol::copyInLE(bytebuffer *buffer, uint32_t value) {
	buffer->append(hostToLE(value));
}

void sqlrprotocol::copyInBE(bytebuffer *buffer, uint32_t value) {
	buffer->append(hostToBE(value));
}

void sqlrprotocol::copyIn(bytebuffer *buffer, uint64_t value) {
	buffer->append(hostTo(value));
}

void sqlrprotocol::copyInLE(bytebuffer *buffer, uint64_t value) {
	buffer->append(hostToLE(value));
}

void sqlrprotocol::copyInBE(bytebuffer *buffer, uint64_t value) {
	buffer->append(hostToBE(value));
}

void sqlrprotocol::copyInLenEncInt(bytebuffer *buffer, uint64_t value) {
	if (value>=16777216) {
		buffer->append((char)0xfe);
		buffer->append((uint64_t)value);
	} else if (value>=65536) {
		buffer->append((char)0xfd);
		copyInTriplet(buffer,(uint32_t)value);
	} else if (value>=251) {
		buffer->append((char)0xfc);
		buffer->append((uint16_t)value);
	} else {
		buffer->append((char)value);
	}
}

void sqlrprotocol::copyInTriplet(bytebuffer *buffer, uint32_t value) {
	value=hostToBE(value);
	unsigned char	*valuebytes=(unsigned char *)&value;
	buffer->append(valuebytes[3]);
	buffer->append(valuebytes[2]);
	buffer->append(valuebytes[1]);
}

void sqlrprotocol::copyInLenEncStr(bytebuffer *buffer,
						const char *string) {
	copyInLenEncInt(buffer,charstring::length(string));
	buffer->append(string);
}

void sqlrprotocol::copyInLenEncStr(bytebuffer *buffer,
						const char *string,
						uint64_t length) {
	copyInLenEncInt(buffer,length);
	buffer->append(string,length);
}

uint16_t sqlrprotocol::toHost(uint16_t value) {
	return (getProtocolIsBigEndian())?beToHost(value):leToHost(value);
}

uint32_t sqlrprotocol::toHost(uint32_t value) {
	return (getProtocolIsBigEndian())?beToHost(value):leToHost(value);
}

uint64_t sqlrprotocol::toHost(uint64_t value) {
	return (getProtocolIsBigEndian())?beToHost(value):leToHost(value);
}

uint16_t sqlrprotocol::leToHost(uint16_t value) {
	return filedescriptor::littleEndianToHost(value);
}

uint32_t sqlrprotocol::leToHost(uint32_t value) {
	return filedescriptor::littleEndianToHost(value);
}

uint64_t sqlrprotocol::leToHost(uint64_t value) {
	return filedescriptor::littleEndianToHost(value);
}

uint16_t sqlrprotocol::beToHost(uint16_t value) {
	return filedescriptor::netToHost(value);
}

uint32_t sqlrprotocol::beToHost(uint32_t value) {
	return filedescriptor::netToHost(value);
}

uint64_t sqlrprotocol::beToHost(uint64_t value) {
	return filedescriptor::netToHost(value);
}

uint16_t sqlrprotocol::hostTo(uint16_t value) {
	return (getProtocolIsBigEndian())?hostToBE(value):hostToLE(value);
}

uint32_t sqlrprotocol::hostTo(uint32_t value) {
	return (getProtocolIsBigEndian())?hostToBE(value):hostToLE(value);
}

uint64_t sqlrprotocol::hostTo(uint64_t value) {
	return (getProtocolIsBigEndian())?hostToBE(value):hostToLE(value);
}

uint16_t sqlrprotocol::hostToLE(uint16_t value) {
	return filedescriptor::hostToLittleEndian(value);
}

uint32_t sqlrprotocol::hostToLE(uint32_t value) {
	return filedescriptor::hostToLittleEndian(value);
}

uint64_t sqlrprotocol::hostToLE(uint64_t value) {
	return filedescriptor::hostToLittleEndian(value);
}

uint16_t sqlrprotocol::hostToBE(uint16_t value) {
	return filedescriptor::hostToNet(value);
}

uint32_t sqlrprotocol::hostToBE(uint32_t value) {
	return filedescriptor::hostToNet(value);
}

uint64_t sqlrprotocol::hostToBE(uint64_t value) {
	return filedescriptor::hostToNet(value);
}

bool sqlrprotocol::getDebug() {
	return pvt->_debug;
}

void sqlrprotocol::debugStart(const char *title) {
	debugStart(title,0);
}

void sqlrprotocol::debugStart(const char *title, uint16_t indent) {
	if (pvt->_debug) {
		for (uint16_t i=0; i<indent; i++) {
			stdoutput.write('	');
		}
		if (!indent) {
			stdoutput.printf("%d: ",process::getProcessId());
		}
		stdoutput.write(title);
		stdoutput.write(" {\n");
	}
}

void sqlrprotocol::debugEnd() {
	return debugEnd(0);
}

void sqlrprotocol::debugEnd(uint16_t indent) {
	if (pvt->_debug) {
		for (uint16_t i=0; i<indent; i++) {
			stdoutput.write('	');
		}
		stdoutput.write("}\n");
	}
}

void sqlrprotocol::debugHexDump(const unsigned char *data, uint64_t size) {
	return debugHexDump(data,size,1);
}

void sqlrprotocol::debugHexDump(const unsigned char *data,
						uint64_t size,
						uint16_t indent) {
	if (!pvt->_debug) {
		return;
	}
	if (!size) {
		return;
	}
	for (uint16_t j=0; j<indent; j++) {
		stdoutput.write("	");
	}
	for (uint64_t i=0; i<size; i++) {
		stdoutput.printf("%02x  ",data[i]);
		if (!((i+1)%8)) {
			stdoutput.write("   ");
		}
		if (!((i+1)%16)) {
			stdoutput.write("\n");
			for (uint16_t j=0; j<indent; j++) {
				stdoutput.write("	");
			}
		}
	}
	stdoutput.write("\n");
	for (uint16_t i=0; i<indent; i++) {
		stdoutput.write("	");
	}
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
			stdoutput.write("\n");
			for (uint16_t j=0; j<indent; j++) {
				stdoutput.write("	");
			}
		}
	}
	stdoutput.write('\n');
}
