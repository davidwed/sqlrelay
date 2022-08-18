// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <string.h>

class SQLRSERVER_DLLSPEC sqlrdirective_singlestep : public sqlrdirective {
	public:
			sqlrdirective_singlestep(sqlrservercontroller *cont,
							sqlrdirectives *sqlds,
							domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);
	private:
		void	parseDirective(sqlrservercursor *sqlrcur,
					const char *directivestart,
					uint32_t length);

		sqlrservercontroller	*cont;

		bool	debug;
		bool	enabled;
};

sqlrdirective_singlestep::sqlrdirective_singlestep(
					sqlrservercontroller *cont,
					sqlrdirectives *sqlds,
					domnode *parameters) :
				sqlrdirective(cont,sqlds,parameters) {
	debugFunction();

	this->cont=cont;

	debug=cont->getConfig()->getDebugDirectives();

	enabled=!charstring::isNo(parameters->getAttributeValue("enabled"));
}

bool sqlrdirective_singlestep::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query) {
	debugFunction();

	if (!enabled) {
		return true;
	}

	// bail if not postgresql
	if (charstring::compare(sqlrcon->identify(),"postgresql")) {
		return true;
	}

	// reset fetch-at-once to whatever was configured in the config file
	// (or whatever we defaulted to if there was no configuration)
	sqlrcur->setFetchAtOnce(cont->getFetchAtOnce());

	// run through the query, processing directives
	const char	*line=query;
	const char	*directivestart=NULL;
	uint32_t	directivelength=0;
	while (getDirective(line,&directivestart,&directivelength,&line)) {

		// if we found singlestep=on then set fetch-at-once to 1
		if (directivelength==13 &&
			!charstring::compare(directivestart,
						"singlestep=on",13)) {
			sqlrcur->setFetchAtOnce(1);

		} else

		// if we found singlestep=off then set fetch-at-once to 0
		if (directivelength==14 &&
			!charstring::compare(directivestart,
						"singlestep=off",14)) {
			sqlrcur->setFetchAtOnce(0);
		} 
	}

	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrdirective *new_sqlrdirective_singlestep(
						sqlrservercontroller *cont,
						sqlrdirectives *sqlds,
						domnode *parameters) {
		return new sqlrdirective_singlestep(cont,sqlds,parameters);
	}
}
