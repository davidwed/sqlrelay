#ifndef ORACLE8SQLTRANSLATOR_H
#define ORACLE8SQLTRANSLATOR_H

#include <sqltranslations.h>

using namespace rudiments;

class oracle8sqltranslations : public sqltranslations {
	public:
			oracle8sqltranslations();
			~oracle8sqltranslations();
	protected:
		bool	applyTranslationsToQuery(xmldomnode *query);
		bool	tempTablesPreserveRowsByDefault(xmldomnode *query,
							xmldomnode *rule);
		bool	translateDatatypes(xmldomnode *query,
							xmldomnode *rule);
		bool	tempTablesAddMissingColumns(xmldomnode *query,
							xmldomnode *rule);
};

#endif
