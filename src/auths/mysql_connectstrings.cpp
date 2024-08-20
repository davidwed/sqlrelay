// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/bytebuffer.h>
#include <rudiments/sha1.h>
#include <rudiments/sha256.h>
#include <rudiments/sensitivevalue.h>

class SQLRSERVER_DLLSPEC sqlrauth_mysql_connectstrings : public sqlrauth {
	public:
		sqlrauth_mysql_connectstrings(sqlrservercontroller *cont,
							domnode *parameters);
		const char	*auth(sqlrcredentials *cred);
		bool		compare(const char *suppliedresponse,
					uint64_t suppliedresponsesize,
					const char *validpassword,
					const char *method,
					const char *extra);
	private:
		const char	**users;
		const char	**passwords;
		const char	**passwordencryptions;
		uint64_t	usercount;

		sensitivevalue	passwordvalue;
};

sqlrauth_mysql_connectstrings::sqlrauth_mysql_connectstrings(
					sqlrservercontroller *cont,
					domnode *parameters) :
					sqlrauth(cont,parameters) {

	linkedlist< connectstringcontainer * >	*connectstrings=
				cont->getConfig()->getConnectStringList();

	users=NULL;
	passwords=NULL;
	passwordencryptions=NULL;
	usercount=connectstrings->getCount();
	if (!usercount) {
		return;
	}

	// create an array of users and passwords and store the
	// users and passwords from the configuration in them
	// this is faster than running through the xml over and over
	users=new const char *[usercount];
	passwords=new const char *[usercount];
	passwordencryptions=new const char *[usercount];

	passwordvalue.setPath(cont->getConfig()->getPasswordPath());

	uint64_t	i=0;
	for (listnode< connectstringcontainer * > *node=
				connectstrings->getFirst();
				node; node=node->getNext()) {

		users[i]=node->getValue()->
				getConnectStringValue("user");
		passwordvalue.parse(node->getValue()->
				getConnectStringValue("password"));
		passwords[i]=passwordvalue.detachTextValue();

		passwordencryptions[i]=node->getValue()->
						getPasswordEncryption();
		i++;
	}
}

static const char *supportedauthplugins[]={
	"mysql_native_password",
	//"mysql_old_password",
	//"authentication_windows_client",
	"mysql_clear_password",
	//"sha256_password",
	//"caching_sha2_password",
	NULL
};

const char *sqlrauth_mysql_connectstrings::auth(sqlrcredentials *cred) {

	// this module only supports mysql credentials
	if (charstring::compare(cred->getType(),"mysql")) {
		return NULL;
	}

	const char	*user=((sqlrmysqlcredentials *)cred)->getUser();
	const char	*password=((sqlrmysqlcredentials *)cred)->getPassword();
	uint64_t	passwordsize=((sqlrmysqlcredentials *)cred)->
							getPasswordSize();
	const char	*method=((sqlrmysqlcredentials *)cred)->getMethod();
	const char	*extra=((sqlrmysqlcredentials *)cred)->getExtra();

	if (getDebug()) {
		debugStart("auth %s",method);
		debugWrite("user: \"%s\"",user);
		stringbuffer	b;
		b.append("password: \"");
		b.safePrint(password,passwordsize);
		b.append("\"");
		debugWrite(b.getString());
		debugWrite("method: \"%s\"",method);
		debugWrite("extra: \"%s\"",extra);
		debugEnd();
	}

	// sanity check on method
	if (!charstring::isInSet(method,supportedauthplugins)) {
		return NULL;
	}

	// run through the user/password arrays...
	for (uint32_t i=0; i<usercount; i++) {

		// if the user matches...
		if (!charstring::compare(user,users[i])) {

			if (charstring::getLength(passwordencryptions[i])) {

				// if password encryption is being used...

				// get the module
				sqlrpwdenc	*pe=
					cont->getPasswordEncryptionById(
							passwordencryptions[i]);
				if (!pe) {
					return NULL;
				}

				// The way the mysql_native_password encryption
				// works, one-way passwords won't work.  For
				// two-way encryption, decrypt the password
				// from the configuration and compare it to the
				// password that was passed in...

				// FIXME: one-way encryption does work with
				// the mysql_clear_password method.

				bool	retval=false;
				if (!pe->oneWay()) {

					// decrypt the password
					// from the configuration
					char	*pwd=pe->decrypt(passwords[i]);

					// compare it to the password
					// that was passed in
					retval=compare(password,
							passwordsize,
							pwd,
							method,extra);

					// clean up
					delete[] pwd;
				}

				// return user or NULL
				return (retval)?user:NULL;

			} else {

				// if password encryption isn't being used,
				// return the user if the passwords match
				return (compare(password,
						passwordsize,
						passwords[i],
						method,
						extra))?user:NULL;
			}
		}
	}
	return NULL;
}

bool sqlrauth_mysql_connectstrings::compare(const char *suppliedresponse,
						uint64_t suppliedresponsesize,
						const char *validpassword,
						const char *method,
						const char *extra) {

	bytebuffer	expectedresponse;

	// mysql_clear_password is really simple
	if (!charstring::compare(method,"mysql_clear_password")) {
		expectedresponse.append(suppliedresponse,
					suppliedresponsesize);
	} else

	// mysql_native_password is more complicated...
	if (!charstring::compare(method,"mysql_native_password")) {

		// expectedresponse = sha1(password) xor
		// 		sha1(concat(randombytes,sha1(sha1(password))))

		// sha1(password)
		sha1	s1;
		s1.append((const byte_t *)validpassword,
				charstring::getLength(validpassword));
		bytebuffer	sha1pass;
		sha1pass.append(s1.getHash(),s1.getHashSize());

		// sha1(sha1(password))))
		sha1	s2;
		s2.append(sha1pass.getBuffer(),sha1pass.getSize());
		bytebuffer	sha1sha1pass;
		sha1sha1pass.append(s2.getHash(),s2.getHashSize());

		// concat(randombytes,sha1(sha1(password)))
		bytebuffer	rbsha1sha1pass;
		rbsha1sha1pass.append(extra,charstring::getLength(extra));
		rbsha1sha1pass.append(sha1sha1pass.getBuffer(),
						sha1sha1pass.getSize());

		// sha1(concat(randombytes,sha1(sha1(password))))
		sha1	s3;
		s3.append(rbsha1sha1pass.getBuffer(),rbsha1sha1pass.getSize());
		bytebuffer	sha1rbsha1sha1pass;
		sha1rbsha1sha1pass.append(s3.getHash(),s3.getHashSize());
	
		// sha1(password) xor
		// sha1(concat(randombytes,sha1(sha1(password))))
		const byte_t	*bytes1=sha1pass.getBuffer();
		const byte_t	*bytes2=sha1rbsha1sha1pass.getBuffer();
		for (uint64_t i=0; i<sha1pass.getSize(); i++) {
			expectedresponse.append((byte_t)(bytes1[i]^bytes2[i]));
		}
	} else

	// sha256_password/caching_sha2_password are also more complicated
	if (!charstring::compare(method,"sha256_password") ||
		!charstring::compare(method,"caching_sha2_password")) {

		if (suppliedresponsesize) {

			// scramblebuffer = sha256(password) xor
			// sha256(concat(randombytes,sha256(sha256(password))))

			// sha256(password)
			sha256	s256;
			s256.append((const byte_t *)validpassword,
					charstring::getLength(validpassword));
			bytebuffer	sha256pass;
			sha256pass.append(s256.getHash(),s256.getHashSize());

			// sha256(sha256(password))))
			sha256	s2;
			s2.append(sha256pass.getBuffer(),sha256pass.getSize());
			bytebuffer	sha256sha256pass;
			sha256sha256pass.append(s2.getHash(),s2.getHashSize());

			// concat(randombytes,sha256(sha256(password)))
			bytebuffer	rbsha256sha256pass;
			rbsha256sha256pass.append(extra,
						charstring::getLength(extra));
			rbsha256sha256pass.append(sha256sha256pass.getBuffer(),
						sha256sha256pass.getSize());

			// sha256(concat(randombytes,sha256(sha256(password))))
			sha256	s3;
			s3.append(rbsha256sha256pass.getBuffer(),
						rbsha256sha256pass.getSize());
			bytebuffer	sha256rbsha256sha256pass;
			sha256rbsha256sha256pass.append(s3.getHash(),
							s3.getHashSize());

			// sha256(password) xor
			// sha256(concat(randombytes,sha256(sha256(password))))
			bytebuffer	scramblebuffer;
			const byte_t	*bytes1=
					sha256pass.getBuffer();
			const byte_t	*bytes2=
					sha256rbsha256sha256pass.getBuffer();
			for (uint64_t i=0; i<sha256pass.getSize(); i++) {
				scramblebuffer.append(
					(byte_t)(bytes1[i]^bytes2[i]));
			}

			// expectedresponse = rsa(scramblebuffer,
			// 				server-public-key,
			//			 	RSA_PKCS1_OAEP_PADDING)
			// FIXME: ...
		}

	} else {

		return false;
	}

	if (getDebug()) {
		debugStart("auth compare");
		stringbuffer	b;
		b.append("expected response: ");
		b.safePrint(expectedresponse.getBuffer(),
					expectedresponse.getSize());
		debugWrite(b.getString());
		b.clear();
		b.append("supplied response: ");
		b.safePrint(suppliedresponse,suppliedresponsesize);
		debugWrite(b.getString());
		debugEnd();
	}

	// compare the expected and supplied response sizes and values
	return (expectedresponse.getSize()==suppliedresponsesize) &&
		!bytestring::compare(expectedresponse.getBuffer(),
						suppliedresponse,
						suppliedresponsesize);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_mysql_connectstrings(
						sqlrservercontroller *cont,
						domnode *parameters) {
		return new sqlrauth_mysql_connectstrings(cont,parameters);
	}
}
