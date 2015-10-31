// Copyright (c) 2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/character.h>
#include <debugprint.h>

class SQLRSERVER_DLLSPEC sqlrtranslation_normalize : public sqlrtranslation {
	public:
			sqlrtranslation_normalize(sqlrtranslations *sqlts,
							xmldomnode *parameters,
							bool debug);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					stringbuffer *translatedquery);
	private:
		bool	skipQuotedStrings(const char *ptr,
						stringbuffer *sb,
						const char **newptr);

		stringbuffer	pass1;
		stringbuffer	pass2;

		bool	enabled;
};

sqlrtranslation_normalize::sqlrtranslation_normalize(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
	debugFunction();

	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
}

bool sqlrtranslation_normalize::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					stringbuffer *translatedquery) {
	debugFunction();

	if (!enabled) {
		return true;
	}

	if (debug) {
		stdoutput.printf("original query:\n\"%s\"\n\n",query);
	}

	// clear the normalized query buffers
	pass1.clear();
	pass2.clear();

	// normalize the query, first pass...
	// * remove comments
	// * translate all whitespace characters into spaces and compress it
	// * lower-case whatever we can
	// FIXME:
	// * translate printable hex (or other) encoded values into characters
	const char	*ptr=query;
	for (;;) {

		// NOTE: it matters what order these are in...

		// remove comments
		if (!charstring::compare(ptr,"-- ",3)) {
			while (*ptr && *ptr!='\n') {
				ptr++;
			}
			if (*ptr) {
				ptr++;
			}
			continue;
		}

		// convert whitespace into spaces and compress them
		if (character::isWhitespace(*ptr)) {
			do {
				ptr++;
			} while (character::isWhitespace(*ptr));
			if (*ptr && pass1.getStringLength()) {
				pass1.append(' ');
			}
			continue;
		}

		// skip quoted strings
		if (skipQuotedStrings(ptr,&pass1,&ptr)) {
			continue;
		}

		// check for end of query
		if (!*ptr) {
			break;
		}

		// convert the character to lower case and append it
		// FIXME: what if the db supports case-sensitive identifiers?
		pass1.append((char)character::toLowerCase(*ptr));

		// move on to the next character
		ptr++;
	}

	if (debug) {
		stdoutput.printf("normalized query (pass 1):\n\"%s\"\n\n",
							pass1.getString());
	}

	// normalize the query, second pass...
	// * remove spaces around symbols
	ptr=pass1.getString();
	for (;;) {

		// skip quoted strings
		if (skipQuotedStrings(ptr,&pass2,&ptr)) {
			continue;
		}

		// Remove spaces around most symbols.
		// Parentheses, asterisks and right brackets require special
		// handling.
		static const char symbols[]="!%^-_+=[{}\\|;,<.>/";
		if (*ptr==' ' &&
			(character::inSet(*(ptr+1),symbols) ||
			character::inSet(*(ptr-1),symbols))) {
			ptr++;
			continue;
		}

		// [ and ] are used to define ranges.  It's always safe to
		// remove spaces around the [.  It's always safe to remove
		// spaces before the ], but not after it.  We don't need to
		// remove spaces after ] though.  If it's followed by any
		// other symbol, then spaces will be removed by that symbol.
		// If not, then spaces shouldn't be removed anyway.
		if (*ptr==' ' &&
			(*(ptr+1)=='[' || *(ptr-1)=='[' || *(ptr+1)==']')) {
			ptr++;
			continue;
		}

		// Parentheses can be used to group expressions or to define
		// function parameters...

		// It's always safe to remove spaces after ( and before )
		if (*ptr==' ' && (*(ptr-1)=='(' || *(ptr+1)==')')) {
			ptr++;
			continue;
		}

		// We don't need to remove spaces after ).  If it's followed by
		// any other symbol, then spaces will be removed by that symbol.
		// If not, then spaces shouldn't be removed anyway.

		// We would like to remove spaces before ( but this is
		// difficult and expensive.  We can't if the ( is preceeded by
		// a long list of specific keywords, including column types, or
		// in the cases of inserts and creates, an object name.  There
		// could be other cases too.  For now, it's much easier and
		// less expensive to stipulate that function calls might have
		// spaces between the function name and parameters.

		// Asterisks can mean either "times" or "all columns".
		// We generally want to remove spaces around them, but we don't
		// want to remove the space after the last "all columns"
		// asterisk, so...
		// Remove spaces before and after asterisks unless the asterisk
		// is followed by a from clause.
		if (*ptr==' ' &&
			(*(ptr+1)=='*' ||
			(*(ptr-1)=='*' &&
			charstring::compare(ptr," from ",6)))) {
			ptr++;
			continue;
		}

		// check for end of query
		if (!*ptr) {
			break;
		}

		// append the character
		pass2.append(*ptr);

		// move on to the next character
		ptr++;
	}

	if (debug) {
		stdoutput.printf("normalized query (pass 2):\n\"%s\"\n\n",
							pass2.getString());
	}

	// normalize the query, third pass...
	// * convert static concatenations to equivalent strings
	// FIXME:
	// * convert static char() calls to equivalent strings
	// 	(if db supports static char() calls)
	ptr=pass2.getString();
	const char	*start=ptr;
	for (;;) {

		// skip quoted strings
		if (skipQuotedStrings(ptr,translatedquery,&ptr)) {
			continue;
		}

		// convert static concatenations
		if (ptr!=start && !charstring::compare(ptr-1,"'||'",4)) {
			ptr=ptr+3;
			translatedquery->setPosition(
				translatedquery->getPosition()-1);
			continue;
		}

		// check for end of query
		if (!*ptr) {
			break;
		}

		// write, rather than append, the character because
		// we may have fiddled with the current position above
		translatedquery->write(*ptr);

		// move on to the next character
		ptr++;
	}

	if (debug) {
		stdoutput.printf("normalized query (pass 3):\n\"%s\"\n\n",
						translatedquery->getString());
	}

	return true;
}

bool sqlrtranslation_normalize::skipQuotedStrings(const char *ptr,
						stringbuffer *sb,
						const char **newptr) {

	bool	found=false;
	for (;;) {
		if (*ptr=='\'' || *ptr=='"') {
			found=true;
			char	quote=*ptr;
			do {
				sb->append(*ptr);
				ptr++;
			} while (*ptr && *ptr!=quote);
			if (*ptr) {
				sb->append(*ptr);
				ptr++;
			}
		} else {
			*newptr=ptr;
			return found;
		}
	}
}


extern "C" {
	SQLRSERVER_DLLSPEC sqlrtranslation *new_sqlrtranslation_normalize(
							sqlrtranslations *sqlts,
							xmldomnode *parameters,
							bool debug) {
		return new sqlrtranslation_normalize(sqlts,parameters,debug);
	}
}
