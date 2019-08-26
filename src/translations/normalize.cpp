// Copyright (c) 1999-2018 David Muse
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
							domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					uint32_t querylength,
					stringbuffer *translatedquery);
	private:
		bool	skipQuotedStrings(const char *ptr,
						const char *end,
						stringbuffer *sb,
						const char **newptr,
						bool sq,
						bool dq,
						bool bq,
						bool alreadyinside);
		bool	caseConvertQuotedStrings(
						const char *ptr,
						const char *end,
						stringbuffer *sb,
						const char **newptr,
						bool upper,
						char quote);
		bool	removeQuotes(const char *ptr,
						const char *end,
						stringbuffer *sb,
						const char **newptr,
						bool upper,
						bool lower,
						char quote);

		stringbuffer	pass1;
		stringbuffer	pass2;
		stringbuffer	pass3;

		bool	enabled;
		bool	foreigndecimals;
		bool	uppercase;
		bool	lowercase;
		bool	uppercasedq;
		bool	lowercasedq;
		bool	removedq;
		bool	uppercasebq;
		bool	lowercasebq;
		bool	removebq;
		bool	doubleescape;
		bool	slashescape;

		bool	debug;
};

sqlrtranslation_normalize::sqlrtranslation_normalize(
					sqlrservercontroller *cont,
					sqlrtranslations *sqlts,
					domnode *parameters) :
				sqlrtranslation(cont,sqlts,parameters) {
	debugFunction();

	debug=cont->getConfig()->getDebugTranslations();

	enabled=!charstring::isNo(parameters->getAttributeValue("enabled"));

	foreigndecimals=charstring::isYes(
			parameters->getAttributeValue("foreigndecimals"));

	uppercase=!charstring::compareIgnoringCase(
		parameters->getAttributeValue("convertcase"),"upper");
	lowercase=(!uppercase &&
			!charstring::isNo(parameters->getAttributeValue(
							"convertcase")));

	uppercasedq=false;
	lowercasedq=false;
	const char	*convertcasedq=
				parameters->getAttributeValue(
					"convertcasedoublequoted");
	if (!charstring::compareIgnoringCase(convertcasedq,"upper")) {
		uppercasedq=true;
	} else if (!charstring::compareIgnoringCase(convertcasedq,"lower")) {
		lowercasedq=true;
	} else if (charstring::isYes(convertcasedq)) {
		if (lowercase) {
			lowercasedq=true;
		} else if (uppercase) {
			uppercasedq=true;
		}
	}

	removedq=charstring::isYes(
			parameters->getAttributeValue("removedoublequotes"));

	uppercasebq=false;
	lowercasebq=false;
	const char	*convertcasebq=
				parameters->getAttributeValue(
					"convertcasebackquoted");
	if (!charstring::compareIgnoringCase(convertcasebq,"upper")) {
		uppercasebq=true;
	} else if (!charstring::compareIgnoringCase(convertcasebq,"lower")) {
		lowercasebq=true;
	} else if (charstring::isYes(convertcasebq)) {
		if (lowercase) {
			lowercasebq=true;
		} else if (uppercase) {
			uppercasebq=true;
		}
	}

	removebq=charstring::isYes(
			parameters->getAttributeValue("removebackquotes"));

	doubleescape=!charstring::isNo(
			parameters->getAttributeValue("doubleescape"));
	slashescape=!charstring::isNo(
			parameters->getAttributeValue("slashescape"));
}

static const char beforeset[]=" +-/*=<>(";
static const char afterset[]=" +-/*=<>)";

bool sqlrtranslation_normalize::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					uint32_t querylength,
					stringbuffer *translatedquery) {
	debugFunction();

	if (!enabled) {
		translatedquery->append(query,querylength);
		return true;
	}

	if (debug) {
		stdoutput.write("original query:\n\"");
		stdoutput.safePrint(query,querylength);
		stdoutput.write("\"\n\n");
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
	const char	*end=query+querylength;
	for (;;) {

		// NOTE: it matters what order these are in...

		// remove comments
		if (!charstring::compare(ptr,"-- ",3)) {
			while (ptr!=end && *ptr!='\n') {
				ptr++;
			}
			if (ptr!=end) {
				ptr++;
			}
			continue;
		}

		// convert whitespace into spaces and compress them
		if (character::isWhitespace(*ptr)) {
			do {
				ptr++;
			} while (character::isWhitespace(*ptr));
			if (ptr!=end && pass1.getStringLength()) {
				pass1.append(' ');
			}
			continue;
		}

		// skip quoted strings
		if (skipQuotedStrings(ptr,end,&pass1,&ptr,
					true,
					(!uppercasedq &&
						!lowercasedq &&
						!removedq),
					(!uppercasebq &&
						!lowercasebq &&
						!removebq),
					false)) {
			continue;
		}

		// remove double quotes
		if (removedq) {
			if (removeQuotes(ptr,end,&pass1,&ptr,
						uppercase,lowercase,'"')) {
				continue;
			}
		}

		// case-convert double-quoted strings
		else if (uppercasedq || lowercasedq) {
			if (caseConvertQuotedStrings(
					ptr,end,&pass1,&ptr,uppercasedq,'"')) {
				continue;
			}
		}

		// remove back quotes
		if (removebq) {
			if (removeQuotes(ptr,end,&pass1,&ptr,
						uppercase,lowercase,'`')) {
				continue;
			}
		}

		// case-convert back-quoted strings
		else if (uppercasebq || lowercasebq) {
			if (caseConvertQuotedStrings(
					ptr,end,&pass1,&ptr,uppercasebq,'`')) {
				continue;
			}
		}

		// check for end of query
		if (ptr==end) {
			break;
		}

		// case-convert the character and append it
		if (uppercase) {
			pass1.append((char)character::toUpperCase(*ptr));
		} else if (lowercase) {
			pass1.append((char)character::toLowerCase(*ptr));
		} else {
			pass1.append(*ptr);
		}

		// move on to the next character
		ptr++;
	}

	if (debug) {
		stdoutput.write("normalized query (pass 1):\n\"");
		stdoutput.safePrint(pass1.getString(),pass1.getSize());
		stdoutput.write("\"\n\n");
	}

	// normalize the query, second pass...
	// * convert any identifiable comma-separated
	//   decimals to period-separated decimals
	const char	*start=NULL;
	if (!foreigndecimals) {

		pass2.append(pass1.getString(),pass1.getSize());

	} else {

		ptr=pass1.getString();
		end=ptr+pass1.getSize();
		start=ptr;

		while (ptr!=end) {

			// skip whitespace
			if (character::isWhitespace(*ptr)) {
				pass2.append(*ptr);
				ptr++;
				continue;
			}

			// skip quoted strings
			if (skipQuotedStrings(ptr,end,&pass2,&ptr,
						true,true,true,false)) {
				continue;
			}

			// look for a comma, followed by a digit
			// (unless it's at the beginning of the query)
			if (*ptr==',' &&
				character::isDigit(*(ptr+1)) &&
				ptr!=start) {

				// skip backwards until we find a non-number
				// (also skip a leading negative sign)
				const char	*before=ptr-1;
				while (character::isDigit(*before) &&
							before!=start) {
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

					// *before is one of a valid set of
					// characters that can preceed a decimal
					character::inSet(*before,beforeset) &&

					// *after is one of a valid set of
					// characters that can follow a decimal
					(character::inSet(*after,afterset) ||

					// *after is at the end of the query
					!*after ||

					// *after is a comma, followed by
					// whitespace
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
	}

	if (debug) {
		stdoutput.write("normalized query (pass 2):\n\"");
		stdoutput.safePrint(pass2.getString(),pass2.getSize());
		stdoutput.write("\"\n\n");
	}

	// normalize the query, third pass...
	// * remove spaces around symbols
	ptr=pass2.getString();
	start=ptr;
	end=ptr+pass2.getSize();
	for (;;) {

		// skip quoted strings
		if (skipQuotedStrings(ptr,end,&pass3,&ptr,
					true,true,true,false)) {
			continue;
		}

		// Remove spaces around most symbols.
		// Parentheses, asterisks and right brackets require special
		// handling.
		static const char symbols[]="!%^-+=[{}\\|;,<.>/";
		if (
			(*ptr==' ' &&
			(character::inSet(*(ptr+1),symbols) ||
			character::inSet(*(ptr-1),symbols) ||
			!charstring::compare(ptr+1,":=",2))) &&

			// actually, - and ! also require special handling
			// because they can be unary operators and we don't
			// want to remove the space if they follow certain
			// other things...
			!(
				(*(ptr+1)=='-' || *(ptr+1)=='!') &&
				(
				(ptr-start==6 &&
				!charstring::compare(ptr-6,"select ",7)) ||
				(ptr-start>=7 && (
				!charstring::compare(ptr-7,"(select ",8) ||
				!charstring::compare(ptr-7," select ",8) ||
				!charstring::compare(ptr-7," regexp ",8) ||
				!charstring::compare(ptr-7," having ",8) ||
				!charstring::compare(ptr-7," offset ",8))) ||
				(ptr-start>=6 && (
				!charstring::compare(ptr-6," where ",7) ||
				!charstring::compare(ptr-6," rlike ",7) ||
				!charstring::compare(ptr-6," limit ",7))) ||
				(ptr-start>=4 && (
				!charstring::compare(ptr-4," and ",5) ||
				!charstring::compare(ptr-4," div ",5) ||
				!charstring::compare(ptr-4," mod ",5) ||
				!charstring::compare(ptr-4," not ",5) ||
				!charstring::compare(ptr-4," for ",5))) ||
				(ptr-start>=3 &&
				!charstring::compare(ptr-3," or ",4)) ||
				(ptr-start>=8 && (
				!charstring::compare(ptr-8," between ",9) ||
				!charstring::compare(ptr-8," matches ",9))) ||
				(ptr-start>=5 && (
				!charstring::compare(ptr-5," like ",6) ||
				!charstring::compare(ptr-5," case ",6) ||
				!charstring::compare(ptr-5," then ",6) ||
				!charstring::compare(ptr-5," when ",6) ||
				!charstring::compare(ptr-5," else ",6) ||
				!charstring::compare(ptr-5," from ",6))) ||
				(ptr-start>=9 && (
				!charstring::compare(ptr-9," order by ",10) ||
				!charstring::compare(ptr-9," interval ",10)))
				)
			)
			) {

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
		if (ptr==end) {
			break;
		}

		// append the character
		pass3.append(*ptr);

		// move on to the next character
		ptr++;
	}

	if (debug) {
		stdoutput.write("normalized query (pass 3):\n\"");
		stdoutput.safePrint(pass3.getString(),pass3.getSize());
		stdoutput.write("\"\n\n");
	}

	// normalize the query, fourth pass...
	// * convert static concatenations to equivalent strings
	// FIXME:
	// * convert static char() calls to equivalent strings
	//   (if db supports static char() calls)
	ptr=pass3.getString();
	start=ptr;
	end=ptr+pass3.getSize();
	for (;;) {

		// skip quoted strings
		if (skipQuotedStrings(ptr,end,translatedquery,&ptr,
						true,true,true,false)) {
			continue;
		}

		// convert static concatenations
		if (ptr!=start && !charstring::compare(ptr-1,"'||'",4)) {
			ptr=ptr+3;
			translatedquery->setPosition(
					translatedquery->getPosition()-1);
			skipQuotedStrings(ptr,end,translatedquery,&ptr,
							true,true,true,true);
			continue;
		}

		// check for end of query
		if (ptr==end) {
			break;
		}

		// write, rather than append, the character because
		// we may have fiddled with the current position above
		translatedquery->write(*ptr);

		// move on to the next character
		ptr++;
	}

	if (debug) {
		stdoutput.write("normalized query (pass 4):\n\"");
		stdoutput.safePrint(translatedquery->getString(),
					translatedquery->getSize());
		stdoutput.write("\"\n\n");
	}

	return true;
}

bool sqlrtranslation_normalize::skipQuotedStrings(const char *ptr,
						const char *end,
						stringbuffer *sb,
						const char **newptr,
						bool sq,
						bool dq,
						bool bq,
						bool alreadyinside) {

	// bail immediately if we're not skipping any quotes
	if (!sq && !dq && !bq) {
		return false;
	}

	// build the set of quotes to skip
	char	set[4];
	char	*setptr=set;
	if (sq) {
		*setptr='\'';
		setptr++;
	}
	if (dq) {
		*setptr='"';
		setptr++;
	}
	if (bq) {
		*setptr='`';
		setptr++;
	}
	*setptr='\0';

	bool	found=false;

	// if we're on a quote, or are already one-character inside of a
	// quoted string...
	if (character::inSet(*ptr,set) || alreadyinside) {

		found=true;

		// get the type of quote
		char	quote=(alreadyinside)?(*(ptr-1)):(*ptr);

		// write the quote
		if (!alreadyinside) {
			sb->write(*ptr);
			ptr++;
		}

		// until we find the end-quote...
		do {

			// if we found a double-escaped quote like '' or ""...
			if (doubleescape && (*ptr==quote && *(ptr+1)==quote)) {
				// write the double-escaped quote
				sb->write(*ptr);
				sb->write(*ptr);
				ptr=ptr+2;
			} else

			// if we found a slash-escaped quote like \' or \",
			// or if we found a slash-escaped slash like \\...
			if (slashescape && (*ptr=='\\' &&
						(*(ptr+1)==quote ||
						*(ptr+1)=='\\'))) {
				// insert the thing after the slash twice
				// converts \" to "" and leaves \\ alone
				sb->write(*(ptr+1));
				sb->write(*(ptr+1));
				ptr=ptr+2;
			} else

			// if we didn't find escaped quotes,
			// or if we found an empty string...
			if (*ptr!=quote) {
				sb->write(*ptr);
				ptr++;
			}

		} while (ptr!=end && *ptr!=quote);

		// write the end-quote
		if (ptr!=end) {
			sb->write(*ptr);
			ptr++;
		}
	}

	// set output pointer
	*newptr=ptr;

	return found;
}

bool sqlrtranslation_normalize::caseConvertQuotedStrings(
						const char *ptr,
						const char *end,
						stringbuffer *sb,
						const char **newptr,
						bool upper,
						char quote) {

	bool	found=false;

	// if we're on a quote...
	if (*ptr==quote) {

		found=true;

		// write the quote
		sb->write(*ptr);
		ptr++;

		// until we find the end-quote...
		do {

			// if we found a double-escaped quote like "" or ``...
			if (doubleescape && (*ptr==quote && *(ptr+1)==quote)) {
				// write the double-escaped quote
				sb->write(*ptr);
				sb->write(*ptr);
				ptr=ptr+2;
			} else

			// if we found a slash-escaped quote like \" or \`...
			if (slashescape && (*ptr=='\\' && *(ptr+1)==quote)) {
				// convert to a double-escaped quote
				sb->write(*(ptr+1));
				sb->write(*(ptr+1));
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

		} while (ptr!=end && *ptr!=quote);

		// write the end-quote
		if (ptr!=end) {
			sb->write(*ptr);
			ptr++;
		}
	}

	// set output pointer
	*newptr=ptr;

	return found;
}

bool sqlrtranslation_normalize::removeQuotes(
					const char *ptr,
					const char *end,
					stringbuffer *sb,
					const char **newptr,
					bool upper,
					bool lower,
					char quote) {

	bool	found=false;

	// if we're on a quote...
	if (*ptr==quote) {

		found=true;

		// skip the quote
		ptr++;

		// until we find the end-quote...
		do {

			// if we found a double-escaped quote like "" or ``
			// or a slash-escaped quote like \" or \`...
			if ((doubleescape &&
					(*ptr==quote && *(ptr+1)==quote)) ||
				(slashescape &&
					(*ptr=='\\' && *(ptr+1)==quote))) {

				// unescape it
				sb->write(*(ptr+1));
				ptr=ptr+2;

			} else

			// if we didn't find escaped quotes...
			{
				if (upper) {
					sb->write((char)character::
							toUpperCase(*ptr));
				} else if (lower) {
					sb->write((char)character::
							toLowerCase(*ptr));
				} else {
					sb->write(*ptr);
				}
				ptr++;
			}

		} while (ptr!=end && *ptr!=quote);

		// skip the end-quote
		if (ptr!=end) {
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
						domnode *parameters) {
		return new sqlrtranslation_normalize(cont,sqlts,parameters);
	}
}
