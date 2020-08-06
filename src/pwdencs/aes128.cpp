// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/aes128.h>
#include <rudiments/file.h>
#include <rudiments/sys.h>

class SQLRSERVER_DLLSPEC sqlrpwenc_aes128 : public sqlrpwdenc {
	public:
			sqlrpwenc_aes128(domnode *parameters, bool debug);
		char	*encrypt(const char *value);
		char	*decrypt(const char *value);
	private:
		char	*convert(const char *value, bool dec);
		void	getValue(const char *in,
					bool verbatimishex,
					bool fileishex,
					const char *path,
					const char *binext,
					const char *hexext,
					unsigned char **out,
					uint64_t *outlen);
		bool	getFile(const char *filename,
					unsigned char **out,
					uint64_t *outlen,
					bool hexdecode);

		const char	*debug;

		const char	*keypath;
		bool		keybin;
		const char	*keybinext;
		const char	*keyhexext;

		const char	*credpath;
		bool		credbin;
		const char	*credbinext;
		const char	*credhexext;

		bytebuffer	converted;

		aes128		aes;
};

sqlrpwenc_aes128::sqlrpwenc_aes128(domnode *parameters, bool debug) :
						sqlrpwdenc(parameters,debug) {

	debug=parameters->getAttributeValue("debug");

	// get optional flags, paths, and extensions
	keypath=parameters->getAttributeValue("keypath");
	if (charstring::isNullOrEmpty(keypath)) {
		keypath=SYSCONFDIR;
	}
	keybin=charstring::isYes(parameters->getAttributeValue("keybin"));
	keybinext=parameters->getAttributeValue("keybinext");
	keyhexext=parameters->getAttributeValue("keyhexext");
	credpath=parameters->getAttributeValue("credpath");
	if (charstring::isNullOrEmpty(credpath)) {
		credpath=SYSCONFDIR;
	}
	credbin=charstring::isYes(parameters->getAttributeValue("credbin"));
	credbinext=parameters->getAttributeValue("credbinext");
	credhexext=parameters->getAttributeValue("credhexext");
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
	getValue(getParameters()->getAttributeValue("key"),true,!keybin,
				keypath,keybinext,keyhexext,&key,&keylen);
	if (keylen<aes.getKeySize()) {
		delete[] key;
		return NULL;
	}
	aes.setKey(key,aes.getKeySize());

	// get the credentials, either directly or from a file
	unsigned char	*cred;
	uint64_t	credlen;
	getValue(value,dec,!credbin,
			credpath,credbinext,credhexext,&cred,&credlen);
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
					aes.getDecryptedDataLength());
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
		converted.append(data,aes.getEncryptedDataLength());
		return charstring::hexEncode(converted.getBuffer(),
						converted.getSize());
	}
}

void sqlrpwenc_aes128::getValue(const char *in,
					bool verbatimishex,
					bool fileishex,
					const char *path,
					const char *binext,
					const char *hexext,
					unsigned char **out,
					uint64_t *outlen) {
stdoutput.printf("getValue(%s)...\n",in);
	*out=NULL;
	*outlen=0;

	size_t	inlen=charstring::length(in);

	// if the input is [...file...] then attempt to get the
	// contents of the specified file
	if (in[0]=='[' && in[inlen-1]==']') {

		stringbuffer	fn;

		// try the filename as-is
		// (if the filename doesn't end in the hexext,
		// then assume it's binary)
		fn.clear();
		fn.append(in+1,inlen-2);
stdoutput.printf("%s\n",fn.getString());
		if (getFile(fn.getString(),out,outlen,fileishex)) {
stdoutput.printf("found file\n");
			return;
		}

		// try prepending a path
		// (if the filename doesn't end in the hexext,
		// then assume it's binary)
		fn.clear();
		fn.append(path)->append(sys::getDirectorySeparator());
		fn.append(in+1,inlen-2);
stdoutput.printf("%s\n",fn.getString());
		if (getFile(fn.getString(),out,outlen,fileishex)) {
stdoutput.printf("found path + file\n");
			return;
		}

		if (!charstring::isNullOrEmpty(binext)) {

			// try appending the binary extension
			fn.clear();
			fn.append(in+1,inlen-2);
			fn.append('.');
			fn.append(binext);
stdoutput.printf("%s\n",fn.getString());
			if (getFile(fn.getString(),out,outlen,false)) {
stdoutput.printf("found bin ext\n");
				return;
			}

			// try path + binary extension
			fn.clear();
			fn.append(path)->append(sys::getDirectorySeparator());
			fn.append(in+1,inlen-2);
			fn.append('.');
			fn.append(binext);
stdoutput.printf("%s\n",fn.getString());
			if (getFile(fn.getString(),out,outlen,false)) {
stdoutput.printf("found path + bin ext\n");
				return;
			}
		}

		if (!charstring::isNullOrEmpty(hexext)) {

			// try appending the hex extension
			fn.clear();
			fn.append(in+1,inlen-2);
			fn.append('.');
			fn.append(hexext);
stdoutput.printf("%s\n",fn.getString());
			if (getFile(fn.getString(),out,outlen,true)) {
stdoutput.printf("found hex ext\n");
				return;
			}

			// try path + hex extension
			fn.clear();
			fn.append(path)->append(sys::getDirectorySeparator());
			fn.append(in+1,inlen-2);
			fn.append('.');
			fn.append(hexext);
stdoutput.printf("%s\n",fn.getString());
			if (getFile(fn.getString(),out,outlen,true)) {
stdoutput.printf("found path + hex ext\n");
				return;
			}
		}
	}

stdoutput.printf("verbatim\n");
	// just return the in verbatim
	if (verbatimishex) {
		charstring::hexDecode(in,inlen,out,outlen);
	} else {
		*out=(unsigned char *)charstring::duplicate(in,inlen);
		*outlen=inlen;
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
