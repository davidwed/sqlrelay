// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrconnection.h>

#include <stdlib.h>

#include <math.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif

sqlrcursor::sqlrcursor(sqlrconnection *conn) {
	this->conn=conn;
	inbindcount=0;
	outbindcount=0;
	busy=0;
}

sqlrcursor::~sqlrcursor() {}

int	sqlrcursor::openCursor(int id) {
	// by default do nothing
	return 1;
}

int	sqlrcursor::closeCursor() {
	// by default do nothing
	return 1;
}

int	sqlrcursor::prepareQuery(const char *query, long querylength) {
	// by default, do nothing...
	return 1;
}

void	sqlrcursor::checkForTempTable(const char *query, unsigned long length) {

	char	*ptr=(char *)query;
	char	*endptr=(char *)query+length;

	// skip any leading comments
	if (!skipWhitespace(&ptr,endptr) || !skipComment(&ptr,endptr) ||
		!skipWhitespace(&ptr,endptr)) {
		return;
	}

	// look for "create [local|global] temporary"
	if (!strncasecmp(ptr,"create",6)) {
		if (!advance(&ptr,endptr,6) ||
					!skipWhitespace(&ptr,endptr)) {
			return;
		}
		if (!strncasecmp(ptr,"local",5)) {
			if (!advance(&ptr,endptr,5) ||
					!skipWhitespace(&ptr,endptr)) {
				return;
			}
		} else if (!strncasecmp(ptr,"global",6)) {
			if (!advance(&ptr,endptr,6) ||
					!skipWhitespace(&ptr,endptr)) {
				return;
			}
		}
		if (!strncasecmp(ptr,"temporary",9)) {
			if (!advance(&ptr,endptr,9) ||
					!skipWhitespace(&ptr,endptr)) {
				return;
			}
		} else if (!strncasecmp(ptr,"temp",4)) {
			if (!advance(&ptr,endptr,4) ||
					!skipWhitespace(&ptr,endptr)) {
				return;
			}
		} else {
			// not a temp table
			return;
		}
	} else {
		// not a temp table
		return;
	}

	// check for the word table
	if (!skipWhitespace(&ptr,endptr) || strncasecmp(ptr,"table",5)) {
		return;
	}

	// skip any whitespace before the table name
	if (!advance(&ptr,endptr,5) || !skipWhitespace(&ptr,endptr)) {
		return;
	}

	// get the table name
	stringbuffer	tablename;
	while (*ptr!=' ' && *ptr!='\n' && *ptr!='	' && ptr<endptr) {
		tablename.append(*ptr);
		ptr++;
	}

	// append to list of temp tables
	conn->addSessionTempTable(tablename.getString());
}

int	sqlrcursor::skipComment(char **ptr, const char *endptr) {
	while (*ptr<endptr && !strncmp(*ptr,"--",2)) {
		while (**ptr && **ptr!='\n') {
			(*ptr)++;
		}
	}
	return *ptr!=endptr;
}

int	sqlrcursor::skipWhitespace(char **ptr, const char *endptr) {
	while ((**ptr==' ' || **ptr=='\n' || **ptr=='	') && *ptr<endptr) {
		(*ptr)++;
	}
	return *ptr!=endptr;
}

int	sqlrcursor::advance(char **ptr, const char *endptr,
						unsigned short steps) {
	for (unsigned short i=0; i<steps && *ptr<endptr; i++) {
		(*ptr)++;
	}
	return *ptr!=endptr;
}

int	sqlrcursor::handleBinds() {
	
	// iterate through the arrays, binding values to variables
	for (int i=0; i<inbindcount; i++) {

		// bind the value to the variable
		if (inbindvars[i].type==STRING_BIND ||
				inbindvars[i].type==NULL_BIND) {
			if (!inputBindString(inbindvars[i].variable,
				inbindvars[i].variablesize,
				inbindvars[i].value.stringval,
				inbindvars[i].valuesize,
				&inbindvars[i].isnull)) {
				return 0;
			}
		} else if (inbindvars[i].type==LONG_BIND) {
			if (!inputBindLong(inbindvars[i].variable,
				inbindvars[i].variablesize,
				(unsigned long *)
					&inbindvars[i].value.longval)) {
				return 0;
			}
		} else if (inbindvars[i].type==DOUBLE_BIND) {
			if (!inputBindDouble(inbindvars[i].variable,
				inbindvars[i].variablesize,
				&inbindvars[i].value.doubleval.value,
				inbindvars[i].value.doubleval.precision,
				inbindvars[i].value.doubleval.scale)) {
				return 0;
			}
		} else if (inbindvars[i].type==BLOB_BIND) {
			if (!inputBindBlob(inbindvars[i].variable,
				inbindvars[i].variablesize,
				inbindvars[i].value.stringval,
				inbindvars[i].valuesize,
				&inbindvars[i].isnull)) {
				return 0;
			}
		} else if (inbindvars[i].type==CLOB_BIND) {
			if (!inputBindClob(inbindvars[i].variable,
				inbindvars[i].variablesize,
				inbindvars[i].value.stringval,
				inbindvars[i].valuesize,
				&inbindvars[i].isnull)) {
				return 0;
			}
		}
	}
	for (int i=0; i<outbindcount; i++) {

		// bind the value to the variable
		if (outbindvars[i].type==STRING_BIND) {
			if (!outputBindString(outbindvars[i].variable,
					outbindvars[i].variablesize,
					outbindvars[i].value.stringval,
					outbindvars[i].valuesize+1,
					&outbindvars[i].isnull)) {
				return 0;
			}
		} else if (outbindvars[i].type==BLOB_BIND) {
			if (!outputBindBlob(outbindvars[i].variable,
					outbindvars[i].variablesize,i,
					&outbindvars[i].isnull)) {
				return 0;
			}
		} else if (outbindvars[i].type==CLOB_BIND) {
			if (!outputBindClob(outbindvars[i].variable,
					outbindvars[i].variablesize,i,
					&outbindvars[i].isnull)) {
				return 0;
			}
		} else if (outbindvars[i].type==CURSOR_BIND) {
			if (!outputBindCursor(outbindvars[i].variable,
						outbindvars[i].variablesize,
						conn->cur[outbindvars[i].value.
								cursorid])) {
				return 0;
			}
		}
	}
	return 1;
}

int	sqlrcursor::inputBindString(const char *variable,
					unsigned short variablesize,
					const char *value,
					unsigned short valuesize,
					short *isnull) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::inputBindLong(const char *variable,
					unsigned short variablesize,
					unsigned long *value) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::inputBindDouble(const char *variable,
					unsigned short variablesize,
					double *value, 
					unsigned short precision,
					unsigned short scale) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::inputBindBlob(const char *variable,
					unsigned short variablesize,
					const char *value,
					unsigned long valuesize,
					short *isnull) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::inputBindClob(const char *variable,
					unsigned short variablesize,
					const char *value,
					unsigned long valuesize,
					short *isnull) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::outputBindString(const char *variable,
					unsigned short variablesize,
					char *value, 
					unsigned short valuesize, 
					short *isnull) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::outputBindBlob(const char *variable,
					unsigned short variablesize,
					int index,
					short *isnull) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::outputBindClob(const char *variable,
					unsigned short variablesize,
					int index,
					short *isnull) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::outputBindCursor(const char *variable,
					unsigned short variablesize,
					sqlrcursor *cursor) {
	// by default, do nothing...
	return 1;
}

void	sqlrcursor::returnOutputBindBlob(int index) {
	// by default, do nothing...
	return;
}

void	sqlrcursor::returnOutputBindClob(int index) {
	// by default, do nothing...
	return;
}

void	sqlrcursor::returnOutputBindCursor(int index) {
	// by default, do nothing...
	return;
}

stringbuffer	*sqlrcursor::fakeInputBinds(const char *query) {

	// return null if there aren't any input binds
	if (!inbindcount) {
		return NULL;
	}

	stringbuffer	*outputquery=new stringbuffer();

	// loop through the query, performing substitutions
	char	prefix=inbindvars[0].variable[0];
	char	*ptr=(char *)query;
	int	index=1;
	int	inquotes=0;
	while (*ptr) {

		// are we inside of quotes ?
		if (*ptr=='\'') {
			if (inquotes) {
				inquotes==0;
			} else {
				inquotes==1;
			}
		}

		// look for the bind var prefix or ? if not inside of quotes
		if (!inquotes && (*ptr==prefix || *ptr=='?')) {

			// look through the list of vars
			int	match=0;
			for (int i=0; i<inbindcount; i++) {

				// if we find a match, perform the substitution
				// and skip past the variable...
				//
				// for bind-by-name cases, we need to check
				// after the variable for whitespace, a comma
				// or a right parenthesis to make sure that we 
				// don't make the following mistake:
				//
				// select :var1,:var15 from mytable
				//
				// :var1=3
				// :var15=2
				//
				// correct:
				//      select 3,2 from mytable
				//
				// mistake:
				//      select 3,35 from mytable
				if (
					(*ptr=='?' && 
					atoi(inbindvars[i].variable+1)==index) 

					||

					(!strncmp(ptr,inbindvars[i].variable,
						inbindvars[i].variablesize) 
					 		&&
					(*(ptr+inbindvars[i].variablesize)==
					 		' ' ||
					*(ptr+inbindvars[i].variablesize)==
							'	' ||
					*(ptr+inbindvars[i].variablesize)==
							'\n' ||
					*(ptr+inbindvars[i].variablesize)==
							')' ||
					*(ptr+inbindvars[i].variablesize)==
							','))
					) {

					performSubstitution(outputquery,i);
					if (*ptr=='?') {
						ptr++;
					} else {
						ptr=ptr+inbindvars[i].
								variablesize;
					}
					index++;
					break;
				}
			}
		}

		// write the input query to the output query
		if (*ptr) {
			outputquery->append(*ptr);
			ptr++;
		}
	}

	return outputquery;
}

void	sqlrcursor::performSubstitution(stringbuffer *buffer, int which) {
	if (inbindvars[which].type==STRING_BIND) {
		buffer->append("'");
		buffer->append(inbindvars[which].value.stringval);
		buffer->append("'");
	} else if (inbindvars[which].type==LONG_BIND) {
		buffer->append((long)inbindvars[which].value.longval);
	} else if (inbindvars[which].type==DOUBLE_BIND) {
		/**buffer << setprecision(inbindvars[which].
					value.doubleval.precision);
		*buffer << floor(inbindvars[which].value.doubleval.value*
			pow(10,inbindvars[which].value.doubleval.scale))/
			pow(10,inbindvars[which].value.doubleval.scale);*/
		buffer->append(inbindvars[which].value.doubleval.value,
				inbindvars[which].value.doubleval.precision,
				inbindvars[which].value.doubleval.scale);
	}
}

int	sqlrcursor::queryIsNotSelect() {

	// scan the query, bypassing whitespace and comments.
	char	*ptr=skipWhitespaceAndComments(querybuffer);

	// if the query is a select but not a select into then return false,
	// otherwise return true
	if (!strncasecmp(ptr,"select",6) && 
			strncasecmp(ptr,"select into ",12)) {
		return 0;
	}
	return 1;
}

int	sqlrcursor::queryIsCommitOrRollback() {

	// scan the query, bypassing whitespace and comments.
	char	*ptr=skipWhitespaceAndComments(querybuffer);

	// if the query is a commit or rollback, return true
	// otherwise return false
	if (!strncasecmp(ptr,"commit",6) || !strncasecmp(ptr,"rollback",8)) {
		return 1;
	}
	return 0;
}

char	*sqlrcursor::skipWhitespaceAndComments(const char *querybuffer) {
	// scan the query, bypassing whitespace and comments.
	char	*ptr=(char *)querybuffer;
	while (*ptr && 
		(*ptr==' ' || *ptr=='\n' || *ptr=='	' || *ptr=='-')) {

		// skip any comments
		if (*ptr=='-') {
			while (*ptr && *ptr!='\n') {
				ptr++;
			}
		}
		ptr++;
	}
	return ptr;
}

void	sqlrcursor::cleanUpData() {
	// by default, do nothing...
	return;
}

void	sqlrcursor::abort() {
	cleanUpData();
	suspendresultset=0;
	busy=0;
}
