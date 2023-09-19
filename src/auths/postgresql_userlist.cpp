// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/bytebuffer.h>
#include <rudiments/md5.h>
#include <rudiments/sensitivevalue.h>

class SQLRSERVER_DLLSPEC sqlrauth_postgresql_userlist : public sqlrauth {
	public:
			sqlrauth_postgresql_userlist(sqlrservercontroller *cont,
							sqlrauths *auths,
							sqlrpwdencs *sqlrpe,
							domnode *parameters);
		const char	*auth(sqlrcredentials *cred);
		bool		compare(const char *suppliedresponse,
					uint64_t suppliedresponselength,
					const char *user,
					const char *validpassword,
					const char *method,
					uint32_t salt);
	private:
		const char	**users;
		const char	**passwords;
		const char	**passwordencryptions;
		uint64_t	usercount;

		sensitivevalue	passwordvalue;

		bool	debug;
};

sqlrauth_postgresql_userlist::sqlrauth_postgresql_userlist(
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

	passwordvalue.setPath(cont->getConfig()->getPasswordPath());

	domnode *user=parameters->getFirstTagChild("user");
	for (uint64_t i=0; i<usercount; i++) {

		users[i]=user->getAttributeValue("user");
		passwordvalue.parse(user->getAttributeValue("password"));
		passwords[i]=passwordvalue.detachTextValue();

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

static const char *supportedmethods[]={
	"postgresql_cleartext",
	"postgresql_md5",
	NULL
};

const char *sqlrauth_postgresql_userlist::auth(sqlrcredentials *cred) {

	// this module only supports postgresql credentials
	if (charstring::compare(cred->getType(),"postgresql")) {
		return NULL;
	}

	const char	*user=
		((sqlrpostgresqlcredentials *)cred)->getUser();
	const char	*password=
		((sqlrpostgresqlcredentials *)cred)->getPassword();
	uint64_t	passwordlength=
		((sqlrpostgresqlcredentials *)cred)->getPasswordLength();
	const char	*method=
		((sqlrpostgresqlcredentials *)cred)->getMethod();
	uint32_t		salt=
		((sqlrpostgresqlcredentials *)cred)->getSalt();

	if (debug) {
		stdoutput.printf("auth %s {\n",method);
		stdoutput.printf("	user: \"%s\"\n",user);
		stdoutput.printf("	password: \"");
		stdoutput.safePrint(password,passwordlength);
		stdoutput.printf("\"\n");
		stdoutput.printf("	method: \"%s\"\n",method);
		stdoutput.printf("	salt: \"%d\"\n",salt);
		stdoutput.printf("}\n");
	}

	// sanity check on method
	if (!charstring::isInSet(method,supportedmethods)) {
		return NULL;
	}

	// run through the user/password arrays...
	for (uint32_t i=0; i<usercount; i++) {

		// if the user matches...
		if (!charstring::compare(user,users[i])) {

			if (getPasswordEncryptions() &&
				charstring::getLength(passwordencryptions[i])) {

				// if password encryption is being used...

				// get the module
				sqlrpwdenc	*pe=
					getPasswordEncryptions()->
						getPasswordEncryptionById(
							passwordencryptions[i]);
				if (!pe) {
					return NULL;
				}

				// The way the postgresql_md5 encryption
				// works, one-way passwords won't work.  For
				// two-way encryption, decrypt the password
				// from the configuration and compare it to the
				// password that was passed in...

				// FIXME: one-way encryption does work with
				// the postgresql_cleartext method.

				bool	retval=false;
				if (!pe->oneWay()) {

					// decrypt the password
					// from the configuration
					char	*pwd=pe->decrypt(passwords[i]);

					// compare it to the password
					// that was passed in
					retval=compare(password,
							passwordlength,
							user,pwd,
							method,salt);

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
						user,passwords[i],
						method,salt))?user:NULL;
			}
		}
	}
	return NULL;
}

bool sqlrauth_postgresql_userlist::compare(const char *suppliedresponse,
						uint64_t suppliedresponselength,
						const char *user,
						const char *validpassword,
						const char *method,
						uint32_t salt) {

	bytebuffer	expectedresponse;

	// postgresql_cleartext is really simple
	if (!charstring::compare(method,"postgresql_cleartext")) {
		expectedresponse.append(suppliedresponse,
					suppliedresponselength);
	} else

	// postgresql_md5 is more complicated...
	if (!charstring::compare(method,"postgresql_md5")) {

		// expectedresponse =
		//    concat('md5',
		//      md5(
		//        concat(
		//          md5(
		//            concat(password,user)
		//          ),
		//          random-salt
		//        )
		//      )
		//    )

		// md5(concat(password,user))
		md5	md1;
		md1.append((byte_t *)validpassword,
				charstring::getLength(validpassword));
		md1.append((byte_t *)user,
				charstring::getLength(user));
		char	*md1str=charstring::hexEncode(md1.getHash(),
							md1.getHashSize());

		// md5(concat(...above...,salt))
		md5	md2;
		md2.append((byte_t *)md1str,charstring::getLength(md1str));
		md2.append((byte_t *)&salt,sizeof(salt));
		char	*md2str=charstring::hexEncode(md2.getHash(),
							md2.getHashSize());
		
		// concat('md5',...above...)
		stringbuffer	result;
		result.append("md5",3);
		result.append(md2str,charstring::getLength(md2str));
		delete[] md2str;

		return (result.getSize()==suppliedresponselength) &&
			!charstring::compare(result.getString(),
						suppliedresponse,
						suppliedresponselength);
	} else {
		return false;
	}

	if (debug) {
		stdoutput.printf("auth compare {\n");
		stdoutput.printf("	expected response: ");
		stdoutput.safePrint(expectedresponse.getBuffer(),
					expectedresponse.getSize());
		stdoutput.printf("\n");
		stdoutput.printf("	supplied response: ");
		stdoutput.safePrint(suppliedresponse,
					suppliedresponselength);
		stdoutput.printf("\n");
		stdoutput.printf("}\n");
	}

	// compare the expected and supplied response sizes and values
	return (expectedresponse.getSize()==suppliedresponselength) &&
		!bytestring::compare(expectedresponse.getBuffer(),
						suppliedresponse,
						suppliedresponselength);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrauth *new_sqlrauth_postgresql_userlist(
						sqlrservercontroller *cont,
						sqlrauths *auths,
						sqlrpwdencs *sqlrpe,
						domnode *parameters) {
		return new sqlrauth_postgresql_userlist(cont,auths,
							sqlrpe,parameters);
	}
}
