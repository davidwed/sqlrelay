// Copyright (c) 1999-2018 David Muse
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
	pattern_t 		*patterns;
	uint32_t		patterncount;
};

class SQLRSERVER_DLLSPEC sqlrtranslation_patterns : public sqlrtranslation {
	public:
			sqlrtranslation_patterns(sqlrservercontroller *cont,
						sqlrtranslations *sqlts,
						domnode *parameters);
			~sqlrtranslation_patterns();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					uint32_t querylength,
					stringbuffer *translatedquery);
	private:
		void	buildPatternsTree(domnode *root,
						pattern_t **p,
						uint32_t *pcount,
						bool toplevel);
		void	freePatternsTree(pattern_t *p, uint32_t pcount);

		void	applyPatterns(const char *str,
					pattern_t *p,
					uint32_t pcount,
					stringbuffer *outb);
		void	applyPattern(const char *str,
					pattern_t *p,
					stringbuffer *outb);
		void	matchAndReplace(const char *str,
					pattern_t *p,
					stringbuffer *outb);

		pattern_t	*patterns;
		uint32_t	patterncount;

		bool	enabled;

		bool	debug;
};

sqlrtranslation_patterns::sqlrtranslation_patterns(sqlrservercontroller *cont,
						sqlrtranslations *sqlts,
						domnode *parameters) :
					sqlrtranslation(cont,sqlts,parameters) {
	debugFunction();

	debug=cont->getConfig()->getDebugTranslations();

	patterns=NULL;
	patterncount=0;

	enabled=!charstring::isNo(parameters->getAttributeValue("enabled"));
	if (!enabled) {
		return;
	}

	buildPatternsTree(parameters,&patterns,&patterncount,true);
}

void sqlrtranslation_patterns::buildPatternsTree(domnode *root,
						pattern_t **p,
						uint32_t *pcount,
						bool toplevel) {

	// count patterns
	(*pcount)=0;
	for (domnode *c=root->getFirstTagChild("pattern");
			!c->isNullNode(); c=c->getNextTagSibling("pattern")) {
		(*pcount)++;
	}
	if (!(*pcount)) {
		*p=NULL;
		return;
	}

	// build pattern list
	*p=new pattern_t[*pcount];
	uint32_t	i=0;
	for (domnode *c=root->getFirstTagChild("pattern");
			!c->isNullNode(); c=c->getNextTagSibling("pattern")) {

		const char	*match=c->getAttributeValue("match");
		(*p)[i].match=match;
		(*p)[i].matchre=NULL;
		(*p)[i].matchglobal=true;
		const char	*from=c->getAttributeValue("from");
		(*p)[i].from=from;
		(*p)[i].fromre=NULL;
		(*p)[i].replaceglobal=true;
		(*p)[i].to=c->getAttributeValue("to");
		(*p)[i].ignorecase=false;
		(*p)[i].scope=SCOPE_QUERY;

		const char	*type=c->getAttributeValue("type");
		if (!charstring::compareIgnoringCase(type,"regex")) {
			if (!charstring::isNullOrEmpty(match)) {
				(*p)[i].matchre=new regularexpression();
				(*p)[i].matchre->setPattern(match);
				(*p)[i].matchre->study();
				(*p)[i].matchglobal=
					!charstring::isNo(
					c->getAttributeValue("global"));
			} else if (!charstring::isNullOrEmpty(from)) {
				(*p)[i].fromre=new regularexpression();
				(*p)[i].fromre->setPattern(from);
				(*p)[i].fromre->study();
				(*p)[i].replaceglobal=
					!charstring::isNo(
					c->getAttributeValue("global"));
			}
		} else if (!charstring::compareIgnoringCase(type,"cistring")) {
			(*p)[i].ignorecase=true;
		}

		if (toplevel) {
			const char	*scope=c->getAttributeValue("scope");
			if (!charstring::compareIgnoringCase(
						scope,"outsidequotes")) {
				(*p)[i].scope=SCOPE_OUTSIDE_QUOTES;
			} else if (!charstring::compareIgnoringCase(
						scope,"insidequotes")) {
				(*p)[i].scope=SCOPE_INSIDE_QUOTES;
			}
		}

		buildPatternsTree(c,
			&((*p)[i].patterns),
			&((*p)[i].patterncount),
			false);

		i++;
	}
}

void sqlrtranslation_patterns::freePatternsTree(pattern_t *p, uint32_t pcount) {
	if (!p || !pcount) {
		return;
	}
	freePatternsTree(p->patterns,p->patterncount);
	for (uint32_t i=0; i<pcount; i++) {
		delete p[i].matchre;
		delete p[i].fromre;
	}
	delete[] p;
}

sqlrtranslation_patterns::~sqlrtranslation_patterns() {
	freePatternsTree(patterns,patterncount);
}

bool sqlrtranslation_patterns::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					uint32_t querylength,
					stringbuffer *translatedquery) {
	debugFunction();

	if (!enabled) {
		translatedquery->append(query);
		return true;
	}

	if (debug) {
		stdoutput.printf("original query:\n\"%s\"\n\n",query);
	}

	applyPatterns(query,patterns,patterncount,translatedquery);

	return true;
}

void sqlrtranslation_patterns::applyPatterns(const char *str,
						pattern_t *p,
						uint32_t pcount,
						stringbuffer *outb) {

	// run through the patterns
	stringbuffer	querybuffer1;
	stringbuffer	querybuffer2;
	for (uint32_t i=0; i<pcount; i++) {

		// choose which buffer to write to and clear it
		stringbuffer	*outbuffer=&querybuffer1;
		if (i%2) {
			outbuffer=&querybuffer2;
		}
		if (i==pcount-1) {
			outbuffer=outb;
		} else {
			outbuffer->clear();
		}

		// get the current pattern
		pattern_t	*pc=&(p[i]);

		if (pc->scope==SCOPE_QUERY) {

			// match against the entire str
			applyPattern(str,pc,outbuffer);

		} else {

			// split the string on single-quotes
			// (NOTE: this presumes that backslash-escaped quotes
			// have been normalized by the normalize translation)
			char		**parts=NULL;
			uint64_t	partcount=0;
			charstring::split(str,"'",false,&parts,&partcount);

			// If we're looking at the query outside of quoted
			// strings, then that ought to be the even numbered
			// parts.  Otherwise it'll be the odd numbered parts.
			// However, if the query starts with a single-quote
			// (which a valid query wouldn't, but who knows...)
			// then flip the logic.
			bool	mod=(str[0]!='\'');

			// check every other part...
			for (uint64_t j=0; j<partcount; j++) {
				bool	quoted=(j%2==mod);
				if (quoted) {
					outbuffer->append('\'');
				}
				if ((quoted &&
					pc->scope==SCOPE_INSIDE_QUOTES) ||
					pc->scope==SCOPE_OUTSIDE_QUOTES) {
					applyPattern(parts[j],pc,outbuffer);
				} else {
					outbuffer->append(parts[j]);
				}
				if (quoted) {
					outbuffer->append('\'');
				}
				delete[] parts[j];
			}

			if (debug) {
				stdoutput.printf("translated to:\n\"%s\"\n\n",
							outbuffer->getString());
			}

			// clean up
			delete[] parts;
		}

		// reset input
		str=outbuffer->getString();
	}
}

void sqlrtranslation_patterns::applyPattern(const char *str,
						pattern_t *p,
						stringbuffer *outb) {

	ssize_t		pfromlen=(debug)?charstring::length(p->from):0;
	const char	*fromellipses="";
	if (pfromlen>77) {
		pfromlen=74;
		fromellipses="...";
	}
	ssize_t		ptolen=(debug)?charstring::length(p->to):0;
	const char	*toellipses="";
	if (ptolen>77) {
		ptolen=74;
		toellipses="...";
	}

	char	*convstr=NULL;

	if (p->matchre) {
		if (debug) {
			stdoutput.printf("applying "
					"match:\n\"%s\"\n",
					p->match);
		}
		matchAndReplace(str,p,outb);
	} else if (p->fromre) {
		if (debug) {
			stdoutput.printf("applying regex "
					"from:\n\"%.*s%s\"\n"
					"to:\n\"%.*s%s\"\n\n",
					pfromlen,p->from,fromellipses,
					ptolen,p->to,toellipses);
		}
		convstr=charstring::replace(str,
					p->fromre,
					p->to,
					p->replaceglobal);
		outb->append(convstr);
	} else if (!p->ignorecase) {
		if (debug) {
			stdoutput.printf("applying string "
					"from:\n\"%.*s%s\"\n"
					"to:\n\"%.*s%s\"\n",
					pfromlen,p->from,fromellipses,
					ptolen,p->to,toellipses);
			if (p->scope==SCOPE_INSIDE_QUOTES) {
				stdoutput.printf("inside quotes on chunk:\n"
							"\"%s\"\n",str);
			}
			if (p->scope==SCOPE_OUTSIDE_QUOTES) {
				stdoutput.printf("outside quotes on chunk:\n"
							"\"%s\"\n",str);
			}
			stdoutput.write("\n");
		}
		convstr=charstring::replace(str,p->from,p->to);
		outb->append(convstr);
	} else {
		if (debug) {
			stdoutput.printf("applying case-insensitive string "
					"from:\n\"%.*s%s\"\n"
					"to:\n\"%.*s%s\"\n\n",
					pfromlen,p->from,fromellipses,
					ptolen,p->to,toellipses);
		}
		char	*lowstr=charstring::duplicate(str);
		charstring::lower(lowstr);
		char	*lowfrom=charstring::duplicate(p->from);
		charstring::lower(lowfrom);
		convstr=charstring::replace(lowstr,lowfrom,p->to);
		outb->append(convstr);
		delete[] lowstr;
		delete[] lowfrom;
	}

	delete[] convstr;

	if (debug && p->scope!=SCOPE_INSIDE_QUOTES &&
			p->scope!=SCOPE_OUTSIDE_QUOTES) {
		stdoutput.printf("translated to:\n\"%s\"\n\n",
						outb->getString());
	}
}

void sqlrtranslation_patterns::matchAndReplace(const char *str,
						pattern_t *p,
						stringbuffer *outb) {

	const char	*start=str;
	for (;;) {

		// look for a matching part
		const char	*ptr=start;
		if (!*ptr || !p->matchre->match(ptr) ||
				!p->matchre->getSubstringCount()) {

			// bail if no match is found
			break;
		}

		// get the bounds of the matching chunk
		int32_t		mi=p->matchre->getSubstringCount()-1;
		const char	*matchstart=p->matchre->getSubstringStart(mi);
		const char	*matchend=p->matchre->getSubstringEnd(mi);

		// move on if they're the same
		if (matchend==matchstart) {
			ptr++;
			continue;
		}

		// get a copy of the matching chunk
		char	*matchchunk=charstring::duplicate(matchstart,
							matchend-matchstart);

		// append the previous, non-matching part of the main string
		outb->append(start,matchstart-start);

		// transform the chunk...
		applyPatterns(matchchunk,p->patterns,p->patterncount,outb);

		// move the start forward
		start=matchend;

		// bail if we're not matching globally
		if (!p->matchglobal) {
			break;
		}
	}

	// append the rest of the main string
	outb->append(start);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrtranslation
			*new_sqlrtranslation_patterns(
						sqlrservercontroller *cont,
						sqlrtranslations *ts,
						domnode *parameters) {
		return new sqlrtranslation_patterns(cont,ts,parameters);
	}
}
