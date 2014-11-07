// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLRRESULTSETTRANSLATIONS_H
#define SQLRRESULTSETTRANSLATIONS_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>
#include <rudiments/dynamiclib.h>
#include <rudiments/singlylinkedlist.h>
#include <sqlrelay/sqlrresultsettranslation.h>

class sqlrserverconnection;
class sqlrservercursor;

class SQLRSERVER_DLLSPEC sqlrresultsettranslationplugin {
	public:
		sqlrresultsettranslation	*rstr;
		dynamiclib			*dl;
};

class SQLRSERVER_DLLSPEC sqlrresultsettranslations {
	public:
			sqlrresultsettranslations();
			~sqlrresultsettranslations();

		bool	loadResultSetTranslations(
					const char *resultsettranslations);
		bool	runResultSetTranslations(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint16_t fieldindex,
						const char *field,
						uint32_t fieldlength,
						const char **newfield,
						uint32_t *newfieldlength);
	private:
		void	unloadResultSetTranslations();
		void	loadResultSetTranslation(
					xmldomnode *resultsettranslation);
		
		xmldom	*xmld;

		singlylinkedlist< sqlrresultsettranslationplugin * >	tlist;
};

#endif
