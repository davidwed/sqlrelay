// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::setIsolationLevel(const char *isolevel) {

	// if no isolation level was passed in then bail
	if (!charstring::length(isolevel)) {
		return false;
	}

	// get the set isolation level query base
	const char	*silquerybase=setIsolationLevelQuery();

	// If there is no query for this then the db we're using doesn't
	// support switching.  Return true as if it succeeded though.
	if (!charstring::length(silquerybase)) {
		return true;
	}

	// bounds checking
	size_t		silquerylen=charstring::length(silquerybase)+
					charstring::length(isolevel)+1;
	if (silquerylen>maxquerysize) {
		dbgfile.debugPrint("connection",2,
			"get list failed: client sent bad db length");
		return false;
	}

	// create the set isolation level query
	char	*silquery=new char[silquerylen];
	snprintf(silquery,silquerylen,silquerybase,isolevel);
	silquerylen=charstring::length(silquery);

	sqlrcursor_svr	*silcur=initCursorUpdateStats();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	bool	retval=false;
	if (silcur->openCursorInternal(cursorcount+1) &&
		silcur->prepareQuery(silquery,silquerylen) &&
		executeQueryUpdateStats(silcur,silquery,silquerylen,true)) {
		retval=true;
	}

	// FIXME: we don't really need to do this now but we will
	// later if we ever add an API call to set the isolation level
	/* else {
		bool	liveconnection;
		*error=charstring::duplicate(
				silcur->errorMessage(&liveconnection));
	} */

	delete[] silquery;
	silcur->cleanUpData(true,true);
	silcur->closeCursor();
	deleteCursorUpdateStats(silcur);
	return retval;
}

const char *sqlrconnection_svr::setIsolationLevelQuery() {
	return "set transaction isolation level %s";
}

const char *sqlrconnection_svr::getDefaultIsolationLevel() {
	return "read committed";
}
