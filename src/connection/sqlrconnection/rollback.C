// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::rollbackCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"rollback");
	#endif
	clientsock->write(rollback());
	commitorrollback=false;
}

bool sqlrconnection::rollback() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"rollback...");
	#endif

	sqlrcursor	*rollbackcur=initCursor();
	char	*rollbackquery="rollback";
	int	rollbackquerylen=8;
	bool	retval=false;
	if (rollbackcur->openCursor(-1) &&
		rollbackcur->prepareQuery(rollbackquery,rollbackquerylen)) {
		retval=rollbackcur->executeQuery(rollbackquery,
						rollbackquerylen,true);
	}
	rollbackcur->cleanUpData(false,false,false);
	rollbackcur->closeCursor();
	delete rollbackcur;

	#ifdef SERVER_DEBUG
	char	string[38];
	sprintf(string,"rollback result: %d",retval);
	debugPrint("connection",2,string);
	#endif

	return retval;
}
