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

	if (isBeginQuery(cursor)) {
		fakeBegin();
		cursor->inbindcount=0;
		cursor->outbindcount=0;
		sendcolumninfo=DONT_SEND_COLUMN_INFO;
		return true;
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

		// make sure there are only spaces or comments after the begin
		const char	*spaceptr=
				cursor->skipWhitespaceAndComments(ptr+5);
		if (*spaceptr=='\0') {
			return true;
		}
		return false;

	} else if (!charstring::compareIgnoringCase(ptr,"START ",6) ||
		!charstring::compareIgnoringCase(ptr,"SET TRANSACTION",15)) {
		return true;
	}
	return false;
}

bool sqlrconnection_svr::fakeBegin() {
printf("faking begin\n");

	// save the current autocommit state
	fakebeginsautocommiton=autocommit;

	// if autocommit is on, turn it off
	if (autocommit) {
		return autoCommitOffInternal();
	}
	return true;
}

bool sqlrconnection_svr::endFakeBegin() {
printf("end fake begin\n");

	// if we're faking begins and autocommit is on,
	// reset autocommit behavior
	if (fakebegins && fakebeginsautocommiton) {
printf("resetting autocommit on\n");
		return autoCommitOnInternal();
	} else if (fakebegins && !fakebeginsautocommiton) {
printf("resetting autocommit off\n");
	}
	return true;
}
