// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/bytestring.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/process.h>
#include <rudiments/userentry.h>
#include <rudiments/file.h>
#include <rudiments/gss.h>
#include <rudiments/tls.h>

#include <config.h>
#include <defaults.h>

class sqlrprotocolprivate {
	friend class sqlrprotocol;
	private:
		sqlrprotocols		*_ps;
		domnode			*_parameters;
		bool			_bigendian;
		bool			_debug;
		bool			_usekrb;
		bool			_usetls;
		gsscredentials		_gcred;
		gssmechanism		_gmech;
		gsscontext		_gctx;
		tlscontext		_tctx;
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

	// tls initialization
	pvt->_usetls=charstring::isYes(parameters->getAttributeValue("tls"));
	pvt->_usekrb=charstring::isYes(parameters->getAttributeValue("krb"));

	if (pvt->_usekrb) {

		if (gss::supported()) {

			// set the keytab file to use
			const char	*keytab=
				parameters->getAttributeValue("krbkeytab");
			if (!charstring::isNullOrEmpty(keytab)) {
				pvt->_gcred.setKeytab(keytab);
			}

			// set the service to use
			const char	*service=
				parameters->getAttributeValue("krbservice");
			if (charstring::isNullOrEmpty(service)) {
				service=DEFAULT_KRBSERVICE;
			}

			// acquire service credentials
			if (!pvt->_gcred.acquireForService(service)) {
				const char	*status=
					pvt->_gcred.getMechanismMinorStatus();
				stderror.printf("kerberos acquire-"
						"service %s failed:\n%s",
						service,status);
				if (charstring::contains(status,
							"Permission denied")) {
					char	*user=userentry::getName(
							process::getUserId());
					stderror.printf("(keytab file likely "
							"not readable by user "
							"%s)\n",user);
					delete[] user;
				}
			}

			// initialize the gss context
			pvt->_gmech.open(
				parameters->getAttributeValue("krbmech"));
			pvt->_gctx.setDesiredMechanism(&pvt->_gmech);
			pvt->_gctx.setDesiredFlags(
				parameters->getAttributeValue("krbflags"));
			pvt->_gctx.setCredentials(&pvt->_gcred);

		} else {
			stderror.printf("Warning: kerberos support requested "
					"but platform doesn't support "
					"kerberos\n");
		}

	} else if (pvt->_usetls) {

		if (tls::supported()) {

			// get the protocol version to use
			pvt->_tctx.setProtocolVersion(
				parameters->getAttributeValue("tlsversion"));

			// get the certificate chain file to use
			const char	*tlscert=
				parameters->getAttributeValue("tlscert");
			if (file::getIsReadable(tlscert)) {
				pvt->_tctx.setCertificateChainFile(tlscert);
			} else if (!charstring::isNullOrEmpty(tlscert)) {
				stderror.printf("Warning: TLS certificate "
						"file %s is not readable.\n",
						tlscert);
			}

			// get the private key file to use
			const char	*tlskey=
				parameters->getAttributeValue("tlskey");
			if (file::getIsReadable(tlskey)) {
				pvt->_tctx.setPrivateKeyFile(tlskey);
			} else if (!charstring::isNullOrEmpty(tlskey)) {
				stderror.printf("Warning: TLS private key "
						"file %s is not readable.\n",
						tlskey);
			}

			// get the private key password to use
			pvt->_tctx.setPrivateKeyPassword(
				parameters->getAttributeValue("tlspassword"));

			// get whether to validate
			pvt->_tctx.setValidatePeer(
				charstring::isYes(
				parameters->getAttributeValue("tlsvalidate")));

			// get the certificate authority file to use
			const char	*tlsca=
				parameters->getAttributeValue("tlsca");
			if (file::getIsReadable(tlsca)) {
				pvt->_tctx.setCertificateAuthority(tlsca);
			} else if (!charstring::isNullOrEmpty(tlsca)) {
				stderror.printf("Warning: TLS certificate "
						"authority file %s is not "
						"readable.\n",tlsca);
			}

			// get the cipher list to use
			pvt->_tctx.setCiphers(
				parameters->getAttributeValue("tlsciphers"));

			// get the validation depth
			pvt->_tctx.setValidationDepth(
				charstring::toUnsignedInteger(
				parameters->getAttributeValue("tlsdepth")));

		} else {

			pvt->_usetls=false;

			stderror.printf("Warning: TLS support requested "
					"but platform doesn't support "
					"TLS\n");
		}
	}
}

sqlrprotocol::~sqlrprotocol() {
	delete pvt;
}

gsscontext *sqlrprotocol::getGssContext() {
	return &pvt->_gctx;
}

bool sqlrprotocol::useKrb() {
	return pvt->_usekrb;
}

tlscontext *sqlrprotocol::getTlsContext() {
	return &pvt->_tctx;
}

bool sqlrprotocol::useTls() {
	return pvt->_usetls;
}

void sqlrprotocol::endTransaction(bool commit) {
}

void sqlrprotocol::endSession() {
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

void sqlrprotocol::read(const byte_t *rp, char *value, const byte_t **rpout) {
	*value=(char)*rp;
	*rpout=rp+sizeof(char);
}

bool sqlrprotocol::read(const byte_t *rp, char *value,
						const char *name,
						char expected,
						const byte_t **rpout) {
	read(rp,value,rpout);
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

void sqlrprotocol::read(const byte_t *rp, byte_t *value,
						const byte_t **rpout) {
	*value=*rp;
	*rpout=rp+sizeof(byte_t);
}

bool sqlrprotocol::read(const byte_t *rp, byte_t *value,
						const char *name,
						byte_t expected,
						const byte_t **rpout) {
	read(rp,value,rpout);
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

void sqlrprotocol::read(const byte_t *rp, char *value,
						size_t length,
						const byte_t **rpout) {
	bytestring::copy(value,rp,length);
	*rpout=rp+length;
}

void sqlrprotocol::read(const byte_t *rp, byte_t *value,
						size_t length,
						const byte_t **rpout) {
	bytestring::copy(value,rp,length);
	*rpout=rp+length;
}

void sqlrprotocol::read(const byte_t *rp, ucs2_t *value,
						size_t length,
						const byte_t **rpout) {
	bytestring::copy(value,rp,length*sizeof(ucs2_t));
	*rpout=rp+length*sizeof(ucs2_t);
}

void sqlrprotocol::read(const byte_t *rp, float *value,
						const byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(float));
	*rpout=rp+sizeof(float);
}

void sqlrprotocol::read(const byte_t *rp, double *value,
						const byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(double));
	*rpout=rp+sizeof(double);
}

void sqlrprotocol::read(const byte_t *rp, uint16_t *value,
						const byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint16_t));
	*value=toHost(*value);
	*rpout=rp+sizeof(uint16_t);
}


void sqlrprotocol::readLE(const byte_t *rp, uint16_t *value,
						const byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint16_t));
	*value=leToHost(*value);
	*rpout=rp+sizeof(uint16_t);
}

bool sqlrprotocol::readLE(const byte_t *rp, uint16_t *value,
						const char *name,
						uint16_t expected,
						const byte_t **rpout) {
	readLE(rp,value,rpout);
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

void sqlrprotocol::readBE(const byte_t *rp, uint16_t *value,
						const byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint16_t));
	*value=beToHost(*value);
	*rpout=rp+sizeof(uint16_t);
}

bool sqlrprotocol::readBE(const byte_t *rp,
					uint16_t *value,
					const char *name,
					uint16_t expected,
					const byte_t **rpout) {
	readBE(rp,value,rpout);
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

void sqlrprotocol::read(const byte_t *rp, uint32_t *value,
						const byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint32_t));
	*value=toHost(*value);
	*rpout=rp+sizeof(uint32_t);
}

void sqlrprotocol::readLE(const byte_t *rp, uint32_t *value,
						const byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint32_t));
	*value=leToHost(*value);
	*rpout=rp+sizeof(uint32_t);
}

bool sqlrprotocol::readLE(const byte_t *rp, uint32_t *value,
						const char *name,
						uint32_t expected,
						const byte_t **rpout) {
	readLE(rp,value,rpout);
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

void sqlrprotocol::readBE(const byte_t *rp, uint32_t *value,
						const byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint32_t));
	*value=beToHost(*value);
	*rpout=rp+sizeof(uint32_t);
}

bool sqlrprotocol::readBE(const byte_t *rp, uint32_t *value,
						const char *name,
						uint32_t expected,
						const byte_t **rpout) {
	readBE(rp,value,rpout);
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

void sqlrprotocol::read(const byte_t *rp, uint64_t *value,
						const byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint64_t));
	*value=toHost(*value);
	*rpout=rp+sizeof(uint64_t);
}

void sqlrprotocol::readLE(const byte_t *rp, uint64_t *value,
						const byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint64_t));
	*value=leToHost(*value);
	*rpout=rp+sizeof(uint64_t);
}

bool sqlrprotocol::readLE(const byte_t *rp, uint64_t *value,
						const char *name,
						uint64_t expected,
						const byte_t **rpout) {
	readLE(rp,value,rpout);
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

void sqlrprotocol::readBE(const byte_t *rp, uint64_t *value,
						const byte_t **rpout) {
	bytestring::copy(value,rp,sizeof(uint64_t));
	*value=beToHost(*value);
	*rpout=rp+sizeof(uint64_t);
}

bool sqlrprotocol::readBE(const byte_t *rp, uint64_t *value,
						const char *name,
						uint64_t expected,
						const byte_t **rpout) {
	readBE(rp,value,rpout);
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

uint64_t sqlrprotocol::readLenEncInt(const byte_t *rp, const byte_t **rpout) {
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
			byte_t		*valbytes=(byte_t *)&val;
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

void sqlrprotocol::write(bytebuffer *buffer, char value) {
	buffer->append(value);
}

void sqlrprotocol::write(bytebuffer *buffer, byte_t value) {
	buffer->append(value);
}

void sqlrprotocol::write(bytebuffer *buffer, const char *value) {
	write(buffer,value,charstring::length(value));
}

void sqlrprotocol::write(bytebuffer *buffer, const char *value,
							size_t length) {
	buffer->append(value,length);
}

void sqlrprotocol::write(bytebuffer *buffer, const byte_t *value,
							size_t length) {
	buffer->append(value,length);
}

void sqlrprotocol::write(bytebuffer *buffer, const ucs2_t *str, size_t length) {
	buffer->appendUcs2(str,length);
}

void sqlrprotocol::write(bytebuffer *buffer, float value) {
	buffer->append(value);
}

void sqlrprotocol::write(bytebuffer *buffer, double value) {
	buffer->append(value);
}

void sqlrprotocol::write(bytebuffer *buffer, uint16_t value) {
	buffer->append(hostTo(value));
}

void sqlrprotocol::writeLE(bytebuffer *buffer, uint16_t value) {
	buffer->append(hostToLE(value));
}

void sqlrprotocol::writeBE(bytebuffer *buffer, uint16_t value) {
	buffer->append(hostToBE(value));
}

void sqlrprotocol::write(bytebuffer *buffer, uint32_t value) {
	buffer->append(hostTo(value));
}

void sqlrprotocol::writeLE(bytebuffer *buffer, uint32_t value) {
	buffer->append(hostToLE(value));
}

void sqlrprotocol::writeBE(bytebuffer *buffer, uint32_t value) {
	buffer->append(hostToBE(value));
}

void sqlrprotocol::write(bytebuffer *buffer, uint64_t value) {
	buffer->append(hostTo(value));
}

void sqlrprotocol::writeLE(bytebuffer *buffer, uint64_t value) {
	buffer->append(hostToLE(value));
}

void sqlrprotocol::writeBE(bytebuffer *buffer, uint64_t value) {
	buffer->append(hostToBE(value));
}

void sqlrprotocol::writeLenEncInt(bytebuffer *buffer, uint64_t value) {
	if (value>=16777216) {
		buffer->append((char)0xfe);
		buffer->append((uint64_t)value);
	} else if (value>=65536) {
		buffer->append((char)0xfd);
		writeTriplet(buffer,(uint32_t)value);
	} else if (value>=251) {
		buffer->append((char)0xfc);
		buffer->append((uint16_t)value);
	} else {
		buffer->append((char)value);
	}
}

void sqlrprotocol::writeTriplet(bytebuffer *buffer, uint32_t value) {
	value=hostToBE(value);
	byte_t	*valuebytes=(byte_t *)&value;
	buffer->append(valuebytes[3]);
	buffer->append(valuebytes[2]);
	buffer->append(valuebytes[1]);
}

void sqlrprotocol::writeLenEncStr(bytebuffer *buffer,
						const char *string) {
	writeLenEncInt(buffer,charstring::length(string));
	buffer->append(string);
}

void sqlrprotocol::writeLenEncStr(bytebuffer *buffer,
						const char *string,
						uint64_t length) {
	writeLenEncInt(buffer,length);
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
	debugEnd(0);
}

void sqlrprotocol::debugEnd(uint16_t indent) {
	if (pvt->_debug) {
		for (uint16_t i=0; i<indent; i++) {
			stdoutput.write('	');
		}
		stdoutput.write("}\n");
	}
}

void sqlrprotocol::debugHexDump(const byte_t *data, uint64_t size) {
	debugHexDump(data,size,1);
}

void sqlrprotocol::debugHexDump(const byte_t *data,
						uint64_t size,
						uint16_t indent) {
	if (!pvt->_debug) {
		return;
	}
	stdoutput.printHex(data,size,indent);
}
