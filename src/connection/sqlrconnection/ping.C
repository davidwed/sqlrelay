// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::pingCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"ping");
	#endif
	clientsock->write(ping());
}

bool sqlrconnection::ping() {
	sqlrcursor	*pingcur=initCursor();
	char	*pingquery=pingQuery();
	int	pingquerylen=strlen(pingQuery());
	if (pingcur->openCursor(-1) &&
		pingcur->prepareQuery(pingquery,pingquerylen) &&
		pingcur->executeQuery(pingquery,pingquerylen,true)) {
		pingcur->cleanUpData(true,true);
		pingcur->closeCursor();
		delete pingcur;
		return true;
	}
	pingcur->closeCursor();
	delete pingcur;
	return false;
}

char *sqlrconnection::pingQuery() {
	return "select 1";
}
