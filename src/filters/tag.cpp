// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
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
	const char		*tag;
};

class SQLRSERVER_DLLSPEC sqlrfilter_tag : public sqlrfilter {
	public:
			sqlrfilter_tag(sqlrservercontroller *cont,
						sqlrfilters *fs,
						domnode *parameters);
			~sqlrfilter_tag();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);
	private:
		pattern_t	*p;
		uint32_t	patterncount;
		bool		hasscope;

		bool	enabled;

		sqlrmoduledata_tag	*md;
};

sqlrfilter_tag::sqlrfilter_tag(sqlrservercontroller *cont,
						sqlrfilters *fs,
						domnode *parameters) :
						sqlrfilter(cont,fs,parameters) {
	debugFunction();

	p=NULL;
	patterncount=0;
	hasscope=false;

	enabled=!charstring::isNo(parameters->getAttributeValue("enabled"));
	if (!enabled) {
		return;
	}

	// count patterns
	patterncount=0;
	for (domnode *c=parameters->getFirstTagChild("pattern");
			!c->isNullNode(); c=c->getNextTagSibling("pattern")) {
		patterncount++;
	}

	// build pattern list
	p=new pattern_t[patterncount];
	uint32_t	i=0;
	for (domnode *c=parameters->getFirstTagChild("pattern");
			!c->isNullNode(); c=c->getNextTagSibling("pattern")) {

		const char	*pattern=c->getAttributeValue("pattern");
		p[i].pattern=pattern;
		p[i].re=NULL;
		p[i].ignorecase=false;
		p[i].tag=c->getAttributeValue("tag");

		const char	*type=c->getAttributeValue("type");
		if (!charstring::compareIgnoringCase(type,"regex")) {
			p[i].re=new regularexpression();
			p[i].re->setPattern(pattern);
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
		i++;
	}

	// get the moduledata
	const char	*moduledataid=
			parameters->getAttributeValue("moduledataid");
	if (charstring::isNullOrEmpty(moduledataid)) {
		moduledataid="tags";
	}
	md=(sqlrmoduledata_tag *)cont->getModuleData(moduledataid);
}

sqlrfilter_tag::~sqlrfilter_tag() {
	for (uint32_t i=0; i<patterncount; i++) {
		delete p[i].re;
	}
	delete[] p;
}

bool sqlrfilter_tag::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query) {
	debugFunction();

	if (!enabled) {
		return true;
	}

	if (!md) {
		return true;
	}

	// split the string on single-quotes if necessary
	// (NOTE: this presumes that backslash-escaped quotes
	// have been normalized by the normalize translation)
	char		**parts=NULL;
	uint64_t	partcount=0;
	if (hasscope) {
		charstring::split(query,"'",false,&parts,&partcount);
	}

	// run through the patterns until one of them fails...
	for (uint32_t i=0; i<patterncount; i++) {

		pattern_t	*pc=&(p[i]);

		// match against the entire query, if necessary...
		if (pc->scope==SCOPE_QUERY) {

			// handle regex patterns
			if (pc->re && pc->re->match(query)) {

				// create tag in module data
				md->addTag(sqlrcur->getId(),pc->tag);

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
						*cq=character::lower(*cq);
					}
					qry=lowquery;

					lowpattern=charstring::duplicate(
								pc->pattern);
					for (char *cp=lowpattern; *cp; cp++) {
						*cp=character::lower(*cp);
					}
					ptrn=lowpattern;
				}

				// compare
				if (charstring::contains(qry,ptrn)) {

					// create tag in module data
					md->addTag(sqlrcur->getId(),pc->tag);
				}

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
		for (uint64_t j=start; j<partcount; j=j+2) {

			// handle regex patterns
			if (pc->re && pc->re->match(parts[j])) {

				// create tag in module data
				md->addTag(sqlrcur->getId(),pc->tag);

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
						*cq=character::lower(*cq);
					}
					prt=lowpart;


					lowpattern=charstring::duplicate(
								pc->pattern);
					for (char *cp=lowpattern; *cp; cp++) {
						*cp=character::lower(*cp);
					}
					ptrn=lowpattern;
				}

				// compare
				if (charstring::contains(prt,ptrn)) {

					// create tag in module data
					md->addTag(sqlrcur->getId(),pc->tag);
				}

				// clean up
				delete[] lowpart;
				delete[] lowpattern;
			}
		}
	}

	// clean up
	for (uint32_t k=0; k<partcount; k++) {
		delete[] parts[k];
	}
	delete[] parts;

	// always return true, we're not actually filtering out queries
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrfilter
			*new_sqlrfilter_tag(sqlrservercontroller *cont,
						sqlrfilters *fs,
						domnode *parameters) {
		return new sqlrfilter_tag(cont,fs,parameters);
	}
}
