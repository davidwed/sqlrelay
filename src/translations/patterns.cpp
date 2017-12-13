// Copyright (c) 2017  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/regularexpression.h>
#include <rudiments/character.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

enum scope_t {
	SCOPE_QUERY=0,
	SCOPE_OUTSIDE_QUOTES,
	SCOPE_INSIDE_QUOTES
};

struct pattern_t {
	const char		*pattern;
	regularexpression	*re;
	bool			ignorecase;
	scope_t			scope;
	const char		*replacement;
};

class SQLRSERVER_DLLSPEC sqlrtranslation_patterns : public sqlrtranslation {
	public:
			sqlrtranslation_patterns(sqlrservercontroller *cont,
						sqlrtranslations *sqlts,
						xmldomnode *parameters);
			~sqlrtranslation_patterns();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					stringbuffer *translatedquery);
	private:
		pattern_t	*p;
		uint32_t	patterncount;
		bool		hasscope;

		bool	enabled;

		bool	debug;
};

sqlrtranslation_patterns::sqlrtranslation_patterns(sqlrservercontroller *cont,
						sqlrtranslations *sqlts,
						xmldomnode *parameters) :
					sqlrtranslation(cont,sqlts,parameters) {
	debugFunction();

	debug=cont->getConfig()->getDebugTranslations();

	p=NULL;
	patterncount=0;
	hasscope=false;

	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
	if (!enabled) {
		return;
	}

	// count patterns
	patterncount=0;
	for (xmldomnode *c=parameters->getFirstTagChild("pattern");
			!c->isNullNode(); c=c->getNextTagSibling("pattern")) {
		patterncount++;
	}

	// build pattern list
	p=new pattern_t[patterncount];
	uint32_t	i=0;
	for (xmldomnode *c=parameters->getFirstTagChild("pattern");
			!c->isNullNode(); c=c->getNextTagSibling("pattern")) {

		const char	*pattern=c->getAttributeValue("pattern");
		p[i].pattern=pattern;
		p[i].re=NULL;
		p[i].ignorecase=false;

		const char	*type=c->getAttributeValue("type");
		if (!charstring::compareIgnoringCase(type,"regex")) {
			p[i].re=new regularexpression();
			p[i].re->compile(pattern);
			p[i].re->study();
		} else if (!charstring::compareIgnoringCase(type,"cistring")) {
			p[i].ignorecase=true;
		}

		const char	*scope=c->getAttributeValue("scope");
		if (!charstring::compareIgnoringCase(
						scope,"outsidequotes")) {
			p[i].scope=SCOPE_OUTSIDE_QUOTES;
			hasscope=true;
		} else if (!charstring::compareIgnoringCase(
						scope,"insidequotes")) {
			p[i].scope=SCOPE_INSIDE_QUOTES;
			hasscope=true;
		} else {
			p[i].scope=SCOPE_QUERY;
		}

		p[i].replacement=c->getFirstChild("text")->getValue();
		i++;
	}
}

sqlrtranslation_patterns::~sqlrtranslation_patterns() {
	for (uint32_t i=0; i<patterncount; i++) {
		delete p[i].re;
	}
	delete[] p;
}

bool sqlrtranslation_patterns::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					stringbuffer *translatedquery) {
	debugFunction();

	if (!enabled) {
		return true;
	}

	// split the string on single-quotes if necessary
	char		**parts=NULL;
	uint64_t	partcount=0;
	if (hasscope) {
		charstring::split(query,"'",false,&parts,&partcount);
	}

	// run through the patterns until one of them matches...
	bool		match=false;
	uint32_t	matchindex=0;
	for (uint32_t i=0; i<patterncount && !match; i++) {

		pattern_t	*pc=&(p[i]);

		// match against the entire query, if necessary...
		if (pc->scope==SCOPE_QUERY) {

			// handle regex patterns
			if (pc->re && pc->re->match(query)) {

				match=true;
				matchindex=i;

			// handle string patterns
			} else {

				// if case is ignored then lowercase everything
				const char	*qry=query;
				const char	*ptrn=pc->pattern;
				char		*lowquery=NULL;
				char		*lowpattern=NULL;
				if (pc->ignorecase) {

					lowquery=charstring::duplicate(query);
					for (char *cq=lowquery; *cq; cq++) {
						*cq=character::toLowerCase(*cq);
					}
					qry=lowquery;

					lowpattern=charstring::duplicate(
								pc->pattern);
					for (char *cp=lowpattern; *cp; cp++) {
						*cp=character::toLowerCase(*cp);
					}
					ptrn=lowpattern;
				}

				// compare
				match=charstring::contains(qry,ptrn);
				matchindex=i;

				// clean up
				delete[] lowquery;
				delete[] lowpattern;
			}
			continue;
		}

		// If we're looking at the query outside of quoted strings,
		// then that ought to be the even numbered parts.  Otherwise
		// it'll be the odd numbered parts.  However, if the query
		// starts with a single-quote (which a valid query wouldn't,
		// but who knows...) then flip the logic.
		uint64_t	start=0;
		if (pc->scope==SCOPE_INSIDE_QUOTES && query[0]!='\'') {
			start=1;
		}

		// check every other part...
		for (uint64_t j=start; j<partcount && !match; j=j+2) {

			// handle regex patterns
			if (pc->re && pc->re->match(parts[j])) {

				match=true;
				matchindex=i;

			// handle string patterns
			} else {

				// if case is ignored then lowercase everything
				const char	*prt=parts[j];
				const char	*ptrn=pc->pattern;
				char		*lowpart=NULL;
				char		*lowpattern=NULL;
				if (pc->ignorecase) {

					lowpart=charstring::duplicate(parts[j]);
					for (char *cq=lowpart; *cq; cq++) {
						*cq=character::toLowerCase(*cq);
					}
					prt=lowpart;


					lowpattern=charstring::duplicate(
								pc->pattern);
					for (char *cp=lowpattern; *cp; cp++) {
						*cp=character::toLowerCase(*cp);
					}
					ptrn=lowpattern;
				}

				// compare
				match=charstring::contains(prt,ptrn);
				matchindex=i;

				// clean up
				delete[] lowpart;
				delete[] lowpattern;
			}
		}
	}

	// apply match
	if (match) {
		if (debug) {
			stdoutput.printf("original query:\n\"%s\"\n",
						query);
			stdoutput.printf("matched pattern:\n\"%s\"\n",
						p[matchindex].pattern);
			stdoutput.printf("translated to:\n\"%s\"\n\n",
						p[matchindex].replacement);
		}
		translatedquery->append(p[matchindex].replacement);
	} else {
		if (debug) {
			stdoutput.printf("original query:\n\"%s\"\n",
						query);
			stdoutput.printf("didn't match any patterns\n\n");
		}
		translatedquery->append(query);
	}

	// clean up
	for (uint32_t k=0; k<partcount; k++) {
		delete[] parts[k];
	}
	delete[] parts;

	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrtranslation
			*new_sqlrtranslation_patterns(
						sqlrservercontroller *cont,
						sqlrtranslations *ts,
						xmldomnode *parameters) {
		return new sqlrtranslation_patterns(cont,ts,parameters);
	}
}