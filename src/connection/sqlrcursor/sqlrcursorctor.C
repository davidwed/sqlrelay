// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

sqlrcursor::sqlrcursor(sqlrconnection *conn) {

	this->conn=conn;
	inbindcount=0;
	outbindcount=0;
	busy=false;

	createtemplower.compile("(create|declare)[ \\t\\r\\n]+((global|local)?[ \\t\\r\\n]+)?temp(orary)?[ \\t\\r\\n]+table[ \\t\\r\\n]+");
	createtempupper.compile("(CREATE|declare)[ \\t\\r\\n]+((GLOBAL|LOCAL)?[ \\t\\r\\n]+)?TEMP(ORARY)?[ \\t\\r\\n]+TABLE[ \\t\\r\\n]+");
}
