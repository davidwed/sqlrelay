// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/aes128.h>
#include <rudiments/file.h>

class SQLRSERVER_DLLSPEC sqlrpwenc_aes128 : public sqlrpwdenc {
	public:
			sqlrpwenc_aes128(domnode *parameters, bool debug);
			~sqlrpwenc_aes128();
		char	*encrypt(const char *value);
		char	*decrypt(const char *value);
	private:
		char	*rotate(const char *value, int64_t count);

		const char	*debug;
		char		*decrypted;
		stringbuffer	keyfile;

		aes128		aes;
};

sqlrpwenc_aes128::sqlrpwenc_aes128(domnode *parameters, bool debug) :
						sqlrpwdenc(parameters,debug) {

	debug=parameters->getAttributeValue("debug");
	decrypted=NULL;

	// build the key file name
	const char	*key=parameters->getAttributeValue("key");
// FIXME: make this path configurable and have a better default
	keyfile.append("/usr/local/firstworks/etc/priv/dek");
	keyfile.append((!charstring::isNullOrEmpty(key))?key:"credentials");
	keyfile.append(".aes128");
}

sqlrpwenc_aes128::~sqlrpwenc_aes128() {
	delete[] decrypted;
}

char *sqlrpwenc_aes128::encrypt(const char *value) {
	return decrypt(value);
}

char *sqlrpwenc_aes128::decrypt(const char *value) {

	// get the key from the key file
	unsigned char	*key=
		(unsigned char *)file::getContents(keyfile.getString());
	if (charstring::length(key)<aes.getKeySize()) {
		delete[] key;
		return NULL;
	}

	// build the name of the file containing the credentials from the value
	stringbuffer	credfile;
// FIXME: make this path configurable and have a better default
	credfile.append("/usr/local/firstworks/etc/priv/credentials");
	credfile.append(value);
	credfile.append(".aes128");

	// get the iv and credentials from the credentials file
	file	cf;
	if (!cf.open(credfile.getString(),O_RDONLY)) {
		delete[] key;
		return NULL;
	}
	off64_t	credsize=cf.getSize();
	if (credsize<aes.getIvSize()) {
		delete[] key;
		return NULL;
	}
	unsigned char	*cred=(unsigned char *)cf.getContents();

	// set the key, iv (first 16 bytes of the credentials file)
	// and encrypted data (remaining bytes of the credentials file)
	aes.clear();
	aes.setKey(key,aes.getKeySize());
	aes.setIv(cred,aes.getIvSize());
	aes.append(cred+aes.getIvSize(),credsize-aes.getIvSize());

	// copy out the decrypted data, making sure to null-terminate it
	delete[] decrypted;
	decrypted=new char[aes.getDataLength()+1];
	charstring::copy(decrypted,
			(const char *)aes.getDecryptedData(),
			aes.getDataLength());
	decrypted[aes.getDataLength()]='\0';

	return decrypted;
}

extern "C" {
	 SQLRSERVER_DLLSPEC sqlrpwdenc *new_sqlrpwdenc_aes128(
						domnode *parameters,
						bool debug) {
		return new sqlrpwenc_aes128(parameters,debug);
	}
}
