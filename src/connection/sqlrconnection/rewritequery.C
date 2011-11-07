// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <rudiments/character.h>

enum queryparsestate_t {
	IN_QUERY=0,
	IN_QUOTES,
	BEFORE_BIND,
	IN_BIND
};

void sqlrconnection_svr::rewriteQueryInternal(sqlrcursor_svr *cursor) {

	if (cursor->supportsNativeBinds()) {
		nativizeBindVariables(cursor);
	} else {
		// FIXME: move fake bind code here
	}

	if (supportsBegin()) {
		nativizeBegins(cursor);
	}

	// run database-specific rewrites
	cursor->rewriteQuery();
}

void sqlrconnection_svr::nativizeBindVariables(sqlrcursor_svr *cursor) {

	// debug
	dbgfile.debugPrint("connection",1,"nativizing bind variables...");
	dbgfile.debugPrint("connection",2,"original:");
	dbgfile.debugPrint("connection",2,cursor->querybuffer);
	dbgfile.debugPrint("connection",2,"input binds:");
	if (dbgfile.debugEnabled()) {
		for (uint16_t i=0; i<cursor->inbindcount; i++) {
			dbgfile.debugPrint("connection",3,
					cursor->inbindvars[i].variable);
		}
	}
	dbgfile.debugPrint("connection",2,"output binds:");
	if (dbgfile.debugEnabled()) {
		for (uint16_t i=0; i<cursor->outbindcount; i++) {
			dbgfile.debugPrint("connection",3,
					cursor->outbindvars[i].variable);
		}
	}

	// convert queries from whatever bind variable format they currently
	// use to the format required by the database...

	bool			convert=false;
	queryparsestate_t	parsestate=IN_QUERY;
	stringbuffer	newquery;
	stringbuffer	currentbind;
	const char	*endptr=cursor->querybuffer+cursor->querylength-1;

	// use 1-based index for bind variables
	uint16_t	bindindex=1;
	
	// run through the querybuffer...
	char *c=cursor->querybuffer;
	do {

		// if we're in the query...
		if (parsestate==IN_QUERY) {

			// if we find a quote, we're in quotes
			if (*c=='\'') {
				parsestate=IN_QUOTES;
			}

			// if we find whitespace or a couple of other things
			// then the next thing could be a bind variable
			if (character::isWhitespace(*c) ||
					*c==',' || *c=='(' || *c=='=') {
				parsestate=BEFORE_BIND;
			}

			// append the character
			newquery.append(*c);
			c++;
			continue;
		}

		// copy anything in quotes verbatim
		if (parsestate==IN_QUOTES) {
			if (*c=='\'') {
				parsestate=IN_QUERY;
			}
			newquery.append(*c);
			c++;
			continue;
		}

		if (parsestate==BEFORE_BIND) {

			// if we find a bind variable...
			if (*c=='?' || *c==':' || *c=='@' || *c=='$') {
				parsestate=IN_BIND;
				currentbind.clear();
				continue;
			}

			// if we didn't find a bind variable then we're just
			// back in the query
			parsestate=IN_QUERY;
			continue;
		}

		// if we're in a bind variable...
		if (parsestate==IN_BIND) {

			// If we find whitespace or a few other things
			// then we're done with the bind variable.  Process it.
			// Otherwise get the variable itself in another buffer.
			bool	isspecialchar=(character::isWhitespace(*c) ||
						*c==',' || *c==')' || *c==';');
			if (isspecialchar || c==endptr) {

				// special case if we hit the end of the string
				// an it's not one of the special chars
				if (c==endptr && !isspecialchar) {
					currentbind.append(*c);
					c++;
				}

				// Bail if the current bind variable format
				// matches the db bind format.
				if (matchesNativeBindFormat(
						currentbind.getString())) {
					return;
				}

				// translate...
				convert=true;
				replaceBindVariableInStringAndArray(cursor,
								&currentbind,
								bindindex,
								&newquery);
				bindindex++;

				parsestate=IN_QUERY;

			} else {
				currentbind.append(*c);
				c++;
			}
			continue;
		}

	} while (c<=endptr);

	if (!convert) {
		return;
	}


	// if we made it here then some conversion
	// was done - update the querybuffer...
	const char	*newq=newquery.getString();
	cursor->querylength=newquery.getStringLength();
	if (cursor->querylength>maxquerysize) {
		cursor->querylength=maxquerysize;
	}
	charstring::copy(cursor->querybuffer,newq,cursor->querylength);
	cursor->querybuffer[cursor->querylength]='\0';


	// debug
	dbgfile.debugPrint("connection",2,"converted:");
	dbgfile.debugPrint("connection",2,cursor->querybuffer);
	dbgfile.debugPrint("connection",2,"input binds:");
	if (dbgfile.debugEnabled()) {
		for (uint16_t i=0; i<cursor->inbindcount; i++) {
			dbgfile.debugPrint("connection",3,
					cursor->inbindvars[i].variable);
		}
	}
	dbgfile.debugPrint("connection",2,"output binds:");
	if (dbgfile.debugEnabled()) {
		for (uint16_t i=0; i<cursor->outbindcount; i++) {
			dbgfile.debugPrint("connection",3,
					cursor->outbindvars[i].variable);
		}
	}
}

bool sqlrconnection_svr::matchesNativeBindFormat(const char *bind) {

	const char	*bindformat=bindFormat();
	size_t		bindformatlen=charstring::length(bindformat);

	// the bind variable name matches the format if...
	// * the first character of the bind variable name matches the 
	//   first character of the bind format
	//
	//	and...
	//
	// * the format is just a single character
	// 	or..
	// * the second character of the format is a 1 and the second character
	//   of the bind variable name is a digit
	// 	or..
	// * the second character of the format is a * and the second character
	//   of the bind varaible name is alphanumeric
	return (bind[0]==bindformat[0]  &&
		(bindformatlen==1 ||
		(bindformat[1]=='1' && character::isDigit(bind[1])) ||
		(bindformat[1]=='*' && !character::isAlphanumeric(bind[1]))));
}

void sqlrconnection_svr::replaceBindVariableInStringAndArray(
						sqlrcursor_svr *cursor,
						stringbuffer *currentbind,
						uint16_t bindindex,
						stringbuffer *newquery) {

	const char	*bindformat=bindFormat();
	size_t		bindformatlen=charstring::length(bindformat);

	// append the first character of the bind format to the new query
	newquery->append(bindformat[0]);

	if (bindformatlen==1) {

		// replace bind variable itself with number
		replaceBindVariableInArray(cursor,NULL,bindindex);

	} else if (bindformat[1]=='1' &&
			!charstring::isNumber(currentbind->getString()+1)) {

		// replace bind variable in string with number
		newquery->append(bindindex);

		// replace bind variable itself with number
		replaceBindVariableInArray(cursor,
					currentbind->getString(),
					bindindex);

	} else {

		// if the bind variable contained a name or number then use
		// it, otherwise replace the bind variable in the string and
		// the bind variable itself with a number 
		if (currentbind->getStringLength()>1) {
			newquery->append(currentbind->getString()+1,
					currentbind->getStringLength()-1);
		} else {
			// replace bind variable in string with number
			newquery->append(bindindex);

			// replace bind variable itself with number
			replaceBindVariableInArray(cursor,
						currentbind->getString(),
						bindindex);
		}
	}
}

void sqlrconnection_svr::replaceBindVariableInArray(sqlrcursor_svr *cursor,
						const char *currentbind,
						uint16_t bindindex) {

	// run two passes
	for (uint16_t i=0; i<2; i++) {

		// first pass for input binds, second pass for output binds
		uint16_t	count=(!i)?cursor->inbindcount:
						cursor->outbindcount;
		bindvar_svr	*vars=(!i)?cursor->inbindvars:
						cursor->outbindvars;

		for (uint16_t j=0; j<count; j++) {

			// get the bind var
			bindvar_svr	*b=&(vars[j]);

			// If a bind var name was passed in, look for a bind
			// variable with a matching name.
			// If no name was passed in then the bind vars are
			// numeric; get the variable who's numeric name matches
			// the index passed in.
			if ((currentbind &&
				!charstring::compare(currentbind,
							b->variable)) ||
				(charstring::toInteger((b->variable)+1)==
								bindindex)) {

				// create the new bind var
				// name and get its length
				char		*tempnumber=charstring::
							parseNumber(bindindex);
				uint16_t	tempnumberlen=charstring::
							length(tempnumber);

				// allocate memory for the new name
				b->variable=(char *)bindpool->
							malloc(tempnumberlen+2);

				// replace the existing bind var name and size
				b->variable[0]=bindVariablePrefix();
				charstring::copy(b->variable+1,tempnumber);
				b->variable[tempnumberlen+1]='\0';
				b->variablesize=tempnumberlen+1;

				// clean up
				delete[] tempnumber;
			}
		}
	}
}

void sqlrconnection_svr::nativizeBegins(sqlrcursor_svr *cursor) {

	if (!isBeginQuery(cursor)) {
		return;
	}

	// debug
	dbgfile.debugPrint("connection",1,"nativizing begin query...");
	dbgfile.debugPrint("connection",2,"original:");
	dbgfile.debugPrint("connection",2,cursor->querybuffer);

	// translate query
	const char	*beginquery=beginQuery();
	cursor->querylength=charstring::length(beginquery);
	charstring::copy(cursor->querybuffer,beginquery,cursor->querylength);
	cursor->querybuffer[cursor->querylength]='\0';

	// debug
	dbgfile.debugPrint("connection",2,"converted:");
	dbgfile.debugPrint("connection",2,cursor->querybuffer);
}

const char *sqlrconnection_svr::beginQuery() {
	return "BEGIN";
}
