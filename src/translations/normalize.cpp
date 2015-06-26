// Copyright (c) 2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/character.h>
#include <debugprint.h>

class SQLRSERVER_DLLSPEC normalize : public sqlrtranslation {
	public:
			normalize(sqlrtranslations *sqlts,
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
};

normalize::normalize(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

static const char symbols[]="!@#$%^&*-_+=[{]}\\|;:,<.>/?";

bool normalize::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					stringbuffer *translatedquery) {
	debugFunction();

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

		// skip comments
		if (!charstring::compare(ptr,"-- ",3)) {
			while (*ptr && *ptr!='\n') {
				ptr++;
			}
			if (*ptr) {
				ptr++;
			}
		}

		// convert whitespace into spaces and compress them
		if (character::isWhitespace(*ptr)) {
			do {
				ptr++;
			} while (character::isWhitespace(*ptr));
			if (*ptr) {
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

		// remove spaces around symbols
		if (*ptr==' ' &&
			(character::inSet(*(ptr+1),symbols) ||
			character::inSet(*(ptr-1),symbols))) {
			ptr++;
			continue;
		}

		// FIXME: parentheses require special handling
		// as they serve multiple purposes

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

bool normalize::skipQuotedStrings(const char *ptr,
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
	sqlrtranslation	*new_sqlrtranslation_normalize(
						sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) {
		return new normalize(sqlts,parameters,debug);
	}
}
