// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>
#include <rudiments/sha1.h>
#include <rudiments/charstring.h>

class SQLRUTIL_DLLSPEC sqlrpwenc_sha1 : public sqlrpwdenc {
	public:
		sqlrpwenc_sha1(domnode *parameters, bool debug);
		bool	oneWay();
		char	*encrypt(const char *value);
};

sqlrpwenc_sha1::sqlrpwenc_sha1(domnode *parameters, bool debug) :
						sqlrpwdenc(parameters,debug) {
}

bool sqlrpwenc_sha1::oneWay() {
	return true;
}

char *sqlrpwenc_sha1::encrypt(const char *value) {
	sha1	s;
	s.append((const byte_t *)value,charstring::getLength(value));
	return charstring::hexEncode(s.getHash(),s.getHashSize());
}

extern "C" {
	SQLRUTIL_DLLSPEC sqlrpwdenc *new_sqlrpwdenc_sha1(
						domnode *parameters,
						bool debug) {
		return new sqlrpwenc_sha1(parameters,debug);
	}
}
