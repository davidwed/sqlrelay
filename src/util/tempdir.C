#include <tempdir.h>
#include <stdio.h>
#include <string.h>

tempdir::tempdir(cmdline *cmdl) {
	if (cmdl->getLocalStateDir()[0]) {
		tmpdir=new char[strlen(cmdl->getLocalStateDir())+14];
		sprintf(tmpdir,"%s/sqlrelay/tmp",cmdl->getLocalStateDir());
	} else {
		tmpdir=strdup(TMP_DIR);
	}
	tmpdirlen=strlen(tmpdir);
}

tempdir::~tempdir() {
	delete[] tmpdir;
}

char	*tempdir::getString() {
	return tmpdir;
}

int	tempdir::getLength() {
	return tmpdirlen;
}
