// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::commitCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"commit");
	#endif
	clientsock->write(commit());
	commitorrollback=false;
}

bool sqlrconnection::commit() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"commit...");
	#endif

	sqlrcursor	*commitcur=initCursor();
	char	*commitquery="commit";
	int	commitquerylen=6;
	bool	retval=false;
	if (commitcur->openCursor(-1) &&
		commitcur->prepareQuery(commitquery,commitquerylen)) {
		retval=commitcur->executeQuery(commitquery,commitquerylen,true);
	}
	commitcur->cleanUpData(true,true);
	commitcur->closeCursor();
	delete commitcur;

	#ifdef SERVER_DEBUG
	char	string[36];
	sprintf(string,"commit result: %d",retval);
	debugPrint("connection",2,string);
	#endif

	return retval;
}
