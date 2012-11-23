// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <oracle8connection.h>

int main(int argc, const char **argv) {
	sqlrcontroller_svr::main(argc,argv,
			new oracle8connection(new sqlrcontroller_svr()));
}
