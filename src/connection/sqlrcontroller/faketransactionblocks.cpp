// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::setFakeTransactionBlocksBehavior(bool ftb) {
	faketransactionblocks=ftb;
}

bool sqlrcontroller_svr::handleFakeTransactionQueries(sqlrcursor_svr *cursor,
						bool *wasfaketransactionquery) {

	*wasfaketransactionquery=false;

	// Intercept begins and handle them.  If we're faking begins, commit
	// and rollback queries also need to be intercepted as well, otherwise
	// the query will be sent directly to the db and endFakeBeginTransaction
	// won't get called.
	if (isBeginTransactionQuery(cursor)) {
		*wasfaketransactionquery=true;
		cursor->inbindcount=0;
		cursor->outbindcount=0;
		sendcolumninfo=DONT_SEND_COLUMN_INFO;
		if (intransactionblock) {
			charstring::copy(cursor->error,
				"begin while in transaction block");
			cursor->errorlength=charstring::length(cursor->error);
			cursor->errnum=999999;
			return false;
		}
		return begin();
		// FIXME: if the begin fails and the db api doesn't support
		// a begin command then the connection-level error needs to
		// be copied to the cursor so handleQueryOrBindCursor can
		// report it
	} else if (isCommitQuery(cursor)) {
		*wasfaketransactionquery=true;
		cursor->inbindcount=0;
		cursor->outbindcount=0;
		sendcolumninfo=DONT_SEND_COLUMN_INFO;
		if (!intransactionblock) {
			charstring::copy(cursor->error,
				"commit while not in transaction block");
			cursor->errorlength=charstring::length(cursor->error);
			cursor->errnum=999998;
			return false;
		}
		return commit();
		// FIXME: if the commit fails and the db api doesn't support
		// a commit command then the connection-level error needs to
		// be copied to the cursor so handleQueryOrBindCursor can
		// report it
	} else if (isRollbackQuery(cursor)) {
		*wasfaketransactionquery=true;
		cursor->inbindcount=0;
		cursor->outbindcount=0;
		sendcolumninfo=DONT_SEND_COLUMN_INFO;
		if (!intransactionblock) {
			charstring::copy(cursor->error,
				"rollback while not in transaction block");
			cursor->errorlength=charstring::length(cursor->error);
			cursor->errnum=999997;
			return false;
		}
		return rollback();
		// FIXME: if the rollback fails and the db api doesn't support
		// a rollback command then the connection-level error needs to
		// be copied to the cursor so handleQueryOrBindCursor can
		// report it
	}
	return false;
}

bool sqlrcontroller_svr::isBeginTransactionQuery(sqlrcursor_svr *cursor) {

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

bool sqlrcontroller_svr::beginFakeTransactionBlock() {

	// save the current autocommit state
	faketransactionblocksautocommiton=autocommitforthissession;

	// if autocommit is on, turn it off
	if (autocommitforthissession) {
		if (!autoCommitOff()) {
			return false;
		}
	}
	intransactionblock=true;
	return true;
}

bool sqlrcontroller_svr::endFakeTransactionBlock() {

	// if we're faking begins and autocommit is on,
	// reset autocommit behavior
	if (faketransactionblocks && faketransactionblocksautocommiton) {
		if (!autoCommitOn()) {
			return false;
		}
	}
	intransactionblock=false;
	return true;
}

bool sqlrcontroller_svr::isCommitQuery(sqlrcursor_svr *cursor) {
	return !charstring::compareIgnoringCase(
			cursor->skipWhitespaceAndComments(
						cursor->querybuffer),
			"commit",6);
}

bool sqlrcontroller_svr::isRollbackQuery(sqlrcursor_svr *cursor) {
	return !charstring::compareIgnoringCase(
			cursor->skipWhitespaceAndComments(
						cursor->querybuffer),
			"rollback",8);
}
