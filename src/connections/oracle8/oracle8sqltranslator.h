#ifndef ORACLE8SQLTRANSLATOR_H
#define ORACLE8SQLTRANSLATOR_H

#include <sqltranslator.h>

using namespace rudiments;

class oracle8sqltranslator : public sqltranslator {
	public:
			oracle8sqltranslator();
			~oracle8sqltranslator();
	protected:
		bool	applyRulesToQuery(xmldomnode *query);
		bool	tempTablesPreserveRowsByDefault(xmldomnode *query,
							xmldomnode *rule);
		bool	translateDatatypes(xmldomnode *query,
							xmldomnode *rule);
};

#endif
