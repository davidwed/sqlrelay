// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/aes128.h>
#include <rudiments/sensitivevalue.h>

class SQLRSERVER_DLLSPEC sqlrpwenc_aes128 : public sqlrpwdenc {
	public:
			sqlrpwenc_aes128(domnode *parameters, bool debug);
		char	*encrypt(const char *value);
		char	*decrypt(const char *value);
	private:
		char	*convert(const char *value, bool dec);

		const char	*debug;

		sensitivevalue	keyvalue;
		sensitivevalue	credvalue;

		bytebuffer	converted;

		aes128		aes;
};

sqlrpwenc_aes128::sqlrpwenc_aes128(domnode *parameters, bool debug) :
						sqlrpwdenc(parameters,debug) {

	debug=parameters->getAttributeValue("debug");

	// get optional flags, paths, and extensions
	const char	*keypath=parameters->getAttributeValue("keypath");
	if (charstring::isNullOrEmpty(keypath)) {
		keypath=SYSCONFDIR;
	}
	keyvalue.setPath(keypath);
	keyvalue.setVerbatimFormat(FORMAT_HEX);
	if (charstring::isYes(parameters->getAttributeValue("keybin"))) {
		keyvalue.setFileFormat(FORMAT_BINARY);
	} else {
		keyvalue.setFileFormat(FORMAT_HEX);
	}
	keyvalue.setBinaryExtension(
			parameters->getAttributeValue("keybinext"));
	keyvalue.setHexExtension(
			parameters->getAttributeValue("keyhexext"));

	const char	*credpath=parameters->getAttributeValue("credpath");
	if (charstring::isNullOrEmpty(credpath)) {
		credpath=SYSCONFDIR;
	}
	credvalue.setPath(credpath);
	if (charstring::isYes(parameters->getAttributeValue("credbin"))) {
		credvalue.setFileFormat(FORMAT_BINARY);
	} else {
		credvalue.setFileFormat(FORMAT_HEX);
	}
	credvalue.setBinaryExtension(
			parameters->getAttributeValue("credbinext"));
	credvalue.setHexExtension(
			parameters->getAttributeValue("credhexext"));
}

char *sqlrpwenc_aes128::encrypt(const char *value) {
	return convert(value,false);
}

char *sqlrpwenc_aes128::decrypt(const char *value) {
	return convert(value,true);
}

char *sqlrpwenc_aes128::convert(const char *value, bool dec) {

	converted.clear();
	aes.clear();

	// get the key, either directly or from a file
	keyvalue.parse(getParameters()->getAttributeValue("key"));
	uint64_t	keylen=keyvalue.getValueSize();
	unsigned char	*key=keyvalue.detachValue();
	if (keylen<aes.getKeySize()) {
		delete[] key;
		return NULL;
	}
	aes.setKey(key,aes.getKeySize());

	// get the credentials, either directly or from a file
	credvalue.setVerbatimFormat((dec)?FORMAT_HEX:FORMAT_BINARY);
	credvalue.parse(value);
	uint64_t	credlen=credvalue.getValueSize();
	unsigned char	*cred=credvalue.detachValue();
	if (dec) {
		if (credlen<aes.getIvSize()) {
			delete[] key;
			delete[] cred;
			return NULL;
		}

		// set the iv (first 16 bytes of the credentials)
		// and data (remaining bytes of the credentials)
		aes.setIv(cred,aes.getIvSize());
		aes.append(cred+aes.getIvSize(),credlen-aes.getIvSize());
	} else {
		// set a random iv and set the data
		aes.setRandomIv();
		aes.append(cred,credlen);
	}

	delete[] key;
	delete[] cred;

	// Below, if the encrypted/decrypted data was NULL then an
	// encryption/decryption error occurred.  We should also return NULL to
	// indicate than an error occurred, rather than returning an empty
	// string.  In the case of decryption, returning an empty string would
	// allow an empty password to succeed!
	if (dec) {
		const unsigned char	*data=aes.getDecryptedData();
		if (!data) {
			return NULL;
		}
		converted.append(aes.getDecryptedData(),
					aes.getDecryptedDataSize());
		converted.append('\0');
		return (char *)converted.detachBuffer();
	} else {
		// if the encrypted data was NULL (an error occurred),
		// then also return NULL, indicating than an error occurred
		const unsigned char	*data=aes.getEncryptedData();
		if (!data) {
			return NULL;
		}
		converted.append(aes.getIv(),aes.getIvSize());
		converted.append(data,aes.getEncryptedDataSize());
		return charstring::hexEncode(converted.getBuffer(),
						converted.getSize());
	}
}

extern "C" {
	 SQLRSERVER_DLLSPEC sqlrpwdenc *new_sqlrpwdenc_aes128(
						domnode *parameters,
						bool debug) {
		return new sqlrpwenc_aes128(parameters,debug);
	}
}
