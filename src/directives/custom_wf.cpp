// Copyright (c) 2017  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <string.h>

class SQLRSERVER_DLLSPEC sqlrdirective_custom_wf : public sqlrdirective {
	public:
			sqlrdirective_custom_wf(sqlrservercontroller *cont,
							sqlrdirectives *sqlds,
							xmldomnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);
	private:
		void	parseDirective(sqlrservercursor *sqlrcur,
					const char *directivestart,
					uint32_t length);
		void	crashmeTest(int32_t iargument);

		bool	enabled;
		bool	debug;

		sqlrservercontroller	*cont;
};

sqlrdirective_custom_wf::sqlrdirective_custom_wf(
					sqlrservercontroller *cont,
					sqlrdirectives *sqlds,
					xmldomnode *parameters) :
				sqlrdirective(cont,sqlds,parameters) {
	debugFunction();

	debug=cont->getConfig()->getDebugTranslations();

	enabled=charstring::compareIgnoringCase(
		parameters->getAttributeValue("enabled"),"no");

	this->cont=cont;
}

#define KEYWORD_SQLEXECDIRECT "sqlexecdirect"
#define KEYWORD_QUERYTIMEOUT "querytimeout:"
#define KEYWORD_SQLPREPARE "sqlprepare"
#define KEYWORD_SQLRELAY_CRASH "sqlrelay-crash"
#define KEYWORD_SQLRELAY_CRASH_ARG "sqlrelay-crash:"
#define MARKER_ODBC_RPC '{'

bool sqlrdirective_custom_wf::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query) {
	debugFunction();

	if (!enabled) {
		return true;
	}

	sqlrcur->setQueryTimeout(cont->getQueryTimeout());
	sqlrcur->setExecuteDirect(cont->getExecuteDirect());
	sqlrcur->setExecuteRpc(false);

	const char	*linestart=NULL;
	const char	*lineend=NULL;

	if (query[0]==MARKER_ODBC_RPC) {
		sqlrcur->setExecuteDirect(true);
		sqlrcur->setExecuteRpc(true);
	}
	linestart=query;

	uint32_t	length=charstring::length(query);
	for (uint32_t i=0; i<length; ++i) {
		if ((query[i]=='\n') || !((i+1)<length)) {
			if (query[i]=='\n') {
				lineend=&query[i];
			} else {
				lineend=&query[i+1];
			}
			if (((lineend-linestart)>2) &&
				(linestart[0]=='-') &&
				(linestart[1]=='-')) {
				parseDirective(sqlrcur,
						&linestart[2],
						lineend-linestart-2);
			}
			linestart=&query[i+1];
		}
	}

	return true;
}

void sqlrdirective_custom_wf::parseDirective(
				sqlrservercursor *sqlrcur,
				const char *directivestart,
				uint32_t length) {

	uint32_t	cleanlength=length;
	int32_t		argumentsize;
	const char	*argument;
	int32_t		iargument=0;

	if (cleanlength && directivestart[cleanlength-1]=='\r') {
		cleanlength--;
	}
	if (!cleanlength) {
		return;
	}

	// Note: These are not intended to be human friendly declarations,
	// just very strict and simple formats for a code generator to emit.
	if (!charstring::compare(directivestart,
				KEYWORD_SQLEXECDIRECT,
				cleanlength)) {
		sqlrcur->setExecuteDirect(true);
		return;
	}

	if (!charstring::compare(directivestart,
				KEYWORD_SQLPREPARE,
				cleanlength)) {
		sqlrcur->setExecuteDirect(false);
	}

	if (!charstring::compare(directivestart,
				KEYWORD_SQLRELAY_CRASH,
				cleanlength)) {
		crashmeTest(0);
		return;
	}

	if ((cleanlength>charstring::length(KEYWORD_SQLRELAY_CRASH_ARG)) &&
		(!charstring::compare(directivestart,
			KEYWORD_SQLRELAY_CRASH_ARG,
			charstring::length(KEYWORD_SQLRELAY_CRASH_ARG)))) {

		argumentsize=cleanlength-
				charstring::length(KEYWORD_SQLRELAY_CRASH_ARG);
		argument=&directivestart[
				charstring::length(KEYWORD_SQLRELAY_CRASH_ARG)];

		if (charstring::isInteger(argument,argumentsize)) {
			iargument=charstring::toInteger(argument);
		}

		crashmeTest(iargument);
		return;
	}

	if ((cleanlength>charstring::length(KEYWORD_QUERYTIMEOUT)) &&
		(!charstring::compare(directivestart,
				KEYWORD_QUERYTIMEOUT,
				charstring::length(KEYWORD_QUERYTIMEOUT)))) {

		argumentsize=cleanlength-
				charstring::length(KEYWORD_QUERYTIMEOUT);
		argument=&directivestart[
				charstring::length(KEYWORD_QUERYTIMEOUT)];

		if (charstring::isInteger(argument,argumentsize)) {
			// well, I know that the directive is always zero
			// terminated someplace, and I already know this it
			// appears to be an integer, so let it rip even though
			// we would like to use the argumentsize.
			sqlrcur->setQueryTimeout(
					charstring::toInteger(argument));
		}
		return;
	}
}

void sqlrdirective_custom_wf::crashmeTest(int32_t iargument) {

	char	*some_ptr=(char *)NULL;

	if (iargument==0) {
		// assignment to NULL pointer
		some_ptr[0]=1;
	} else if (iargument==1) {
		// delete NULL pointer
		delete[] some_ptr;
	} else if (iargument==2) {
		// double free
		some_ptr=new char[100];
		delete[] some_ptr;
		delete[] some_ptr;
	} else if (iargument==3) {
		// accessing NULL pointer
		some_ptr[0]=(memchr(some_ptr,25,30))?1:2;
	} else if (iargument==4) {
		// accessing NULL pointer 
		some_ptr[0]=some_ptr[10];
	}
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrdirective *new_sqlrdirective_custom_wf(
						sqlrservercontroller *cont,
						sqlrdirectives *sqlds,
						xmldomnode *parameters) {
		return new sqlrdirective_custom_wf(cont,sqlds,parameters);
	}
}
