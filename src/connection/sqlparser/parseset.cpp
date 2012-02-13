// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>

bool sqlparser::parseSet(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a set clause
	if (!setClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*setnode=newNode(currentnode,_set);

	// global
	parseSetGlobal(setnode,*newptr,newptr);

	// session
	parseSetSession(setnode,*newptr,newptr);

	// look for known options
	if (parseTransaction(setnode,*newptr,newptr)) {
		return true;
	}

	// for now we only support transactions
	parseRemainderVerbatim(setnode,*newptr,newptr);
	return true;
}

bool sqlparser::setClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"set ");
}

const char *sqlparser::_set="set";

bool sqlparser::parseSetGlobal(xmldomnode *currentnode,
				const char *ptr,
				const char **newptr) {
	if (!setGlobalClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_set_global);
	return true;
}

bool sqlparser::setGlobalClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"global ");
}

const char *sqlparser::_set_global="set_global";

bool sqlparser::parseSetSession(xmldomnode *currentnode,
				const char *ptr,
				const char **newptr) {
	if (!setSessionClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_set_session);
	return true;
}

bool sqlparser::setSessionClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"session ");
}

const char *sqlparser::_set_session="set_session";

bool sqlparser::parseTransaction(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// transaction
	if (!transactionClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*txnode=newNode(currentnode,_transaction);

	// parse the remaining clauses
	for (;;) {

		// known clauses
		if (parseIsolationLevel(txnode,*newptr,newptr)) {
			continue;
		}

		// If we didn't encounter one of the known clauses
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand or until we hit the
		// end.
		if (!parseVerbatim(txnode,*newptr,newptr)) {
			return true;
		}
	}
	return true;
}

bool sqlparser::transactionClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"transaction ");
}

const char *sqlparser::_transaction="transaction";

bool sqlparser::parseIsolationLevel(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// isolation level
	if (!isolationLevelClause(ptr,newptr)) {
		return false;
	}
	
	// create the node
	xmldomnode	*isonode=newNode(currentnode,_isolation_level);

	// value
	const char	*startptr=*newptr;
	if (isolationLevelOptionClause(startptr,newptr)) {
		char	*value=getClause(startptr,*newptr);
		setAttribute(isonode,_value,value);
		delete[] value;
		return true;
	}

	parseRemainderVerbatim(isonode,*newptr,newptr);
	return true;
}

bool sqlparser::isolationLevelClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"isolation level ");
}

const char *sqlparser::_isolation_level="isolation_level";

bool sqlparser::isolationLevelOptionClause(const char *ptr,
						const char **newptr) {
	debugFunction();
	const char *parts[]={
		// standard SQL
		"read uncommitted",
		"read committed",
		"repeatable read",
		"serializable",
		// DB2
		"cursor stability","cs",
		"repeatable read","rr",
		"read stability","rs",
		"uncomitted read","ur"
		// Sybase
		"0", // read uncommited
		"1", // read committed
		"2", // repeatable read
		"3", // serializable
		// Firebird
		"SNAPSHOT",
		"SNAPSHOT TABLE STABILITY",
		"READ COMMITTED NO RECORD_VERSION",
		"READ COMMITTED RECORD VERSION",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}
