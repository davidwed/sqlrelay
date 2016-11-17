// Copyright (c) 2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/character.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class SQLRSERVER_DLLSPEC sqlrtranslation_normalize : public sqlrtranslation {
	public:
			sqlrtranslation_normalize(sqlrservercontroller *cont,
							sqlrtranslations *sqlts,
							xmldomnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					stringbuffer *translatedquery);
	private:
		bool	skipQuotedStrings(const char *ptr,
						stringbuffer *sb,
						const char **newptr,
						bool sq,
						bool dq,
						bool alreadyinside);
		bool	caseConvertDoubleQuotedStrings(
						const char *ptr,
						stringbuffer *sb,
						const char **newptr,
						bool upper);

		stringbuffer	pass1;
		stringbuffer	pass2;
		stringbuffer	pass3;

		bool	enabled;
		bool	foreigndecimals;
		bool	uppercase;
		bool	lowercase;
		bool	uppercasedq;
		bool	lowercasedq;

		bool	debug;
};

sqlrtranslation_normalize::sqlrtranslation_normalize(
					sqlrservercontroller *cont,
					sqlrtranslations *sqlts,
					xmldomnode *parameters) :
				sqlrtranslation(cont,sqlts,parameters) {
	debugFunction();

	debug=cont->getConfig()->getDebugTranslations();

	enabled=charstring::compareIgnoringCase(
		parameters->getAttributeValue("enabled"),"no");

	foreigndecimals=!charstring::compareIgnoringCase(
		parameters->getAttributeValue("foreign_decimals"),"yes");

	uppercase=!charstring::compareIgnoringCase(
		parameters->getAttributeValue(
				"convertcase"),"upper");
	lowercase=(!uppercase &&
			charstring::compareIgnoringCase(
				parameters->getAttributeValue(
					"convertcase"),"no"));

	uppercasedq=!charstring::compareIgnoringCase(
		parameters->getAttributeValue(
				"convertcasedoublequoted"),"upper");
	lowercasedq=(!uppercasedq &&
			charstring::compareIgnoringCase(
				parameters->getAttributeValue(
					"convertcasedoublequoted"),"no"));
}

static const char beforeset[]=" +-/*=<>(";
static const char afterset[]=" +-/*=<>)";

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
	pass3.clear();

	// normalize the query, first pass...
	// * remove comments
	// * translate all whitespace characters into spaces and compress it
	// * case-convert whatever we can
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
		if (skipQuotedStrings(ptr,&pass1,&ptr,
					true,(!uppercasedq && !lowercasedq),
					false)) {
			continue;
		}

		// convert quoted strings
		if (uppercasedq || lowercasedq) {
			if (caseConvertDoubleQuotedStrings(
					ptr,&pass1,&ptr,uppercasedq)) {
				continue;
			}
		}

		// check for end of query
		if (!*ptr) {
			break;
		}

		// case-convert the character and append it
		if (uppercase) {
			pass1.append((char)character::toUpperCase(*ptr));
		} else if (lowercase) {
			pass1.append((char)character::toLowerCase(*ptr));
		}

		// move on to the next character
		ptr++;
	}

	if (debug) {
		stdoutput.printf("normalized query (pass 1):\n\"%s\"\n\n",
							pass1.getString());
	}

	// normalize the query, second pass...
	// * convert any identifiable comma-separated
	//   decimals to period-separated decimals
	ptr=pass1.getString();
	const char	*start=ptr;
	if (!foreigndecimals) {
		pass2.append(ptr);
		ptr="";
	}
	while (*ptr) {

		// skip whitespace
		if (character::isWhitespace(*ptr)) {
			pass2.append(*ptr);
			ptr++;
			continue;
		}

		// skip quoted strings
		if (skipQuotedStrings(ptr,&pass2,&ptr,true,true,false)) {
			continue;
		}

		// look for a comma, followed by a digit
		// (unless it's at the beginning of the query)
		if (*ptr==',' && character::isDigit(*(ptr+1)) && ptr!=start) {

			// skip backwards until we find a non-number
			// (also skip a leading negative sign)
			const char	*before=ptr-1;
			while (character::isDigit(*before) && before!=start) {
				before--;
			}
			if (*before=='-') {
				before--;
			}

			// skip forwards until we find a non-number
			const char	*after=ptr+1;
			while (character::isDigit(*after) && *after) {
				after++;
			}

			// if various conditions are true then
			// switch the comma to a dot...
			//
			// This is tricky with decimals in parentheses.
			// Eg. How should the following be interpreted?
			//    (111,222,333,444)
			// The logic below breaks on spaces.
			// Eg:
			//    (111,222, 333,444)
			// Would be recognized as having 2 decimals:
			//    111,222 and 333,444
			// and
			//    (111, 222, 333,444)
			// Would be recognized as having 3 decimals:
			//    111 and 222 and 333,444
			//
			// Another difficult case is something like:
			//	f(111,222)
			// Is that 1 or 2 parameters?
			// For now, we're calling that 2 parameters.
			if (
				// it's not something like (111,222)
				!(*before=='(' && *after==')') &&

				// *before is one of a valid set of characters
				// that can preceed a decimal
				character::inSet(*before,beforeset) &&

				// *after is one of a valid set of characters
				// that can follow a decimal
				(character::inSet(*after,afterset) ||

				// *after is at the end of the query
				!*after ||

				// *after is a comma, followed by whitespace
				(*after==',' &&
				character::isWhitespace(*(after+1)))

				)) {

				pass2.append('.');
			} else {
				pass2.append(',');
			}

			// move on...
			ptr++;
			continue;
		}

		// move on to the next character
		pass2.append(*ptr);
		ptr++;
	}

	if (debug) {
		stdoutput.printf("normalized query (pass 2):\n\"%s\"\n\n",
							pass2.getString());
	}

	// normalize the query, third pass...
	// * remove spaces around symbols
	ptr=pass2.getString();
	start=ptr;
	for (;;) {

		// skip quoted strings
		if (skipQuotedStrings(ptr,&pass3,&ptr,true,true,false)) {
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
		// We generally want to remove spaces around them, but we have
		// to be careful removing them in the "all columns" case.
		if (ptr!=start &&
			!charstring::compare(ptr," *",2) &&
			charstring::compare(ptr-1,". *",3) &&
			*(ptr+2)!=',' &&
			charstring::compare(ptr+2," ,",2) &&
			charstring::compare(ptr+2," from ",6) &&
			*(ptr+2)!='\0') {
			ptr++;
			continue;
		}
		if (ptr!=start &&
			!charstring::compare(ptr-1,"* ",2) &&
			charstring::compare(ptr," from ",6)) {
			ptr++;
			continue;
		}

		// check for end of query
		if (!*ptr) {
			break;
		}

		// append the character
		pass3.append(*ptr);

		// move on to the next character
		ptr++;
	}

	if (debug) {
		stdoutput.printf("normalized query (pass 3):\n\"%s\"\n\n",
							pass3.getString());
	}

	// normalize the query, fourth pass...
	// * convert static concatenations to equivalent strings
	// FIXME:
	// * convert static char() calls to equivalent strings
	//   (if db supports static char() calls)
	ptr=pass3.getString();
	start=ptr;
	for (;;) {

		// skip quoted strings
		if (skipQuotedStrings(ptr,translatedquery,&ptr,
							true,true,false)) {
			continue;
		}

		// convert static concatenations
		if (ptr!=start && !charstring::compare(ptr-1,"'||'",4)) {
			ptr=ptr+3;
			translatedquery->setPosition(
					translatedquery->getPosition()-1);
			skipQuotedStrings(ptr,translatedquery,&ptr,
							true,true,true);
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
		stdoutput.printf("normalized query (pass 4):\n\"%s\"\n\n",
						translatedquery->getString());
	}

	return true;
}

bool sqlrtranslation_normalize::skipQuotedStrings(const char *ptr,
						stringbuffer *sb,
						const char **newptr,
						bool sq,
						bool dq,
						bool alreadyinside) {

	// are we skipping single quoted strings,
	// double quoted strings, or both
	const char	*set="";
	if (sq && dq) {
		set="'\"";
	} else if (sq && !dq) {
		set="'";
	} else if (!sq && dq) {
		set="\"";
	} else if (!sq && !dq) {
		return false;
	}

	bool	found=false;

	// if we're on a quote...
	if (character::inSet(*ptr,set) || alreadyinside) {

		found=true;

		// get the type of quote (single or double)
		char	quote=(alreadyinside)?(*(ptr-1)):(*ptr);

		// write the quote
		if (!alreadyinside) {
			sb->write(*ptr);
			ptr++;
		}

		// until we find the end-quote...
		do {

			// if we found escaped quotes ('' or "")...
			if (*ptr==quote && *(ptr+1)==quote) {
				sb->write(*ptr)->write(*ptr);
				ptr=ptr+2;

			} else

			// if we didn't find escaped quotes...
			{
				sb->write(*ptr);
				ptr++;
			}

		} while (*ptr && *ptr!=quote);

		// write the end-quote
		if (*ptr) {
			sb->write(*ptr);
			ptr++;
		}
	}

	// set output pointer
	*newptr=ptr;

	return found;
}

bool sqlrtranslation_normalize::caseConvertDoubleQuotedStrings(
						const char *ptr,
						stringbuffer *sb,
						const char **newptr,
						bool upper) {

	bool	found=false;

	// if we're on a quote...
	if (*ptr=='"') {

		found=true;

		// write the quote
		sb->write(*ptr);
		ptr++;

		// until we find the end-quote...
		do {

			// if we found escaped quotes ("")...
			if (*ptr=='"' && *(ptr+1)=='"') {
				sb->write(*ptr)->write(*ptr);
				ptr=ptr+2;

			} else

			// if we didn't find escaped quotes...
			{
				if (upper) {
					sb->write((char)character::
							toUpperCase(*ptr));
				} else {
					sb->write((char)character::
							toLowerCase(*ptr));
				}
				ptr++;
			}

		} while (*ptr && *ptr!='"');

		// write the end-quote
		if (*ptr) {
			sb->write(*ptr);
			ptr++;
		}
	}

	// set output pointer
	*newptr=ptr;

	return found;
}


extern "C" {
	SQLRSERVER_DLLSPEC sqlrtranslation *new_sqlrtranslation_normalize(
						sqlrservercontroller *cont,
						sqlrtranslations *sqlts,
						xmldomnode *parameters) {
		return new sqlrtranslation_normalize(cont,sqlts,parameters);
	}
}
