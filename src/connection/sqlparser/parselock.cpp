// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>

bool sqlparser::parseLock(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a lock clause
	if (!lockClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*locknode=newNode(currentnode,_lock);

	// table...
	if (!tableClause(*newptr,newptr)) {
		debugPrintf("missing table clause\n");
		error=true;
		return false;
	}

	// table node
	xmldomnode	*tablenode=newNode(locknode,_table);

	// table name
	if (!parseTableName(tablenode,*newptr,newptr)) {
		debugPrintf("missing table name\n");
		error=true;
		return false;
	}

	// in
	if (!inClause(*newptr,newptr)) {
		debugPrintf("missing in clause\n");
		error=true;
		return false;
	}
	newNode(tablenode,_in_mode);

	// lock mode
	if (!parseLockMode(locknode,*newptr,newptr)) {
		debugPrintf("invalid lock mode\n");
		error=true;
		return false;
	}

	// mode
	if (!parseMode(locknode,*newptr,newptr)) {
		debugPrintf("missing mode clause\n");
		error=true;
		return false;
	}

	// nowait
	parseNoWait(locknode,*newptr,newptr);

	parseRemainderVerbatim(locknode,*newptr,newptr);
	return true;
}

bool sqlparser::lockClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"lock ");
}

const char *sqlparser::_lock="lock";

const char *sqlparser::_in_mode="in_mode";

bool sqlparser::parseLockMode(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// lock mode
	if (!lockModeClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*lockmodenode=newNode(currentnode,_lock_mode);

	// value
	char	*value=getClause(ptr,*newptr);
	setAttribute(lockmodenode,_value,value);
	delete[] value;
	return true;
}

bool sqlparser::lockModeClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char	*parts[]={
		"row share",
		"row exclusive",
		"share update",
		"share",
		"share row",
		"exclusive",
		"or exclusive",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlparser::_lock_mode="lock_mode";

bool sqlparser::parseMode(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!modeClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_mode);
	return true;
}

bool sqlparser::modeClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"mode");
}

const char *sqlparser::_mode="mode";

bool sqlparser::parseNoWait(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!noWaitClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_nowait);
	return true;
}

bool sqlparser::noWaitClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"nowait");
}

const char *sqlparser::_nowait="nowait";
