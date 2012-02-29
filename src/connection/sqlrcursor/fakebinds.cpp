// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrconnection.h>

char sqlrcursor_svr::escapeChar() {
	return '\\';
}

void sqlrcursor_svr::setFakeInputBindsForThisQuery(bool fake) {
	fakeinputbindsforthisquery=fake;
}

stringbuffer *sqlrcursor_svr::fakeInputBinds(const char *query) {

	// return NULL if there aren't any input binds
	if (!inbindcount) {
		return NULL;
	}

	stringbuffer	*outputquery=new stringbuffer();

	// loop through the query, performing substitutions
	char	prefix=inbindvars[0].variable[0];
	char	*ptr=(char *)query;
	int	index=1;
	bool	inquotes=false;
	while (*ptr) {

		// are we inside of quotes ?
		if (*ptr=='\'') {
			if (inquotes) {
				inquotes=false;
			} else {
				inquotes=true;
			}
		}

		// look for the bind var prefix or ? if not inside of quotes
		if (!inquotes && (*ptr==prefix || *ptr=='?')) {

			// look through the list of vars
			for (int16_t i=0; i<inbindcount; i++) {

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
					charstring::toInteger(
						inbindvars[i].variable+1)==
									index) 

					||

					(!charstring::compare(ptr,
						inbindvars[i].variable,
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
							',' ||
					*(ptr+inbindvars[i].variablesize)==
							'\0')
					)) {

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

	if (conn->debugsqltranslation) {
		printf("after faking input binds:\n%s\n\n",
				outputquery->getString());
	}

	return outputquery;
}

void sqlrcursor_svr::performSubstitution(stringbuffer *buffer, int16_t index) {

	if (inbindvars[index].type==STRING_BIND ||
		inbindvars[index].type==CLOB_BIND) {

		char	escchar=escapeChar();

		buffer->append("'");

		size_t	length=inbindvars[index].valuesize;

		for (size_t ind=0; ind<length; ind++) {

			char	ch=inbindvars[index].value.stringval[ind];

			// escape quotes, the escape char and NULL's
			if (ch=='\'' || ch==escchar) {
				buffer->append(escchar);
			} else if (ch=='\0') {
				buffer->append("\\0");
			}
			buffer->append(ch);
		}

		buffer->append("'");

	} else if (inbindvars[index].type==INTEGER_BIND) {
		buffer->append(inbindvars[index].value.integerval);
	} else if (inbindvars[index].type==DOUBLE_BIND) {
		char	*dbuf=NULL;
		if (!inbindvars[index].value.doubleval.precision &&
				!inbindvars[index].value.doubleval.scale) {
			dbuf=charstring::parseNumber(
				inbindvars[index].value.doubleval.value);
		} else {
			dbuf=charstring::parseNumber(
				inbindvars[index].value.doubleval.value,
				inbindvars[index].value.doubleval.precision,
				inbindvars[index].value.doubleval.scale);
		}
		// In some regions a comma is used rather than a period for
		// the decimal and the i8n settings will cause snprintf to use
		// a comma as the separator.  Databases don't like commas in
		// their numbers.  Convert commas to periods here. */
		for (char *ptr=dbuf; *ptr; ptr++) {
			if (*ptr==',') {
				*ptr='.';
			}
		}
		buffer->append(dbuf);
		delete[] dbuf;
	} else if (inbindvars[index].type==NULL_BIND) {
		buffer->append("NULL");
	}
}
