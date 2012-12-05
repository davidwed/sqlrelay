// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <rudiments/character.h>

enum queryparsestate_t {
	IN_QUERY=0,
	IN_QUOTES,
	BEFORE_BIND,
	IN_BIND
};

void sqlrcontroller_svr::rewriteQuery(sqlrcursor_svr *cursor) {

	if (sqlp && sqlt && sqlw) {
		if (!translateQuery(cursor)) {
			// FIXME: do something?
		}
	}

	if (translatebinds) {
		translateBindVariables(cursor);
	}

	if (conn->supportsTransactionBlocks()) {
		translateBeginTransaction(cursor);
	}
}

bool sqlrcontroller_svr::translateQuery(sqlrcursor_svr *cursor) {

	if (debugsqltranslation) {
		printf("original:\n\"%s\"\n\n",cursor->querybuffer);
	}

	// parse the query
	bool	parsed=sqlp->parse(cursor->querybuffer);

	// get the parsed tree
	delete cursor->querytree;
	cursor->querytree=sqlp->detachTree();
	if (!cursor->querytree) {
		return false;
	}

	if (debugsqltranslation) {
		printf("before translation:\n");
		xmldomnode::print(cursor->querytree->getRootNode());
		printf("\n");
	}

	if (!parsed) {
		if (debugsqltranslation) {
			printf("parse failed, using original:\n\"%s\"\n\n",
							cursor->querybuffer);
		}
		delete cursor->querytree;
		cursor->querytree=NULL;
		return false;
	}

	// apply translation rules
	if (!sqlt->runTranslations(conn,cursor,cursor->querytree)) {
		return false;
	}

	if (debugsqltranslation) {
		printf("after translation:\n");
		xmldomnode::print(cursor->querytree->getRootNode());
		printf("\n");
	}

	// write the query back out
	stringbuffer	translatedquery;
	if (!sqlw->write(conn,cursor,cursor->querytree,&translatedquery)) {
		return false;
	}

	if (debugsqltranslation) {
		printf("translated:\n\"%s\"\n\n",
				translatedquery.getString());
	}

	// copy the translated query into query buffer
	if (translatedquery.getStringLength()>maxquerysize) {
		// the translated query was too large
		return false;
	}
	charstring::copy(cursor->querybuffer,
			translatedquery.getString(),
			translatedquery.getStringLength());
	cursor->querylength=translatedquery.getStringLength();
	cursor->querybuffer[cursor->querylength]='\0';
	return true;
}

void sqlrcontroller_svr::translateBindVariables(sqlrcursor_svr *cursor) {

	// debug
	dbgfile.debugPrint("connection",1,"translating bind variables...");
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
			// (make sure to catch @'s but not @@'s
			if (*c=='?' || *c==':' ||
				(*c=='@' && *(c+1)!='@') || *c=='$') {
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
				translateBindVariableInStringAndArray(cursor,
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
	if (debugsqltranslation) {
		printf("bind translation:\n\"%s\"\n",cursor->querybuffer);
		for (uint16_t i=0; i<cursor->inbindcount; i++) {
			printf("  inbind: \"%s\"\n",
					cursor->inbindvars[i].variable);
		}
		for (uint16_t i=0; i<cursor->outbindcount; i++) {
			printf("  outbind: \"%s\"\n",
					cursor->outbindvars[i].variable);
		}
		printf("\n");
	}
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

bool sqlrcontroller_svr::matchesNativeBindFormat(const char *bind) {

	const char	*bindformat=conn->bindFormat();
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

void sqlrcontroller_svr::translateBindVariableInStringAndArray(
						sqlrcursor_svr *cursor,
						stringbuffer *currentbind,
						uint16_t bindindex,
						stringbuffer *newquery) {

	const char	*bindformat=conn->bindFormat();
	size_t		bindformatlen=charstring::length(bindformat);

	// append the first character of the bind format to the new query
	newquery->append(bindformat[0]);

	if (bindformatlen==1) {

		// replace bind variable itself with number
		translateBindVariableInArray(cursor,NULL,bindindex);

	} else if (bindformat[1]=='1' &&
			!charstring::isNumber(currentbind->getString()+1)) {

		// replace bind variable in string with number
		newquery->append(bindindex);

		// replace bind variable itself with number
		translateBindVariableInArray(cursor,
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
			translateBindVariableInArray(cursor,
						currentbind->getString(),
						bindindex);
		}
	}
}

void sqlrcontroller_svr::translateBindVariableInArray(sqlrcursor_svr *cursor,
						const char *currentbind,
						uint16_t bindindex) {

	// run two passes
	for (uint16_t i=0; i<2; i++) {

		// first pass for input binds, second pass for output binds
		uint16_t	count=(!i)?cursor->inbindcount:
						cursor->outbindcount;
		bindvar_svr	*vars=(!i)?cursor->inbindvars:
						cursor->outbindvars;
		namevaluepairs	*mappings=(!i)?inbindmappings:outbindmappings;

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

				// keep track of the old name
				char	*oldvariable=b->variable;

				// allocate memory for the new name
				b->variable=(char *)bindmappingspool->
							malloc(tempnumberlen+2);

				// replace the existing bind var name and size
				b->variable[0]=conn->bindVariablePrefix();
				charstring::copy(b->variable+1,tempnumber);
				b->variable[tempnumberlen+1]='\0';
				b->variablesize=tempnumberlen+1;

				// create bind variable mappings
				mappings->setData(oldvariable,b->variable);
				
				// clean up
				delete[] tempnumber;
			}
		}
	}
}

void sqlrcontroller_svr::translateBindVariablesFromMappings(
						sqlrcursor_svr *cursor) {

	// run two passes
	for (uint16_t i=0; i<2; i++) {

		// first pass for input binds, second pass for output binds
		uint16_t	count=(!i)?cursor->inbindcount:
						cursor->outbindcount;
		bindvar_svr	*vars=(!i)?cursor->inbindvars:
						cursor->outbindvars;
		namevaluepairs	*mappings=(!i)?inbindmappings:outbindmappings;

		for (uint16_t j=0; j<count; j++) {

			// get the bind var
			bindvar_svr	*b=&(vars[j]);

			// remap it
			char	*newvariable;
			if (mappings->getData(b->variable,&newvariable)) {
				b->variable=newvariable;
			}
		}
	}

	// debug
	dbgfile.debugPrint("connection",2,"remapped input binds:");
	if (dbgfile.debugEnabled()) {
		for (uint16_t i=0; i<cursor->inbindcount; i++) {
			dbgfile.debugPrint("connection",3,
					cursor->inbindvars[i].variable);
		}
	}
	dbgfile.debugPrint("connection",2,"remapped output binds:");
	if (dbgfile.debugEnabled()) {
		for (uint16_t i=0; i<cursor->outbindcount; i++) {
			dbgfile.debugPrint("connection",3,
					cursor->outbindvars[i].variable);
		}
	}
}

void sqlrcontroller_svr::translateBeginTransaction(sqlrcursor_svr *cursor) {

	if (!isBeginTransactionQuery(cursor)) {
		return;
	}

	// debug
	dbgfile.debugPrint("connection",1,"translating begin tx query...");
	dbgfile.debugPrint("connection",2,"original:");
	dbgfile.debugPrint("connection",2,cursor->querybuffer);

	// translate query
	const char	*beginquery=conn->beginTransactionQuery();
	cursor->querylength=charstring::length(beginquery);
	charstring::copy(cursor->querybuffer,beginquery,cursor->querylength);
	cursor->querybuffer[cursor->querylength]='\0';

	// debug
	dbgfile.debugPrint("connection",2,"converted:");
	dbgfile.debugPrint("connection",2,cursor->querybuffer);
}

bool sqlrcontroller_svr::getColumnNames(const char *query,
					stringbuffer *output) {

	// sanity check on the query
	if (!query) {
		return false;
	}

	size_t		querylen=charstring::length(query);

	sqlrcursor_svr	*gcncur=initCursor();
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	bool	retval=false;
	if (gcncur->openInternal(cursorcount+1) &&
		gcncur->prepareQuery(query,querylen) &&
		executeQuery(gcncur,query,querylen)) {

		// build column list...
		retval=gcncur->getColumnNameList(output);

	}
	gcncur->cleanUpData();
	gcncur->close();
	deleteCursor(gcncur);
	return retval;
}
