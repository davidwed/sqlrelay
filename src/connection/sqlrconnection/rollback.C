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

	sqlrcursor	*rollbackcur=initCursor();
	char	*rollbackquery="rollback";
	int	rollbackquerylen=8;
	int	retval=0;
	if (rollbackcur->openCursor(-1) &&
		rollbackcur->prepareQuery(rollbackquery,rollbackquerylen) &&
		rollbackcur->executeQuery(rollbackquery,rollbackquerylen,1)) {
		rollbackcur->cleanUpData(true,true,true);
		retval=1;
	}
	rollbackcur->closeCursor();
	delete rollbackcur;

	#ifdef SERVER_DEBUG
	char	string[38];
	sprintf(string,"rollback result: %d",retval);
	debugPrint("connection",2,string);
	#endif

	return retval;
}
