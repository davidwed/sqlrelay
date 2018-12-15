// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/sha256.h>
#include <rudiments/charstring.h>

class SQLRSERVER_DLLSPEC sqlrpwenc_sha256 : public sqlrpwdenc {
	public:
			sqlrpwenc_sha256(domnode *parameters, bool debug);
		bool	oneWay();
		char	*encrypt(const char *value);
};

sqlrpwenc_sha256::sqlrpwenc_sha256(domnode *parameters, bool debug) :
						sqlrpwdenc(parameters,debug) {
}

bool sqlrpwenc_sha256::oneWay() {
	return true;
}

char *sqlrpwenc_sha256::encrypt(const char *value) {
	sha256	s;
	s.append((const unsigned char *)value,charstring::length(value));
	return charstring::hexEncode(s.getHash(),s.getHashLength());
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrpwdenc *new_sqlrpwdenc_sha256(
						domnode *parameters,
						bool debug) {
		return new sqlrpwenc_sha256(parameters,debug);
	}
}
