// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>

bool sqlrcursor::executeQuery() {

	if (!queryptr) {
		setError("No query to execute.");
		return false;
	}

	// a useful variable
	bool	retval=false;

	if (!subcount) {

		// validate the bind variables
		if (validatebinds) {
			validateBindsInternal(queryptr);
		}
		
		// run the query
		retval=runQuery(queryptr);

	} else {

		// perform substitutions
		stringbuffer	container;
		char		*ptr=queryptr;
		bool		found=false;
		bool		inquotes=false;
		bool		inbraces=false;
		int		len=0;
		stringbuffer	*braces;

		// iterate through the string
		while (*ptr) {
		
			// figure out whether we're inside a quoted 
			// string or not
			if (*ptr=='\'' && *(ptr-1)!='\\') {
				if (inquotes) {
					inquotes=false;
				} else {
					inquotes=true;
				}
			}
		
			// if we find an open-brace then start 
			// sending to a new buffer
			if (*ptr=='[' && !inbraces && !inquotes) {
				braces=new stringbuffer();
				inbraces=true;
				ptr++;
			}
		
			// if we find a close-brace then process 
			// the brace buffer
			if (*ptr==']' && inbraces && !inquotes) {
		
				// look for an = sign, skipping whitespace
				const char	*bptr=braces->getString();
				while (*bptr && (*bptr==' ' || 
					*bptr=='	' || *bptr=='\n')) {
					bptr++;
				}
		
				if (*bptr=='=') {
					// if we find an equals sign first 
					// then process the rest of the buffer
					bptr++;
		
					// skip whitespace
					while (*bptr && (*bptr==' ' || 
						*bptr=='	' || 
					 	*bptr=='\n')) {
						bptr++;
					}
		
					// if the remaining contents of the 
					// buffer are '' or nothing then we 
					// must have an ='' or just an = with 
					// some whitespace, replace this
					// with "is NULL" otherwise, just write
					// out the contents of the buffer
					if (!bptr || 
						(bptr &&
						!charstring::compare(bptr,
								"''"))) {
						container.append(" is NULL ");
					} else {
						container.append(
							braces->getString());
					}
				} else {
					// if we don't find an equals sign, 
					// then write the contents out directly
					container.append(braces->getString());
				}
				delete braces;
				inbraces=false;
				ptr++;
			}
		
			// if we encounter $(....) then replace the 
			// variable within
			if ((*ptr)=='$' && (*(ptr+1))=='(') {
		
				// first iterate through the arrays passed in
				found=false;
				for (int i=0; i<subcount && !found; i++) {
		
					// if we find a match, write the 
					// value to the container and skip 
					// past the $(variable)
					len=charstring::length(
							subvars[i].variable);
					if (!charstring::compare((ptr+2),
						subvars[i].variable,len) &&
						(*(ptr+2+len))==')') {
		
						if (inbraces) {
							performSubstitution(
								braces,i);
						} else {
							performSubstitution(
								&container,i);
						}
						ptr=ptr+3+len;
						found=true;
					}
				}
		
				// if the variable wasn't found, then 
				// just write the $(
				if (!found) {
					if (inbraces) {
						braces->append("$(");
					} else {
						container.append("$(");
					}
					ptr=ptr+2;
				}
		
			} else {
		
				// print out the current character and proceed
				if (inbraces) {
					braces->append(*ptr);
				} else {
					container.append(*ptr);
				}
				ptr++;
			}
		}

		// validate the bind variables
		if (validatebinds) {
			validateBindsInternal(container.getString());
		}

		// run the query
		querylen=charstring::length(container.getString());
		retval=runQuery(container.getString());
	}

	// set up to re-execute the same query if executeQuery is called
	// again before calling prepareQuery
	reexecute=true;

	return retval;
}

void sqlrcursor::validateBindsInternal(const char *query) {

	// some useful variables
	char	*ptr;
	char	*start;
	char	*after;
	bool	found;
	int	len;
	unsigned short	count;

	// check each input bind
	count=inbindcount;
	for (unsigned short i=0; i<count; i++) {

		// don't check bind-by-position variables
		len=charstring::length(inbindvars[i].variable);
		if (charstring::isInteger(inbindvars[i].variable,len)) {
			continue;
		}

		found=false;
		start=((char *)query)+1;

		// there may be more than 1 match for the variable name as in
		// "select * from table where table_name=:table_name", both
		// table_name's would match, but only the second is a bind
		// variable
		while ((ptr=charstring::findFirst(start,
					inbindvars[i].variable))) {

			// for a match to be a bind variable, it must be 
			// preceded by a colon and can't be followed by an
			// alphabet character, number or underscore
			after=ptr+len;
			if ((*(ptr-1)==':' || *(ptr-1)=='@') && *after!='_' &&
				!(*(after)>='a' && *(after)<='z') &&
				!(*(after)>='A' && *(after)<='Z') &&
				!(*(after)>='0' && *(after)<='9')) {
				found=true;
				break;
			} else {
				// jump past this instance to look for the
				// next one
				start=ptr+len;
			}
		}

		if (!found) {
			inbindvars[i].send=0;
			inbindcount--;
		}
	}

	// check each output bind
	count=outbindcount;
	for (unsigned short i=0; i<count; i++) {

		// don't check bind-by-position variables
		len=charstring::length(outbindvars[i].variable);
		if (charstring::isInteger(outbindvars[i].variable,len)) {
			continue;
		}

		found=false;
		start=((char *)query)+1;

		// there may be more than 1 match for the variable name as in
		// "select * from table where table_name=:table_name", both
		// table_name's would match, but only 1 is correct
		while ((ptr=charstring::findFirst(start,
					outbindvars[i].variable))) {

			// for a match to be a bind variable, it must be 
			// preceded by a colon and can't be followed by an
			// alphabet character, number or underscore
			after=ptr+len;
			if (*(ptr-1)==':' && *after!='_' &&
				!(*(after)>='a' && *(after)<='z') &&
				!(*(after)>='A' && *(after)<='Z') &&
				!(*(after)>='0' && *(after)<='9')) {
				found=true;
				break;
			} else {
				// jump past this instance to look for the
				// next one
				start=ptr+len;
			}
		}

		if (!found) {
			outbindvars[i].send=0;
			outbindcount--;
		}
	}
}

void sqlrcursor::performSubstitution(stringbuffer *buffer, int which) {

	if (subvars[which].type==STRING_BIND) {
		buffer->append(subvars[which].value.stringval);
	} else if (subvars[which].type==LONG_BIND) {
		buffer->append(subvars[which].value.longval);
	} else if (subvars[which].type==DOUBLE_BIND) {
		buffer->append(subvars[which].value.doubleval.value,
				subvars[which].value.doubleval.precision,
				subvars[which].value.doubleval.scale);
	}
}
