// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void	sqlrconnection::pingCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"ping");
	#endif
	clientsock->write((unsigned short)ping());
}

int	sqlrconnection::ping() {
	sqlrcursor	*pingcur=initCursor();
	char	*pingquery=pingQuery();
	int	pingquerylen=strlen(pingQuery());
	if (pingcur->openCursor(-1) &&
		pingcur->prepareQuery(pingquery,pingquerylen) &&
		pingcur->executeQuery(pingquery,pingquerylen,1)) {
		pingcur->cleanUpData(true,true,false);
		pingcur->closeCursor();
		delete pingcur;
		return 1;
	}
	pingcur->closeCursor();
	delete pingcur;
	return 0;
}

char	*sqlrconnection::pingQuery() {
	return "select 1";
}
