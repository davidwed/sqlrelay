// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/bytebuffer.h>
#include <rudiments/sha1.h>

#include <fwpk/fwpkincludes.h>

class SQLRSERVER_DLLSPEC sqlrauth_oracle_userlist : public sqlrauth {
	public:
			sqlrauth_oracle_userlist(sqlrservercontroller *cont,
							sqlrauths *auths,
							sqlrpwdencs *sqlrpe,
							domnode *parameters);
		const char	*auth(sqlrcredentials *cred);
		bool	compare(const char *suppliedresponse,
					uint64_t suppliedresponselength,
					const char *validpassword,
					const char *method,
					const char *extra);
	private:
		const char	**users;
		const char	**passwords;
		const char	**passwordencryptions;
		uint64_t	usercount;

		bool	debug;
};

sqlrauth_oracle_userlist::sqlrauth_oracle_userlist(
					sqlrservercontroller *cont,
					sqlrauths *auths,
					sqlrpwdencs *sqlrpe,
					domnode *parameters) :
					sqlrauth(cont,auths,sqlrpe,parameters) {

	debug=cont->getConfig()->getDebugAuths();

	users=NULL;
	passwords=NULL;
	passwordencryptions=NULL;
	usercount=parameters->getChildCount();
	if (!usercount) {
		return;
	}

	// create an array of users and passwords and store the
	// users and passwords from the configuration in them
	// this is faster than running through the xml over and over
	users=new const char *[usercount];
	passwords=new const char *[usercount];
	passwordencryptions=new const char *[usercount];

	domnode *user=parameters->getFirstTagChild("user");
	for (uint64_t i=0; i<usercount; i++) {

		users[i]=user->getAttributeValue("user");
		passwords[i]=user->getAttributeValue("password");

		// support modern "passwordencryptionid" and fall back to
		// older "passwordencryption" attribute
		const char	*pwdencid=
				user->getAttributeValue("passwordencryptionid");
		if (!pwdencid) {
			pwdencid=user->getAttributeValue("passwordencryption");
		}
		passwordencryptions[i]=pwdencid;

		user=user->getNextTagSibling("user");
	}
}

static const char *supportedauthplugins[]={
	"oracle_native_password",
	//"oracle_old_password",
	//"authentication_windows_client",
	"oracle_clear_password",
	NULL
};

const char *sqlrauth_oracle_userlist::auth(sqlrcredentials *cred) {

	// this module only supports oracle credentials
	if (charstring::compare(cred->getType(),"oracle")) {
		return NULL;
	}

	const char	*user=((sqlroraclecredentials *)cred)->getUser();
	const char	*password=((sqlroraclecredentials *)cred)->getPassword();
	uint64_t	passwordlength=((sqlroraclecredentials *)cred)->
							getPasswordLength();
	const char	*method=((sqlroraclecredentials *)cred)->getMethod();
	const char	*extra=((sqlroraclecredentials *)cred)->getExtra();

	if (debug) {
		stdoutput.printf("auth %s {\n",method);
		stdoutput.printf("	user: \"%s\"\n",user);
		stdoutput.printf("	password: \"");
		stdoutput.safePrint(password,passwordlength);
		stdoutput.printf("\"\n");
		stdoutput.printf("	method: \"%s\"\n",method);
		stdoutput.printf("	extra: \"%s\"\n",extra);
		stdoutput.printf("}\n");
	}

	// sanity check on method
	if (!charstring::inSet(method,supportedauthplugins)) {
		return NULL;
	}

	// run through the user/password arrays...
	for (uint32_t i=0; i<usercount; i++) {

		// if the user matches...
		if (!charstring::compare(user,users[i])) {

			if (getPasswordEncryptions() &&
				charstring::length(passwordencryptions[i])) {

				// if password encryption is being used...

				// get the module
				sqlrpwdenc	*pe=
					getPasswordEncryptions()->
						getPasswordEncryptionById(
							passwordencryptions[i]);
				if (!pe) {
					return NULL;
				}

				// The way the oracle_native_password encryption
				// works, one-way passwords won't work.  For
				// two-way encryption, decrypt the password
				// from the configuration and compare it to the
				// password that was passed in...

				// FIXME: one-way encryption does work with
				// the oracle_clear_password method.

				bool	retval=false;
				if (!pe->oneWay()) {

					// decrypt the password
					// from the configuration
					char	*pwd=pe->decrypt(passwords[i]);

					// compare it to the password
					// that was passed in
					retval=compare(password,
							passwordlength,
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
						passwordlength,
						passwords[i],
						method,
						extra))?user:NULL;
			}
		}
	}
	return NULL;
}

bool sqlrauth_oracle_userlist::compare(const char *suppliedresponse,
						uint64_t suppliedresponselength,
						const char *validpassword,
						const char *method,
						const char *extra) {

	// oracle_clear_password is really simple
	if (!charstring::compare(method,"oracle_clear_password")) {
		if (debug) {
			stdoutput.printf("auth compare {\n");
			stdoutput.printf("	expected response: ");
			stdoutput.safePrint(validpassword);
			stdoutput.printf("\n");
			stdoutput.printf("	supplied response: ");
			stdoutput.safePrint(suppliedresponse,
						suppliedresponselength);
			stdoutput.printf("\n");
			stdoutput.printf("}\n");
		}
		return !charstring::compare(suppliedresponse,validpassword);
	}

	// oracle_native_password is more complicated...

	// expectedresponse = sha1(password) xor
	// 			sha1(concat(randombytes,sha1(sha1(password))))

	// sha1(password)
	sha1	s1;
	s1.append((const unsigned char *)validpassword,
			charstring::length(validpassword));
	bytebuffer	sha1pass;
	sha1pass.append(s1.getHash(),s1.getHashSize());

	// sha1(sha1(password))))
	sha1	s2;
	s2.append(sha1pass.getBuffer(),sha1pass.getSize());
	bytebuffer	sha1sha1pass;
	sha1sha1pass.append(s2.getHash(),s2.getHashSize());

	// concat(randombytes,sha1(sha1(password)))
	bytebuffer	rbsha1sha1pass;
	rbsha1sha1pass.append(extra,charstring::length(extra));
	rbsha1sha1pass.append(sha1sha1pass.getBuffer(),sha1sha1pass.getSize());

	// sha1(concat(randombytes,sha1(sha1(password))))
	sha1	s3;
	s3.append(rbsha1sha1pass.getBuffer(),rbsha1sha1pass.getSize());
	bytebuffer	sha1rbsha1sha1pass;
	sha1rbsha1sha1pass.append(s3.getHash(),s3.getHashSize());
	
	// sha1(password) xor sha1(concat(randombytes,sha1(sha1(password))))
	bytebuffer	expectedresponse;
	const unsigned char	*bytes1=sha1pass.getBuffer();
	const unsigned char	*bytes2=sha1rbsha1sha1pass.getBuffer();
	for (uint64_t i=0; i<sha1pass.getSize(); i++) {
		expectedresponse.append((unsigned char)(bytes1[i]^bytes2[i]));
	}

	if (debug) {
		stdoutput.printf("auth compare {\n");
		stdoutput.printf("	expected response: ");
		stdoutput.safePrint(expectedresponse.getBuffer(),
					expectedresponse.getSize());
		stdoutput.printf("\n");
		stdoutput.printf("	supplied response: ");
		stdoutput.safePrint(suppliedresponse,suppliedresponselength);
		stdoutput.printf("\n");
		stdoutput.printf("}\n");
	}

	// compare the expected and supplied response sizes and values
	return (expectedresponse.getSize()==suppliedresponselength) &&
		!bytestring::compare(expectedresponse.getBuffer(),
				suppliedresponse,suppliedresponselength);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_oracle_userlist(
						sqlrservercontroller *cont,
						sqlrauths *auths,
						sqlrpwdencs *sqlrpe,
						domnode *parameters) {
		return new sqlrauth_oracle_userlist(cont,auths,
							sqlrpe,parameters);
	}
}
