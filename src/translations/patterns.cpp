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
	const char		*match;
	regularexpression	*matchre;
	bool			matchglobal;
	const char		*from;
	regularexpression	*fromre;
	bool			replaceglobal;
	const char		*to;
	bool			ignorecase;
	scope_t			scope;
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

		void	applyPattern(const char *str,
					pattern_t *pc,
					stringbuffer *outbuffer);

		pattern_t	*p;
		uint32_t	patterncount;

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

		p[i].matchre=NULL;
		p[i].matchglobal=true;
		p[i].fromre=NULL;
		p[i].replaceglobal=true;
		p[i].ignorecase=false;

		const char	*from=c->getAttributeValue("from");
		p[i].from=from;

		const char	*match=c->getAttributeValue("match");
		p[i].match=match;

		const char	*type=c->getAttributeValue("type");
		if (!charstring::compareIgnoringCase(type,"regex")) {
			if (!charstring::isNullOrEmpty(match)) {
				p[i].matchre=new regularexpression();
				p[i].matchre->compile(match);
				p[i].matchre->study();
				p[i].matchglobal=
					!charstring::isNo(
					c->getAttributeValue("matchglobal"));
			}
			p[i].fromre=new regularexpression();
			p[i].fromre->compile(from);
			p[i].fromre->study();
			p[i].replaceglobal=
				!charstring::isNo(
				c->getAttributeValue("replaceglobal"));
		} else if (!charstring::compareIgnoringCase(type,"cistring")) {
			p[i].ignorecase=true;
		}

		const char	*scope=c->getAttributeValue("scope");
		if (!charstring::compareIgnoringCase(
						scope,"outsidequotes")) {
			p[i].scope=SCOPE_OUTSIDE_QUOTES;
		} else if (!charstring::compareIgnoringCase(
						scope,"insidequotes")) {
			p[i].scope=SCOPE_INSIDE_QUOTES;
		} else {
			p[i].scope=SCOPE_QUERY;
		}

		p[i].to=c->getAttributeValue("to");
		i++;
	}
}

sqlrtranslation_patterns::~sqlrtranslation_patterns() {
	for (uint32_t i=0; i<patterncount; i++) {
		delete p[i].matchre;
		delete p[i].fromre;
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

	if (debug) {
		stdoutput.printf("original query:\n\"%s\"\n\n",query);
	}

	// run through the patterns
	stringbuffer	querybuffer1;
	stringbuffer	querybuffer2;
	for (uint32_t i=0; i<patterncount; i++) {

		// choose which buffer to write to and clear it
		stringbuffer	*outbuffer=&querybuffer1;
		if (i%2) {
			outbuffer=&querybuffer2;
		}
		if (i==patterncount-1) {
			outbuffer=translatedquery;
		}
		outbuffer->clear();

		// get the current pattern
		pattern_t	*pc=&(p[i]);

		// match against the entire query, if necessary...
		if (pc->scope==SCOPE_QUERY) {

			applyPattern(query,pc,outbuffer);

		} else {

			// split the string on single-quotes
			// FIXME: what about backslash-escaped quotes
			char		**parts=NULL;
			uint64_t	partcount=0;
			charstring::split(query,"'",false,&parts,&partcount);

			// If we're looking at the query outside of quoted
			// strings, then that ought to be the even numbered
			// parts.  Otherwise it'll be the odd numbered parts.
			// However, if the query starts with a single-quote
			// (which a valid query wouldn't, but who knows...)
			// then flip the logic.
			bool	mod=0;
			if (pc->scope==SCOPE_INSIDE_QUOTES && query[0]!='\'') {
				mod=1;
			}

			// check every other part...
			for (uint64_t j=0; j<partcount; j++) {
				if (j%2==mod) {
					applyPattern(parts[j],pc,outbuffer);
				} else {
					outbuffer->append('\'');
					outbuffer->append(parts[j]);
					outbuffer->append('\'');
				}
			}

			// clean up
			for (uint32_t k=0; k<partcount; k++) {
				delete[] parts[k];
			}
			delete[] parts;
		}

		// reset input
		query=outbuffer->getString();
	}

	return true;
}

void sqlrtranslation_patterns::applyPattern(const char *str,
						pattern_t *pc,
						stringbuffer *outbuffer) {

	ssize_t		pcfromlen=(debug)?charstring::length(pc->from):0;
	const char	*fromellipses="";
	if (pcfromlen>77) {
		pcfromlen=74;
		fromellipses="...";
	}
	ssize_t		pctolen=(debug)?charstring::length(pc->to):0;
	const char	*toellipses="";
	if (pctolen>77) {
		pctolen=74;
		toellipses="...";
	}

	char	*convstr=NULL;

	if (pc->matchre) {
		if (debug) {
			stdoutput.printf("applying "
					"match:\n\"%s\"\n"
					"from:\n\"%.*s%s\"\n"
					"to:\n\"%.*s%s\"\n\n",
					pc->match,
					pcfromlen,pc->from,fromellipses,
					pctolen,pc->to,toellipses);
		}
		convstr=charstring::replace(str,
					pc->matchre,
					pc->matchglobal,
					pc->fromre,
					pc->to,
					pc->replaceglobal);
	} else if (pc->fromre) {
		if (debug) {
			stdoutput.printf("applying regex "
					"from:\n\"%.*s%s\"\n"
					"to:\n\"%.*s%s\"\n\n",
					pcfromlen,pc->from,fromellipses,
					pctolen,pc->to,toellipses);
		}
		convstr=charstring::replace(str,
					pc->fromre,
					pc->to,
					pc->replaceglobal);
	} else if (!pc->ignorecase) {
		if (debug) {
			stdoutput.printf("applying string "
					"from:\n\"%.*s%s\"\n"
					"to:\n\"%.*s%s\"\n\n",
					pcfromlen,pc->from,fromellipses,
					pctolen,pc->to,toellipses);
		}
		convstr=charstring::replace(str,pc->from,pc->to);
	} else {
		if (debug) {
			stdoutput.printf("applying case-insensitive string "
					"from:\n\"%.*s%s\"\n"
					"to:\n\"%.*s%s\"\n\n",
					pcfromlen,pc->from,fromellipses,
					pctolen,pc->to,toellipses);
		}
		char	*lowstr=charstring::duplicate(str);
		charstring::lower(lowstr);
		char	*lowfrom=charstring::duplicate(pc->from);
		charstring::lower(lowfrom);
		convstr=charstring::replace(lowstr,lowfrom,pc->to);
		delete[] lowstr;
		delete[] lowfrom;
	}

	outbuffer->append(convstr);
	delete[] convstr;

	if (debug) {
		stdoutput.printf("translated to:\n\"%s\"\n\n",
						outbuffer->getString());
	}
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
