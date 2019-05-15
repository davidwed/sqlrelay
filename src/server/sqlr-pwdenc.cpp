// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>
#include <sqlrelay/sqlrserver.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>
#include <config.h>
#include <defaults.h>
#include <version.h>

static void helpmessage(const char *progname) {
	stdoutput.printf(
		"%s is the %s password encryption utility.\n"
		"\n"
		"When used with the appropriate password encryption module, passwords can be stored in encrypted form in the %s configuration, rather than in plain text.\n"
		"\n"
		"The %s utility can be used to encrypt a given plaintext password, using the specified instance and password encryption id.  The encrypted output may then included in the configuration in place of the plaintext password.\n"
		"\n"
		"Usage: %s [OPTIONS]\n"
		"\n"
		"Options:\n"
		CONFIGID
		"	-pwdencid passwordencryptionid	id of instance of passwordencryption\n"
		"					module to use\n"
		"\n"
		"	-password password		password to encrypt\n"
		"\n"
		"Examples:\n"
		"\n"
		"Encrypt the password \"pwd\" using the password encryption module with id\n"
		"\"rot16\" as defined in instance \"myinst\" in the default configuration.\n"
		"\n"
		"	%s -id myinst -pwdencid rot16 -password pwd\n"
		"\n"
		"Encrypt the password \"pwd\" using the password encryption module with id\n"
		"\"rot16\" as defined in instance \"myinst\" in the config file ./myconfig.conf.\n"
		"\n"
		"	%s -config ./myconfig.conf -id myinst -pwdencid rot16 -password pwd\n"
		"\n",
		progname,SQL_RELAY,SQL_RELAY,
		progname,progname,progname,progname);
}

int main(int argc, const char **argv) {

	version(argc,argv);
	help(argc,argv);

	sqlrcmdline 	cmdline(argc,argv);
	sqlrpaths	sqlrpth(&cmdline);
	sqlrconfigs	sqlrcfgs(&sqlrpth);

	// get the command line arguments
	const char	*configurl=sqlrpth.getConfigUrl();
	const char	*id=cmdline.getValue("-id");
	const char	*pwdencid=cmdline.getValue("-pwdencid");
	const char	*password=cmdline.getValue("-password");

	// sanity check and usage message
	if (charstring::isNullOrEmpty(id) ||
		charstring::isNullOrEmpty(pwdencid) ||
		charstring::isNullOrEmpty(password)) {

		stderror.printf("usage:\n"
			" %s-pwdenc [-config config] "
			"-id instance\n"
			"             -pwdencid passwordencryptionid "
			"-password password\n",SQLR);
		process::exit(1);
	}

	// load the configuration
	sqlrconfig	*cfg=sqlrcfgs.load(configurl,id);
	if (!cfg) {
		stderror.printf("%s instance %s not found in %s\n",
						SQL_RELAY,id,configurl);
		process::exit(1);
	}

	// initialize the password encryption framework
	domnode	*pwdencs=cfg->getPasswordEncryptions();
	if (pwdencs->isNullNode()) {
		stderror.printf("password encryption id %s not found\n",
								pwdencid);
		process::exit(1);
	}
	sqlrpwdencs	sqlrpe(&sqlrpth,false);
	sqlrpe.load(pwdencs);
	sqlrpwdenc	*sqlrp=sqlrpe.getPasswordEncryptionById(pwdencid);
	if (!sqlrp) {
		stderror.printf("password encryption id %s not found\n",
								pwdencid);
		process::exit(1);
	}

	// encrypt the password and print the result
	char	*encryptedpassword=sqlrp->encrypt(password);
	stdoutput.printf("%s\n",encryptedpassword);
	delete[] encryptedpassword;
	process::exit(0);
}
