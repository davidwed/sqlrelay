// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void	sqlrconnection::rollbackCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"rollback");
	#endif
	clientsock->write((unsigned short)rollback());
	commitorrollback=0;
}

int	sqlrconnection::rollback() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"rollback...");
	#endif

	cur[currentcur]->prepareQuery("rollback",8);
	int	retval=cur[currentcur]->executeQuery("rollback",8,1);
	cur[currentcur]->abort();

	#ifdef SERVER_DEBUG
	char	string[38];
	sprintf(string,"rollback result: %d",retval);
	debugPrint("connection",2,string);
	#endif

	return retval;
}
