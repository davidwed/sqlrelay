// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>

#include <rudiments/commandline.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>
#include <sqlrconfigfile.h>
#include <sqlrelay/sqlrpwdencs.h>

int main(int argc, const char **argv) {

	#include <version.h>

	// get the command line arguments
	commandline	cmdline(argc,argv);
	const char	*config=cmdline.getValue("-config");
	const char	*id=cmdline.getValue("-id");
	const char	*pwdencid=cmdline.getValue("-pwdencid");
	const char	*password=cmdline.getValue("-password");

	// sanity check and usage message
	if (!charstring::length(id) ||
		!charstring::length(pwdencid) ||
		!charstring::length(password)) {

		stderror.printf("usage: sqlrpwdenc [-config configfile] "
			"-id instance -pwdencid passwordencryptionid "
			"-password password\n");
		process::exit(1);
	}

	// open the config file
	sqlrconfigfile	cfgfl;
	if (!cfgfl.parse(config,id)) {
		stderror.printf("SQL Relay instance %s not found in %s\n",
								id,config);
		process::exit(1);
	}

	// initialize the password encryption framework
	const char	*pwdencs=cfgfl.getPasswordEncryptions();
	if (!charstring::length(pwdencs)) {
		stderror.printf("password encryption id %s not found\n",
								pwdencid);
		process::exit(1);
	}
	sqlrpwdencs	sqlrpe;
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
