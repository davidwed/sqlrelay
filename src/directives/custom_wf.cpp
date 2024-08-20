// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <string.h>

class SQLRSERVER_DLLSPEC sqlrdirective_custom_wf : public sqlrdirective {
	public:
		sqlrdirective_custom_wf(sqlrservercontroller *cont,
							domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);
	private:
		void	parseDirective(sqlrservercursor *sqlrcur,
					const char *directivestart,
					uint32_t length);
};

sqlrdirective_custom_wf::sqlrdirective_custom_wf(
					sqlrservercontroller *cont,
					domnode *parameters) :
				sqlrdirective(cont,parameters) {
	debugFunction();
}

#define KEYWORD_SQLEXECDIRECT "sqlexecdirect"
#define KEYWORD_QUERYTIMEOUT "querytimeout:"
#define KEYWORD_SQLPREPARE "sqlprepare"
#define MARKER_ODBC_RPC '{'

bool sqlrdirective_custom_wf::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query) {
	debugFunction();

	// reset directives
	sqlrcur->setQueryTimeout(cont->getQueryTimeout());
	sqlrcur->setExecuteDirect(cont->getExecuteDirect());
	sqlrcur->setExecuteRpc(false);

	// run through the query, processing directives
	const char	*line=query;
	const char	*directivestart=NULL;
	uint32_t	directivelength=0;
	while (getDirective(line,&directivestart,&directivelength,&line)) {
		parseDirective(sqlrcur,directivestart,directivelength);
	}

	// check for rpc markers (which might follow the comments)
	if (*line==MARKER_ODBC_RPC) {
		debugWrite("%s...",MARKER_ODBC_RPC);
		sqlrcur->setExecuteDirect(true);
		sqlrcur->setExecuteRpc(true);
	}

	return true;
}

void sqlrdirective_custom_wf::parseDirective(
				sqlrservercursor *sqlrcur,
				const char *directivestart,
				uint32_t length) {
	debugFunction();

	if (directivestart[length]=='\r') {
		length--;
	}
	if (!length) {
		return;
	}

	// Note: These are not intended to be human friendly declarations,
	// just very strict and simple formats for a code generator to emit.
	if (!charstring::compare(directivestart,
				KEYWORD_SQLEXECDIRECT,
				length)) {
		debugWrite("%s...",KEYWORD_SQLEXECDIRECT);
		sqlrcur->setExecuteDirect(true);
		return;
	}

	if (!charstring::compare(directivestart,
				KEYWORD_SQLPREPARE,
				length)) {
		debugWrite("%s...",KEYWORD_SQLPREPARE);
		sqlrcur->setExecuteDirect(false);
	}

	if ((length>charstring::getLength(KEYWORD_QUERYTIMEOUT)) &&
		(!charstring::compare(directivestart,
				KEYWORD_QUERYTIMEOUT,
				charstring::getLength(KEYWORD_QUERYTIMEOUT)))) {

		int32_t		argumentsize=length-
				charstring::getLength(KEYWORD_QUERYTIMEOUT);
		const char	*argument=&directivestart[
				charstring::getLength(KEYWORD_QUERYTIMEOUT)];

		if (charstring::isInteger(argument,argumentsize)) {
			// well, I know that the directive is always zero
			// terminated someplace, and I already know this it
			// appears to be an integer, so let it rip even though
			// we would like to use the argumentsize.
			debugWrite("%s%lld...",
					KEYWORD_QUERYTIMEOUT,
					charstring::convertToInteger(argument));
			sqlrcur->setQueryTimeout(
					charstring::convertToInteger(argument));
		} else {
			debugWrite("%s...bad argument...",
					KEYWORD_QUERYTIMEOUT);
		}
		return;
	}
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrdirective *new_sqlrdirective_custom_wf(
						sqlrservercontroller *cont,
						domnode *parameters) {
		return new sqlrdirective_custom_wf(cont,parameters);
	}
}
