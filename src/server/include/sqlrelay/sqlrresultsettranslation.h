// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRRESULTSETTRANSLATION_H
#define SQLRRESULTSETTRANSLATION_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <rudiments/xmldomnode.h>

class sqlrconnection_svr;
class sqlrcursor_svr;
class sqlrresultsettranslations;

class SQLRSERVER_DLLSPEC sqlrresultsettranslation {
	public:
			sqlrresultsettranslation(
					sqlrresultsettranslations *sqlrrsts,
					xmldomnode *parameters);
		virtual	~sqlrresultsettranslation();

		virtual bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					uint16_t fieldindex,
					const char *field,
					uint32_t fieldlength,
					const char **newfield,
					uint32_t *newfieldlength);
	protected:
		sqlrresultsettranslations	*sqlrrsts;
		xmldomnode			*parameters;
};

#endif
