// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int	sqlrconnection::returnResultSetData() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"returning result set data...");
	#endif

	// see if this result set even has any rows to return
	int	norows=cur[currentcur]->noRowsToReturn();

	// get the number of rows to skip
	unsigned long	skip;
	if (clientsock->read(&skip)!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"returning result set data failed");
		#endif
		return 0;
	}

	// get the number of rows to fetch
	unsigned long	fetch;
	if (clientsock->read(&fetch)!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"returning result set data failed");
		#endif
		return 0;
	}


	// for some queries, there are no rows to return, 
	if (norows) {
		clientsock->write((unsigned short)END_RESULT_SET);
		cur[currentcur]->abort();
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"done returning result set data");
		#endif
		return 1;
	}


	// reinit suspendresultset
	cur[currentcur]->suspendresultset=0;


	// skip the specified number of rows
	if (!skipRows(skip)) {
		clientsock->write((unsigned short)END_RESULT_SET);
		cur[currentcur]->abort();
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"done returning result set data");
		#endif
		return 1;
	}


	#ifdef SERVER_DEBUG
	debugstr=new stringbuffer();
	debugstr->append("fetching ");
	debugstr->append((long)fetch);
	debugstr->append(" rows...");
	debugPrint("connection",2,debugstr->getString());
	delete debugstr;
	#endif

	// send the specified number of rows back
	for (unsigned long i=0; (!fetch || i<fetch); i++) {

		if (!cur[currentcur]->fetchRow()) {
			clientsock->write((unsigned short)END_RESULT_SET);
			cur[currentcur]->abort();
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"done returning result set data");
			#endif
			return 1;
		}

		#ifdef SERVER_DEBUG
		debugstr=new stringbuffer();
		#endif
		cur[currentcur]->returnRow();
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,debugstr->getString());
		delete debugstr;
		#endif

		lastrow++;
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done returning result set data");
	#endif
	return 1;
}
