// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

sqlrcursor::sqlrcursor(sqlrconnection *conn) {
	this->conn=conn;
	inbindcount=0;
	outbindcount=0;
	busy=0;
}
