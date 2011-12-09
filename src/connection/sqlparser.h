#ifndef SQLPARSER
#define SQLPARSER

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

using namespace rudiments;

class sqlparser {
	public:
			sqlparser();
		virtual	~sqlparser();

		bool	parse(const char *query);
		xmldom	*detachTree();
	private:

		char	*cleanQuery(const char *query);
		bool	inSet(char c, const char *set);

		bool	comparePart(const char *ptr,
					const char **newptr,
					const char *part);
		bool	comparePart(const char *ptr,
					const char **newptr,
					const char * const *parts);

		char	*getWord(const char *ptr,
					const char **newptr);
		char	*getUntil(const char *set,
					const char *ptr,
					const char **newptr);
		char	*getClause(const char *ptr,
					const char *newptr);
		char	*getVerbatim(const char *ptr,
					const char **newptr);


		xmldomnode	*newNode(xmldomnode *parentnode,
						const char *type);
		xmldomnode	*newNode(xmldomnode *parentnode,
						const char *type,
						const char *value);
		void		setAttribute(xmldomnode *node,
						const char *name,
						const char *value);


		bool	parseQuery(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseTableName(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseColumnName(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseType(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseValues(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseLength(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseScale(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseVerbatim(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseRemainderVerbatim(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);


		bool	parseCreate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseTemporary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseCreateTable(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseIfNotExists(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseColumnAndConstraintDefinitions(
						xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseColumnDefinition(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseConstraints(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseUnsigned(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseZeroFill(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseBinary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseCharacterSet(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseCollate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseNullable(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseNotNullable(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseDefaultValue(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseAutoIncrement(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseUniqueKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parsePrimaryKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseComment(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseColumnFormat(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseReferenceDefinition(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseReferenceColumns(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseMatch(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseOnDelete(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseOnUpdate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseReferenceOption(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseOnCommit(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseTableOptions(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parsePartitionOptions(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);


		bool	parseDrop(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);


		bool	parseInsert(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);


		bool	parseUpdate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);


		bool	parseDelete(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);


		bool	parseSelect(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseSelectOptions(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseUnique(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseDistinct(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);


		bool	space(const char *ptr, const char **newptr);
		bool	comma(const char *ptr, const char **newptr);
		bool	leftParen(const char *ptr, const char **newptr);
		bool	rightParen(const char *ptr, const char **newptr);


		bool	createClause(const char *ptr,
						const char **newptr);
		bool	temporaryClause(const char *ptr,
						const char **newptr);
		bool	tableClause(const char *ptr,
						const char **newptr);
		bool	ifNotExistsClause(const char *ptr,
						const char **newptr);
		bool	unsignedClause(const char *ptr,
						const char **newptr);
		bool	zeroFillClause(const char *ptr,
						const char **newptr);
		bool	binaryClause(const char *ptr,
						const char **newptr);
		bool	characterSetClause(const char *ptr,
						const char **newptr);
		bool	collateClause(const char *ptr,
						const char **newptr);
		bool	nullableClause(const char *ptr,
						const char **newptr);
		bool	notNullableClause(const char *ptr,
						const char **newptr);
		bool	defaultValueClause(const char *ptr,
						const char **newptr);
		bool	autoIncrementClause(const char *ptr,
						const char **newptr);
		bool	uniqueKeyClause(const char *ptr,
						const char **newptr);
		bool	primaryKeyClause(const char *ptr,
						const char **newptr);
		bool	keyClause(const char *ptr,
						const char **newptr);
		bool	commentClause(const char *ptr,
						const char **newptr);
		bool	columnFormatClause(const char *ptr,
						const char **newptr);
		bool	referencesClause(const char *ptr,
						const char **newptr);
		bool	matchClause(const char *ptr,
						const char **newptr);
		bool	onDeleteClause(const char *ptr,
						const char **newptr);
		bool	onUpdateClause(const char *ptr,
						const char **newptr);
		bool	referenceOptionClause(const char *ptr,
						const char **newptr);
		bool	onCommitClause(const char *ptr,
						const char **newptr);
		bool	onCommitOptionClause(const char *ptr,
						const char **newptr);


		bool	dropClause(const char *ptr,
						const char **newptr);

		bool	insertClause(const char *ptr,
						const char **newptr);

		bool	updateClause(const char *ptr,
						const char **newptr);

		bool	deleteClause(const char *ptr,
						const char **newptr);

		bool	selectClause(const char *ptr,
						const char **newptr);
		bool	uniqueClause(const char *ptr,
						const char **newptr);
		bool	distinctClause(const char *ptr,
						const char **newptr);

		xmldom		*tree;
};

#endif
