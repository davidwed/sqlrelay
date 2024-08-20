// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <string.h>

class SQLRSERVER_DLLSPEC sqlrdirective_crash : public sqlrdirective {
	public:
		sqlrdirective_crash(sqlrservercontroller *cont,
							domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);
	private:
		void	parseDirective(sqlrservercursor *sqlrcur,
					const char *directivestart,
					uint32_t length);
		void	crashmeTest(int32_t iargument);
};

sqlrdirective_crash::sqlrdirective_crash(sqlrservercontroller *cont,
					domnode *parameters) :
				sqlrdirective(cont,parameters) {
	debugFunction();
}

#define KEYWORD_SQLRELAY_CRASH "sqlrelay-crash"
#define KEYWORD_SQLRELAY_CRASH_ARG "sqlrelay-crash:"

bool sqlrdirective_crash::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query) {
	debugFunction();

	// run through the query, processing directives
	const char	*line=query;
	const char	*directivestart=NULL;
	uint32_t	directivelength=0;
	while (getDirective(line,&directivestart,&directivelength,&line)) {
		parseDirective(sqlrcur,directivestart,directivelength);
	}
	return true;
}

void sqlrdirective_crash::parseDirective(
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

	if (!charstring::compare(directivestart,
				KEYWORD_SQLRELAY_CRASH,
				length)) {
		debugWrite("%s...",KEYWORD_SQLRELAY_CRASH);
		crashmeTest(0);
		return;
	}

	if ((length>charstring::getLength(KEYWORD_SQLRELAY_CRASH_ARG)) &&
		(!charstring::compare(directivestart,
			KEYWORD_SQLRELAY_CRASH_ARG,
			charstring::getLength(KEYWORD_SQLRELAY_CRASH_ARG)))) {

		int32_t		argumentsize=length-
				charstring::getLength(KEYWORD_SQLRELAY_CRASH_ARG);
		const char	*argument=&directivestart[
				charstring::getLength(KEYWORD_SQLRELAY_CRASH_ARG)];

		int32_t		iargument=0;
		if (charstring::isInteger(argument,argumentsize)) {
			iargument=charstring::convertToInteger(argument);
		}
		crashmeTest(iargument);
		return;
	}
}

void sqlrdirective_crash::crashmeTest(int32_t iargument) {
	debugFunction();

	char	*some_ptr=(char *)NULL;

	if (iargument==0) {
		// assignment to NULL pointer
		debugWrite("%s:assignment to NULL ptr...",
					KEYWORD_SQLRELAY_CRASH_ARG);
		some_ptr[0]=1;
	} else if (iargument==1) {
		// delete NULL pointer
		debugWrite("%s:delete NULL ptr...",
					KEYWORD_SQLRELAY_CRASH_ARG);
		delete[] some_ptr;
	} else if (iargument==2) {
		// double free
		debugWrite("%s:double free...",
					KEYWORD_SQLRELAY_CRASH_ARG);
		some_ptr=new char[100];
		delete[] some_ptr;
		delete[] some_ptr;
	} else if (iargument==3) {
		// accessing NULL pointer
		debugWrite("%s:access NULL ptr...",
					KEYWORD_SQLRELAY_CRASH_ARG);
		some_ptr[0]=(memchr(some_ptr,25,30))?1:2;
	} else if (iargument==4) {
		// accessing NULL pointer 
		debugWrite("%s:assign and access NULL ptr...",
					KEYWORD_SQLRELAY_CRASH_ARG);
		some_ptr[0]=some_ptr[10];
	} else {
		debugWrite("%s:...bad argument...",
					KEYWORD_SQLRELAY_CRASH_ARG);
	}
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrdirective *new_sqlrdirective_crash(
						sqlrservercontroller *cont,
						domnode *parameters) {
		return new sqlrdirective_crash(cont,parameters);
	}
}
