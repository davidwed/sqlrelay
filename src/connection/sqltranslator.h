// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLTRANSLATOR_H
#define SQLTRANSLATOR_H

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

using namespace rudiments;

class sqlrconnection_svr;
class sqlrcursor_svr;

class sqltranslator {
	public:
			sqltranslator();
		virtual	~sqltranslator();

		virtual bool	loadRules(const char *rules);
		virtual bool	applyRules(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldom *querytree);
	protected:
		virtual bool	applyRulesToQuery(xmldomnode *query);
		virtual bool	translateDatatypes(xmldomnode *query,
							xmldomnode *rule);
		virtual bool	convertDatatypes(xmldomnode *query,
							xmldomnode *rule);
		virtual bool	trimColumnsComparedToStringBinds(
							xmldomnode *query,
							xmldomnode *rule);
		virtual bool	translateDateTimes(xmldomnode *query,
							xmldomnode *rule);
		bool		translateDateTimesInQuery(xmldomnode *querynode,
							xmldomnode *rule);
		bool		translateDateTimesInBindVariables(
							xmldomnode *querynode,
							xmldomnode *rule);
		char		*convertDateTime(const char *format,
							int16_t year,
							int16_t month,
							int16_t day,
							int16_t hour,
							int16_t minute,
							int16_t second);


		// helper methods
		xmldomnode	*newNode(xmldomnode *parentnode,
						const char *type);
		xmldomnode	*newNode(xmldomnode *parentnode,
						const char *type,
						const char *value);
		xmldomnode	*newNodeAfter(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type);
		xmldomnode	*newNodeAfter(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type,
						const char *value);
		void		setAttribute(xmldomnode *node,
						const char *name,
						const char *value);
		bool		isString(const char *value);
		const char * const	*getShortMonths();
		const char * const	*getLongMonths();


		xmldomnode	*rulesnode;
	private:
		xmldom		*xmld;
		xmldom		*tree;

		sqlrconnection_svr	*sqlrcon;
		sqlrcursor_svr		*sqlrcur;
};

#endif
