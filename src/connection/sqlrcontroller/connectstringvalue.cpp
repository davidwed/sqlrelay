// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

const char *sqlrcontroller_svr::connectStringValue(const char *variable) {

	// If we're using password encryption and the password is requested,
	// then return the decrypted version of it, otherwise just return
	// the value as-is.
	const char	*peid=constr->getPasswordEncryption();
	if (sqlrpe && charstring::length(peid) &&
			!charstring::compare(variable,"password")) {
		sqlrpwdenc	*pe=sqlrpe->getPasswordEncryptionById(peid);
		delete[] decrypteddbpassword;
		decrypteddbpassword=pe->decrypt(
			constr->getConnectStringValue(variable));
		return decrypteddbpassword;
	}
	return constr->getConnectStringValue(variable);
}
