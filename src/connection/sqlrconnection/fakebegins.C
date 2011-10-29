// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::setFakeBeginBehavior(bool fb) {
	fakebegins=true;
}

bool sqlrconnection_svr::supportsBegin() {
	return true;
}

bool sqlrconnection_svr::handleFakeBegin(sqlrcursor_svr *cursor) {

	// just return if we're not faking begins
	if (!fakebegins) {
		return false;
	}

	// Intercept begins and handle them.  If we're faking begins, commit
	// and rollback queries also need to be intercepted as well, otherwise
	// the query will be sent directly to the db and endFakeBegin won't get
	// called.
	if (isBeginQuery(cursor)) {
		fakeBegin();
		cursor->inbindcount=0;
		cursor->outbindcount=0;
		sendcolumninfo=DONT_SEND_COLUMN_INFO;
		return true;
	} else if (isCommitQuery(cursor)) {
		cursor->inbindcount=0;
		cursor->outbindcount=0;
		sendcolumninfo=DONT_SEND_COLUMN_INFO;
		return commitInternal();
	} else if (isRollbackQuery(cursor)) {
		cursor->inbindcount=0;
		cursor->outbindcount=0;
		sendcolumninfo=DONT_SEND_COLUMN_INFO;
		return rollbackInternal();
	}
	return false;
}

bool sqlrconnection_svr::isBeginQuery(sqlrcursor_svr *cursor) {

	// find the start of the actual query
	const char	*ptr=cursor->skipWhitespaceAndComments(
						cursor->querybuffer);

	// See if it was any of the different queries used to start a
	// transaction.  IMPORTANT: don't just look for the first 5 characters
	// to be "BEGIN", make sure it's the entire query.  Many db's use
	// "BEGIN" to start a stored procedure block, but in those cases,
	// something will follow it.
	if (!charstring::compareIgnoringCase(ptr,"BEGIN",5)) {

		// make sure there are only spaces, comments or the word "work"
		// after the begin
		const char	*spaceptr=
				cursor->skipWhitespaceAndComments(ptr+5);
		
		if (!charstring::compareIgnoringCase(spaceptr,"WORK",4) ||
			*spaceptr=='\0') {
			return true;
		}
		return false;

	} else if (!charstring::compareIgnoringCase(ptr,"START ",6)) {
		return true;
	}
	return false;
}

bool sqlrconnection_svr::fakeBegin() {

	// save the current autocommit state
	fakebeginsautocommiton=autocommit;

	// if autocommit is on, turn it off
	if (autocommit) {
		return autoCommitOffInternal();
	}
	return true;
}

bool sqlrconnection_svr::endFakeBegin() {

	// if we're faking begins and autocommit is on,
	// reset autocommit behavior
	if (fakebegins && fakebeginsautocommiton) {
		return autoCommitOnInternal();
	} else if (fakebegins && !fakebeginsautocommiton) {
	}
	return true;
}

bool sqlrconnection_svr::isCommitQuery(sqlrcursor_svr *cursor) {
	return !charstring::compareIgnoringCase(
			cursor->skipWhitespaceAndComments(
						cursor->querybuffer),
			"commit",6);
}

bool sqlrconnection_svr::isRollbackQuery(sqlrcursor_svr *cursor) {
	return !charstring::compareIgnoringCase(
			cursor->skipWhitespaceAndComments(
						cursor->querybuffer),
			"rollback",8);
}
