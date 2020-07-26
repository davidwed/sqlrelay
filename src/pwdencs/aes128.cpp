// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/aes128.h>
#include <rudiments/file.h>

class SQLRSERVER_DLLSPEC sqlrpwenc_aes128 : public sqlrpwdenc {
	public:
			sqlrpwenc_aes128(domnode *parameters, bool debug);
		char	*encrypt(const char *value);
		char	*decrypt(const char *value);
	private:
		char	*convert(const char *value, bool dec);
		void	getValue(const char *in,
					const char *path,
					const char *ext,
					unsigned char **out,
					uint64_t *outlen,
					bool hexdecode);
		bool	getFile(const char *filename,
					unsigned char **out,
					uint64_t *outlen,
					bool hexdecode);

		const char	*debug;

		const char	*keypath;
		const char	*keyext;

		const char	*credpath;
		const char	*credext;

		bytebuffer	converted;

		aes128		aes;
};

sqlrpwenc_aes128::sqlrpwenc_aes128(domnode *parameters, bool debug) :
						sqlrpwdenc(parameters,debug) {

	debug=parameters->getAttributeValue("debug");

	// get optional keypath/keyext
	keypath=parameters->getAttributeValue("keypath");
	if (charstring::isNullOrEmpty(keypath)) {
		keypath="/usr/local/firstworks/etc/priv/dek";
	}
	keyext=parameters->getAttributeValue("keyext");
	if (charstring::isNullOrEmpty(keyext)) {
		keyext=".aes128";
	}

	// get optional credpath/credext
	credpath=parameters->getAttributeValue("credpath");
	if (charstring::isNullOrEmpty(credpath)) {
		credpath="/usr/local/firstworks/etc/priv/credentials";
	}
	credext=parameters->getAttributeValue("credext");
	if (charstring::isNullOrEmpty(credext)) {
		credext=".aes128";
	}
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
	unsigned char	*key;
	uint64_t	keylen;
	getValue(getParameters()->getAttributeValue("key"),
				keypath,keyext,&key,&keylen,true);
	if (keylen<aes.getKeySize()) {
		delete[] key;
		return NULL;
	}
	aes.setKey(key,aes.getKeySize());

	// get the credentials, either directly or from a file
	unsigned char	*cred;
	uint64_t	credlen;
	getValue(value,credpath,credext,&cred,&credlen,dec);
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

	if (dec) {
		converted.append(aes.getDecryptedData(),
					aes.getDecryptedDataLength());
		converted.append('\0');
		return (char *)converted.detachBuffer();
	} else {
		converted.append(aes.getIv(),aes.getIvSize());
		converted.append(aes.getEncryptedData(),
					aes.getEncryptedDataLength());
		return charstring::hexEncode(converted.getBuffer(),
						converted.getSize());
	}
}

void sqlrpwenc_aes128::getValue(const char *in,
					const char *path,
					const char *ext,
					unsigned char **out,
					uint64_t *outlen,
					bool hexdecode) {
	*out=NULL;
	*outlen=0;

	size_t	inlen=charstring::length(in);

	// if the input is [...file...] then attempt to get the
	// contents of the specified file
	if (in[0]=='[' && in[inlen-1]==']') {

		stringbuffer	fn;
		fn.append(in+1,inlen-2);

		// try the filename as-is
		if (getFile(fn.getString(),out,outlen,hexdecode)) {
			return;
		}

		// try appending an extension
		fn.append(ext);
		if (getFile(fn.getString(),out,outlen,hexdecode)) {
			return;
		}

		// try prepending a path
		fn.clear();
		fn.append(path);
		fn.append(in+1,inlen-2);
		if (getFile(fn.getString(),out,outlen,hexdecode)) {
			return;
		}

		// try appending a path again
		fn.append(path);
		if (getFile(fn.getString(),out,outlen,hexdecode)) {
			return;
		}
	}

	// just return the in verbatim
	if (hexdecode) {
		charstring::hexDecode(in,inlen,out,outlen);
	} else {
		*outlen=inlen;
		*out=(unsigned char *)charstring::duplicate(in,inlen);
	}
}

bool sqlrpwenc_aes128::getFile(const char *filename,
					unsigned char **out,
					uint64_t *outlen,
					bool hexdecode) {
	file	f;
	if (f.open(filename,O_RDONLY)) {
		if (hexdecode) {
			charstring::hexDecode(f.getContents(),
							f.getSize(),
							out,outlen);
		} else {
			*outlen=f.getSize();
			*out=(unsigned char *)f.getContents();
		}
		return true;
	}
	return false;
}

extern "C" {
	 SQLRSERVER_DLLSPEC sqlrpwdenc *new_sqlrpwdenc_aes128(
						domnode *parameters,
						bool debug) {
		return new sqlrpwenc_aes128(parameters,debug);
	}
}
