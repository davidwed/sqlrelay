// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

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


		bool	space(const char *ptr, const char **newptr);
		bool	comma(const char *ptr, const char **newptr);
		bool	leftParen(const char *ptr, const char **newptr);
		bool	rightParen(const char *ptr, const char **newptr);


	// Ideally these would be private but other classes (and their children)
	// need to access the static constants.  I could declare them in a
	// separate block but it's way easier if the parse/clause methods and
	// variable are all together.  Maybe I'll move them later.
	public:
		// generic...
		bool	parseQuery(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseName(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_name;
		bool	parseType(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_type;
		static const char *_size;
		bool	parseValues(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_values;
		static const char *_value;
		bool	parseLength(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_length;
		bool	parseScale(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_scale;
		bool	parseVerbatim(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_verbatim;
		bool	parseRemainderVerbatim(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);



		// create query...
		bool	parseCreate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	createClause(const char *ptr,
						const char **newptr);
		static const char *_create;
		bool	parseCreateTemporary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	temporaryClause(const char *ptr,
						const char **newptr);
		static const char *_create_temporary;
		bool	tableClause(const char *ptr,
						const char **newptr);


		// create table...
		bool	parseCreateTable(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_table;
		bool	parseIfNotExists(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	ifNotExistsClause(const char *ptr,
						const char **newptr);
		static const char *_if_not_exists;
		bool	parseColumnAndConstraintDefinitions(
						xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_columns;
		static const char *_column;
		bool	parseColumnDefinition(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseConstraints(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_constraints;
		bool	parseUnsigned(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	unsignedClause(const char *ptr,
						const char **newptr);
		static const char *_unsigned;
		bool	parseZeroFill(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	zeroFillClause(const char *ptr,
						const char **newptr);
		static const char *_zerofill;
		bool	parseBinary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	binaryClause(const char *ptr,
						const char **newptr);
		static const char *_binary;
		bool	parseCharacterSet(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	characterSetClause(const char *ptr,
						const char **newptr);
		static const char *_character_set;
		bool	parseCollate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	collateClause(const char *ptr,
						const char **newptr);
		static const char *_collate;
		bool	parseNull(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	nullClause(const char *ptr,
						const char **newptr);
		static const char *_null;
		bool	parseNotNull(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	notNullClause(const char *ptr,
						const char **newptr);
		static const char *_not_null;
		bool	parseDefault(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	defaultClause(const char *ptr,
						const char **newptr);
		static const char *_default;
		bool	parseAutoIncrement(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	autoIncrementClause(const char *ptr,
						const char **newptr);
		static const char *_auto_increment;
		bool	parseUniqueKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	uniqueKeyClause(const char *ptr,
						const char **newptr);
		static const char *_unique_key;
		bool	parsePrimaryKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	primaryKeyClause(const char *ptr,
						const char **newptr);
		static const char *_primary_key;
		bool	parseKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	keyClause(const char *ptr,
						const char **newptr);
		static const char *_key;
		bool	parseComment(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	commentClause(const char *ptr,
						const char **newptr);
		static const char *_comment;
		bool	parseColumnFormat(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	columnFormatClause(const char *ptr,
						const char **newptr);
		static const char *_column_format;
		bool	parseReferenceDefinition(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	referencesClause(const char *ptr,
						const char **newptr);
		static const char *_references;
		bool	parseReferenceColumns(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseMatch(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	matchClause(const char *ptr,
						const char **newptr);
		static const char *_match;
		bool	parseOnDelete(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	onDeleteClause(const char *ptr,
						const char **newptr);
		static const char *_on_delete;
		bool	parseOnUpdate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	onUpdateClause(const char *ptr,
						const char **newptr);
		static const char *_on_update;
		bool	parseReferenceOption(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	referenceOptionClause(const char *ptr,
						const char **newptr);
		bool	parseOnCommit(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	onCommitClause(const char *ptr,
						const char **newptr);
		bool	onCommitOptionClause(const char *ptr,
						const char **newptr);
		static const char *_on_commit;
		bool	parseAs(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	asClause(const char *ptr,
						const char **newptr);
		static const char *_as;



		// drop query...
		bool	parseDrop(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	dropClause(const char *ptr,
						const char **newptr);
		static const char *_drop;
		bool	parseDropTemporary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_drop_temporary;
		bool	parseDropTable(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseIfExists(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	ifExistsClause(const char *ptr,
						const char **newptr);
		static const char *_if_exists;
		bool	parseTableNameList(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_table_name_list;
		static const char *_table_name_list_item;
		bool	parseRestrict(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	restrictClause(const char *ptr,
						const char **newptr);
		static const char *_restrict;
		bool	parseCascade(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	cascadeClause(const char *ptr,
						const char **newptr);
		static const char *_cascade;



		// insert query...
		bool	parseInsert(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertClause(const char *ptr,
						const char **newptr);
		static const char *_insert;
		bool	parseInsertInto(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertIntoClause(const char *ptr,
						const char **newptr);
		static const char *_insert_into;



		// update query...
		bool	parseUpdate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	updateClause(const char *ptr,
						const char **newptr);
		static const char *_update;



		// delete query...
		bool	parseDelete(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	deleteClause(const char *ptr,
						const char **newptr);
		static const char *_delete;



		// select query...
		bool	parseSelect(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	selectClause(const char *ptr,
						const char **newptr);
		static const char *_select;
		bool	parseUnique(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	uniqueClause(const char *ptr,
						const char **newptr);
		static const char *_unique;
		bool	parseDistinct(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	distinctClause(const char *ptr,
						const char **newptr);
		static const char *_distinct;
		bool	parseFrom(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	fromClause(const char *ptr,
						const char **newptr);
		static const char *_from;
		bool	parseWhere(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	whereClause(const char *ptr,
						const char **newptr);
		static const char *_where;
		bool	parseGroupBy(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	groupByClause(const char *ptr,
						const char **newptr);
		static const char *_group_by;
		bool	parseHaving(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	havingClause(const char *ptr,
						const char **newptr);
		static const char *_having;
		bool	parseOrderBy(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	orderByClause(const char *ptr,
						const char **newptr);
		static const char *_order_by;
		bool	parseLimit(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	limitClause(const char *ptr,
						const char **newptr);
		static const char *_limit;
		bool	parseProcedure(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	procedureClause(const char *ptr,
						const char **newptr);
		static const char *_procedure;
		bool	parseSelectInto(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	selectIntoClause(const char *ptr,
						const char **newptr);
		static const char *_select_into;
		bool	parseForUpdate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	forUpdateClause(const char *ptr,
						const char **newptr);
		static const char *_for_update;


		// set query...
		bool parseSet(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool setClause(const char *ptr, const char **newptr);
		static const char *_set;
		bool parseSetGlobal(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool setGlobalClause(const char *ptr, const char **newptr);
		static const char *_set_global;
		bool parseSetSession(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool setSessionClause(const char *ptr, const char **newptr);
		static const char *_set_session;
		bool parseTransaction(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool transactionClause(const char *ptr,
						const char **newptr);
		static const char *_transaction;
		bool parseIsolationLevel(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool isolationLevelClause(const char *ptr,
						const char **newptr);
		static const char *_isolation_level;
		bool isolationLevelOptionClause(const char *ptr,
						const char **newptr);
		bool parseTransactionName(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool transactionNameClause(const char *ptr,
						const char **newptr);
		static const char *_transaction_name;

		xmldom	*tree;
};

#endif
