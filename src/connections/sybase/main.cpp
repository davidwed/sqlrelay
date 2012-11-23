// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sybaseconnection.h>

int main(int argc, const char **argv) {
	sqlrcontroller_svr::main(argc,argv,
				new sybaseconnection(new sqlrcontroller_svr()));
}
