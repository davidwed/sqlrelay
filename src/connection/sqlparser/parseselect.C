// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqlelement.h>
#include <sqltranslatordebug.h>

bool sqlparser::parseSelect(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	*newptr=ptr;

	// look for a select clause
	if (!selectClause(*newptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*selectnode=
			newNode(currentnode,sqlelement::_select);

	// options (unique, distinct, etc.)
	parseSelectOptions(selectnode,*newptr,newptr);

	// FIXME: implement this for real
	parseRemainderVerbatim(selectnode,*newptr,newptr);
	return true;
}

bool sqlparser::parseSelectOptions(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create the node
	xmldomnode	*optionsnode=newNode(currentnode,sqlelement::_options);
/*
	// find the from clause (or end of string)
	const char	*from=charstring::findFirstOrEnd(ptr," from ");

	// scan back until we find the beginning of the expressions
	bool	inquotes=false;
	bool	indoublequotes=false;
	bool	parenlevel=0;
	for (const char *chr=from; chr>=ptr; chr--) {
		if (!indoublequotes && *chr=='\'') {
			inquotes=!inquotes;
		}
		if (!inquotes && *chr=='"') {
			indoublequotes=!indoublequotes;
		}
		if (*chr==')') {
			inparens=true;
		}
		if (inparens && *chr=='(') {
			inparens=false;
		}
	}*/

	// options
	*newptr=ptr;
	for (;;) {

		// look for known options
		if (parseUnique(optionsnode,*newptr,newptr) ||
			parseDistinct(optionsnode,*newptr,newptr)) {
			continue;
		}

		// If we didn't encounter one of the known options
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand.
		if (parseVerbatim(optionsnode,*newptr,newptr)) {
			space(*newptr,newptr);
		} else {
			break;
		}
	}

	return true;
}

bool sqlparser::parseUnique(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!uniqueClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_unique);
	return true;
}

bool sqlparser::parseDistinct(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!distinctClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_distinct);
	return true;
}
