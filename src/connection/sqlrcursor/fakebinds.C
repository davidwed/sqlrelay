// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrconnection.h>

#include <string.h>
#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif

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
				inquotes=0;
			} else {
				inquotes=1;
			}
		}

		// look for the bind var prefix or ? if not inside of quotes
		if (!inquotes && (*ptr==prefix || *ptr=='?')) {

			// look through the list of vars
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
							',') ||
					*(ptr+inbindvars[i].variablesize)==
							(char)NULL)
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
	} else if (inbindvars[which].type==NULL_BIND) {
		buffer->append("NULL");
	}
}
