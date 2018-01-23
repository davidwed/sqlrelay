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

	debug=cont->getConfig()->getDebugDirectives();

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

	if (query[0]==MARKER_ODBC_RPC) {
		if (debug) {
			stdoutput.printf("%s...\n",MARKER_ODBC_RPC);
		}
		sqlrcur->setExecuteDirect(true);
		sqlrcur->setExecuteRpc(true);
	}

	const char	*linestart=query;
	for (const char *ptr=query; *ptr; ptr++) {
		if ((*ptr=='\n' || !*ptr) &&
				(ptr-linestart)>2 &&
				(linestart[0]=='-') &&
				(linestart[1]=='-')) {
			parseDirective(sqlrcur,
					linestart+2,
					ptr-linestart-2);
			linestart=ptr+1;
		}
	}

	return true;
}

void sqlrdirective_custom_wf::parseDirective(
				sqlrservercursor *sqlrcur,
				const char *directivestart,
				uint32_t length) {
	debugFunction();

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
		if (debug) {
			stdoutput.printf("%s...\n",KEYWORD_SQLEXECDIRECT);
		}
		sqlrcur->setExecuteDirect(true);
		return;
	}

	if (!charstring::compare(directivestart,
				KEYWORD_SQLPREPARE,
				cleanlength)) {
		if (debug) {
			stdoutput.printf("%s...\n",KEYWORD_SQLPREPARE);
		}
		sqlrcur->setExecuteDirect(false);
	}

	if (!charstring::compare(directivestart,
				KEYWORD_SQLRELAY_CRASH,
				cleanlength)) {
		if (debug) {
			stdoutput.printf("%s...\n",KEYWORD_SQLRELAY_CRASH);
		}
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
			if (debug) {
				stdoutput.printf("%s%lld...\n",
					KEYWORD_QUERYTIMEOUT,
					charstring::toInteger(argument));
			}
			sqlrcur->setQueryTimeout(
					charstring::toInteger(argument));
		} else if (debug) {
			stdoutput.printf("%s...bad argument...\n",
					KEYWORD_QUERYTIMEOUT);
		}
		return;
	}
}

void sqlrdirective_custom_wf::crashmeTest(int32_t iargument) {
	debugFunction();

	char	*some_ptr=(char *)NULL;

	if (iargument==0) {
		// assignment to NULL pointer
		if (debug) {
			stdoutput.printf("%s:assignment to NULL ptr...\n",
					KEYWORD_SQLRELAY_CRASH_ARG);
		}
		some_ptr[0]=1;
	} else if (iargument==1) {
		// delete NULL pointer
		if (debug) {
			stdoutput.printf("%s:delete NULL ptr...\n",
					KEYWORD_SQLRELAY_CRASH_ARG);
		}
		delete[] some_ptr;
	} else if (iargument==2) {
		// double free
		if (debug) {
			stdoutput.printf("%s:double free...\n",
					KEYWORD_SQLRELAY_CRASH_ARG);
		}
		some_ptr=new char[100];
		delete[] some_ptr;
		delete[] some_ptr;
	} else if (iargument==3) {
		// accessing NULL pointer
		if (debug) {
			stdoutput.printf("%s:access NULL ptr...\n",
					KEYWORD_SQLRELAY_CRASH_ARG);
		}
		some_ptr[0]=(memchr(some_ptr,25,30))?1:2;
	} else if (iargument==4) {
		// accessing NULL pointer 
		if (debug) {
			stdoutput.printf("%s:assign and access NULL ptr...\n",
					KEYWORD_SQLRELAY_CRASH_ARG);
		}
		some_ptr[0]=some_ptr[10];
	} else if (debug) {
		stdoutput.printf("%s:...bad argument...\n",
				KEYWORD_SQLRELAY_CRASH_ARG);
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
