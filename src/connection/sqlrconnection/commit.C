// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void	sqlrconnection::commitCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"commit");
	#endif
	clientsock->write((unsigned short)commit());
	commitorrollback=0;
}

int	sqlrconnection::commit() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"commit...");
	#endif

	cur[currentcur]->prepareQuery("commit",6);
	int	retval=cur[currentcur]->executeQuery("commit",6,1);
	cur[currentcur]->abort();

	#ifdef SERVER_DEBUG
	char	string[36];
	sprintf(string,"commit result: %d",retval);
	debugPrint("connection",2,string);
	#endif

	return retval;
}
