// Copyright (c) 1999-2010  David Muse
// See the file COPYING for more information

#include <db2connection.h>

int main(int argc, const char **argv) {
	return sqlrconnection_svr::main(argc,argv,new db2connection());
}
