// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>
#include <sqlrelay/sqlrserver.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>
#include <config.h>
#include <defaults.h>
#include <version.h>

static void helpmessage() {
	stdoutput.printf(
		"%s-pwdenc is the SQL Relay password encryption utility.\n"
		"\n"
		"When used with the appropriate password encryption module, passwords can be stored in encrypted form in the SQL Relay configuration file, rather than in plain text.\n"
		"\n"
		"The %s-pwdenc utility can be used to encrypt a given plaintext password, using the specified instance and password encryption id.  The encrypted output may then included in the configuration file in place of the plaintext password.\n"
		"\n"
		"Usage: %s-pwdenc [OPTIONS]\n"
		"\n"
		"Options:\n"
		CONFIGID
		"	-pwdencid passwordencryptionid	...\n"
		"	-password password		...\n"
		"\n"
		"Examples:\n"
		"...\n"
		"\n"
		REPORTBUGS,
		SQLR,SQLR,SQLR);
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
			"-id instance \\\n"
			"             -pwdencid passwordencryptionid "
			"-password password\n",SQLR);
		process::exit(1);
	}

	// load the configuration
	sqlrconfig	*cfg=sqlrcfgs.load(configurl,id);
	if (!cfg) {
		stderror.printf("SQL Relay instance %s not found in %s\n",
								id,configurl);
		process::exit(1);
	}

	// initialize the password encryption framework
	const char	*pwdencs=cfg->getPasswordEncryptions();
	if (charstring::isNullOrEmpty(pwdencs)) {
		stderror.printf("password encryption id %s not found\n",
								pwdencid);
		process::exit(1);
	}
	sqlrpwdencs	sqlrpe(&sqlrpth);
	sqlrpe.loadPasswordEncryptions(pwdencs);
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
