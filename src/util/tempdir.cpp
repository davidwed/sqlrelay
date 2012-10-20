// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <rudiments/charstring.h>
#include <tempdir.h>
#include <stdio.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

tempdir::tempdir(cmdline *cmdl) {
	if (cmdl->getLocalStateDir()[0]) {
		tmpdirlen=charstring::length(cmdl->getLocalStateDir())+13;
		tmpdir=new char[tmpdirlen+1];
		charstring::copy(tmpdir,cmdl->getLocalStateDir());
		charstring::append(tmpdir,"/sqlrelay/tmp");
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

int32_t tempdir::getLength() {
	return tmpdirlen;
}
