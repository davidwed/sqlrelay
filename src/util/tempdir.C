#include <tempdir.h>
#include <stdio.h>

tempdir::tempdir(cmdline *cmdl) {
	if (cmdl->getLocalStateDir()[0]) {
		tmpdirlen=charstring::length(cmdl->getLocalStateDir())+13;
		tmpdir=new char[tmpdirlen+1];
		sprintf(tmpdir,"%s/sqlrelay/tmp",cmdl->getLocalStateDir());
	} else {
		tmpdir=charstring::duplicate(TMP_DIR);
		tmpdirlen=charstring::length(tmpdir);
	}
}

tempdir::~tempdir() {
	delete[] tmpdir;
}

char *tempdir::getString() {
	return tmpdir;
}

int tempdir::getLength() {
	return tmpdirlen;
}
