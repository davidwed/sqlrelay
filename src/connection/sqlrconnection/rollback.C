// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::rollbackCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"rollback");
	#endif
	clientsock->write(rollback());
	flushWriteBuffer();
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
	if (rollbackcur->openCursor(0) &&
		rollbackcur->prepareQuery(rollbackquery,rollbackquerylen)) {
		retval=rollbackcur->executeQuery(rollbackquery,
						rollbackquerylen,true);
	}
	rollbackcur->cleanUpData(true,true);
	rollbackcur->closeCursor();
	deleteCursor(rollbackcur);

	#ifdef SERVER_DEBUG
	char	string[38];
	snprintf(string,38,"rollback result: %d",retval);
	debugPrint("connection",2,string);
	#endif

	return retval;
}
