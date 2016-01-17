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
	stdoutput.printf("FIXME: implement this\n");
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
	if (!charstring::length(id) ||
		!charstring::length(pwdencid) ||
		!charstring::length(password)) {

		stderror.printf("usage: %s-pwdenc [-config config] "
			"-id instance -pwdencid passwordencryptionid "
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
	if (!charstring::length(pwdencs)) {
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
